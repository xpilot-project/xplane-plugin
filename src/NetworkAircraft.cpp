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
        enginesRunning(false),
        gearDown(false),
        onGround(false),
        renderCount(0),
        reverseThrust(false),
        spoilersDeployed(false),
        targetFlapPosition(0.0f),
        targetGearPosition(0.0f),
        targetSpoilerPosition(0.0f),
        targetReversersPosition(0.0f),
        terrainAltitude(0.0),
        groundSpeed(0.0)
    {

    }

    void NetworkAircraft::UpdatePosition(float, int)
    {
        label = callsign;
        if (callsign.length() > 7)
        {
            strScpy(acInfoTexts.tailNum, callsign.substr(0, 7).c_str(), sizeof(acInfoTexts.tailNum));
        }
        else
        {
            strScpy(acInfoTexts.tailNum, callsign.c_str(), sizeof(acInfoTexts.tailNum));
        }
        strScpy(acInfoTexts.icaoAcType, acIcaoType.c_str(), sizeof(acInfoTexts.icaoAcType));
        strScpy(acInfoTexts.icaoAirline, acIcaoAirline.c_str(), sizeof(acInfoTexts.icaoAirline));

        HexToRgb(Config::Instance().getAircraftLabelColor(), colLabel);

        SetLocation(position.lat, position.lon, position.elevation);

        SetHeading(position.heading);
        SetPitch(position.pitch);
        SetRoll(position.roll);

        const auto now = std::chrono::system_clock::now();
        static const float epsilon = std::numeric_limits<float>::epsilon();
        const auto diffMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - previousSurfaceUpdateTime);

        targetGearPosition = gearDown ? 1.0f : 0.0f;
        targetSpoilerPosition = spoilersDeployed ? 1.0f : 0.0f;
        targetReversersPosition = reverseThrust ? 1.0f : 0.0f;

        if (renderCount <= 2)
        {
            // we don't want to wait for the animation on first load...
            // looks particular funny with the gear extending after the aircraft
            // loads on the ground
            surfaces.gearPosition = targetGearPosition;
            surfaces.flapRatio = targetFlapPosition;
            surfaces.spoilerRatio = targetSpoilerPosition;
            surfaces.reversRatio = targetReversersPosition;
        }
        else
        {
            const float f = surfaces.gearPosition - targetGearPosition;
            if (std::abs(f) > epsilon)
            {
                // interpolate gear position
                constexpr float gearMoveTimeMs = 10000;
                const auto gearPositionDiffRemaining = targetGearPosition - surfaces.gearPosition;

                const auto gearPositionDiffThisFrame = (diffMs.count()) / gearMoveTimeMs;
                surfaces.gearPosition += std::copysign(gearPositionDiffThisFrame, gearPositionDiffRemaining);
                surfaces.gearPosition = (std::max)(0.0f, (std::min)(surfaces.gearPosition, 1.0f));
            }

            const float f2 = surfaces.flapRatio - targetFlapPosition;
            if (std::abs(f2) > epsilon)
            {
                // interpolate flap position
                constexpr float flapMoveTimeMs = 10000;
                const auto flapPositionDiffRemaining = targetFlapPosition - surfaces.flapRatio;

                const auto flapPositionDiffThisFrame = (diffMs.count()) / flapMoveTimeMs;
                surfaces.flapRatio += std::copysign(flapPositionDiffThisFrame, flapPositionDiffRemaining);
                surfaces.flapRatio = (std::max)(0.0f, (std::min)(surfaces.flapRatio, 1.0f));
            }

            const float f3 = surfaces.spoilerRatio - targetSpoilerPosition;
            if (std::abs(f3) > epsilon)
            {
                // interpolate spoiler position
                constexpr float spoilerMoveTimeMs = 2000;
                const auto spoilerPositionDiffRemaining = targetSpoilerPosition - surfaces.spoilerRatio;

                const auto spoilerPositionDiffThisFrame = (diffMs.count()) / spoilerMoveTimeMs;
                surfaces.spoilerRatio += std::copysign(spoilerPositionDiffThisFrame, spoilerPositionDiffRemaining);
                surfaces.spoilerRatio = (std::max)(0.0f, (std::min)(surfaces.spoilerRatio, 1.0f));
            }

            const float f4 = surfaces.reversRatio - targetReversersPosition;
            if (std::abs(f4) > epsilon)
            {
                constexpr float reversersMoveTimeMs = 4000;
                const auto reverserPositionDiffRemaining = targetReversersPosition - surfaces.reversRatio;

                const auto reverserPositionDiffThisFrame = (diffMs.count()) / reversersMoveTimeMs;
                surfaces.reversRatio += std::copysign(reverserPositionDiffThisFrame, reverserPositionDiffRemaining);
                surfaces.reversRatio = (std::max)(0.0f, (std::min)(surfaces.reversRatio, 1.0f));
            }

            previousSurfaceUpdateTime = now;
        }

        SetGearRatio(surfaces.gearPosition);
        SetFlapRatio(surfaces.flapRatio);
        if (surfaces.flapRatio <= 0.25f)
        {
            SetSlatRatio((std::min)(surfaces.flapRatio / 4, 0.0f));
        }
        else
        {
            SetSlatRatio(surfaces.flapRatio);
        }
        SetSpoilerRatio(surfaces.spoilerRatio);
        SetSpeedbrakeRatio(surfaces.spoilerRatio);
        SetWingSweepRatio(0.0f);
        SetThrustRatio(enginesRunning ? 1.0f : 0.0f);
        SetYokePitchRatio(0.0f);
        SetYokeHeadingRatio(0.0f);
        SetYokeRollRatio(0.0f);
        SetThrustReversRatio(surfaces.reversRatio * -1);

        SetLightsTaxi(surfaces.lights.taxiLights);
        SetLightsLanding(surfaces.lights.landLights);
        SetLightsBeacon(surfaces.lights.bcnLights);
        SetLightsStrobe(surfaces.lights.strbLights);
        SetLightsNav(surfaces.lights.navLights);

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

    void NetworkAircraft::copyBulkData(XPilotAPIAircraft::XPilotAPIBulkData* pOut, size_t size) const
    {
        double lat, lon, alt;
        GetLocation(lat, lon, alt);

        pOut->keyNum = modeS_id;
        pOut->lat = lat;
        pOut->lon = lon;
        pOut->alt_ft = alt;
        pOut->pitch = GetPitch();
        pOut->roll = GetRoll();
        pOut->terrainAlt_ft = (float)terrainAltitude;
        pOut->speed_kt = (float)groundSpeed;
        pOut->heading = GetHeading();
        pOut->flaps = (float)surfaces.flapRatio;
        pOut->gear = (float)surfaces.gearPosition;
        pOut->bearing = GetCameraBearing();
        pOut->dist_nm = GetCameraDist();
        pOut->bits.taxi = GetLightsTaxi();
        pOut->bits.land = GetLightsLanding();
        pOut->bits.bcn = GetLightsBeacon();
        pOut->bits.strb = GetLightsStrobe();
        pOut->bits.nav = GetLightsNav();
        pOut->bits.onGnd = onGround;
        pOut->bits.filler1 = 0;
        pOut->bits.multiIdx = GetTcasTargetIdx();
        pOut->bits.filler2 = 0;
        pOut->bits.filler3 = 0;
    }

    void NetworkAircraft::copyBulkData(XPilotAPIAircraft::XPilotAPIBulkInfoTexts* pOut, size_t size) const
    {
        pOut->keyNum = modeS_id;
        STRCPY_ATMOST(pOut->callSign, callsign);
        STRCPY_ATMOST(pOut->modelIcao, acIcaoType);
        STRCPY_ATMOST(pOut->cslModel, GetModelName());
        STRCPY_ATMOST(pOut->acClass, GetModelInfo().doc8643Classification);
        STRCPY_ATMOST(pOut->wtc, GetModelInfo().doc8643WTC);
        if (radar.code > 0 || radar.code <= 9999)
        {
            char s[10];
            snprintf(s, sizeof(s), "%04ld", radar.code);
            STRCPY_ATMOST(pOut->squawk, std::string(s));
        }
        STRCPY_ATMOST(pOut->origin, origin);
        STRCPY_ATMOST(pOut->destination, destination);
    }
}