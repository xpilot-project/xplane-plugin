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

#ifndef NetworkAircraft_h
#define NetworkAircraft_h

#include <deque>

#include "XPilotAPI.h"
#include "InterpolatedState.h"
#include "TerrainProbe.h"

#include "XPCAircraft.h"
#include "XPMPAircraft.h"
#include "XPMPMultiplayer.h"

#include "Vector3.hpp"

namespace xpilot
{
    struct AircraftVisualState
    {
        double Lat;
        double Lon;
        double Altitude;
        double Pitch;
        double Heading;
        double Bank;
    };

    class NetworkAircraft : public XPMP2::Aircraft
    {
    public:
        NetworkAircraft(const std::string& _callsign, const AircraftVisualState& _visualState, const std::string& _icaoType, const std::string& _icaoAirline, const std::string& _livery, XPMPPlaneID _modeS_id, const std::string& _modelName);

        void copyBulkData(XPilotAPIAircraft::XPilotAPIBulkData* pOut, size_t size) const;
        void copyBulkData(XPilotAPIAircraft::XPilotAPIBulkInfoTexts* pOut, size_t size) const;

        void UpdateErrorVectors(double interval);

        bool on_ground;
        bool gear_down;
        bool engines_running;
        bool reverse_thrust;
        XPMPPlaneSurfaces_t surfaces;
        XPMPPlanePosition_t position;
        XPMPPlaneRadar_t radar;
        TerrainProbe terrain_probe;
        float ground_speed;
        float target_gear_position;
        float target_flaps_position;
        float target_spoiler_position;
        bool spoilersDeployed;
        int fast_positions_received_count;
        std::string origin;
        std::string destination;
        std::chrono::system_clock::time_point previousSurfaceUpdateTime;

        double terrain_altitude;
        bool just_touched_down;

        AircraftVisualState remote_visual_state;
        AircraftVisualState current_visual_state;

        Vector3 positional_velocity_vector;
        Vector3 positional_velocity_vector_error;
        Vector3 rotational_velocity_vector;
        Vector3 rotational_velocity_vector_error;

        std::chrono::steady_clock::time_point last_fast_position_timestamp;
        std::chrono::steady_clock::time_point last_slow_position_timestamp;

    protected:
        virtual void UpdatePosition(float, int);
        void Extrapolate(Vector3 velocityVector, Vector3 rotationVector, double interval);
        static double NormalizeDegrees(double value, double lowerBound, double upperBound);
    };
}

#endif // !NetworkAircraft_h


