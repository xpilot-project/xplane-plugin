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
#include "SettingsWindow.h"
#include "NotificationPanel.h"
#include "TextMessageConsole.h"
#include "sha512.hh"
#include "json.hpp"
#include "../protobuf/Wrapper.pb.h"

using json = nlohmann::json;

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
		// pilot client datarefs
		m_audioComSelection("sim/cockpit2/radios/actuators/audio_com_selection", ReadOnly),
		m_com1Power("sim/cockpit2/radios/actuators/com1_power", ReadOnly),
		m_com1Frequency833("sim/cockpit2/radios/actuators/com1_frequency_hz_833", ReadOnly),
		m_com1StandbyFrequency833("sim/cockpit2/radios/actuators/com1_standby_frequency_hz_833", ReadOnly),
		m_com1AudioSelection("sim/cockpit2/radios/actuators/audio_selection_com1", ReadOnly),
		m_com1AudioVolume("sim/cockpit2/radios/actuators/audio_volume_com1", ReadOnly),
		m_com2Power("sim/cockpit2/radios/actuators/com2_power", ReadOnly),
		m_com2Frequency833("sim/cockpit2/radios/actuators/com2_frequency_hz_833", ReadOnly),
		m_com2StandbyFrequency833("sim/cockpit2/radios/actuators/com2_standby_frequency_hz_833", ReadOnly),
		m_com2AudioSelection("sim/cockpit2/radios/actuators/audio_selection_com2", ReadOnly),
		m_com2AudioVolume("sim/cockpit2/radios/actuators/audio_volume_com2", ReadOnly),
		m_avionicsPowerOn("sim/cockpit2/switches/avionics_power_on", ReadOnly),
		m_positionLatitude("sim/flightmodel/position/latitude", ReadOnly),
		m_positionLongitude("sim/flightmodel/position/longitude", ReadOnly),
		m_positionAltitude("sim/flightmodel/position/elevation", ReadOnly),
		m_positionPressureAltitude("sim/cockpit2/gauges/indicators/altitude_ft_pilot", ReadOnly),
		m_groundSpeed("sim/flightmodel/position/groundspeed", ReadOnly),
		m_positionPitch("sim/flightmodel/position/theta", ReadOnly),
		m_positionRoll("sim/flightmodel/position/phi", ReadOnly),
		m_positionYaw("sim/flightmodel/position/psi", ReadOnly),
		m_velocityLongitude("sim/flightmodel/position/local_vx", ReadOnly),
		m_velocityAltitude("sim/flightmodel/position/local_vy", ReadOnly),
		m_velocityLatitude("sim/flightmodel/position/local_vz", ReadOnly),
		m_velocityPitch("sim/flightmodel/position/Qrad", ReadOnly),
		m_velocityHeading("sim/flightmodel/position/Rrad", ReadOnly),
		m_velocityBank("sim/flightmodel/position/Prad", ReadOnly),
		m_transponderCode("sim/cockpit/radios/transponder_code", ReadOnly),
		m_transponderMode("sim/cockpit/radios/transponder_mode", ReadOnly),
		m_transponderIdent("sim/cockpit/radios/transponder_id", ReadOnly),
		m_beaconLightsOn("sim/cockpit/electrical/beacon_lights_on", ReadOnly),
		m_landingLightsOn("sim/cockpit/electrical/landing_lights_on", ReadOnly),
		m_navLightsOn("sim/cockpit/electrical/nav_lights_on", ReadOnly),
		m_strobeLightsOn("sim/cockpit/electrical/strobe_lights_on", ReadOnly),
		m_taxiLightsOn("sim/cockpit/electrical/taxi_light_on", ReadOnly),
		m_flapRatio("sim/flightmodel/controls/flaprat", ReadOnly), // flap_handle_deploy_ratio
		m_gearDown("sim/cockpit/switches/gear_handle_status", ReadOnly),
		m_speedBrakeRatio("sim/cockpit2/controls/speedbrake_ratio", ReadOnly), // speedbrake_ratio
		m_engineCount("sim/aircraft/engine/acf_num_engines", ReadOnly),
		m_enginesRunning("sim/flightmodel/engine/ENGN_running", ReadOnly),
		m_onGround("sim/flightmodel/failures/onground_any", ReadOnly),
		m_replayMode("sim/operation/prefs/replay_mode", ReadOnly),
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
		pluginHash = sw::sha512::file(GetTruePluginPath().c_str());
		m_pluginVersion = PLUGIN_VERSION;

		XPLMRegisterFlightLoopCallback(deferredStartup, -1.0f, this);
	}

	XPilot::~XPilot()
	{
		XPLMUnregisterDataAccessor(m_bulkDataQuick);
		XPLMUnregisterDataAccessor(m_bulkDataExpensive);
		XPLMUnregisterFlightLoopCallback(deferredStartup, this);
		XPLMUnregisterFlightLoopCallback(mainFlightLoop, this);
		XPLMUnregisterFlightLoopCallback(drFlightLoop, this);
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
			m_zmqSocket->bind("tcp://*:" + Config::Instance().getTcpPort());
		}
		catch (zmq::error_t& e)
		{
			LOG_MSG(logERROR, "Error binding port: %s", e.what());
		}
		catch (const std::exception& e)
		{
			LOG_MSG(logERROR, "Error binding port: %s", e.what());
		}
		catch (...)
		{
		}

		XPLMRegisterFlightLoopCallback(mainFlightLoop, -1.0f, this);
		XPLMRegisterFlightLoopCallback(drFlightLoop, -1.0f, this);

		tryGetTcasControl();

		m_keepAlive = true;
		m_zmqThread = std::make_unique<std::thread>(&XPilot::zmqWorker, this);
		LOG_MSG(logMSG, "xPilot is now listening on port %s.", Config::Instance().getTcpPort());
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
			LOG_MSG(logERROR, "Error closing socket: %s", e.what());
		}
		catch (std::exception& e)
		{
			LOG_MSG(logERROR, "Socket exception: %s", e.what());
		}
		catch (...)
		{
		}

		m_keepAlive = false;

		if (m_zmqThread)
		{
			m_zmqThread->join();
			m_zmqThread.reset();
		}
	}

	void XPilot::sendPbArray(xpilot::Wrapper& wrapper)
	{
		if (isSocketConnected())
		{
			try
			{
				int dataSize = wrapper.ByteSize();
				char* dataArray = new char[dataSize];
				wrapper.SerializeToArray(dataArray, dataSize);
				
				std::string identity = "CLIENT";
				zmq::message_t msgIdentity(identity.size());
				std::memcpy(msgIdentity.data(), identity.data(), identity.size());
				m_zmqSocket->send(msgIdentity, zmq::send_flags::sndmore);
				m_zmqSocket->send(dataArray, dataSize, ZMQ_NOBLOCK);
			}
			catch (zmq::error_t)
			{
			}
			catch (...)
			{
			}
		}
	}

	float XPilot::drFlightLoop(float, float, int, void* ref)
	{
		auto* instance = static_cast<XPilot*>(ref);
		if (instance)
		{
			instance->checkDatarefs(false);
		}
		return 0.2;
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

	void XPilot::checkDatarefs(bool force)
	{
		xpilot::Wrapper msg;
		xpilot::XplaneDatarefs * data = new xpilot::XplaneDatarefs();
		msg.set_allocated_xplane_datarefs(data);

		auto ts = new google::protobuf::Timestamp{};
		ts->set_seconds(time(NULL));
		ts->set_nanos(0);
		msg.set_allocated_timestamp(ts);

		data->set_audio_com_selection(m_audioComSelection);

		// com1
		data->set_com1_power(m_com1Power);
		data->set_com1_freq(m_com1Frequency833);
		data->set_com1_stby_freq(m_com1StandbyFrequency833);
		data->set_com1_audio_selection(m_com1AudioSelection);

		// com2
		data->set_com2_power(m_com2Power);
		data->set_com2_freq(m_com2Frequency833);
		data->set_com2_stby_freq(m_com2StandbyFrequency833);
		data->set_com2_audio_selection(m_com2AudioSelection);

		data->set_avionics_power_on(m_avionicsPowerOn);

		// transponder
		data->set_transponder_code(m_transponderCode);
		data->set_transponder_mode(m_transponderMode);
		data->set_transponder_ident(m_transponderIdent);

		// lights
		data->set_beacon_lights_on(m_beaconLightsOn);
		data->set_landing_lights_on(m_landingLightsOn);
		data->set_nav_lights_on(m_navLightsOn);
		data->set_strobe_lights_on(m_strobeLightsOn);
		data->set_taxi_lights_on(m_taxiLightsOn);

		// control surfaces
		data->set_flaps(m_flapRatio);
		data->set_gear_down(m_gearDown);
		data->set_speed_brakes(m_speedBrakeRatio);

		// engines
		data->set_engine_count(m_engineCount);
		std::vector<int> val = m_enginesRunning;
		for (size_t i = 0; i < 4; ++i)
		{
			switch (i)
			{
				case 0:
					data->set_engine1_running(val[i]);
					break;
				case 1:
					data->set_engine2_running(val[i]);
					break;
				case 2:
					data->set_engine3_running(val[i]);
					break;
				case 3:
					data->set_engine4_running(val[i]);
					break;
			}
		}

		// position
		data->set_latitude(m_positionLatitude);
		data->set_longitude(m_positionLongitude);
		data->set_altitude(m_positionAltitude);
		data->set_pressure_altitude(m_positionPressureAltitude);
		data->set_ground_speed(m_groundSpeed);
		data->set_pitch(m_positionPitch);
		data->set_roll(m_positionRoll);
		data->set_yaw(m_positionYaw);

		// velocity
		data->set_velocity_latitude(m_velocityLatitude * -1.0);
		data->set_velocity_altitude(m_velocityAltitude);
		data->set_velocity_longitude(m_velocityLongitude);
		data->set_velocity_pitch(m_velocityPitch * -1.0);
		data->set_velocity_heading(m_velocityHeading);
		data->set_velocity_bank(m_velocityBank * -1.0);

		// misc
		data->set_on_ground(m_onGround);
		data->set_replay_mode(m_replayMode);

		sendPbArray(msg);
	}

	void XPilot::zmqWorker()
	{
		while (isSocketReady())
		{
			try
			{
				zmq::message_t zmqMsg;
				m_zmqSocket->recv(zmqMsg, zmq::recv_flags::none);

				xpilot::Wrapper wrapper;
				wrapper.ParseFromArray(zmqMsg.data(), zmqMsg.size());

				if (wrapper.has_plugin_hash())
				{
					xpilot::Wrapper reply;
					xpilot::PluginHash* msg = new xpilot::PluginHash();
					reply.set_allocated_plugin_hash(msg);
					msg->set_hash(pluginHash.c_str());
					sendPbArray(reply);
				}

				if (wrapper.has_plugin_version())
				{
					xpilot::Wrapper reply;
					xpilot::PluginVersion* msg = new xpilot::PluginVersion();
					reply.set_allocated_plugin_version(msg);
					msg->set_version(PLUGIN_VERSION);
					sendPbArray(reply);
				}

				if (wrapper.has_csl_validate())
				{
					xpilot::Wrapper reply;
					xpilot::CslValidate* msg = new xpilot::CslValidate();
					reply.set_allocated_csl_validate(msg);
					msg->set_valid(Config::Instance().hasValidPaths() && XPMPGetNumberOfInstalledModels() > 0);
					sendPbArray(reply);
				}

				if (wrapper.has_add_plane())
				{
					xpilot::AddPlane msg = wrapper.add_plane();
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

				if (wrapper.has_change_model())
				{
					xpilot::ChangeModel msg = wrapper.change_model();
					if(msg.has_callsign() && msg.has_equipment())
					{
						queueCallback([=]()
						{
							m_aircraftManager->ChangeAircraftModel(msg.callsign(), msg.equipment(), msg.airline());
						});
					}
				}

				if (wrapper.has_position_update())
				{
					xpilot::PositionUpdate msg = wrapper.position_update();

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

				if (wrapper.has_fast_position_update())
				{
					xpilot::FastPositionUpdate msg = wrapper.fast_position_update();

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
					rotationalVector.Z = msg.velocity_bank();

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

				if (wrapper.has_airplane_config())
				{
					xpilot::AirplaneConfig msg = wrapper.airplane_config();
					if (msg.has_callsign())
					{
						queueCallback([=]()
						{
							m_aircraftManager->UpdateAircraftConfiguration(msg);
						});
					}
				}

				if (wrapper.has_remove_plane())
				{
					xpilot::RemovePlane msg = wrapper.remove_plane();
					if (msg.has_callsign())
					{
						queueCallback([=]()
						{
							m_aircraftManager->DeleteAircraft(msg.callsign());
						});
					}
				}

				if (wrapper.has_remove_all_planes())
				{
					queueCallback([=]()
					{
						m_aircraftManager->DeleteAllAircraft();
					});
				}

				if (wrapper.has_network_connected())
				{
					if (wrapper.network_connected().has_callsign())
					{
						m_networkCallsign = wrapper.network_connected().callsign();
					}
					queueCallback([=]()
					{
						onNetworkConnected();
					});
				}

				if (wrapper.has_network_disconnected())
				{
					m_networkCallsign = "";
					queueCallback([=]()
					{
						onNetworkDisconnected();
					});
				}

				if (wrapper.has_nearby_controllers())
				{
					queueCallback([=]()
					{
						m_nearbyAtcWindow->UpdateList(wrapper.nearby_controllers());
					});
				}

				if (wrapper.has_clear_nearby_controllers())
				{
					queueCallback([=]()
					{
						m_nearbyAtcWindow->ClearList();
					});
				}

				if (wrapper.has_private_message_received())
				{
					xpilot::PrivateMessageReceived msg = wrapper.private_message_received();
					addConsoleMessageTab(msg.from(), msg.message(), ConsoleTabType::Incoming);
					addNotificationPanelMessage(string_format("%s [pvt]:  %s", msg.from(), msg.message().c_str()), 230, 94, 230);
				}

				if (wrapper.has_private_message_sent())
				{
					xpilot::PrivateMessageSent msg = wrapper.private_message_sent();
					addConsoleMessageTab(msg.to(), msg.message(), ConsoleTabType::Outgoing);
					addNotificationPanelMessage(string_format("%s [pvt: %s]:  %s", m_networkCallsign.value().c_str(), msg.to(), msg.message().c_str()), 50, 205, 50);
				}

				//std::string data(static_cast<char*>(msg.data()), msg.size());

				/*if (!data.empty())
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
				}*/
			}
			catch (zmq::error_t& e)
			{
				if (e.num() != ETERM)
				{
					LOG_MSG(logERROR, "Socket recv exception: %s", e.what());
				}
			}
			catch (std::exception& e)
			{
				LOG_MSG(logERROR, "Socket recv exception: %s", e.what());
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
			return Config::Instance().getLogLevel();
		return defaultVal;
	}

	void XPilot::onNetworkConnected()
	{
		m_aircraftManager->DeleteAllAircraft();
		m_frameRateMonitor->startMonitoring();
		tryGetTcasControl();
		m_xplaneAtisEnabled = 0;
		m_networkLoginStatus = 1;
	}

	void XPilot::onNetworkDisconnected()
	{
		m_aircraftManager->DeleteAllAircraft();
		m_frameRateMonitor->stopMonitoring();
		releaseTcasControl();
		m_xplaneAtisEnabled = 1;
		m_networkLoginStatus = 0;
	}

	void XPilot::forceDisconnect(std::string reason)
	{
		//json j;
		//j["Type"] = "ForceDisconnect";
		//j["Data"]["Reason"] = reason;
		//j["Timestamp"] = UtcTimestamp();
		//sendSocketMsg(j.dump());
	}

	void XPilot::requestControllerAtis(std::string callsign)
	{
		//json j;
		//j["Type"] = "RequestAtis";
		//j["Timestamp"] = UtcTimestamp();
		//j["Data"]["Callsign"] = callsign;

		//sendSocketMsg(j.dump());
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