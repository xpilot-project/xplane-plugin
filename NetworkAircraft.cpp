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

#include "NetworkAircraft.h"
#include "Utilities.h"
#include "Config.h"

namespace xpilot
{
    NetworkAircraft::NetworkAircraft(const std::string& _icaoType, const std::string& _icaoAirline, const std::string& _livery,
        XPMPPlaneID _modeS_id = 0, const std::string& _modelName = "") :
        XPMP2::Aircraft(_icaoType, _icaoAirline, _livery, _modeS_id, _modelName),
        EnginesRunning(false),
        GearDown(false),
        OnGround(false),
        RenderCount(0),
        ReverseThrust(false),
        SpoilersDeployed(false),
        TargetFlapPosition(0.0f),
        TargetGearPosition(0.0f),
        TargetSpoilerPosition(0.0f),
        TargetReversersPosition(0.0f),
        TerrainAltitude(0.0)
    {

    }

    void NetworkAircraft::UpdatePosition(float, int)
    {
        label = Callsign;

        if (Callsign.length() > 7) 
        {
            strScpy(acInfoTexts.tailNum, Callsign.substr(0, 7).c_str(), sizeof(acInfoTexts.tailNum));
        }
        else 
        {
            strScpy(acInfoTexts.tailNum, Callsign.c_str(), sizeof(acInfoTexts.tailNum));
        }
        strScpy(acInfoTexts.icaoAcType, acIcaoType.c_str(), sizeof(acInfoTexts.icaoAcType));
        strScpy(acInfoTexts.icaoAirline, acIcaoAirline.c_str(), sizeof(acInfoTexts.icaoAirline));

        HexToRgb(Config::Instance().GetAircraftLabelColor(), colLabel);

        SetLocation(Position.lat, Position.lon, Position.elevation);

        SetHeading(Position.heading);
        SetPitch(Position.pitch);
        SetRoll(Position.roll);

        const auto now = std::chrono::system_clock::now();
        static const float epsilon = std::numeric_limits<float>::epsilon();
        const auto diffMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - PreviousSurfaceUpdateTime);

        TargetGearPosition = GearDown ? 1.0f : 0.0f;
        TargetSpoilerPosition = SpoilersDeployed ? 1.0f : 0.0f;
        TargetReversersPosition = ReverseThrust ? 1.0f : 0.0f;

        if (RenderCount <= 2) 
        {
            // we don't want to wait for the animation on first load...
            // looks particular funny with the gear extending after the aircraft
            // loads on the ground
            Surfaces.gearPosition = TargetGearPosition;
            Surfaces.flapRatio = TargetFlapPosition;
            Surfaces.spoilerRatio = TargetSpoilerPosition;
            Surfaces.reversRatio = TargetReversersPosition;
        }
        else 
        {
            const float f = Surfaces.gearPosition - TargetGearPosition;
            if (std::abs(f) > epsilon) {
                // interpolate gear position
                constexpr float gearMoveTimeMs = 10000;
                const auto gearPositionDiffRemaining = TargetGearPosition - Surfaces.gearPosition;

                const auto gearPositionDiffThisFrame = (diffMs.count()) / gearMoveTimeMs;
                Surfaces.gearPosition += std::copysign(gearPositionDiffThisFrame, gearPositionDiffRemaining);
                Surfaces.gearPosition = (std::max)(0.0f, (std::min)(Surfaces.gearPosition, 1.0f));
            }

            const float f2 = Surfaces.flapRatio - TargetFlapPosition;
            if (std::abs(f2) > epsilon)
            {
                // interpolate flap position
                constexpr float flapMoveTimeMs = 10000;
                const auto flapPositionDiffRemaining = TargetFlapPosition - Surfaces.flapRatio;

                const auto flapPositionDiffThisFrame = (diffMs.count()) / flapMoveTimeMs;
                Surfaces.flapRatio += std::copysign(flapPositionDiffThisFrame, flapPositionDiffRemaining);
                Surfaces.flapRatio = (std::max)(0.0f, (std::min)(Surfaces.flapRatio, 1.0f));
            }

            const float f3 = Surfaces.spoilerRatio - TargetSpoilerPosition;
            if (std::abs(f3) > epsilon)
            {
                // interpolate spoiler position
                constexpr float spoilerMoveTimeMs = 2000;
                const auto spoilerPositionDiffRemaining = TargetSpoilerPosition - Surfaces.spoilerRatio;

                const auto spoilerPositionDiffThisFrame = (diffMs.count()) / spoilerMoveTimeMs;
                Surfaces.spoilerRatio += std::copysign(spoilerPositionDiffThisFrame, spoilerPositionDiffRemaining);
                Surfaces.spoilerRatio = (std::max)(0.0f, (std::min)(Surfaces.spoilerRatio, 1.0f));
            }

            const float f4 = Surfaces.reversRatio - TargetReversersPosition;
            if (std::abs(f4) > epsilon)
            {
                constexpr float reversersMoveTimeMs = 4000;
                const auto reverserPositionDiffRemaining = TargetReversersPosition - Surfaces.reversRatio;

                const auto reverserPositionDiffThisFrame = (diffMs.count()) / reversersMoveTimeMs;
                Surfaces.reversRatio += std::copysign(reverserPositionDiffThisFrame, reverserPositionDiffRemaining);
                Surfaces.reversRatio = (std::max)(0.0f, (std::min)(Surfaces.reversRatio, 1.0f));
            }

            PreviousSurfaceUpdateTime = now;
        }

        SetGearRatio(Surfaces.gearPosition);
        SetFlapRatio(Surfaces.flapRatio);
        if (Surfaces.flapRatio <= 0.25f) 
        {
            SetSlatRatio((std::min)(Surfaces.flapRatio / 4, 0.0f));
        }
        else 
        {
            SetSlatRatio(Surfaces.flapRatio);
        }
        SetSpoilerRatio(Surfaces.spoilerRatio);
        SetSpeedbrakeRatio(Surfaces.spoilerRatio);
        SetWingSweepRatio(0.0f);
        SetThrustRatio(EnginesRunning ? 1.0f : 0.0f);
        SetYokePitchRatio(0.0f);
        SetYokeHeadingRatio(0.0f);
        SetYokeRollRatio(0.0f);
        SetThrustReversRatio(Surfaces.reversRatio * -1);

        SetLightsTaxi(Surfaces.lights.taxiLights);
        SetLightsLanding(Surfaces.lights.landLights);
        SetLightsBeacon(Surfaces.lights.bcnLights);
        SetLightsStrobe(Surfaces.lights.strbLights);
        SetLightsNav(Surfaces.lights.navLights);

        SetTireDeflection(0.0f);
        SetTireRotAngle(0.0f);
        SetTireRotRpm(0.0f);

        SetEngineRotRpm(0.0f);
        SetPropRotRpm(0.0f);
        SetEngineRotAngle(0.0f);
        SetPropRotAngle(0.0f);

        SetReversDeployRatio(0.0f);
        SetTouchDown(false);
    }

    void NetworkAircraft::CopyBulkData(XPilotAPIAircraft::XPilotAPIBulkData* pOut, size_t size) const
    {
        double lat, lon, alt;
        GetLocation(lat, lon, alt);

        pOut->keyNum = modeS_id;
        pOut->lat = lat;
        pOut->lon = lon;
        pOut->alt_ft = alt;
        pOut->pitch = GetPitch();
        pOut->roll = GetRoll();
        pOut->terrainAlt_ft = (float)TerrainAltitude;
        pOut->speed_kt = (float)GroundSpeed;
        pOut->heading = GetHeading();
        pOut->flaps = (float)Surfaces.flapRatio;
        pOut->gear = (float)Surfaces.gearPosition;
        pOut->bearing = GetCameraBearing();
        pOut->dist_nm = GetCameraDist();
        pOut->bits.taxi = GetLightsTaxi();
        pOut->bits.land = GetLightsLanding();
        pOut->bits.bcn = GetLightsBeacon();
        pOut->bits.strb = GetLightsStrobe();
        pOut->bits.nav = GetLightsNav();
        pOut->bits.onGnd = OnGround;
        pOut->bits.filler1 = 0;
        pOut->bits.multiIdx = GetTcasTargetIdx();
        pOut->bits.filler2 = 0;
        pOut->bits.filler3 = 0;
    }

    void NetworkAircraft::CopyBulkData(XPilotAPIAircraft::XPilotAPIBulkInfoTexts* pOut, size_t size) const
    {
        pOut->keyNum = modeS_id;
        STRCPY_ATMOST(pOut->callSign, Callsign);
        STRCPY_ATMOST(pOut->modelIcao, acIcaoType);
        STRCPY_ATMOST(pOut->cslModel, GetModelName());
        STRCPY_ATMOST(pOut->acClass, GetModelInfo().doc8643Classification);
        STRCPY_ATMOST(pOut->wtc, GetModelInfo().doc8643WTC);
        if (Radar.code > 0 || Radar.code <= 9999) 
        {
            char s[10];
            snprintf(s, sizeof(s), "%04ld", Radar.code);
            STRCPY_ATMOST(pOut->squawk, std::string(s));
        }
        STRCPY_ATMOST(pOut->origin, Origin);
        STRCPY_ATMOST(pOut->destination, Destination);
    }
}