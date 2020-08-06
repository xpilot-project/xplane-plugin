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
				if (iter->second.get()) {
					if (++i == idx) {
						return iter;
					}
				}
			}
			return mapPlanes.end();
		}
	}

	void AircraftManager::InterpolateAirplanes()
	{
		std::lock_guard<std::mutex> plane_map_lock(plane_map_mutex);
		{
			for (auto& plane : mapPlanes)
			{
				try
				{
					std::lock_guard<std::mutex> interpolation_stack_lock(interpolation_stack_mutex);
					{
						if (plane.second->InterpolationStack.size() > 0)
						{
							InterpolatedState interpolated;
							long long currentTimestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

							if (plane.second->InterpolationStack.size() == 1)
							{
								interpolated = plane.second->InterpolationStack.front();
							}
							else if (currentTimestamp <= plane.second->InterpolationStack.front().Timestamp)
							{
								interpolated = plane.second->InterpolationStack.front();
							}
							else
							{
								InterpolatedState start{};
								InterpolatedState end{};

								for (int i = 0; i < plane.second->InterpolationStack.size() - 1; i++)
								{
									start = plane.second->InterpolationStack.at(i);
									end = plane.second->InterpolationStack.at(i + 1);
									if ((start.Timestamp < currentTimestamp) && (end.Timestamp >= currentTimestamp))
									{
										break;
									}
								}

								if (currentTimestamp <= start.Timestamp)
								{
									interpolated = start;
								}
								else if (currentTimestamp >= end.Timestamp)
								{
									interpolated = end;
								}
								else
								{
									long long timeDelta = end.Timestamp - start.Timestamp;
									double pct = (currentTimestamp - start.Timestamp) / (double)timeDelta;
									double startHeaing = start.Heading;
									double endHeading = end.Heading;
									if (abs(endHeading - startHeaing) > 180.0)
									{
										endHeading += (endHeading > startHeaing ? -360.0 : 360.0);
									}
									interpolated.Timestamp = currentTimestamp;
									interpolated.Latitude = start.Latitude + ((end.Latitude - start.Latitude) * pct);
									interpolated.Longitude = start.Longitude + ((end.Longitude - start.Longitude) * pct);
									interpolated.Altitude = start.Altitude + ((end.Altitude - start.Altitude) * pct);
									interpolated.Pitch = start.Pitch + ((end.Pitch - start.Pitch) * pct);
									interpolated.Bank = start.Bank + ((end.Bank - start.Bank) * pct);
									interpolated.Heading = NormalizeHeading(startHeaing + ((endHeading - startHeaing) * pct));
									interpolated.GroundSpeed = start.GroundSpeed + ((end.GroundSpeed - start.GroundSpeed) * pct);
								}
							}
							plane.second->Position.lat = interpolated.Latitude;
							plane.second->Position.lon = interpolated.Longitude;
							plane.second->Position.elevation = interpolated.Altitude;
							plane.second->Position.heading = static_cast<float>(interpolated.Heading);
							plane.second->Position.roll = static_cast<float>(interpolated.Bank);
							plane.second->Position.pitch = static_cast<float>(interpolated.Pitch);
							plane.second->GroundSpeed = static_cast<float>(interpolated.GroundSpeed);
						}
					}
				}
				catch (std::exception& e)
				{
					//LOG_MSG(logERR, "Error interpolating airplane: %s", e.what());
				}
				catch (...) {}
			}
		}
	}

	void AircraftManager::AddNewPlane(const std::string& callsign, const std::string& typeIcao,
		const std::string& airlineIcao, const std::string& livery, const std::string& model)
	{
		std::lock_guard<std::mutex> plane_map_lock(plane_map_mutex);
		{
			if (mapPlanes.count(callsign) == 0)
			{
				std::unique_ptr<NetworkAircraft> ac(new NetworkAircraft(typeIcao.c_str(), airlineIcao.c_str(), livery.c_str(), 0, model.c_str()));
				ac->Callsign = callsign;
				mapPlanes.try_emplace(callsign, std::move(ac));
			}
		}
	}

	void AircraftManager::SetPlanePosition(const std::string& callsign, XPMPPlanePosition_t pos, XPMPPlaneRadar_t radar, float groundSpeed)
	{
		std::lock_guard<std::mutex> plane_map_lock(plane_map_mutex);
		{
			auto it = mapPlanes.find(callsign);
			if (it != mapPlanes.end())
			{
				InterpolatedState state;
				state.Timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + 5500000;
				state.Latitude = pos.lat;
				state.Longitude = pos.lon;
				state.Bank = pos.roll;
				state.Pitch = pos.pitch;
				state.Heading = pos.heading;
				state.GroundSpeed = groundSpeed;

				const double latDeg = pos.lat;
				const double lonDeg = pos.lon;
				double groundElevation = 0.0;
				groundElevation = it->second->terrainProbe.GetTerrainElevation(latDeg, lonDeg);
				if (std::isnan(groundElevation))
				{
					groundElevation = 0.0;
				}

				it->second->TerrainAltitude = groundElevation;
				state.Altitude = it->second->OnGround ? groundElevation : pos.elevation;

				if (it->second->RenderCount <= 2)
				{
					it->second->Position.lat = state.Latitude;
					it->second->Position.lon = state.Longitude;
					it->second->Position.elevation = state.Altitude;
					it->second->Position.heading = static_cast<float>(state.Heading);
					it->second->Position.roll = static_cast<float>(state.Bank);
					it->second->Position.pitch = static_cast<float>(state.Pitch);
					it->second->GroundSpeed = static_cast<float>(state.GroundSpeed);
				}

				std::lock_guard<std::mutex> lock(interpolation_stack_mutex);
				{
					it->second->InterpolationStack.push_back(state);
					while (it->second->InterpolationStack.size() > 2
						&& it->second->InterpolationStack.at(1).Timestamp
						<= std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
					{
						it->second->InterpolationStack.pop_front();
					}
				}

				it->second->Radar = radar;
				it->second->RenderCount++;
			}
		}
	}

	void AircraftManager::UpdateAircraftConfig(const std::string& callsign, const NetworkAircraftConfig& config)
	{
		std::lock_guard<std::mutex> plane_map_lock(plane_map_mutex);
		{
			auto it = mapPlanes.find(callsign);
			if (it != mapPlanes.end())
			{
				if (config.Data.FlapsPct.has_value())
				{
					if (config.Data.FlapsPct.value() != it->second->TargetFlapPosition)
					{
						it->second->TargetFlapPosition = config.Data.FlapsPct.value();
					}
				}
				if (config.Data.GearDown.has_value())
				{
					if (config.Data.GearDown.value() != it->second->GearDown)
					{
						it->second->GearDown = config.Data.GearDown.value();
					}
				}
				if (config.Data.SpoilersDeployed.has_value())
				{
					if (config.Data.SpoilersDeployed.value() != it->second->SpoilersDeployed)
					{
						it->second->SpoilersDeployed = config.Data.SpoilersDeployed.value();
					}
				}
				if (config.Data.Lights.has_value())
				{
					if (config.Data.Lights.value().StrobesOn.has_value())
					{
						if (config.Data.Lights.value().StrobesOn.value() != it->second->Surfaces.lights.strbLights)
						{
							it->second->Surfaces.lights.strbLights = config.Data.Lights.value().StrobesOn.value();
						}
					}
					if (config.Data.Lights.value().TaxiOn.has_value())
					{
						if (config.Data.Lights.value().TaxiOn.value() != it->second->Surfaces.lights.taxiLights)
						{
							it->second->Surfaces.lights.taxiLights = config.Data.Lights.value().TaxiOn.value();
						}
					}
					if (config.Data.Lights.value().NavOn.has_value())
					{
						if (config.Data.Lights.value().NavOn.value() != it->second->Surfaces.lights.navLights)
						{
							it->second->Surfaces.lights.navLights = config.Data.Lights.value().NavOn.value();
						}
					}
					if (config.Data.Lights.value().LandingOn.has_value())
					{
						if (config.Data.Lights.value().LandingOn.value() != it->second->Surfaces.lights.landLights)
						{
							it->second->Surfaces.lights.landLights = config.Data.Lights.value().LandingOn.value();
						}
					}
					if (config.Data.Lights.value().BeaconOn.has_value())
					{
						if (config.Data.Lights.value().BeaconOn.value() != it->second->Surfaces.lights.bcnLights)
						{
							it->second->Surfaces.lights.bcnLights = config.Data.Lights.value().BeaconOn.value();
						}
					}
				}
				if (config.Data.EnginesRunning.has_value())
				{
					if (config.Data.EnginesRunning.value() != it->second->EnginesRunning)
					{
						it->second->EnginesRunning = config.Data.EnginesRunning.value();
					}
				}
				if (config.Data.ReverseThrust.has_value())
				{
					if (config.Data.ReverseThrust.value() != it->second->ReverseThrust)
					{
						it->second->ReverseThrust = config.Data.ReverseThrust.value();
					}
				}
				if (config.Data.OnGround.has_value())
				{
					if (config.Data.OnGround.value() != it->second->OnGround)
					{
						it->second->bClampToGround = (it->second->RenderCount <= 2) ? config.Data.OnGround.value() : false;
						it->second->OnGround = config.Data.OnGround.value();
					}
				}
			}
		}
	}

	void AircraftManager::SetFlightPlan(const std::string& callsign, const std::string& origin, const std::string& destination)
	{
		std::lock_guard<std::mutex> plane_map_lock(plane_map_mutex);
		{
			auto it = mapPlanes.find(callsign);
			if (it != mapPlanes.end())
			{
				it->second->Origin = origin;
				it->second->Destination = destination;
			}
		}
	}

	void AircraftManager::RemovePlane(const std::string& _callsign)
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
				catch (...) {}
			}
		}
	}

	void AircraftManager::RemoveAllPlanes()
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
			catch (...) {}
		}
	}

	void AircraftManager::ChangeModel(const std::string& callsign, const std::string& typeIcao, const std::string& airlineIcao)
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
				catch (...) {}
			}
		}
	}
}