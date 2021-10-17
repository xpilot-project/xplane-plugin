/*
 * xPilot: X-Plane pilot client for VATSIM
 * Copyright (C) 2019-2021 Justin Shannon
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
#include <algorithm>
#include <iostream>

#include "imgui.h"
#include "imgui_stdlib.h"
#include "imfilebrowser.h"
#include "XPImgWindow.h"
#include "DataRefAccess.h"
#include "OwnedDataRef.h"
#include "TextMessageConsole.h"
#include "XPLMMenus.h"
#include "XPLMUtilities.h"
#include "XPLMProcessing.h"

#include "zmq.hpp"

namespace xpilot
{
	enum dataRefs
	{
		DR_BULK_QUICK,
		DR_BULK_EXPENSIVE,
		DR_NUM_AIRCRAFT
	};

	class FrameRateMonitor;
	class AircraftManager;
	class NotificationPanel;
	class TextMessageConsole;
	class NearbyATCWindow;
	class SettingsWindow;

	class XPilot
	{
	public:
		XPilot();
		~XPilot();

		void RadioMessageReceived(const std::string& msg, double red = 255, double green = 255, double blue = 255);
		void AddPrivateMessage(const std::string& recipient, const std::string& msg, ConsoleTabType tabType);
		void AddNotificationPanelMessage(const std::string& msg, double red = 255, double green = 255, double blue = 255);
		void addNotification(const std::string& msg, double red = 255, double green = 255, double blue = 255);

		void onNetworkDisconnected();
		void onNetworkConnected();
		void forceDisconnect(std::string reason = "");
		void requestStationInfo(std::string callsign);

		std::string ourCallsign()const
		{
			return m_networkCallsign;
		}
		bool isNetworkConnected()const
		{
			return m_networkLoginStatus;
		}
		void setPttActive(bool active)
		{
			m_pttPressed = active;
		}

		void disableDefaultAtis(bool disabled);
		bool isDefaultAtisDisabled()const
		{
			return !m_xplaneAtisEnabled;
		}

		void TryGetTcasControl();
		void ReleaseTcasControl();

		void togglePreferencesWindow();
		void toggleNearbyAtcWindow();
		void toggleTextMessageConsole();
		void setNotificationPanelAlwaysVisible(bool visible);
		bool setNotificationPanelAlwaysVisible()const;

		void DeleteAllAircraft();

		void Initialize();
		void Shutdown();
	protected:
		OwnedDataRef<int> m_pttPressed;
		OwnedDataRef<int> m_networkLoginStatus;
		OwnedDataRef<std::string> m_networkCallsign;
		OwnedDataRef<int> m_rxCom1;
		OwnedDataRef<int> m_rxCom2;
		OwnedDataRef<float> m_volumeSignalLevel;
		OwnedDataRef<int> m_aiControlled;
		OwnedDataRef<int> m_aircraftCount;
		OwnedDataRef<int> m_pluginVersion;
		DataRefAccess<int> m_xplaneAtisEnabled;
		DataRefAccess<float> m_frameRatePeriod;

	private:
		static float deferredStartup(float, float, int, void* ref);
		static float mainFlightLoop(float, float, int, void* ref);
		bool InitializeXPMP();

		std::thread::id m_xplaneThread;
		void thisThreadIsXP()
		{
			m_xplaneThread = std::this_thread::get_id();
		}
		bool isXPThread()const
		{
			return std::this_thread::get_id() == m_xplaneThread;
		}

		bool m_keepAlive;
		std::unique_ptr<std::thread> m_zmqThread;
		std::unique_ptr<zmq::context_t> m_zmqContext;
		std::unique_ptr<zmq::socket_t> m_zmqSocket;

		void zmqWorker();
		bool isSocketConnected()const
		{
			return m_zmqSocket && m_zmqSocket->connected();
		}
		bool isSocketReady()const
		{
			return m_keepAlive && isSocketConnected();
		}

		std::mutex m_mutex;
		std::deque<std::function<void()>> m_queuedCallbacks;
		void invokeQueuedCallbacks();
		void queueCallback(const std::function<void()>& cb);

		XPLMDataRef m_bulkDataQuick{}, m_bulkDataExpensive{};
		static int getBulkData(void* inRefcon, void* outData, int inStartPos, int inNumBytes);
		int m_currentAircraftCount = 1;

		std::unique_ptr<FrameRateMonitor> m_frameRateMonitor;
		std::unique_ptr<AircraftManager> m_aircraftManager;
		std::unique_ptr<NotificationPanel> m_notificationPanel;
		std::unique_ptr<TextMessageConsole> m_textMessageConsole;
		std::unique_ptr<NearbyATCWindow> m_nearbyAtcWindow;
		std::unique_ptr<SettingsWindow> m_settingsWindow;
	};
}

#endif // !XPilot_h