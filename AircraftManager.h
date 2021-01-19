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

#ifndef AircraftManager_h
#define AircraftManager_h

#include <string>
#include <map>
#include <mutex>

#include "NetworkAircraftConfig.h"
#include "NetworkAircraft.h"

namespace xpilot
{
	typedef std::map<std::string, std::unique_ptr<NetworkAircraft>> mapPlanesTy;
	extern mapPlanesTy mapPlanes;
	inline mapPlanesTy::iterator mapGetNextAircraft(mapPlanesTy::iterator iter)
	{
		return std::find_if(std::next(iter), mapPlanes.end(), [](const mapPlanesTy::value_type& p) {return p.second.get(); });
	}
	mapPlanesTy::iterator mapGetAircraftByIndex(int idx);

	inline double NormalizeHeading(double heading)
	{
		if (heading <= 0.0) {
			heading += 360.0;
		}
		else if (heading > 360.0) {
			heading -= 360.0;
		}
		return heading;
	}

	class AircraftManager
	{
	public:
		AircraftManager() {};
		~AircraftManager() {};
		void interpolateAirplanes();
		void addNewPlane(const std::string& callsign, const std::string& typeIcao, const std::string& airlineIcao,
			const std::string& livery = "", const std::string& modelName = "");
		void setPlanePosition(const std::string& callsign, XPMPPlanePosition_t pos, XPMPPlaneRadar_t radar, float groundSpeed, const std::string& origin, const std::string& destination);
		void updateAircraftConfig(const std::string& callsign, const NetworkAircraftConfig& config);
		void changeModel(const std::string& callsign, const std::string& typeIcao, const std::string& airlineIcao);
		void removePlane(const std::string& callsign);
		void removeAllPlanes();
	};
}

#endif // !AircraftManager_h
