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
#include "GeoCalc.hpp"
#include "Quaternion.hpp"
#include "XPilot.h"

namespace xpilot
{
	const long FAST_POSITION_INTERVAL_TOLERANCE = 300;
	const double ERROR_CORRECTION_INTERVAL_FAST = 2.0;
	const double ERROR_CORRECTION_INTERVAL_SLOW = 5.0;

	static double NormalizeDegrees(double value, double lowerBound, double upperBound)
	{
		double range = upperBound - lowerBound;
		if (value < lowerBound)
		{
			return value + range;
		}
		if (value > upperBound)
		{
			return value - range;
		}
		return value;
	}

	static double CalculateNormalizedDelta(double start, double end, double lowerBound, double upperBound)
	{
		double range = upperBound - lowerBound;
		double halfRange = range / 2.0;

		if (abs(end - start) > halfRange)
		{
			end += (end > start ? -range : range);
		}

		return end - start;
	}

	static double Round(double value, int to)
	{
		double places = pow(10.0, to);
		return round(value * places) / places;
	}

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

	AircraftManager::AircraftManager(XPilot* instance) : mEnv(instance)
	{

	}

	void AircraftManager::SetUpNewPlane(const std::string& callsign, const AircraftVisualState& visualState, const std::string& typeIcao, const std::string& airlineIcao, const std::string& livery, const std::string& model)
	{
		auto planeIt = mapPlanes.find(callsign);
		if (planeIt != mapPlanes.end()) return;

		NetworkAircraft* plane = new NetworkAircraft(
			callsign.c_str(),
			visualState,
			typeIcao.c_str(),
			airlineIcao.c_str(),
			livery.c_str(), 0,
			model.c_str()
		);
		mapPlanes.emplace(callsign, std::move(plane));

		xpilot::Envelope envelope;
		xpilot::PlaneAddedToSim* msg = new xpilot::PlaneAddedToSim();
		envelope.set_allocated_plane_added_to_sim(msg);
		msg->set_callsign(plane->label);
		if (mEnv)
		{
			mEnv->SendClientEvent(envelope);
		}
	}

	void AircraftManager::DeleteAircraft(const std::string& callsign)
	{
		auto aircraft = GetAircraft(callsign);
		if (!aircraft) return;

		mapPlanes.erase(callsign);

		xpilot::Envelope envelope;
		xpilot::PlaneRemovedFromSim* msg = new xpilot::PlaneRemovedFromSim();
		envelope.set_allocated_plane_removed_from_sim(msg);
		msg->set_callsign(callsign);
		if (mEnv)
		{
			mEnv->SendClientEvent(envelope);
		}
	}

	void AircraftManager::DeleteAllAircraft()
	{
		for (auto const& plane : mapPlanes)
		{
			xpilot::Envelope envelope;
			xpilot::PlaneRemovedFromSim* msg = new xpilot::PlaneRemovedFromSim();
			envelope.set_allocated_plane_removed_from_sim(msg);
			msg->set_callsign(plane.first);
			if (mEnv)
			{
				mEnv->SendClientEvent(envelope);
			}
		}
		mapPlanes.clear();
	}

	void AircraftManager::ProcessSlowPositionUpdate(const std::string& callsign, AircraftVisualState visualState, double speed)
	{
		auto aircraft = GetAircraft(callsign);
		if (!aircraft)
		{
			return;
		}

		auto now = std::chrono::steady_clock::now();

		// The logic here is that if we have not received a fast position packet recently, then
		// we need to derive positional velocities from the last position that we received (either
		// fast or slow) and the position that we're currently processing. We also snap to the
		// newly-reported rotation rather than trying to derive rotational velocities.
		if (!ReceivingFastPositionUpdates(aircraft))
		{
			auto lastUpdateTimeStamp = (aircraft->last_slow_position_timestamp < aircraft->last_fast_position_timestamp) ? aircraft->last_slow_position_timestamp : aircraft->last_fast_position_timestamp;

			auto intervalMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdateTimeStamp).count();

			aircraft->positional_velocity_vector = DerivePositionalVelocityVector(
				aircraft->remote_visual_state,
				visualState,
				intervalMs
			);
			aircraft->rotational_velocity_vector = Vector3::Zero();

			AircraftVisualState newVisualState{};
			newVisualState.Lat = aircraft->predicted_visual_state.Lat;
			newVisualState.Lon = aircraft->predicted_visual_state.Lon;
			newVisualState.Altitude = aircraft->predicted_visual_state.Altitude;
			newVisualState.Pitch = visualState.Pitch;
			newVisualState.Heading = visualState.Heading;
			newVisualState.Bank = visualState.Bank;

			aircraft->predicted_visual_state = newVisualState;
			aircraft->remote_visual_state = visualState;
			aircraft->UpdateErrorVectors(ERROR_CORRECTION_INTERVAL_SLOW);
		}

		aircraft->last_slow_position_timestamp = now;
		aircraft->ground_speed = speed;
	}

	void AircraftManager::HandleFastPositionUpdate(const std::string& callsign, const AircraftVisualState& visualState, Vector3 positionalVector, Vector3 rotationalVector)
	{
		auto aircraft = GetAircraft(callsign);
		if (!aircraft)
		{
			return;
		}

		aircraft->positional_velocity_vector = positionalVector;
		aircraft->rotational_velocity_vector = rotationalVector;
		aircraft->remote_visual_state = visualState;
		aircraft->last_fast_position_timestamp = std::chrono::steady_clock::now();
		aircraft->fast_positions_received_count++;

		aircraft->UpdateErrorVectors(ERROR_CORRECTION_INTERVAL_FAST);
	}

	NetworkAircraft* AircraftManager::GetAircraft(const std::string& callsign)
	{
		auto planeIt = mapPlanes.find(callsign);
		if (planeIt == mapPlanes.end()) return nullptr;
		return planeIt->second.get();
	}

	bool AircraftManager::ReceivingFastPositionUpdates(NetworkAircraft* aircraft)
	{
		const auto now = std::chrono::steady_clock::now();
		const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - aircraft->last_fast_position_timestamp);
		return diff.count() < FAST_POSITION_INTERVAL_TOLERANCE;
	}

	Vector3 AircraftManager::DerivePositionalVelocityVector(AircraftVisualState previousVisualState, AircraftVisualState newVisualState, long intervalMs)
	{
		// We're rounding the lat/lon/alt to the lowest common precision among slow and
		// fast position updates, because sometimes the previous visual state is from a
		// fast position. (The new visual state is always from a slow position.)

		double latDelta = DegreesToMeters(CalculateNormalizedDelta(
			Round(previousVisualState.Lat, 5),
			Round(newVisualState.Lat, 5),
			-90.0,
			90.0
		));

		double lonDelta = DegreesToMeters(CalculateNormalizedDelta(
			Round(previousVisualState.Lon, 5),
			Round(newVisualState.Lon, 5),
			-180.0,
			180.0
		)) * LongitudeScalingFactor(newVisualState.Lat);

		double altDelta = (int)newVisualState.Altitude - (int)previousVisualState.Altitude;

		double intervalSec = intervalMs / 1000.0;

		return Vector3(
			lonDelta / intervalSec,
			altDelta / intervalSec,
			latDelta / intervalSec
		);
	}

	void AircraftManager::ChangeAircraftModel(const std::string& callsign, const std::string& typeIcao, const std::string& airlineIcao)
	{
		auto aircraft = GetAircraft(callsign);
		if (!aircraft) return;

		aircraft->ChangeModel(typeIcao.c_str(), airlineIcao.c_str(), "");
	}

	void AircraftManager::UpdateAircraftConfiguration(const xpilot::AirplaneConfig& config)
	{
		auto plane = GetAircraft(config.callsign());
		if (!plane) return;

		if (config.has_is_full_config() && config.is_full_config())
		{
			plane->SetVisible(true);
		}

		if (config.has_flaps())
		{
			plane->target_flaps_position = config.flaps();
		}
		if (config.has_gear_down())
		{
			plane->gear_down = config.gear_down();
		}
		if (config.has_spoilers_deployed())
		{
			plane->spoilers_deployed = config.spoilers_deployed();
		}
		if (config.has_lights())
		{
			if (config.lights().has_strobe_lights_on())
			{
				plane->surfaces.lights.strbLights = config.lights().strobe_lights_on();
			}
			if (config.lights().has_landing_lights_on())
			{
				plane->surfaces.lights.landLights = config.lights().landing_lights_on();
			}
			if (config.lights().has_nav_lights_on())
			{
				plane->surfaces.lights.navLights = config.lights().nav_lights_on();
			}
			if (config.lights().has_beacon_lights_on())
			{
				plane->surfaces.lights.bcnLights = config.lights().beacon_lights_on();
			}
			if (config.lights().has_taxi_lights_on())
			{
				plane->surfaces.lights.taxiLights = config.lights().taxi_lights_on();
			}
		}
		if (config.has_engines_on())
		{
			plane->engines_running = config.engines_on();
		}
		if (config.has_reverse_thrust())
		{
			plane->reverse_thrust = config.reverse_thrust();
		}
		if (config.has_on_ground())
		{
			plane->on_ground = config.on_ground();
		}
	}
}