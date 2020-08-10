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

#ifndef XPilot_h
#define XPilot_h

#include <deque>
#include <thread>
#include <mutex>
#include <functional>
#include <map>
#include <atomic>

#include "imgui.h"
#include "imgui_stdlib.h"
#include "imfilebrowser.h"
#include "XPImgWindow.h"

#include "DataRefAccess.h"
#include "OwnedDataRef.h"
#include "TextMessageConsole.h"
#include "Lib/ZMQ/zmq.hpp"

#include "XPLMMenus.h"
#include "XPLMUtilities.h"
#include "XPLMProcessing.h"

namespace xpilot 
{
	enum dataRefs {
		DR_BULK_QUICK,
		DR_BULK_EXPENSIVE,
		DR_NUM_AIRCRAFT
	};
	enum CommandState {
		START,
		CONTINUE,
		END
	};

	class FrameRateMonitor;
	class AircraftManager;
	class NotificationPanel;
	class TextMessageConsole;
	class NearbyATCWindow;
	class PreferencesWindow;

	class XPilot
	{
	public:
		XPilot();
		~XPilot();

		void StartZmqServer();
		void StopZmqServer();

		void AddConsoleMessage(const std::string& msg, double red = 255, double green = 255, double blue = 255);
		void AddConsoleMessageTab(const std::string& recipient, const std::string& msg, ConsoleTabType tabType);
		void AddNotificationPanelMessage(const std::string& msg, double red = 255, double green = 255, double blue = 255);
		void AddNotification(const std::string& msg, double red = 255, double green = 255, double blue = 255); // add notification to console and notification panel

		bool SendSocketMsg(const std::string& string);

		void OnNetworkDisconnected();
		void OnNetworkConnected();
		void ForceDisconnect(std::string reason = "");
		void OnPluginDisabled();
		void RequestControllerAtis(std::string callsign);

		std::string OurCallsign()const { return networkCallsign; }
		bool NetworkConnected()const { return networkLoginStatus; }
		void SetPttActive(bool active) { pttPressed = active; }

		void DisableDefaultAtis(bool disabled);
		bool IsDefaultAtisDisabled()const { return !xplaneAtisEnabled; }

		void TryGetTcasControl();
		void ReleaseTcasControl();

		void IncNumAc();
		void DecNumAc();

		void QueueCallback(std::function<void()> cb);

		void TogglePreferencesWindow();
		void ToggleNearbyAtcWindow();
		void ToggleTextMessageConsole();
		void SetNotificationPanelAlwaysVisible(bool visible);
		bool IsNotificationPanelAlwaysVisible()const;

	protected:
		OwnedDataRef<int> pttPressed;
		OwnedDataRef<int> networkLoginStatus;
		OwnedDataRef<std::string> networkCallsign;
		OwnedDataRef<int> rxCom1;
		OwnedDataRef<int> rxCom2;
		OwnedDataRef<float> volumeSignalLevel;
		OwnedDataRef<int> aiControlled;
		OwnedDataRef<int> numAircraft;
		OwnedDataRef<int> pluginVersion;
		DataRefAccess<int> xplaneAtisEnabled;

	private:
		std::thread::id xplaneThread;
		void ThisThreadIsXP() { xplaneThread = std::this_thread::get_id(); }
		bool IsXPThread()const { return std::this_thread::get_id() == xplaneThread; }

		std::mutex envMutex;
		std::atomic_bool keepAlive{ false };
		std::unique_ptr<std::thread> zmqThread;
		std::unique_ptr<zmq::context_t> zmqContext;
		std::unique_ptr<zmq::socket_t> zmqSocket;

		std::string pluginHash;

		void ZmqListener();
		bool IsSocketConnected()const { return zmqSocket && zmqSocket->connected(); }
		bool IsSocketReady()const { return keepAlive && IsSocketConnected(); }

		std::mutex queueMutex;
		std::vector<std::function<void()>> queuedCallbacks;
		void ProcessQueuedCallbacks();

		static float deferredStartup(float, float, int, void* ref);
		static float onFlightLoop(float, float, int, void* ref);
		
		bool InitializeXPMP();

		XPLMDataRef bulkDataQuick{}, bulkDataExpensive{};
		static int GetBulkData(void* inRefcon, void* outData, int inStartPos, int inNumBytes);
		int aircraftCount = 1; // we count ourselves as 1

		std::unique_ptr<FrameRateMonitor> frameRateMonitor;
		std::unique_ptr<AircraftManager> aircraftManager;
		std::unique_ptr<NotificationPanel> notificationPanel;
		std::unique_ptr<TextMessageConsole> textMessageConsole;
		std::unique_ptr<NearbyATCWindow> nearbyAtcWindow;
		std::unique_ptr<PreferencesWindow> preferencesWindow;
	};
}

#endif // !XPilot_h