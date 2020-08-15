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
#include "Lib/json.hpp"
#include "sha512.hh"

using json = nlohmann::json;

namespace xpilot {

	void CBPlaneNotifier(XPMPPlaneID inPlaneID, XPMPPlaneNotification inNotification, void* inRefcon)
	{
		auto* env = static_cast<XPilot*>(inRefcon);
		XPMP2::Aircraft* pAc = XPMP2::AcFindByID(inPlaneID);
		if (pAc) {
			switch (inNotification) {
			case xpmp_PlaneNotification_Created:
				env->IncNumAc();
				break;
			case xpmp_PlaneNotification_Destroyed:
				env->DecNumAc();
				break;
			}
		}
	}

	XPilot::XPilot() :
		xplaneAtisEnabled("sim/atc/atis_enabled", ReadWrite),
		pttPressed("xpilot/ptt", ReadWrite),
		rxCom1("xpilot/audio/com1_rx", ReadWrite),
		rxCom2("xpilot/audio/com2_rx", ReadWrite),
		networkLoginStatus("xpilot/login/status", ReadOnly, true),
		networkCallsign("xpilot/login/callsign", ReadOnly, true),
		volumeSignalLevel("xpilot/audio/vu", ReadWrite),
		aiControlled("xpilot/ai_controlled", ReadOnly),
		numAircraft("xpilot/num_aircraft", ReadOnly),
		pluginVersion("xpilot/version", ReadOnly)
	{
		ThisThreadIsXP();

		bulkDataQuick = XPLMRegisterDataAccessor("xpilot/bulk/quick",
			xplmType_Data,
			false,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL, NULL,
			NULL, NULL,
			NULL, NULL,
			GetBulkData,
			NULL,
			(void*)DR_BULK_QUICK,
			(void*)DR_BULK_QUICK
		);

		bulkDataExpensive = XPLMRegisterDataAccessor("xpilot/bulk/expensive",
			xplmType_Data,
			false,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL, NULL,
			NULL, NULL,
			NULL, NULL,
			GetBulkData,
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
		notificationPanel = std::make_unique<NotificationPanel>(left, top, right, bottom);
		textMessageConsole = std::make_unique<TextMessageConsole>(this);
		nearbyAtcWindow = std::make_unique<NearbyATCWindow>(this);
		preferencesWindow = std::make_unique<PreferencesWindow>();
		frameRateMonitor = std::make_unique<FrameRateMonitor>(this);
		aircraftManager = std::make_unique<AircraftManager>();
		pluginHash = sw::sha512::file(GetTruePluginPath().c_str());

		XPMPRegisterPlaneNotifierFunc(CBPlaneNotifier, this);
		XPLMRegisterFlightLoopCallback(deferredStartup, -1.0f, this);
	}

	XPilot::~XPilot()
	{
		XPLMUnregisterDataAccessor(bulkDataQuick);
		XPLMUnregisterDataAccessor(bulkDataExpensive);
		XPMPUnregisterPlaneNotifierFunc(CBPlaneNotifier, this);
		XPLMUnregisterFlightLoopCallback(deferredStartup, this);
		XPLMUnregisterFlightLoopCallback(onFlightLoop, this);
	}

	float XPilot::deferredStartup(float, float, int, void* ref)
	{
		auto* instance = static_cast<XPilot*>(ref);
		if (instance)
		{
			instance->StartZmqServer();
		}
		return 0;
	}

	void XPilot::StartZmqServer()
	{
		LOG_INFO("Initializing xPilot... Version %s", PLUGIN_VERSION_STRING);
		
		InitializeXPMP();

		if (zmqThread)
		{
			keepAlive = false;
			zmqThread->join();
			zmqThread.reset();
		}

		try
		{
			zmqContext = std::make_unique<zmq::context_t>(1);
			zmqSocket = std::make_unique<zmq::socket_t>(*zmqContext.get(), ZMQ_ROUTER);
			zmqSocket->setsockopt(ZMQ_IDENTITY, "PLUGIN", 6);
			zmqSocket->setsockopt(ZMQ_LINGER, 0);
			zmqSocket->bind("tcp://*:" + std::to_string(Config::Instance().GetTcpPort()));
		}
		catch (zmq::error_t& e)
		{
			LOG_ERROR("Error binding TCP socket port: %s", e.what());
			AddNotification(string_format("Error binding TCP socket port: %s", e.what()));
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("Error binding TCP socket port: %s", e.what());
			AddNotification(string_format("Error binding TCP socket port: %s", e.what()));
		}
		catch (...) {}

		XPLMRegisterFlightLoopCallback(onFlightLoop, -1.0f, this);

		keepAlive = true;
		zmqThread = std::make_unique<std::thread>(&XPilot::ZmqListener, this);
		LOG_INFO("TCP socket thread started on port %i", Config::Instance().GetTcpPort());
		AddNotification(string_format("TCP socket thread started on port %i", Config::Instance().GetTcpPort()));
	}

	void XPilot::StopZmqServer()
	{
		try
		{
			if (zmqSocket)
			{
				zmqSocket->close();
				zmqContext->close();
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
		catch (...) {}

		keepAlive = false;
		if (zmqThread)
		{
			zmqThread->join();
			zmqThread.reset();
			LOG_INFO("TCP server successfully stopped");
		}
	}

	void XPilot::SendSocketMsg(const std::string& msg)
	{
		try
		{
			if (IsSocketConnected() && !msg.empty())
			{
				std::string identity = "CLIENT";
				zmq::message_t msg1(identity.size());
				std::memcpy(msg1.data(), identity.data(), identity.size());
				zmqSocket->send(msg1, zmq::send_flags::sndmore);

				zmq::message_t message(msg.size());
				std::memcpy(message.data(), msg.data(), msg.size());
				zmqSocket->send(message, ZMQ_NOBLOCK);
			}
		}
		catch (zmq::error_t& e)
		{
			LOG_ERROR("Error sending socket message: %s", e.what());
		}
		catch (...) {}
	}

	float XPilot::onFlightLoop(float, float, int, void* ref)
	{
		auto* instance = static_cast<XPilot*>(ref);
		if (instance)
		{
			instance->aiControlled = XPMPHasControlOfAIAircraft();
			instance->aircraftManager->InterpolateAirplanes();
			instance->ProcessQueuedCallbacks();
			UpdateMenuItems();
		}
		return -1.0;
	}

	void XPilot::ZmqListener()
	{
		while (IsSocketReady())
		{
			try
			{
				zmq::message_t msg;
				zmqSocket->recv(msg, zmq::recv_flags::none);
				std::string data(static_cast<char*>(msg.data()), msg.size());

				if (!data.empty())
				{
					try
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
										QueueCallback([=]()
										{
											aircraftManager->AddNewPlane(callsign, typeCode, airline);
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
										QueueCallback([=]()
										{
											aircraftManager->ChangeModel(callsign, typeCode, airline);
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
										QueueCallback([=]()
										{
											aircraftManager->SetPlanePosition(callsign, pos, radar, gs);
											aircraftManager->SetFlightPlan(callsign, origin, destination);
										});
									}
								}

								else if (type == "SurfaceUpdate")
								{
									auto acconfig = j.get<NetworkAircraftConfig>();
									QueueCallback([=]()
									{
										aircraftManager->UpdateAircraftConfig(acconfig.Data.Callsign, acconfig);
									});
								}

								else if (type == "RemovePlane")
								{
									std::string callsign(j["Data"]["Callsign"]);
									if (!callsign.empty())
									{
										QueueCallback([=]()
										{
											aircraftManager->RemovePlane(callsign);
										});
									}
								}

								else if (type == "RemoveAllPlanes")
								{
									QueueCallback([=]()
									{
										aircraftManager->RemoveAllPlanes();
									});
								}

								else if (type == "NetworkConnected")
								{
									networkCallsign = j["Data"]["OurCallsign"];
									OnNetworkConnected();
								}

								else if (type == "NetworkDisconnected")
								{
									networkCallsign = "";
									OnNetworkDisconnected();
								}

								else if (type == "WhosOnline")
								{
									QueueCallback([=]()
									{
										nearbyAtcWindow->UpdateList(j);
									});
								}

								else if (type == "PluginVersion")
								{
									json reply;
									reply["Type"] = "PluginVersion";
									reply["Timestamp"] = UtcTimestamp();
									reply["Data"]["Version"] = PLUGIN_VERSION;
									SendSocketMsg(reply.dump());
								}

								else if (type == "PluginHash")
								{
									json j;
									j["Type"] = "PluginHash";
									j["Data"]["Hash"] = pluginHash;
									j["Timestamp"] = UtcTimestamp();
									SendSocketMsg(j.dump());
								}

								else if (type == "RadioMessage") {
									std::string msg(j["Data"]["Message"]);

									int red = static_cast<int>(j["Data"]["R"]);
									int green = static_cast<int>(j["Data"]["G"]);
									int blue = static_cast<int>(j["Data"]["B"]);
									bool direct = static_cast<bool>(j["Data"]["Direct"]);

									AddNotification(msg, red, green, blue);
								}

								else if (type == "PrivateMessageReceived")
								{
									std::string msg(j["Data"]["Message"]);
									std::string from(j["Data"]["From"]);

									AddConsoleMessageTab(from, msg, ConsoleTabType::Incoming);
									AddNotificationPanelMessage(string_format("%s [pvt]:  %s", from, msg.c_str()), 230, 94, 230);
								}

								else if (type == "PrivateMessageSent")
								{
									std::string msg(j["Data"]["Message"]);
									std::string from(j["Data"]["To"]);

									AddConsoleMessageTab(from, msg, ConsoleTabType::Outgoing);
									AddNotificationPanelMessage(string_format("%s [pvt: %s]:  %s", networkCallsign.value().c_str(), from.c_str(), msg.c_str()), 50, 205, 50);
								}

								else if (type == "ValidateCslPaths")
								{
									json j;
									j["Type"] = "ValidateCslPaths";
									j["Data"]["Result"] = Config::Instance().HasValidPaths() && XPMPGetNumberOfInstalledModels() > 0;
									j["Timestamp"] = UtcTimestamp();
									SendSocketMsg(j.dump());
								}
							}
						}
					}
					catch (json::exception& e) {}
					catch (std::exception& e) {}
					catch (...) {}
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
			catch (...) {}
		}
	}

	void XPilot::DisableDefaultAtis(bool disabled)
	{
		xplaneAtisEnabled = (int)disabled;
	}

	int CBIntPrefsFunc(const char*, [[maybe_unused]] const char* item, int defaultVal)
	{
		if (!strcmp(item, "model_matching"))
			return Config::Instance().GetDebugModelMatching();
		if (!strcmp(item, "log_level"))
			return 0;
		return defaultVal;
	}

	void XPilot::OnNetworkConnected()
	{
		QueueCallback([=]()
		{
			aircraftManager->RemoveAllPlanes();
			frameRateMonitor->StartMonitoring();
			TryGetTcasControl();
			xplaneAtisEnabled = 0;
			networkLoginStatus = 1;
		});
	}

	void XPilot::OnNetworkDisconnected()
	{
		QueueCallback([=]() 
		{
			aircraftManager->RemoveAllPlanes();
			frameRateMonitor->StopMonitoring();
			ReleaseTcasControl();
			xplaneAtisEnabled = 1;
			networkLoginStatus = 0;
		});
	}

	void XPilot::ForceDisconnect(std::string reason)
	{
		json j;
		j["Type"] = "ForceDisconnect";
		j["Data"]["Reason"] = reason;
		j["Timestamp"] = UtcTimestamp();
		SendSocketMsg(j.dump());
	}

	void XPilot::OnPluginDisabled()
	{
		json j;
		j["Type"] = "PluginDisabled";
		j["Timestamp"] = UtcTimestamp();
		SendSocketMsg(j.dump());
	}

	void XPilot::RequestControllerAtis(std::string callsign)
	{
		try
		{
			AddNotification(string_format("Requesting controller ATIS for: %s", callsign), 149, 165, 166);
			LOG_INFO("Requesting controller ATIS for %s", callsign.c_str());

			json j;
			j["Type"] = "RequestAtis";
			j["Timestamp"] = UtcTimestamp();
			j["Data"]["Callsign"] = callsign;

			SendSocketMsg(j.dump());
		}
		catch (zmq::error_t& e)
		{
			LOG_ERROR("Error requesting controller ATIS for %s: %s", callsign.c_str(), e.what());
		}
		catch (std::exception& e)
		{
			LOG_ERROR("Error requesting controller ATIS for %s: %s", callsign.c_str(), e.what());
		}
		catch (...) {}
	}

	bool XPilot::InitializeXPMP()
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

		if (!Config::Instance().HasValidPaths())
		{
			std::string err = "No valid CSL paths are configured or the paths are disabled. Verify the CSL configuration in X-Plane (Plugins > xPilot > Settings > CSL Packages).";
			AddNotification(err.c_str(), 192, 57, 43);
			LOG_ERROR(err.c_str());
		}
		else
		{
			for (const CslPackage& p : Config::Instance().GetCslPackages())
			{
				if (!p.path.empty() && p.enabled && CountFilesInPath(p.path) > 0)
				{
					try
					{
						err = XPMPLoadCSLPackage(p.path.c_str());
						if (*err)
						{
							AddNotification(string_format("Error loading CSL package %s: %s", p.path.c_str(), err), 231, 76, 60);
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

		XPMPEnableAircraftLabels(Config::Instance().GetShowHideLabels());
		XPMPSetAircraftLabelDist(Config::Instance().GetMaxLabelDistance(), Config::Instance().GetLabelCutoffVis());
		LOG_INFO("XPMP2 successfully initialized");
		return true;
	}

	void XPilot::AddConsoleMessageTab(const std::string& recipient, const std::string& msg, ConsoleTabType tabType)
	{
		if (!recipient.empty() && !msg.empty())
		{
			QueueCallback([=]()
			{
				textMessageConsole->AddMessageToTab(recipient, msg, tabType);
			});
		}
	}

	void XPilot::AddConsoleMessage(const std::string& msg, double red, double green, double blue)
	{
		if (!msg.empty())
		{
			QueueCallback([=]()
			{
				textMessageConsole->AddIncomingMessage(msg.c_str(), red, green, blue);
			});
		}
	}

	void XPilot::AddNotificationPanelMessage(const std::string& msg, double red, double green, double blue)
	{
		if (!msg.empty()) 
		{
			QueueCallback([=]() 
			{
				notificationPanel->AddNotificationPanelMessage(msg, red, green, blue);
			});
		}
	}

	void XPilot::AddNotification(const std::string& msg, double red, double green, double blue)
	{
		AddConsoleMessage(msg, red, green, blue);
		AddNotificationPanelMessage(msg, red, green, blue);
	}

	void XPilot::QueueCallback(std::function<void()> cb)
	{
		if (IsXPThread())
		{
			cb();
		}
		else
		{
			std::lock_guard<std::mutex> lock(queueMutex);
			queuedCallbacks.push_back(cb);
		}
	}

	void XPilot::ProcessQueuedCallbacks()
	{
		std::lock_guard<std::mutex> lock(queueMutex);
		if (!queuedCallbacks.empty())
		{
			for (auto& cb : queuedCallbacks)
			{
				cb();
			}
			queuedCallbacks.clear();
		}
	}

	void XPilot::TogglePreferencesWindow()
	{
		preferencesWindow->SetVisible(!preferencesWindow->GetVisible());
	}

	void XPilot::ToggleNearbyAtcWindow()
	{
		nearbyAtcWindow->SetVisible(!nearbyAtcWindow->GetVisible());
	}

	void XPilot::ToggleTextMessageConsole()
	{
		textMessageConsole->SetVisible(!textMessageConsole->GetVisible());
	}

	void XPilot::SetNotificationPanelAlwaysVisible(bool visible)
	{
		notificationPanel->SetAlwaysVisible(visible);
	}

	bool XPilot::IsNotificationPanelAlwaysVisible()const
	{
		return notificationPanel->IsAlwaysVisible();
	}

	void XPilot::IncNumAc()
	{
		++aircraftCount;
		numAircraft = aircraftCount;
	}

	void XPilot::DecNumAc()
	{
		--aircraftCount;
		numAircraft = aircraftCount;
	}

	void CBRequestTcasAgain(void*)
	{
		XPMPMultiplayerEnable(CBRequestTcasAgain);
	}

	void XPilot::TryGetTcasControl()
	{
		if (!XPMPHasControlOfAIAircraft())
		{
			auto err = XPMPMultiplayerEnable(CBRequestTcasAgain);
			if (*err)
			{
				AddNotification(err, 231, 76, 60);
				LOG_ERROR(err);
			}
			else
			{
				AddNotification("xPilot has TCAS control");
				LOG_INFO("xPilot has TCAS control");
			}
		}
	}

	void XPilot::ReleaseTcasControl()
	{
		if (XPMPHasControlOfAIAircraft())
		{
			XPMPMultiplayerDisable();
			AddNotification("xPilot released TCAS control");
			LOG_INFO("xPilot released TCAS control");
		}
	}

	int XPilot::GetBulkData(void* inRefcon, void* outData, int inStartPos, int inNumBytes)
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
			const NetworkAircraft& ac = *pIter->second.get();
			if (dr == DR_BULK_QUICK)
				ac.CopyBulkData((XPilotAPIAircraft::XPilotAPIBulkData*)pOut, size);
			else
				ac.CopyBulkData((XPilotAPIAircraft::XPilotAPIBulkInfoTexts*)pOut, size);
		}

		return (iAc - startAc) * size;
	}
}