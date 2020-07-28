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

#ifndef FrameRateMonitor_h
#define FrameRateMonitor_h

#include <atomic>
#include <vector>
#include <sstream>

#include "DataRefAccess.h"
#include "Stopwatch.h"
#include "XPLMProcessing.h"

namespace xpilot
{
	class XPilot;

    class FrameRateMonitor
    {
	public:
		FrameRateMonitor(XPilot* env);
		void StopMonitoring();
		void StartMonitoring();
	protected:
		DataRefAccess<float> drFrameRatePeriod;
		DataRefAccess<float> drGroundSpeed;
		DataRefAccess<int> drIsExternalVisual;
		DataRefAccess<std::vector<int>> drOverridePlanePath;
		DataRefAccess<int> drTimePaused;
	private:
		XPilot* environment;
		Stopwatch mStopwatch;
		bool mGaveFirstWarning;
		bool mGaveSecondWarning;
		bool mGaveDisconnectWarning;
		bool mGaveHealthyWarning;
		bool SkipMonitoring();
		void ResetFrameRateDetection();
		static float flightLoopCallback(float, float, int, void* ref);
    };
}

#endif // !FrameRateMonitor_h
