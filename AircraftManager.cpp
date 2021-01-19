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

#include "AircraftManager.h"
#include "NetworkAircraft.h"
#include "Utilities.h"

namespace xpilot

{
	std::mutex interpolation_stack_mutex;
	std::mutex plane_map_mutex;

	mapPlanesTy mapPlanes;
	mapPlanesTy::iterator mapGetAircraftByIndex(int idx)
	{
		int i = 0;
		std::lock_guard<std::mutex> plane_map_lock(plane_map_mutex);
		{
			for (mapPlanesTy::iterator iter = mapPlanes.begin(); iter != mapPlanes.end(); ++iter)
			{
				if (iter->second.get())
				{
					if (++i == idx)
					{
						return iter;
					}
				}
			}
			return mapPlanes.end();
		}
	}

	void AircraftManager::interpolateAirplanes()
	{
		std::lock_guard<std::mutex> plane_map_lock(plane_map_mutex);
		{
			for (auto& plane : mapPlanes)
			{
				try
				{
					std::lock_guard<std::mutex> interpolation_stack_lock(interpolation_stack_mutex);
					{
						if (plane.second->interpolationStack.size() > 0)
						{
							InterpolatedState interpolated;
							long long currentTimestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

							if (plane.second->interpolationStack.size() == 1)
							{
								interpolated = plane.second->interpolationStack.front();
							}
							else if (currentTimestamp <= plane.second->interpolationStack.front().timestamp)
							{
								interpolated = plane.second->interpolationStack.front();
							}
							else
							{
								InterpolatedState start{};
								InterpolatedState end{};

								for (int i = 0; i < plane.second->interpolationStack.size() - 1; i++)
								{
									start = plane.second->interpolationStack.at(i);
									end = plane.second->interpolationStack.at(i + 1);
									if ((start.timestamp < currentTimestamp) && (end.timestamp >= currentTimestamp))
									{
										break;
									}
								}

								if (currentTimestamp <= start.timestamp)
								{
									interpolated = start;
								}
								else if (currentTimestamp >= end.timestamp)
								{
									interpolated = end;
								}
								else
								{
									long long timeDelta = end.timestamp - start.timestamp;
									double pct = (currentTimestamp - start.timestamp) / (double)timeDelta;
									double startHeaing = start.heading;
									double endHeading = end.heading;
									if (abs(endHeading - startHeaing) > 180.0)
									{
										endHeading += (endHeading > startHeaing ? -360.0 : 360.0);
									}
									interpolated.timestamp = currentTimestamp;
									interpolated.latitude = start.latitude + ((end.latitude - start.latitude) * pct);
									interpolated.longitude = start.longitude + ((end.longitude - start.longitude) * pct);
									interpolated.altitude = start.altitude + ((end.altitude - start.altitude) * pct);
									interpolated.pitch = start.pitch + ((end.pitch - start.pitch) * pct);
									interpolated.bank = start.bank + ((end.bank - start.bank) * pct);
									interpolated.heading = NormalizeHeading(startHeaing + ((endHeading - startHeaing) * pct));
									interpolated.groundSpeed = start.groundSpeed + ((end.groundSpeed - start.groundSpeed) * pct);
								}
							}
							plane.second->position.lat = interpolated.latitude;
							plane.second->position.lon = interpolated.longitude;
							plane.second->position.elevation = interpolated.altitude;
							plane.second->position.heading = static_cast<float>(interpolated.heading);
							plane.second->position.roll = static_cast<float>(interpolated.bank);
							plane.second->position.pitch = static_cast<float>(interpolated.pitch);
							plane.second->groundSpeed = static_cast<float>(interpolated.groundSpeed);
						}
					}
				}
				catch (std::exception& e)
				{
				}
				catch (...)
				{
				}
			}
		}
	}

	void AircraftManager::addNewPlane(const std::string& callsign, const std::string& typeIcao,
		const std::string& airlineIcao, const std::string& livery, const std::string& model)
	{
		std::lock_guard<std::mutex> plane_map_lock(plane_map_mutex);
		{
			if (mapPlanes.count(callsign) == 0)
			{
				std::unique_ptr<NetworkAircraft> ac(new NetworkAircraft(typeIcao.c_str(), airlineIcao.c_str(), livery.c_str(), 0, model.c_str()));
				ac->callsign = callsign;
				mapPlanes.try_emplace(callsign, std::move(ac));
			}
		}
	}

	void AircraftManager::setPlanePosition(const std::string& callsign, XPMPPlanePosition_t pos, XPMPPlaneRadar_t radar, float groundSpeed, const std::string& origin, const std::string& destination)
	{
		std::lock_guard<std::mutex> plane_map_lock(plane_map_mutex);
		{
			auto it = mapPlanes.find(callsign);
			if (it != mapPlanes.end())
			{
				InterpolatedState state;
				state.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + 5500000;
				state.latitude = pos.lat;
				state.longitude = pos.lon;
				state.bank = pos.roll;
				state.pitch = pos.pitch;
				state.heading = pos.heading;
				state.groundSpeed = groundSpeed;

				double groundElevation = 0.0;
				groundElevation = it->second->terrainProbe.getTerrainElevation(pos.lat, pos.lon);
				if (std::isnan(groundElevation))
				{
					groundElevation = 0.0;
				}

				it->second->origin = origin.empty() ? "" : origin;
				it->second->destination = destination.empty() ? "" : destination;

				it->second->terrainAltitude = groundElevation;
				state.altitude = it->second->onGround ? groundElevation : pos.elevation;
				it->second->radar = radar;
				it->second->renderCount++;

				std::lock_guard<std::mutex> lock(interpolation_stack_mutex);
				{
					it->second->interpolationStack.push_back(state);
					while (it->second->interpolationStack.size() > 2
						&& it->second->interpolationStack.at(1).timestamp
						<= std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
					{
						it->second->interpolationStack.pop_front();
					}
				}
			}
		}
	}

	void AircraftManager::updateAircraftConfig(const std::string& callsign, const NetworkAircraftConfig& config)
	{
		std::lock_guard<std::mutex> plane_map_lock(plane_map_mutex);
		{
			auto it = mapPlanes.find(callsign);
			if (it != mapPlanes.end())
			{
				if (config.data.flapsPct.has_value())
				{
					if (config.data.flapsPct.value() != it->second->targetFlapPosition)
					{
						it->second->targetFlapPosition = config.data.flapsPct.value();
					}
				}
				if (config.data.gearDown.has_value())
				{
					if (config.data.gearDown.value() != it->second->gearDown)
					{
						it->second->gearDown = config.data.gearDown.value();
					}
				}
				if (config.data.spoilersDeployed.has_value())
				{
					if (config.data.spoilersDeployed.value() != it->second->spoilersDeployed)
					{
						it->second->spoilersDeployed = config.data.spoilersDeployed.value();
					}
				}
				if (config.data.lights.has_value())
				{
					if (config.data.lights.value().strobesOn.has_value())
					{
						if (config.data.lights.value().strobesOn.value() != it->second->surfaces.lights.strbLights)
						{
							it->second->surfaces.lights.strbLights = config.data.lights.value().strobesOn.value();
						}
					}
					if (config.data.lights.value().taxiOn.has_value())
					{
						if (config.data.lights.value().taxiOn.value() != it->second->surfaces.lights.taxiLights)
						{
							it->second->surfaces.lights.taxiLights = config.data.lights.value().taxiOn.value();
						}
					}
					if (config.data.lights.value().navOn.has_value())
					{
						if (config.data.lights.value().navOn.value() != it->second->surfaces.lights.navLights)
						{
							it->second->surfaces.lights.navLights = config.data.lights.value().navOn.value();
						}
					}
					if (config.data.lights.value().landingOn.has_value())
					{
						if (config.data.lights.value().landingOn.value() != it->second->surfaces.lights.landLights)
						{
							it->second->surfaces.lights.landLights = config.data.lights.value().landingOn.value();
						}
					}
					if (config.data.lights.value().beaconOn.has_value())
					{
						if (config.data.lights.value().beaconOn.value() != it->second->surfaces.lights.bcnLights)
						{
							it->second->surfaces.lights.bcnLights = config.data.lights.value().beaconOn.value();
						}
					}
				}
				if (config.data.enginesRunning.has_value())
				{
					if (config.data.enginesRunning.value() != it->second->enginesRunning)
					{
						it->second->enginesRunning = config.data.enginesRunning.value();
					}
				}
				if (config.data.reverseThrust.has_value())
				{
					if (config.data.reverseThrust.value() != it->second->reverseThrust)
					{
						it->second->reverseThrust = config.data.reverseThrust.value();
					}
				}
				if (config.data.onGround.has_value())
				{
					if (config.data.onGround.value() != it->second->onGround)
					{
						it->second->bClampToGround = (it->second->renderCount <= 2) ? config.data.onGround.value() : false;
						it->second->onGround = config.data.onGround.value();
					}
				}
			}
		}
	}

	void AircraftManager::removePlane(const std::string& _callsign)
	{
		std::lock_guard<std::mutex> plane_map_lock(plane_map_mutex);
		{
			auto it = mapPlanes.find(_callsign);
			if (it != mapPlanes.end())
			{
				try
				{
					mapPlanes.erase(_callsign);
				}
				catch (std::exception& e)
				{
					LOG_ERROR("Error removing network aircraft (%s): %s", _callsign.c_str(), e.what());
				}
				catch (...)
				{
				}
			}
		}
	}

	void AircraftManager::removeAllPlanes()
	{
		std::lock_guard<std::mutex> plane_map_lock(plane_map_mutex);
		{
			try
			{
				mapPlanes.clear();
			}
			catch (std::exception& e)
			{
				LOG_ERROR("Error removing all network aircraft: %s", e.what());
			}
			catch (...)
			{
			}
		}
	}

	void AircraftManager::changeModel(const std::string& callsign, const std::string& typeIcao, const std::string& airlineIcao)
	{
		std::lock_guard<std::mutex> plane_map_lock(plane_map_mutex);
		{
			auto it = mapPlanes.find(callsign);
			if (it != mapPlanes.end())
			{
				try
				{
					it->second->ChangeModel(typeIcao, airlineIcao, "");
				}
				catch (std::exception& e)
				{
					LOG_ERROR("Error changing model for network aircraft (%s): %s", callsign.c_str(), e.what());
				}
				catch (...)
				{
				}
			}
		}
	}
}