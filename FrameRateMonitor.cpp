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

#include "FrameRateMonitor.h"
#include "Utilities.h"
#include "XPilot.h"

namespace xpilot
{
    FrameRateMonitor::FrameRateMonitor(XPilot* env) :
		environment(env),
        drFrameRatePeriod("sim/operation/misc/frame_rate_period", ReadOnly),
        drGroundSpeed("sim/flightmodel/position/groundspeed", ReadOnly),
        drIsExternalVisual("sim/network/dataout/is_external_visual", ReadOnly),
        drOverridePlanePath("sim/operation/override/override_planepath", ReadOnly),
        drTimePaused("sim/time/paused", ReadOnly),
        mGaveFirstWarning(false),
        mGaveSecondWarning(false),
        mGaveDisconnectWarning(false),
        mGaveHealthyWarning(false)
    {

    }
    
	void FrameRateMonitor::StartMonitoring()
	{
		ResetFrameRateDetection();
		XPLMRegisterFlightLoopCallback(flightLoopCallback, 30.0f, this); // give 30 second "warm-up" period after connecting
	}

	void FrameRateMonitor::StopMonitoring()
	{
		ResetFrameRateDetection();
		XPLMUnregisterFlightLoopCallback(flightLoopCallback, this);
	}

	float FrameRateMonitor::flightLoopCallback(float, float, int, void* ref)
	{
		auto* monitor = static_cast<FrameRateMonitor*>(ref);

		if (monitor->SkipMonitoring())
			return -1.0f;

		float fps = 1 / monitor->drFrameRatePeriod;

		if (fps < 20.0f) 
		{
			float elapsed = monitor->mStopwatch.elapsed(xpilot::Stopwatch::SECONDS);
			if (!monitor->mStopwatch.isRunning()) 
			{
				monitor->mStopwatch.start();
			}

			monitor->mGaveHealthyWarning = false;

			std::stringstream warningMsg;
			warningMsg
				<< "X-Plane is not running in real-time because your frame rate is less than 20fps."
				<< " xPilot will automatically disconnect in "
				<< static_cast<int>(floor(30 - elapsed))
				<< " seconds if this is not corrected.";

			if (elapsed >= 10 && elapsed < 20) 
			{
				if (!monitor->mGaveFirstWarning) 
				{
					monitor->environment->AddNotificationPanelMessage(warningMsg.str(), 241, 196, 15);
					monitor->environment->AddConsoleMessage(warningMsg.str(), 241, 196, 15);
					LOG_INFO(warningMsg.str().c_str());
					monitor->mGaveFirstWarning = true;
				}
			}
			else if (elapsed >= 20 && elapsed < 30) 
			{
				if (monitor->mGaveFirstWarning && !monitor->mGaveSecondWarning) 
				{
					monitor->environment->AddNotificationPanelMessage(warningMsg.str(), 241, 196, 15);
					monitor->environment->AddConsoleMessage(warningMsg.str(), 241, 196, 15);
					LOG_INFO(warningMsg.str().c_str());
					monitor->mGaveSecondWarning = true;
				}
			}
			else if (elapsed >= 30) 
			{
				if (monitor->mGaveFirstWarning && monitor->mGaveSecondWarning && !monitor->mGaveDisconnectWarning) 
				{
					std::string msg = "Disconnecting from VATSIM because your frame rates have been less than 20fps for more than 30 seconds. Please adjust your X-Plane performance before reconnecting to the network.";
					monitor->environment->AddNotificationPanelMessage(msg, 241, 196, 15);
					monitor->environment->AddConsoleMessage(msg, 241, 196, 15);
					monitor->environment->ForceDisconnect(msg);
					LOG_INFO(msg.c_str());
					monitor->mGaveDisconnectWarning = true;
				}
			}
		}
		else 
		{
			if ((monitor->mGaveFirstWarning || monitor->mGaveSecondWarning) && !monitor->mGaveHealthyWarning) 
			{
				std::string msg = "X-Plane is now running in real time. The automatic disconnect has been cancelled.";
				monitor->environment->AddNotificationPanelMessage(msg, 241, 196, 15);
				monitor->environment->AddConsoleMessage(msg, 241, 196, 15);
				LOG_INFO(msg.c_str());
				monitor->mGaveHealthyWarning = true;
			}
			monitor->mGaveFirstWarning = false;
			monitor->mGaveSecondWarning = false;
			monitor->mGaveDisconnectWarning = false;
			monitor->mStopwatch.reset();
		}

		return -1.0;
	}

    void FrameRateMonitor::ResetFrameRateDetection()
    {
        mGaveFirstWarning = false;
        mGaveSecondWarning = false;
        mGaveDisconnectWarning = false;
        mGaveHealthyWarning = false;
        mStopwatch.reset();
    }

    bool FrameRateMonitor::SkipMonitoring()
    {
        return drGroundSpeed < 10.0f || drIsExternalVisual == 1 || drOverridePlanePath[0] == 1 || drTimePaused == 1;
    }
}