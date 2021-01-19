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

#include "NetworkAircraftConfig.h"

namespace xpilot
{
	void from_json(const json& j, NetworkAircraftConfigData& data)
	{
		if (j.find("Callsign") != j.end())
			j.at("Callsign").get_to(data.callsign);

		if (j.find("Lights") != j.end())
			j.at("Lights").get_to(data.lights);

		if (j.find("GearDown") != j.end())
			j.at("GearDown").get_to(data.gearDown);

		if (j.find("OnGround") != j.end())
			j.at("OnGround").get_to(data.onGround);

		if (j.find("Flaps") != j.end())
			j.at("Flaps").get_to(data.flapsPct);

		if (j.find("EnginesOn") != j.end())
			j.at("EnginesOn").get_to(data.enginesRunning);

		if (j.find("SpoilersDeployed") != j.end())
			j.at("SpoilersDeployed").get_to(data.spoilersDeployed);

		if (j.find("ReverseThrust") != j.end())
			j.at("ReverseThrust").get_to(data.reverseThrust);
	}

	void from_json(const json& j, NetworkAircraftConfigLights& lights) 
	{
		if (j.find("Strobes") != j.end())
			j.at("Strobes").get_to(lights.strobesOn);

		if (j.find("Landing") != j.end())
			j.at("Landing").get_to(lights.landingOn);

		if (j.find("Taxi") != j.end())
			j.at("Taxi").get_to(lights.taxiOn);

		if (j.find("Beacon") != j.end())
			j.at("Beacon").get_to(lights.beaconOn);

		if (j.find("Nav") != j.end())
			j.at("Nav").get_to(lights.navOn);
	}

	void from_json(const json& j, NetworkAircraftConfig& config) 
	{
		j.at("Type").get_to(config.type);
		j.at("Data").get_to(config.data);
	}
}