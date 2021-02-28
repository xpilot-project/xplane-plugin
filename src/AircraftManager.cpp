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
	mapPlanesTy mapPlanes;
	mapPlanesTy::iterator mapGetAircraftByIndex(int idx)
	{
		int i = 0;
		for (mapPlanesTy::iterator iter = mapPlanes.begin(); iter != mapPlanes.end(); ++iter)
		{
			if (iter->second)
			{
				if (++i == idx)
				{
					return iter;
				}
			}
		}
		return mapPlanes.end();
	}

	void AircraftManager::interpolateAirplanes()
	{
		for (auto& kv : mapPlanes)
		{
			NetworkAircraft* plane = kv.second.get();
			if (!plane) continue;

			if (plane->interpolationStack.size() > 0)
			{
				InterpolatedState interpolated;
				long long currentTimestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

				if (plane->interpolationStack.size() == 1)
				{
					interpolated = plane->interpolationStack.front();
				}
				else if (currentTimestamp <= plane->interpolationStack.front().timestamp)
				{
					interpolated = plane->interpolationStack.front();
				}
				else
				{
					InterpolatedState start{};
					InterpolatedState end{};

					for (int i = 0; i < plane->interpolationStack.size() - 1; i++)
					{
						start = plane->interpolationStack.at(i);
						end = plane->interpolationStack.at(i + 1.0);
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
				plane->position.lat = interpolated.latitude;
				plane->position.lon = interpolated.longitude;
				plane->position.elevation = interpolated.altitude;
				plane->position.heading = static_cast<float>(interpolated.heading);
				plane->position.roll = static_cast<float>(interpolated.bank);
				plane->position.pitch = static_cast<float>(interpolated.pitch);
				plane->groundSpeed = static_cast<float>(interpolated.groundSpeed);
			}
		}
	}

	void AircraftManager::addNewPlane(const std::string& callsign, const std::string& typeIcao,
		const std::string& airlineIcao, const std::string& livery, const std::string& model)
	{
		auto planeIt = mapPlanes.find(callsign);
		if (planeIt != mapPlanes.end()) return;

		NetworkAircraft* plane = new NetworkAircraft(typeIcao.c_str(), airlineIcao.c_str(), livery.c_str(), 0, model.c_str());
		plane->callsign = callsign;
		mapPlanes.emplace(callsign, std::move(plane));
	}

	void AircraftManager::setPlanePosition(const std::string& callsign, XPMPPlanePosition_t pos, XPMPPlaneRadar_t radar, float groundSpeed, const std::string& origin, const std::string& destination)
	{
		auto planeIt = mapPlanes.find(callsign);
		if (planeIt == mapPlanes.end()) return;

		NetworkAircraft* plane = planeIt->second.get();
		if (!plane) return;

		InterpolatedState state{};
		state.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + 5500000;
		state.latitude = pos.lat;
		state.longitude = pos.lon;
		state.bank = pos.roll;
		state.pitch = pos.pitch;
		state.heading = pos.heading;
		state.groundSpeed = groundSpeed;

		double groundElevation = 0.0;
		groundElevation = plane->terrainProbe.getTerrainElevation(pos.lat, pos.lon);
		if (std::isnan(groundElevation))
		{
			groundElevation = 0.0;
		}

		plane->origin = origin.empty() ? "" : origin;
		plane->destination = destination.empty() ? "" : destination;

		plane->terrainAltitude = groundElevation;
		state.altitude = plane->onGround ? groundElevation : pos.elevation;
		plane->radar = radar;
		plane->renderCount++;

		plane->interpolationStack.push_back(state);
		while (plane->interpolationStack.size() > 2
			&& plane->interpolationStack.at(1).timestamp
			<= std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
		{
			plane->interpolationStack.pop_front();
		}
	}

	void AircraftManager::updateAircraftConfig(const xpilot::AirplaneConfig& config)
	{
		auto planeIt = mapPlanes.find(config.callsign());
		if (planeIt == mapPlanes.end()) return;

		NetworkAircraft* plane = planeIt->second.get();
		if (!plane) return;

		if (config.has_flaps())
		{
			if (config.flaps() != plane->targetFlapPosition)
			{
				plane->targetFlapPosition = config.flaps();
			}
		}
		if (config.has_gear_down())
		{
			if (config.gear_down() != plane->gearDown)
			{
				plane->gearDown = config.gear_down();
			}
		}
		if (config.has_spoilers_deployed())
		{
			if (config.spoilers_deployed() != plane->spoilersDeployed)
			{
				plane->spoilersDeployed = config.spoilers_deployed();
			}
		}
		if (config.has_lights())
		{
			if (config.lights().has_strobe_lights_on())
			{
				if (config.lights().strobe_lights_on() != plane->surfaces.lights.strbLights)
				{
					plane->surfaces.lights.strbLights = config.lights().strobe_lights_on();
				}
			}
			if (config.lights().has_landing_lights_on())
			{
				if (config.lights().landing_lights_on() != plane->surfaces.lights.landLights)
				{
					plane->surfaces.lights.landLights = config.lights().landing_lights_on();
				}
			}
			if (config.lights().has_nav_lights_on())
			{
				if (config.lights().nav_lights_on() != plane->surfaces.lights.navLights)
				{
					plane->surfaces.lights.navLights = config.lights().nav_lights_on();
				}
			}
			if (config.lights().has_beacon_lights_on())
			{
				if (config.lights().beacon_lights_on() != plane->surfaces.lights.bcnLights)
				{
					plane->surfaces.lights.bcnLights = config.lights().beacon_lights_on();
				}
			}
			if (config.lights().has_taxi_lights_on())
			{
				if (config.lights().taxi_lights_on() != plane->surfaces.lights.taxiLights)
				{
					plane->surfaces.lights.taxiLights = config.lights().taxi_lights_on();
				}
			}
		}
		if (config.has_engines_on())
		{
			if (config.engines_on() != plane->enginesRunning)
			{
				plane->enginesRunning = config.engines_on();
			}
		}
		if (config.has_reverse_thrust())
		{
			if (config.reverse_thrust() != plane->reverseThrust)
			{
				plane->reverseThrust = config.reverse_thrust();
			}
		}
		if (config.has_on_ground())
		{
			if (config.on_ground() != plane->onGround)
			{
				plane->bClampToGround = (plane->renderCount <= 2) ? config.on_ground() : false;
				plane->onGround = config.on_ground();
			}
		}
	}

	void AircraftManager::removePlane(const std::string& callsign)
	{
		auto planeIt = mapPlanes.find(callsign);
		if (planeIt == mapPlanes.end()) return;

		NetworkAircraft* plane = planeIt->second.get();
		if (!plane) return;

		mapPlanes.erase(callsign);
	}

	void AircraftManager::removeAllPlanes()
	{
		mapPlanes.clear();
	}

	void AircraftManager::changeModel(const std::string& callsign, const std::string& typeIcao, const std::string& airlineIcao)
	{
		auto planeIt = mapPlanes.find(callsign);
		if (planeIt == mapPlanes.end()) return;

		NetworkAircraft* plane = planeIt->second.get();
		if (!plane) return;

		plane->ChangeModel(typeIcao.c_str(), airlineIcao.c_str(), "");
	}
}