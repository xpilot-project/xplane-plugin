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

namespace xpilot
{
    class NetworkAircraft : public XPMP2::Aircraft
    {
    public:
        NetworkAircraft(const std::string& _icaoType, const std::string& _icaoAirline, 
            const std::string& _livery, XPMPPlaneID _modeS_id, const std::string& _modelName);

        void copyBulkData(XPilotAPIAircraft::XPilotAPIBulkData* pOut, size_t size) const;
        void copyBulkData(XPilotAPIAircraft::XPilotAPIBulkInfoTexts* pOut, size_t size) const;

        std::string callsign;
        bool onGround;
        bool gearDown;
        bool enginesRunning;
        bool reverseThrust;
        XPMPPlaneSurfaces_t surfaces;
        XPMPPlanePosition_t position;
        XPMPPlaneRadar_t radar;
        std::deque<InterpolatedState> interpolationStack;
        TerrainProbe terrainProbe;
        float groundSpeed;
        double terrainAltitude;
        float targetGearPosition;
        float targetFlapPosition;
        float targetSpoilerPosition;
        float targetReversersPosition;
        bool spoilersDeployed;
        int renderCount;
        std::string origin;
        std::string destination;
        std::chrono::system_clock::time_point previousSurfaceUpdateTime;

    protected:
        virtual void UpdatePosition(float, int);
    };
}

#endif // !NetworkAircraft_h


