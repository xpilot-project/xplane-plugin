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

#include "NotificationPanel.h"
#include "Utilities.h"
#include "Config.h"

namespace xpilot
{
    struct NotificationTy
    {
        std::string message;
        float red;
        float green;
        float blue;
    };

    static std::list<NotificationTy> NotificationHistory;

    NotificationPanel::NotificationPanel(int left, int top, int right, int bottom) :
        mScrollToBottom(false),
        mAlwaysVisible(false),
        mTogglePanelCommand("xpilot/toggle_notification_panel", "xPilot: Notification Panel", [this] { Toggle(); }),
        ImgWindow(left, top, right, bottom, xplm_WindowDecorationSelfDecorated, xplm_WindowLayerFloatingWindows)
    {
        SetWindowTitle("Notification Panel");
        SetVisible(false);

        XPLMCreateFlightLoop_t flightLoopParams = {
            sizeof(flightLoopParams),
            xplm_FlightLoop_Phase_AfterFlightModel,
            onFlightLoop,
            reinterpret_cast<void*>(this)
        };
        flightLoopId = XPLMCreateFlightLoop(&flightLoopParams);
        XPLMScheduleFlightLoop(flightLoopId, -1.0f, true);
    }

    NotificationPanel::~NotificationPanel()
    {
        XPLMUnregisterFlightLoopCallback(onFlightLoop, this);
    }

    void NotificationPanel::buildInterface()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowBorderSize = 0.0f;

        for (auto& e : NotificationHistory)
        {
            const ImVec4& color = ImVec4(e.red, e.green, e.blue, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextWrapped(e.message.c_str());
            ImGui::PopStyleColor();
        }
        if (mScrollToBottom)
        {
            ImGui::SetScrollHere(1.0f);
            mScrollToBottom = false;
        }

        if (!IsAlwaysVisible())
        {
            if (mDisappearTime != std::chrono::system_clock::time_point()
                && std::chrono::system_clock::now() > mDisappearTime)
            {
                SetVisible(false);
                mDisappearTime = std::chrono::system_clock::time_point();
            }
        }
    }

    float NotificationPanel::onFlightLoop(float, float, int, void* refcon)
    {
        auto* panel = reinterpret_cast<NotificationPanel*>(refcon);

        if (panel->GetVisible())
        {
            int left, top, right, bottom, screenTop, screenRight;
            XPLMGetScreenBoundsGlobal(nullptr, &screenTop, &screenRight, nullptr);
            right = screenRight - 35; /*padding left*/
            top = screenTop - 35; /*width*/
            left = screenRight - 800; /*padding top*/
            bottom = top - 100; /*height*/
            panel->SetWindowGeometry(left, top, right, bottom);
        }

        return -1.0f;
    }

    void NotificationPanel::AddNotificationPanelMessage(const std::string& message, float red, float green, float blue)
    {
        if (!message.empty())
        {
            NotificationTy notification;
            notification.message = string_format("[%s] %s", UtcTimestamp().c_str(), message.c_str());
            notification.red = red / 255;
            notification.green = green / 255;
            notification.blue = blue / 255;
            NotificationHistory.push_back(notification);
            mScrollToBottom = true;

            if (Config::Instance().GetShowNotificationBar())
            {
                SetVisible(true);
                mDisappearTime = std::chrono::system_clock::now() +
                    std::chrono::milliseconds(Config::Instance().GetNotificationBarDisappaerTime() * 1000);
            }
        }
    }

    void NotificationPanel::Toggle()
    {
        SetVisible(!GetVisible());
        mAlwaysVisible = !mAlwaysVisible;
        mDisappearTime = std::chrono::system_clock::time_point();
    }
}