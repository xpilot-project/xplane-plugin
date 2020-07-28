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

#ifndef NetworkAircraftConfig_h
#define NetworkAircraftConfig_h

#include <string>
#include <optional>

#include "Lib/json.hpp"
using json = nlohmann::json;

namespace nlohmann 
{
    template<typename T>
    struct adl_serializer<std::optional<T>>
    {
        static void to_json(json& j, const std::optional<T>& value)
        {
            if (!value)
            {
                j = nullptr;
            }
            else
            {
                j = *value;
            }
        }

        static void from_json(json const& j, std::optional<T>& value)
        {
            if (j.is_null())
            {
                value.reset();
            }
            else
            {
                value = j.get<T>();
            }
        }
    };
}

namespace xpilot
{
    struct NetworkAircraftConfigLights {
        std::optional<bool> StrobesOn;
        std::optional<bool> LandingOn;
        std::optional<bool> TaxiOn;
        std::optional<bool> BeaconOn;
        std::optional<bool> NavOn;
    };

    struct NetworkAircraftConfigData {
        std::string Callsign;
        std::optional<NetworkAircraftConfigLights> Lights;
        std::optional<bool> EnginesRunning;
        std::optional<bool> ReverseThrust;
        std::optional<bool> OnGround;
        std::optional<bool> SpoilersDeployed;
        std::optional<bool> GearDown;
        std::optional<float> FlapsPct;
    };

    struct NetworkAircraftConfig {
        std::string Type;
        NetworkAircraftConfigData Data;
    };

    void from_json(const json& j, NetworkAircraftConfigData& data);
    void from_json(const json& j, NetworkAircraftConfigLights& lights);
    void from_json(const json& j, NetworkAircraftConfig& config);
}

#endif // !NetworkAircraftConfig_h
