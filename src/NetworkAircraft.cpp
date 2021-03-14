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
#include "GeoCalc.hpp"
#include "Quaternion.hpp"

namespace xpilot
{
    double CalculateNormalizedDelta(double start, double end, double lowerBound, double upperBound)
    {
        double range = upperBound - lowerBound;
        double halfRange = range / 2.0;

        if (abs(end - start) > halfRange)
        {
            end += (end > start ? -range : range);
        }

        return end - start;
    }

    double HeadingDiff(double head1, double head2)
    {
        if (std::abs(head2 - head1) > 180)
        {
            if (head1 < head2)
            {
                head1 += 360;
            }
            else
            {
                head2 += 360;
            }
        }
        return head2 - head1;
    }

    NetworkAircraft::NetworkAircraft(
        const std::string& _callsign,
        const AircraftVisualState& _visualState,
        const std::string& _icaoType, 
        const std::string& _icaoAirline, 
        const std::string& _livery,
        XPMPPlaneID _modeS_id = 0,
        const std::string& _modelName = "") :
        XPMP2::Aircraft(_icaoType, _icaoAirline, _livery, _modeS_id, _modelName),
        engines_running(false),
        gear_down(false),
        on_ground(false),
        fast_positions_received_count(0),
        reverse_thrust(false),
        spoilersDeployed(false),
        target_flaps_position(0.0f),
        target_gear_position(0.0f),
        target_spoiler_position(0.0f),
        ground_speed(0.0)
    {
        label = _callsign;
        strScpy(acInfoTexts.tailNum, _callsign.c_str(), sizeof(acInfoTexts.tailNum));
        strScpy(acInfoTexts.icaoAcType, acIcaoType.c_str(), sizeof(acInfoTexts.icaoAcType));
        strScpy(acInfoTexts.icaoAirline, acIcaoAirline.c_str(), sizeof(acInfoTexts.icaoAirline));

        SetLocation(_visualState.Lat, _visualState.Lon, _visualState.Altitude);
        SetHeading(_visualState.Heading);
        SetPitch(_visualState.Pitch);
        SetRoll(_visualState.Bank);

        last_slow_position_timestamp = std::chrono::steady_clock::now();
        current_visual_state = _visualState;
        remote_visual_state = _visualState;
        positional_velocity_vector = Vector3::Zero();
        rotational_velocity_vector = Vector3::Zero();

        SetVisible(false);
    }

    void NetworkAircraft::Extrapolate(
        Vector3 velocityVector,
        Vector3 rotationVector,
        double interval)
    {
        double lat_change = MetersToDegrees(velocityVector.Z * interval);
        double new_lat = NormalizeDegrees(current_visual_state.Lat + lat_change, -90.0, 90.0);

        double lon_change = MetersToDegrees(velocityVector.X * interval / LongitudeScalingFactor(current_visual_state.Lat));
        double new_lon = NormalizeDegrees(current_visual_state.Lon + lon_change, -180.0, 180.0);

        double alt_change = velocityVector.Y * interval * 3.28084;

        // Terrain offset
        terrain_altitude = 0.0;
        if (current_visual_state.Altitude < 18000)
        {
            terrain_altitude = terrain_probe.getTerrainElevation(current_visual_state.Lat, current_visual_state.Lon);
            if (on_ground || (current_visual_state.Altitude < 200.0 && current_visual_state.Altitude < terrain_altitude))
            {
                if (fast_positions_received_count > 1)
                {
                    const double diff = (terrain_altitude - (current_visual_state.Altitude + alt_change)) * interval;
                    if (std::abs(diff) > 0.0)
                    {
                        current_visual_state.Altitude += std::copysign(diff, diff);
                    }

                    SetTouchDown(just_touched_down);
                    just_touched_down = false;
                }
                else
                {
                    current_visual_state.Altitude = terrain_altitude;
                }
            }
            else
            {
                current_visual_state.Altitude += alt_change;
            }
        }
        else
        {
            current_visual_state.Altitude += alt_change;
        }

        double new_alt = current_visual_state.Altitude;

        SetLocation(new_lat, new_lon, new_alt);

        Quaternion current_orientation = Quaternion::FromEuler(
            DegreesToRadians(GetPitch()),
            DegreesToRadians(GetHeading()),
            DegreesToRadians(GetRoll())
        );

        Quaternion rotation = Quaternion::FromEuler(
            rotationVector.X,
            rotationVector.Y,
            rotationVector.Z
        );

        Quaternion slerp = Quaternion::Slerp(
            Quaternion::Identity(),
            rotation,
            interval > 1.0 ? 1.0 : interval
        );

        Quaternion result = current_orientation * slerp;

        Vector3 new_orientation = Quaternion::ToEuler(result);

        double new_pitch = RadiansToDegrees(new_orientation.X);
        double new_heading = RadiansToDegrees(new_orientation.Y);
        double new_roll = RadiansToDegrees(new_orientation.Z);

        current_visual_state.Lat = new_lat;
        current_visual_state.Lon = new_lon;
        current_visual_state.Altitude = new_alt;
        current_visual_state.Pitch = new_pitch;
        current_visual_state.Bank = new_roll;
        current_visual_state.Heading = new_heading;

        SetPitch(new_pitch);
        SetHeading(new_heading);
        SetRoll(new_roll);
    }

    void NetworkAircraft::UpdateErrorVectors(double interval)
    {
        if (positional_velocity_vector == Vector3::Zero())
        {
            positional_velocity_vector_error = Vector3::Zero();
            rotational_velocity_vector_error = Vector3::Zero();
            return;
        }

        double latDelta = DegreesToMeters(CalculateNormalizedDelta(
            current_visual_state.Lat,
            remote_visual_state.Lat,
            -90.0,
            90.0
        ));

        double lonDelta = DegreesToMeters(CalculateNormalizedDelta(
            current_visual_state.Lon,
            remote_visual_state.Lon,
            -180.0,
            180.0
        ));
        lonDelta *= LongitudeScalingFactor(remote_visual_state.Lat);

        double altDelta = (remote_visual_state.Altitude - current_visual_state.Altitude) * 0.3048;

        positional_velocity_vector_error = Vector3(
            lonDelta / interval,
            altDelta / interval,
            latDelta / interval
        );

        Quaternion currentOrientation = Quaternion::FromEuler(
            DegreesToRadians(GetPitch()), 
            DegreesToRadians(GetHeading()), 
            DegreesToRadians(GetRoll())
        );
        
        Quaternion targetOrientation = Quaternion::FromEuler(
            DegreesToRadians(remote_visual_state.Pitch), 
            DegreesToRadians(remote_visual_state.Heading), 
            DegreesToRadians(remote_visual_state.Bank)
        );

        Quaternion delta = Quaternion::Inverse(currentOrientation) * targetOrientation;

        Vector3 result = Quaternion::ToEuler(delta);

        rotational_velocity_vector_error = Vector3(result.X / interval, result.Y / interval, result.Z / interval);
    }

    double NetworkAircraft::NormalizeDegrees(double value, double lowerBound, double upperBound)
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

    void NetworkAircraft::UpdatePosition(float _elapsedSinceLastCall, int)
    {
        Extrapolate(
            positional_velocity_vector + positional_velocity_vector_error,
            rotational_velocity_vector + rotational_velocity_vector_error,
            _elapsedSinceLastCall
        );
        
        double rpm = (60 / (2 * M_PI * 3.2)) * positional_velocity_vector.X * -1;
        double rpmDeg = GetTireRotRpm() / 60.0 * _elapsedSinceLastCall * 360.0;

        SetTireRotRpm(rpm);
        SetTireRotAngle(GetTireRotAngle() + rpmDeg);

        const auto now = std::chrono::system_clock::now();
        static const float epsilon = std::numeric_limits<float>::epsilon();
        const auto diffMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - previousSurfaceUpdateTime);

        target_gear_position = gear_down ? 1.0f : 0.0f;
        target_spoiler_position = spoilersDeployed ? 1.0f : 0.0f;

        if (fast_positions_received_count <= 2)
        {
            // we don't want to wait for the animation on first load...
            // looks particular funny with the gear extending after the aircraft
            // loads on the ground
            surfaces.gearPosition = target_gear_position;
            surfaces.flapRatio = target_flaps_position;
            surfaces.spoilerRatio = target_spoiler_position;
        }
        else
        {
            const float f = surfaces.gearPosition - target_gear_position;
            if (std::abs(f) > epsilon)
            {
                // interpolate gear position
                constexpr float gearMoveTimeMs = 10000;
                const auto gearPositionDiffRemaining = target_gear_position - surfaces.gearPosition;

                const auto gearPositionDiffThisFrame = (diffMs.count()) / gearMoveTimeMs;
                surfaces.gearPosition += std::copysign(gearPositionDiffThisFrame, gearPositionDiffRemaining);
                surfaces.gearPosition = (std::max)(0.0f, (std::min)(surfaces.gearPosition, 1.0f));
            }

            const float f2 = surfaces.flapRatio - target_flaps_position;
            if (std::abs(f2) > epsilon)
            {
                // interpolate flap position
                constexpr float flapMoveTimeMs = 10000;
                const auto flapPositionDiffRemaining = target_flaps_position - surfaces.flapRatio;

                const auto flapPositionDiffThisFrame = (diffMs.count()) / flapMoveTimeMs;
                surfaces.flapRatio += std::copysign(flapPositionDiffThisFrame, flapPositionDiffRemaining);
                surfaces.flapRatio = (std::max)(0.0f, (std::min)(surfaces.flapRatio, 1.0f));
            }

            const float f3 = surfaces.spoilerRatio - target_spoiler_position;
            if (std::abs(f3) > epsilon)
            {
                // interpolate spoiler position
                constexpr float spoilerMoveTimeMs = 2000;
                const auto spoilerPositionDiffRemaining = target_spoiler_position - surfaces.spoilerRatio;

                const auto spoilerPositionDiffThisFrame = (diffMs.count()) / spoilerMoveTimeMs;
                surfaces.spoilerRatio += std::copysign(spoilerPositionDiffThisFrame, spoilerPositionDiffRemaining);
                surfaces.spoilerRatio = (std::max)(0.0f, (std::min)(surfaces.spoilerRatio, 1.0f));
            }

            previousSurfaceUpdateTime = now;
        }

        SetGearRatio(surfaces.gearPosition);
        SetFlapRatio(surfaces.flapRatio);
        SetSlatRatio(surfaces.flapRatio <= 0.25f ? (std::min)(surfaces.flapRatio / 4, 0.0f) : surfaces.flapRatio);
        SetSpoilerRatio(surfaces.spoilerRatio);
        SetSpeedbrakeRatio(surfaces.spoilerRatio);
        SetThrustRatio(engines_running ? 1.0f : 0.0f);

        SetLightsTaxi(surfaces.lights.taxiLights);
        SetLightsLanding(surfaces.lights.landLights);
        SetLightsBeacon(surfaces.lights.bcnLights);
        SetLightsStrobe(surfaces.lights.strbLights);
        SetLightsNav(surfaces.lights.navLights);

        HexToRgb(Config::Instance().getAircraftLabelColor(), colLabel);
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
        pOut->terrainAlt_ft = (float)terrain_altitude;
        pOut->speed_kt = (float)ground_speed;
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
        pOut->bits.onGnd = on_ground;
        pOut->bits.filler1 = 0;
        pOut->bits.multiIdx = GetTcasTargetIdx();
        pOut->bits.filler2 = 0;
        pOut->bits.filler3 = 0;
    }

    void NetworkAircraft::copyBulkData(XPilotAPIAircraft::XPilotAPIBulkInfoTexts* pOut, size_t size) const
    {
        pOut->keyNum = modeS_id;
        STRCPY_ATMOST(pOut->callSign, label);
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