/*
 * xPilot: X-Plane pilot client for VATSIM
 * Copyright (C) 2019-2020 Justin Shannon
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.
*/

#include "Plugin.h"
#include "XPilot.h"
#include "XPMPMultiplayer.h"
#include "Config.h"
#include "Utilities.h"
#include "AircraftManager.h"
#include "NetworkAircraft.h"
#include "FrameRateMonitor.h"
#include "NearbyATCWindow.h"
#include "SettingsWindow.h"
#include "NotificationPanel.h"
#include "TextMessageConsole.h"
#include "sha512.hh"
#include "json.hpp"
#include "Base64.hpp"
#include "../protobuf/Envelope.pb.h"
#include <boost/process.hpp>
#include <boost/asio.hpp>
#ifdef WIN32
#include <boost/process/windows.hpp>
#endif

using json = nlohmann::json;
namespace bp = boost::process;

bp::child bridgeProcess;
bp::opstream in;
bp::ipstream out;

namespace xpilot
{
	XPilot::XPilot() :
		m_xplaneAtisEnabled("sim/atc/atis_enabled", ReadWrite),
		m_pttPressed("xpilot/ptt", ReadWrite),
		m_rxCom1("xpilot/audio/com1_rx", ReadWrite),
		m_rxCom2("xpilot/audio/com2_rx", ReadWrite),
		m_networkLoginStatus("xpilot/login/status", ReadOnly),
		m_networkCallsign("xpilot/login/callsign", ReadOnly),
		m_volumeSignalLevel("xpilot/audio/vu", ReadWrite),
		m_aiControlled("xpilot/ai_controlled", ReadOnly),
		m_aircraftCount("xpilot/num_aircraft", ReadOnly),
		m_pluginVersion("xpilot/version", ReadOnly),
		m_frameRatePeriod("sim/operation/misc/frame_rate_period", ReadOnly)
	{
		thisThreadIsXP();

		m_bulkDataQuick = XPLMRegisterDataAccessor("xpilot/bulk/quick",
			xplmType_Data,
			false,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL, NULL,
			NULL, NULL,
			NULL, NULL,
			getBulkData,
			NULL,
			(void*)DR_BULK_QUICK,
			(void*)DR_BULK_QUICK
		);

		m_bulkDataExpensive = XPLMRegisterDataAccessor("xpilot/bulk/expensive",
			xplmType_Data,
			false,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL, NULL,
			NULL, NULL,
			NULL, NULL,
			getBulkData,
			NULL,
			(void*)DR_BULK_EXPENSIVE,
			(void*)DR_BULK_EXPENSIVE
		);

		int left, top, right, bottom, screenTop, screenRight;
		XPLMGetScreenBoundsGlobal(nullptr, &screenTop, &screenRight, nullptr);
		right = screenRight - 35; /*padding left*/
		top = screenTop - 35; /*width*/
		left = screenRight - 800; /*padding top*/
		bottom = top - 100; /*height*/
		m_notificationPanel = std::make_unique<NotificationPanel>(left, top, right, bottom);
		m_textMessageConsole = std::make_unique<TextMessageConsole>(this);
		m_nearbyAtcWindow = std::make_unique<NearbyATCWindow>(this);
		m_settingsWindow = std::make_unique<SettingsWindow>();
		m_frameRateMonitor = std::make_unique<FrameRateMonitor>(this);
		m_aircraftManager = std::make_unique<AircraftManager>(this);
		m_pluginHash = sw::sha512::file(GetTruePluginPath().c_str());
		m_pluginVersion = PLUGIN_VERSION;

		XPLMRegisterFlightLoopCallback(deferredStartup, -1.0f, this);
	}

	XPilot::~XPilot()
	{
		XPLMUnregisterDataAccessor(m_bulkDataQuick);
		XPLMUnregisterDataAccessor(m_bulkDataExpensive);
		XPLMUnregisterFlightLoopCallback(deferredStartup, this);
		XPLMUnregisterFlightLoopCallback(mainFlightLoop, this);
	}

	float XPilot::deferredStartup(float, float, int, void* ref)
	{
		auto* instance = static_cast<XPilot*>(ref);
		if (instance)
		{
			instance->initializeXPMP();
			instance->tryGetTcasControl();
			instance->StartBridgeProcess();
			XPLMRegisterFlightLoopCallback(mainFlightLoop, -1.0f, ref);
		}
		return 0;
	}

	void XPilot::StopXplaneBridgeProcess()
	{
		try
		{
			if (bridgeProcess.running())
			{
				bridgeProcess.terminate();
				LOG_MSG(logMSG, "XplaneBridge service stopped.");
			}

			if (svcThread)
			{
				svcThread->join();
				svcThread.reset();
			}
		}
		catch (std::exception const& ex)
		{
			LOG_MSG(logERROR, "Error stopping XplaneBridge service: %s", ex.what());
		}
	}

	void XPilot::StartBridgeProcess()
	{
		try
		{
			if (svcThread)
			{
				svcThread->join();
				svcThread.reset();
			}

			if (bridgeProcess.running())
			{
				// this should never happen... but just in case
				bridgeProcess.terminate();
			}

			#ifdef WIN32
			bridgeProcess = bp::child(GetPluginPath() + "\\Resources\\XplaneBridge\\XplaneBridge.exe", bp::std_out > out, bp::std_in < in, bp::windows::hide);
			#else
			bridgeProcess = bp::child(GetPluginPath() + "/Resources/XplaneBridge/XplaneBridge", bp::std_out > out, bp::std_in < in);
			#endif

			if (bridgeProcess.running())
			{
				LOG_MSG(logMSG, "XplaneBridge process started");
			}

			svcThread = std::make_unique<std::thread>([&]
			{
				std::string line;
				while (bridgeProcess.running() && std::getline(out, line) && !line.empty())
				{
					ProcessClientEvent(line);
				}
			});
		}
		catch (std::exception const& ex)
		{
			LOG_MSG(logERROR, "Error starting XplaneBridge: %s", ex.what());
		}
	}

	void XPilot::ProcessClientEvent(const std::string& data)
	{
		xpilot::Envelope envelope;
		envelope.ParseFromString(base64::base64_decode(data));

		if (envelope.has_app_metdata())
		{
			xpilot::Envelope reply{};
			xpilot::AppMetadata* msg = new xpilot::AppMetadata();
			reply.set_allocated_app_metdata(msg);
			msg->set_version(PLUGIN_VERSION);
			msg->set_plugin_hash(m_pluginHash.c_str());

			SendClientEvent(reply);
		}

		if (envelope.has_csl_validation())
		{
			xpilot::Envelope reply{};
			xpilot::CslValidation* msg = new xpilot::CslValidation();
			reply.set_allocated_csl_validation(msg);
			msg->set_is_valid(Config::Instance().hasValidPaths() && XPMPGetNumberOfInstalledModels() > 0);

			SendClientEvent(reply);
		}

		if (envelope.has_add_plane())
		{
			xpilot::AddPlane msg = envelope.add_plane();
			if (msg.has_callsign() && msg.has_visual_state() && msg.has_equipment())
			{
				AircraftVisualState visualState{};
				visualState.Lat = msg.visual_state().latitude();
				visualState.Lon = msg.visual_state().longitude();
				visualState.Altitude = msg.visual_state().altitude();
				visualState.Heading = msg.visual_state().heading();
				visualState.Pitch = msg.visual_state().pitch();
				visualState.Bank = msg.visual_state().bank();

				queueCallback([=]()
				{
					m_aircraftManager->SetUpNewPlane(msg.callsign(), visualState, msg.equipment(), msg.airline());
				});
			}
		}

		if (envelope.has_change_plane_model())
		{
			xpilot::ChangePlaneModel msg = envelope.change_plane_model();
			if (msg.has_callsign() && msg.has_equipment())
			{
				queueCallback([=]()
				{
					m_aircraftManager->ChangeAircraftModel(msg.callsign(), msg.equipment(), msg.airline());
				});
			}
		}

		if (envelope.has_position_update())
		{
			xpilot::PositionUpdate msg = envelope.position_update();

			AircraftVisualState visualState{};
			visualState.Lat = msg.latitude();
			visualState.Lon = msg.longitude();
			visualState.Altitude = msg.altitude();
			visualState.Pitch = msg.pitch();
			visualState.Bank = msg.bank();
			visualState.Heading = msg.heading();

			if (msg.has_callsign())
			{
				queueCallback([=]()
				{
					m_aircraftManager->HandleSlowPositionUpdate(msg.callsign(), visualState, msg.ground_speed());
				});
			}
		}

		if (envelope.has_fast_position_update())
		{
			xpilot::FastPositionUpdate msg = envelope.fast_position_update();

			AircraftVisualState visualState{};
			visualState.Lat = msg.latitude();
			visualState.Lon = msg.longitude();
			visualState.Altitude = msg.altitude();
			visualState.Pitch = msg.pitch();
			visualState.Bank = msg.bank();
			visualState.Heading = msg.heading();

			Vector3 positionalVector{};
			positionalVector.X = msg.velocity_longitude();
			positionalVector.Y = msg.velocity_altitude();
			positionalVector.Z = msg.velocity_latitude();

			Vector3 rotationalVector{};
			rotationalVector.X = msg.velocity_pitch() * -1;
			rotationalVector.Y = msg.velocity_heading();
			rotationalVector.Z = msg.velocity_bank() * -1;

			if (msg.has_callsign())
			{
				queueCallback([=]()
				{
					m_aircraftManager->HandleFastPositionUpdate(
						msg.callsign(),
						visualState,
						positionalVector,
						rotationalVector
					);
				});
			}
		}

		if (envelope.has_airplane_config())
		{
			xpilot::AirplaneConfig msg = envelope.airplane_config();
			if (msg.has_callsign())
			{
				queueCallback([=]()
				{
					m_aircraftManager->UpdateAircraftConfiguration(msg);
				});
			}
		}

		if (envelope.has_delete_plane())
		{
			xpilot::DeletePlane msg = envelope.delete_plane();
			if (msg.has_callsign())
			{
				queueCallback([=]()
				{
					m_aircraftManager->DeleteAircraft(msg.callsign());
				});
			}
		}

		if (envelope.has_network_connected())
		{
			if (envelope.network_connected().has_callsign())
			{
				m_networkCallsign = envelope.network_connected().callsign();
			}
			queueCallback([=]()
			{
				onNetworkConnected();
			});
		}

		if (envelope.has_network_disconnected())
		{
			queueCallback([=]()
			{
				onNetworkDisconnected();
			});
		}

		if (envelope.has_nearby_controllers())
		{
			queueCallback([=]()
			{
				if (envelope.nearby_controllers().list_size() > 0)
				{
					m_nearbyAtcWindow->UpdateList(envelope.nearby_controllers());
				}
				else
				{
					m_nearbyAtcWindow->ClearList();
				}
			});
		}

		if (envelope.has_radio_message_received())
		{
			xpilot::RadioMessageReceived msg = envelope.radio_message_received();
			RadioMessageReceived(msg.message(), 255, 255, 255);
			AddNotificationPanelMessage(msg.message(), 255, 255, 255);
		}

		if (envelope.has_private_message_received())
		{
			xpilot::PrivateMessageReceived msg = envelope.private_message_received();
			AddPrivateMessage(msg.from(), msg.message(), ConsoleTabType::Received);
			AddNotificationPanelMessage(string_format("%s [pvt]:  %s", msg.from(), msg.message().c_str()), 255, 255, 255);
		}

		if (envelope.has_private_message_sent())
		{
			xpilot::PrivateMessageSent msg = envelope.private_message_sent();
			AddPrivateMessage(msg.to(), msg.message(), ConsoleTabType::Sent);
			AddNotificationPanelMessage(string_format("%s [pvt: %s]:  %s", m_networkCallsign.value().c_str(), msg.to(), msg.message().c_str()), 0, 255, 255);
		}
	}

	void XPilot::SendClientEvent(const xpilot::Envelope& envelope)
	{
		std::string data;
		envelope.SerializeToString(&data);

		in << base64::base64_encode(data).c_str() << std::endl;
	}

	float XPilot::mainFlightLoop(float inElapsedSinceLastCall, float, int, void* ref)
	{
		auto* instance = static_cast<XPilot*>(ref);
		if (instance)
		{
			instance->invokeQueuedCallbacks();
			instance->m_aiControlled = XPMPHasControlOfAIAircraft();
			instance->m_aircraftCount = XPMPCountPlanes();
			UpdateMenuItems();
		}
		return -1.0;
	}

	void XPilot::disableDefaultAtis(bool disabled)
	{
		m_xplaneAtisEnabled = (int)disabled;
	}

	int CBIntPrefsFunc(const char*, [[maybe_unused]] const char* item, int defaultVal)
	{
		if (!strcmp(item, XPMP_CFG_ITM_MODELMATCHING))
			return Config::Instance().getDebugModelMatching();
		if (!strcmp(item, XPMP_CFG_ITM_LOGLEVEL))
			return Config::Instance().getLogLevel();
		if (!strcmp(item, XPMP_CFG_ITM_REPLDATAREFS))
			return 1;
		if (!strcmp(item, XPMP_CFG_ITM_CLAMPALL))
			return 0;
		return defaultVal;
	}

	void XPilot::onNetworkConnected()
	{
		m_aircraftManager->DeleteAllAircraft();
		m_frameRateMonitor->startMonitoring();
		m_xplaneAtisEnabled = 0;
		m_networkLoginStatus = 1;
		tryGetTcasControl();
	}

	void XPilot::onNetworkDisconnected()
	{
		m_aircraftManager->DeleteAllAircraft();
		m_frameRateMonitor->stopMonitoring();
		m_xplaneAtisEnabled = 1;
		m_networkLoginStatus = 0;
		m_networkCallsign = "";
		releaseTcasControl();
	}

	void XPilot::forceDisconnect(std::string reason)
	{
		xpilot::Envelope envelope;
		xpilot::TriggerDisconnect* data = new xpilot::TriggerDisconnect();
		envelope.set_allocated_trigger_disconnect(data);
		data->set_reason(reason);
		SendClientEvent(envelope);
	}

	void XPilot::requestStationInfo(std::string callsign)
	{
		xpilot::Envelope envelope;
		xpilot::RequestStationInfo* data = new xpilot::RequestStationInfo();
		envelope.set_allocated_request_station_info(data);
		data->set_station(callsign);
		SendClientEvent(envelope);
	}

	bool XPilot::initializeXPMP()
	{
		const std::string pathResources(GetPluginPath() + "Resources");

		auto err = XPMPMultiplayerInit(PLUGIN_NAME, pathResources.c_str(), &CBIntPrefsFunc);

		if (*err)
		{
			LOG_MSG(logERROR, "Error initializing multiplayer: %s", err);
			XPMPMultiplayerCleanup();
			return false;
		}

		if (!Config::Instance().hasValidPaths())
		{
			std::string err = "No valid CSL paths are configured or the paths are disabled. Verify the CSL configuration in X-Plane (Plugins > xPilot > Settings > CSL Packages).";
			addNotification(err.c_str(), 192, 57, 43);
			LOG_MSG(logERROR, err.c_str());
		}
		else
		{
			for (const CslPackage& p : Config::Instance().getCSLPackages())
			{
				if (!p.path.empty() && p.enabled && CountFilesInPath(p.path) > 0)
				{
					try
					{
						err = XPMPLoadCSLPackage(p.path.c_str());
						if (*err)
						{
							LOG_MSG(logERROR, "Error loading CSL package %s: %s", p.path.c_str(), err);
						}
					}
					catch (std::exception& e)
					{
						LOG_MSG(logERROR, "Error loading CSL package %s: %s", p.path.c_str(), err);
					}
				}
			}
		}

		XPMPEnableAircraftLabels(Config::Instance().getShowHideLabels());
		XPMPSetAircraftLabelDist(Config::Instance().getMaxLabelDistance(), Config::Instance().getLabelCutoffVis());
		return true;
	}

	void XPilot::AddPrivateMessage(const std::string& recipient, const std::string& msg, ConsoleTabType tabType)
	{
		if (!recipient.empty() && !msg.empty())
		{
			queueCallback([=]()
			{
				m_textMessageConsole->PrivateMessageReceived(recipient, msg, tabType);
			});
		}
	}

	void XPilot::RadioMessageReceived(const std::string& msg, double red, double green, double blue)
	{
		if (!msg.empty())
		{
			queueCallback([=]()
			{
				m_textMessageConsole->RadioMessageReceived(msg.c_str(), red, green, blue);
			});
		}
	}

	void XPilot::AddNotificationPanelMessage(const std::string& msg, double red, double green, double blue)
	{
		if (!msg.empty())
		{
			queueCallback([=]()
			{
				m_notificationPanel->AddNotificationPanelMessage(msg, red, green, blue);
			});
		}
	}

	void XPilot::addNotification(const std::string& msg, double red, double green, double blue)
	{
		RadioMessageReceived(msg, red, green, blue);
		AddNotificationPanelMessage(msg, red, green, blue);
	}

	void XPilot::queueCallback(const std::function<void()> &cb)
	{
		std::lock_guard<std::mutex> lck(m_mutex);
		m_queuedCallbacks.push_back(cb);
	}

	void XPilot::invokeQueuedCallbacks()
	{
		std::deque<std::function<void()>> temp;
		{
			std::lock_guard<std::mutex> lck(m_mutex);
			std::swap(temp, m_queuedCallbacks);
		}
		while (!temp.empty())
		{
			auto cb = temp.front();
			temp.pop_front();
			cb();
		}
	}

	void XPilot::togglePreferencesWindow()
	{
		m_settingsWindow->SetVisible(!m_settingsWindow->GetVisible());
	}

	void XPilot::toggleNearbyAtcWindow()
	{
		m_nearbyAtcWindow->SetVisible(!m_nearbyAtcWindow->GetVisible());
	}

	void XPilot::toggleTextMessageConsole()
	{
		m_textMessageConsole->SetVisible(!m_textMessageConsole->GetVisible());
	}

	void XPilot::setNotificationPanelAlwaysVisible(bool visible)
	{
		m_notificationPanel->setAlwaysVisible(visible);
	}

	bool XPilot::setNotificationPanelAlwaysVisible()const
	{
		return m_notificationPanel->isAlwaysVisible();
	}

	void callbackRequestTcasAgain(void*)
	{
		XPMPMultiplayerEnable(callbackRequestTcasAgain);
	}

	void XPilot::tryGetTcasControl()
	{
		if (!XPMPHasControlOfAIAircraft())
		{
			auto err = XPMPMultiplayerEnable(callbackRequestTcasAgain);
			if (*err)
			{
				addNotification(err, 231, 76, 60);
				LOG_MSG(logERROR, err);
			}
		}
	}

	void XPilot::releaseTcasControl()
	{
		if (XPMPHasControlOfAIAircraft())
		{
			XPMPMultiplayerDisable();
			LOG_MSG(logDEBUG, "xPilot has released TCAS control");
		}
	}

	int XPilot::getBulkData(void* inRefcon, void* outData, int inStartPos, int inNumBytes)
	{
		dataRefs dr = (dataRefs)reinterpret_cast<long long>(inRefcon);
		assert(dr == DR_BULK_QUICK || dr == DR_BULK_EXPENSIVE);

		static int size_quick = 0, size_expensive = 0;
		if (!outData)
		{
			if (dr == DR_BULK_QUICK)
			{
				size_quick = inNumBytes;
				return (int)sizeof(XPilotAPIAircraft::XPilotAPIBulkData);
			}
			else
			{
				size_expensive = inNumBytes;
				return (int)sizeof(XPilotAPIAircraft::XPilotAPIBulkInfoTexts);
			}
		}

		int size = dr == DR_BULK_QUICK ? size_quick : size_expensive;
		if (!size) return 0;

		if ((inStartPos % size != 0) ||
			(inNumBytes % size != 0))
			return 0;

		const int startAc = 1 + inStartPos / size;
		const int endAc = startAc + (inNumBytes / size);
		char* pOut = (char*)outData;
		int iAc = startAc;
		for (mapPlanesTy::iterator pIter = mapGetAircraftByIndex(iAc);
			pIter != mapPlanes.end() && iAc < endAc;
			pIter = mapGetNextAircraft(pIter), iAc++, pOut += size)
		{
			const NetworkAircraft& ac = *pIter->second;
			if (dr == DR_BULK_QUICK)
				ac.copyBulkData((XPilotAPIAircraft::XPilotAPIBulkData*)pOut, size);
			else
				ac.copyBulkData((XPilotAPIAircraft::XPilotAPIBulkInfoTexts*)pOut, size);
		}

		return (iAc - startAc) * size;
	}
}