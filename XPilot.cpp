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
#include "NetworkAircraftConfig.h"
#include "FrameRateMonitor.h"
#include "NearbyATCWindow.h"
#include "PreferencesWindow.h"
#include "NotificationPanel.h"
#include "TextMessageConsole.h"
#include "sha512.hh"
#include "Lib/json.hpp"

using json = nlohmann::json;

namespace xpilot
{
	void callbackPlaneNotifier(XPMPPlaneID inPlaneID, XPMPPlaneNotification inNotification, void* inRefcon)
	{
		auto* env = static_cast<XPilot*>(inRefcon);
		XPMP2::Aircraft* pAc = XPMP2::AcFindByID(inPlaneID);
		if (pAc)
		{
			switch (inNotification)
			{
				case xpmp_PlaneNotification_Created:
					env->incrementAircraftCount();
					break;
				case xpmp_PlaneNotification_Destroyed:
					env->decrementAircraftCount();
					break;
			}
		}
	}

	XPilot::XPilot() :
		m_xplaneAtisEnabled("sim/atc/atis_enabled", ReadWrite),
		m_pttPressed("xpilot/ptt", ReadWrite),
		m_rxCom1("xpilot/audio/com1_rx", ReadWrite),
		m_rxCom2("xpilot/audio/com2_rx", ReadWrite),
		m_networkLoginStatus("xpilot/login/status", ReadOnly, true),
		m_networkCallsign("xpilot/login/callsign", ReadOnly, true),
		m_volumeSignalLevel("xpilot/audio/vu", ReadWrite),
		m_aiControlled("xpilot/ai_controlled", ReadOnly),
		m_aircraftCount("xpilot/num_aircraft", ReadOnly),
		m_pluginVersion("xpilot/version", ReadOnly)
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
		m_preferencesWindow = std::make_unique<PreferencesWindow>();
		m_frameRateMonitor = std::make_unique<FrameRateMonitor>(this);
		m_aircraftManager = std::make_unique<AircraftManager>();
		pluginHash = sw::sha512::file(GetTruePluginPath().c_str());

		XPMPRegisterPlaneNotifierFunc(callbackPlaneNotifier, this);
		XPLMRegisterFlightLoopCallback(deferredStartup, -1.0f, this);
	}

	XPilot::~XPilot()
	{
		XPLMUnregisterDataAccessor(m_bulkDataQuick);
		XPLMUnregisterDataAccessor(m_bulkDataExpensive);
		XPMPUnregisterPlaneNotifierFunc(callbackPlaneNotifier, this);
		XPLMUnregisterFlightLoopCallback(deferredStartup, this);
		XPLMUnregisterFlightLoopCallback(onFlightLoop, this);
	}

	float XPilot::deferredStartup(float, float, int, void* ref)
	{
		auto* instance = static_cast<XPilot*>(ref);
		if (instance)
		{
			instance->startZmqServer();
		}
		return 0;
	}

	void XPilot::startZmqServer()
	{
		LOG_INFO("Initializing xPilot... Version %s", PLUGIN_VERSION_STRING);

		initializeXPMP();

		if (m_zmqThread)
		{
			m_keepAlive = false;
			m_zmqThread->join();
			m_zmqThread.reset();
		}

		try
		{
			m_zmqContext = std::make_unique<zmq::context_t>(1);
			m_zmqSocket = std::make_unique<zmq::socket_t>(*m_zmqContext.get(), ZMQ_ROUTER);
			m_zmqSocket->setsockopt(ZMQ_IDENTITY, "PLUGIN", 6);
			m_zmqSocket->setsockopt(ZMQ_LINGER, 0);
			m_zmqSocket->bind("tcp://*:" + std::to_string(Config::Instance().getTcpPort()));
		}
		catch (zmq::error_t& e)
		{
			LOG_ERROR("Error binding TCP socket port: %s", e.what());
			addNotification(string_format("Error binding TCP socket port: %s", e.what()));
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("Error binding TCP socket port: %s", e.what());
			addNotification(string_format("Error binding TCP socket port: %s", e.what()));
		}
		catch (...)
		{
		}

		XPLMRegisterFlightLoopCallback(onFlightLoop, -1.0f, this);

		m_keepAlive = true;
		m_zmqThread = std::make_unique<std::thread>(&XPilot::zmqWorker, this);
		LOG_INFO("TCP socket thread started on port %i", Config::Instance().getTcpPort());
		addNotification(string_format("TCP socket thread started on port %i", Config::Instance().getTcpPort()));
	}

	void XPilot::stopZmqServer()
	{
		try
		{
			if (m_zmqSocket)
			{
				m_zmqSocket->close();
				m_zmqContext->close();
			}
		}
		catch (zmq::error_t& e)
		{
			LOG_ERROR("Error closing TCP socket: %s", e.what());
		}
		catch (std::exception& e)
		{
			LOG_ERROR("Unknown TCP socket exception: %s", e.what());
		}
		catch (...)
		{
		}

		m_keepAlive = false;

		if (m_zmqThread)
		{
			m_zmqThread->join();
			m_zmqThread.reset();
			LOG_INFO("TCP server successfully stopped");
		}
	}

	void XPilot::sendSocketMsg(const std::string& msg)
	{
		try
		{
			if (isSocketConnected() && !msg.empty())
			{
				std::string identity = "CLIENT";
				zmq::message_t msg1(identity.size());
				std::memcpy(msg1.data(), identity.data(), identity.size());
				m_zmqSocket->send(msg1, zmq::send_flags::sndmore);

				zmq::message_t message(msg.size());
				std::memcpy(message.data(), msg.data(), msg.size());
				m_zmqSocket->send(message, ZMQ_NOBLOCK);
			}
		}
		catch (zmq::error_t& e)
		{
			LOG_ERROR("Error sending socket message: %s", e.what());
		}
		catch (...)
		{
		}
	}

	float XPilot::onFlightLoop(float, float, int, void* ref)
	{
		auto* instance = static_cast<XPilot*>(ref);
		if (instance)
		{
			instance->invokeQueuedCallbacks();
			instance->m_aiControlled = XPMPHasControlOfAIAircraft();
			instance->m_aircraftManager->interpolateAirplanes();
			UpdateMenuItems();
		}
		return -1.0;
	}

	void XPilot::zmqWorker()
	{
		while (isSocketReady())
		{
			try
			{
				zmq::message_t msg;
				m_zmqSocket->recv(msg, zmq::recv_flags::none);
				std::string data(static_cast<char*>(msg.data()), msg.size());

				if (!data.empty())
				{
					if (json::accept(data.c_str()))
					{
						json j = json::parse(data.c_str());

						if (j.find("Type") != j.end())
						{
							std::string type(j["Type"]);

							if (!type.empty())
							{
								if (type == "AddPlane")
								{
									std::string callsign(j["Data"]["Callsign"]);
									std::string airline(j["Data"]["Airline"]);
									std::string typeCode(j["Data"]["TypeCode"]);

									if (!callsign.empty() && !typeCode.empty())
									{
										queueCallback([=]()
										{
											m_aircraftManager->addNewPlane(callsign, typeCode, airline);
										});
									}
								}

								else if (type == "ChangeModel")
								{
									std::string callsign(j["Data"]["Callsign"]);
									std::string airline(j["Data"]["Airline"]);
									std::string typeCode(j["Data"]["TypeCode"]);

									if (!callsign.empty() && !typeCode.empty())
									{
										queueCallback([=]()
										{
											m_aircraftManager->changeModel(callsign, typeCode, airline);
										});
									}
								}

								else if (type == "PositionUpdate")
								{
									std::string callsign(j["Data"]["Callsign"]);

									XPMPPlanePosition_t pos;
									pos.lat = static_cast<double>(j["Data"]["Latitude"]);
									pos.lon = static_cast<double>(j["Data"]["Longitude"]);
									pos.elevation = static_cast<double>(j["Data"]["Altitude"]);
									pos.heading = static_cast<float>(j["Data"]["Heading"]);
									pos.pitch = static_cast<float>(j["Data"]["Pitch"]);
									pos.roll = static_cast<float>(j["Data"]["Bank"]);
									float gs = static_cast<float>(j["Data"]["GroundSpeed"]);

									XPMPPlaneRadar_t radar;
									radar.code = static_cast<int>(j["Data"]["TransponderCode"]);
									radar.mode = static_cast<bool>(j["Data"]["TransponderModeC"]) ? xpmpTransponderMode_ModeC : xpmpTransponderMode_Standby;

									std::string origin(j["Data"]["Origin"]);
									std::string destination(j["Data"]["Destination"]);

									if (!callsign.empty())
									{
										queueCallback([=]()
										{
											m_aircraftManager->setPlanePosition(callsign, pos, radar, gs, origin, destination);
										});
									}
								}

								else if (type == "SurfaceUpdate")
								{
									auto acconfig = j.get<NetworkAircraftConfig>();
									queueCallback([=]()
									{
										m_aircraftManager->updateAircraftConfig(acconfig.data.callsign, acconfig);
									});
								}

								else if (type == "RemovePlane")
								{
									std::string callsign(j["Data"]["Callsign"]);
									if (!callsign.empty())
									{
										queueCallback([=]()
										{
											m_aircraftManager->removePlane(callsign);
										});
									}
								}

								else if (type == "RemoveAllPlanes")
								{
									queueCallback([=]()
									{
										m_aircraftManager->removeAllPlanes();
									});
								}

								else if (type == "NetworkConnected")
								{
									m_networkCallsign = j["Data"]["OurCallsign"];
									queueCallback([=]()
									{
										onNetworkConnected();
									});
								}

								else if (type == "NetworkDisconnected")
								{
									m_networkCallsign = "";
									queueCallback([=]()
									{
										onNetworkDisconnected();
									});
								}

								else if (type == "WhosOnline")
								{
									queueCallback([=]()
									{
										m_nearbyAtcWindow->UpdateList(j);
									});
								}

								else if (type == "PluginVersion")
								{
									json reply;
									reply["Type"] = "PluginVersion";
									reply["Timestamp"] = UtcTimestamp();
									reply["Data"]["Version"] = PLUGIN_VERSION;
									sendSocketMsg(reply.dump());
								}

								else if (type == "PluginHash")
								{
									json j;
									j["Type"] = "PluginHash";
									j["Data"]["Hash"] = pluginHash;
									j["Timestamp"] = UtcTimestamp();
									sendSocketMsg(j.dump());
								}

								else if (type == "RadioMessage")
								{
									std::string msg(j["Data"]["Message"]);

									int red = static_cast<int>(j["Data"]["R"]);
									int green = static_cast<int>(j["Data"]["G"]);
									int blue = static_cast<int>(j["Data"]["B"]);
									bool direct = static_cast<bool>(j["Data"]["Direct"]);

									addNotification(msg, red, green, blue);
								}

								else if (type == "PrivateMessageReceived")
								{
									std::string msg(j["Data"]["Message"]);
									std::string from(j["Data"]["From"]);

									addConsoleMessageTab(from, msg, ConsoleTabType::Incoming);
									addNotificationPanelMessage(string_format("%s [pvt]:  %s", from, msg.c_str()), 230, 94, 230);
								}

								else if (type == "PrivateMessageSent")
								{
									std::string msg(j["Data"]["Message"]);
									std::string from(j["Data"]["To"]);

									addConsoleMessageTab(from, msg, ConsoleTabType::Outgoing);
									addNotificationPanelMessage(string_format("%s [pvt: %s]:  %s", m_networkCallsign.value().c_str(), from.c_str(), msg.c_str()), 50, 205, 50);
								}

								else if (type == "ValidateCslPaths")
								{
									json j;
									j["Type"] = "ValidateCslPaths";
									j["Data"]["Result"] = Config::Instance().hasValidPaths() && XPMPGetNumberOfInstalledModels() > 0;
									j["Timestamp"] = UtcTimestamp();
									sendSocketMsg(j.dump());
								}
							}
						}
					}
				}
			}
			catch (zmq::error_t& e)
			{
				if (e.num() != ETERM)
				{
					LOG_ERROR("TCP socket exception: %s", e.what());
				}
			}
			catch (std::exception& e)
			{
				LOG_ERROR("TCP socket exception: %s", e.what());
			}
			catch (...)
			{
			}
		}
	}

	void XPilot::disableDefaultAtis(bool disabled)
	{
		m_xplaneAtisEnabled = (int)disabled;
	}

	int CBIntPrefsFunc(const char*, [[maybe_unused]] const char* item, int defaultVal)
	{
		if (!strcmp(item, "model_matching"))
			return Config::Instance().getDebugModelMatching();
		if (!strcmp(item, "log_level"))
			return 0;
		return defaultVal;
	}

	void XPilot::onNetworkConnected()
	{
		m_aircraftManager->removeAllPlanes();
		m_frameRateMonitor->startMonitoring();
		tryGetTcasControl();
		m_xplaneAtisEnabled = 0;
		m_networkLoginStatus = 1;
	}

	void XPilot::onNetworkDisconnected()
	{
		m_aircraftManager->removeAllPlanes();
		m_frameRateMonitor->stopMonitoring();
		releaseTcasControl();
		m_xplaneAtisEnabled = 1;
		m_networkLoginStatus = 0;
	}

	void XPilot::forceDisconnect(std::string reason)
	{
		json j;
		j["Type"] = "ForceDisconnect";
		j["Data"]["Reason"] = reason;
		j["Timestamp"] = UtcTimestamp();
		sendSocketMsg(j.dump());
	}

	void XPilot::onPluginDisabled()
	{
		json j;
		j["Type"] = "PluginDisabled";
		j["Timestamp"] = UtcTimestamp();
		sendSocketMsg(j.dump());
	}

	void XPilot::requestControllerAtis(std::string callsign)
	{
		try
		{
			addNotification(string_format("Requesting controller ATIS for: %s", callsign), 149, 165, 166);
			LOG_INFO("Requesting controller ATIS for %s", callsign.c_str());

			json j;
			j["Type"] = "RequestAtis";
			j["Timestamp"] = UtcTimestamp();
			j["Data"]["Callsign"] = callsign;

			sendSocketMsg(j.dump());
		}
		catch (zmq::error_t& e)
		{
			LOG_ERROR("Error requesting controller ATIS for %s: %s", callsign.c_str(), e.what());
		}
		catch (std::exception& e)
		{
			LOG_ERROR("Error requesting controller ATIS for %s: %s", callsign.c_str(), e.what());
		}
		catch (...)
		{
		}
	}

	bool XPilot::initializeXPMP()
	{
		const std::string pathResources(GetPluginPath() + "Resources");
		LOG_INFO("Resources path initialized: %s", pathResources.c_str());

		auto err = XPMPMultiplayerInit(PLUGIN_NAME, pathResources.c_str(), &CBIntPrefsFunc);

		if (*err)
		{
			LOG_ERROR("Error initializing XPMP2: %s", err);
			XPMPMultiplayerCleanup();
			return false;
		}

		if (!Config::Instance().hasValidPaths())
		{
			std::string err = "No valid CSL paths are configured or the paths are disabled. Verify the CSL configuration in X-Plane (Plugins > xPilot > Settings > CSL Packages).";
			addNotification(err.c_str(), 192, 57, 43);
			LOG_ERROR(err.c_str());
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
							addNotification(string_format("Error loading CSL package %s: %s", p.path.c_str(), err), 231, 76, 60);
							LOG_ERROR("Error loading CSL package %s: %s", p.path.c_str(), err);
						}
					}
					catch (std::exception& e)
					{
						LOG_ERROR("Error loading CSL package %s: %s", p.path.c_str(), err);
					}
				}
			}
		}

		XPMPEnableAircraftLabels(Config::Instance().getShowHideLabels());
		XPMPSetAircraftLabelDist(Config::Instance().getMaxLabelDistance(), Config::Instance().getLabelCutoffVis());
		LOG_INFO("XPMP2 successfully initialized");
		return true;
	}

	void XPilot::addConsoleMessageTab(const std::string& recipient, const std::string& msg, ConsoleTabType tabType)
	{
		if (!recipient.empty() && !msg.empty())
		{
			queueCallback([=]()
			{
				m_textMessageConsole->addMessageToTab(recipient, msg, tabType);
			});
		}
	}

	void XPilot::addConsoleMessage(const std::string& msg, double red, double green, double blue)
	{
		if (!msg.empty())
		{
			queueCallback([=]()
			{
				m_textMessageConsole->addIncomingMessage(msg.c_str(), red, green, blue);
			});
		}
	}

	void XPilot::addNotificationPanelMessage(const std::string& msg, double red, double green, double blue)
	{
		if (!msg.empty())
		{
			queueCallback([=]()
			{
				m_notificationPanel->addNotificationPanelMessage(msg, red, green, blue);
			});
		}
	}

	void XPilot::addNotification(const std::string& msg, double red, double green, double blue)
	{
		addConsoleMessage(msg, red, green, blue);
		addNotificationPanelMessage(msg, red, green, blue);
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
		m_preferencesWindow->SetVisible(!m_preferencesWindow->GetVisible());
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

	void XPilot::incrementAircraftCount()
	{
		++m_currentAircraftCount;
		m_aircraftCount = m_currentAircraftCount;
	}

	void XPilot::decrementAircraftCount()
	{
		--m_currentAircraftCount;
		m_aircraftCount = m_currentAircraftCount;
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
				LOG_ERROR(err);
			}
			else
			{
				addNotification("xPilot has TCAS control");
				LOG_INFO("xPilot has TCAS control");
			}
		}
	}

	void XPilot::releaseTcasControl()
	{
		if (XPMPHasControlOfAIAircraft())
		{
			XPMPMultiplayerDisable();
			addNotification("xPilot released TCAS control");
			LOG_INFO("xPilot released TCAS control");
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