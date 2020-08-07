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

#include <memory>
#include <thread>

#include "Plugin.h"
#include "XPilot.h"
#include "Config.h"
#include "Utilities.h"
#include "Constants.h"
#include "XPMPMultiplayer.h"
#include "XPLMPlugin.h"

#if !defined(XPLM200) || !defined(XPLM210) || !defined(XPLM300) || !defined(XPLM301)
#error xPilot requires XPLM301 SDK or newer
#endif

std::unique_ptr<XPilot> environment;

PLUGIN_API int XPluginStart(char* outName, char* outSignature, char* outDescription)
{
    strncpy_s(outName, 255, string_format("%s %s", PLUGIN_NAME, PLUGIN_VERSION_STRING).c_str(), 100);
    strncpy_s(outSignature, 255, "org.vatsim.xpilot", 100);
    strncpy_s(outDescription, 255, "X-Plane pilot client for VATSIM.", 100);

    try
    {
        XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);
        XPMPSetPluginName(PLUGIN_NAME);
        RegisterMenuItems();
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Exception in XPluginStart: %s", e.what());
        return 0;
    }
    catch (...) { return 0; }

    return 1;
}

PLUGIN_API int XPluginEnable(void)
{
    try
    {
        XPImgWindowInit();
        Config::Instance().LoadConfig();
        environment = std::make_unique<xpilot::XPilot>();
        XPLMCheckMenuItem(PluginMenu, MenuDefaultAtis, xpilot::Config::Instance().GetDefaultAtisEnabled() ? xplm_Menu_Checked : xplm_Menu_Unchecked);
    }
    catch (std::exception& e)
    {
        LOG_ERROR("Exception in XPluginEnable: %s", e.what());
        return 0;
    }
    catch (...) { return 0; }

    return 1;
}

PLUGIN_API void XPluginDisable(void)
{
    try
    {
        environment->OnPluginDisabled();
        environment->StopZmqServer();
        XPMPMultiplayerDisable();
        XPMPMultiplayerCleanup();
    }
    catch (std::exception& e)
    {
        LOG_ERROR("Exception in XPluginDisable: %s", e.what());
    }
    catch (...) {}
}

PLUGIN_API void XPluginStop(void)
{
    try
    {
        XPImgWindowCleanup();
        XPLMDestroyMenu(PluginMenu);
        XPLMUnregisterCommandHandler(PttCommand, PttCommandHandler, 0, 0);
        XPLMUnregisterCommandHandler(TogglePreferencesCommand, TogglePreferencesCommandHandler, 0, 0);
        XPLMUnregisterCommandHandler(ToggleNotificationPanelCommand, ToggleNotificationPanelCommandHandler, 0, 0);
        XPLMUnregisterCommandHandler(ToggleNearbyATCWindowCommand, ToggleNearbyATCWindowCommandHandler, 0, 0);
        XPLMUnregisterCommandHandler(ToggleTextMessageConsoleCommand, ToggleTextMessageConsoleCommandHandler, 0, 0);
        XPLMUnregisterCommandHandler(ContactAtcCommand, ContactAtcCommandHandler, 0, 0);
        XPLMUnregisterCommandHandler(ToggleDefaultAtisCommand, ToggleDefaultAtisCommandHandler, 0, 0);
        XPLMUnregisterCommandHandler(ToggleTcasCommand, ToggleTcasCommandHandler, 0, 0);
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Exception in XPluginStop: %s", e.what());
    }
    catch (...) {}
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID from, int msg, void* inParam)
{

}

int ContactAtcCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void* inRefcon)
{
    if (xpilot::Config::Instance().GetOverrideContactAtcCommand())
    {
        return 0;
    }
    return 1;
}

int  PttCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void* inRefcon)
{
    if (inPhase == xplm_CommandContinue)
    {
        environment->SetPttActive(1);
    }
    if (inPhase == xplm_CommandEnd)
    {
        environment->SetPttActive(0);
    }
    return 0;
}

int TogglePreferencesCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void* inRefcon)
{
    if (inPhase == xplm_CommandEnd)
    {
        environment->TogglePreferencesWindow();
    }
    return 0;
}

int ToggleNearbyATCWindowCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void* inRefcon)
{
    if (inPhase == xplm_CommandEnd)
    {
        environment->ToggleNearbyAtcWindow();
    }
    return 0;
}

int ToggleNotificationPanelCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void* inRefcon)
{
    if (inPhase == xplm_CommandEnd)
    {
        environment->SetNotificationPanelAlwaysVisible(!environment->IsNotificationPanelAlwaysVisible());
    }
    return 0;
}

int ToggleDefaultAtisCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void* inRefcon)
{
    if (inPhase == xplm_CommandEnd)
    {
        environment->DisableDefaultAtis(environment->IsDefaultAtisDisabled());
    }
    XPLMSetMenuItemName(PluginMenu, MenuDefaultAtis, environment->IsDefaultAtisDisabled() ? "Default ATIS: Disabled" : "Default ATIS: Enabled", 0);
    return 0;
}

int ToggleTcasCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void* inRefcon)
{
    if (inPhase == xplm_CommandEnd)
    {
        if (XPMPHasControlOfAIAircraft())
        {
            environment->ReleaseTcasControl();
        }
        else
        {
            environment->TryGetTcasControl();
        }
    }
    return 0;
}

int ToggleTextMessageConsoleCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void* inRefcon)
{
    if (inPhase == xplm_CommandEnd)
    {
        environment->ToggleTextMessageConsole();
    }
    return 0;
}

int ToggleAircraftLabelsCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void* inRefcon)
{
    if (inPhase == xplm_CommandEnd)
    {
        bool enabled = !xpilot::Config::Instance().GetShowHideLabels();
        xpilot::Config::Instance().SetShowHideLabels(enabled);
        xpilot::Config::Instance().SaveConfig();
        XPMPEnableAircraftLabels(enabled);
    }
    return 0;
}

void RegisterMenuItems()
{
    PttCommand = XPLMCreateCommand("xpilot/ptt", "xPilot: Radio Push-To-Talk (PTT)");
    XPLMRegisterCommandHandler(PttCommand, PttCommandHandler, 1, (void*)0);

    TogglePreferencesCommand = XPLMCreateCommand("xpilot/toggle_preferences", "xPilot: Toggle Preferences Window");
    XPLMRegisterCommandHandler(TogglePreferencesCommand, TogglePreferencesCommandHandler, 1, (void*)0);

    ToggleNearbyATCWindowCommand = XPLMCreateCommand("xpilot/toggle_nearby_atc", "xPilot: Toggle Nearby ATC Window");
    XPLMRegisterCommandHandler(ToggleNearbyATCWindowCommand, ToggleNearbyATCWindowCommandHandler, 1, (void*)0);

    ToggleNotificationPanelCommand = XPLMCreateCommand("xpilot/toggle_notification_panel", "xPilot: Toggle Notification Panel");
    XPLMRegisterCommandHandler(ToggleNotificationPanelCommand, ToggleNotificationPanelCommandHandler, 1, (void*)0);

    ToggleDefaultAtisCommand = XPLMCreateCommand("xpilot/toggle_default_atis", "xPilot: Toggle Default X-Plane ATIS");
    XPLMRegisterCommandHandler(ToggleDefaultAtisCommand, ToggleDefaultAtisCommandHandler, 1, (void*)0);

    ToggleTcasCommand = XPLMCreateCommand("xpilot/toggle_tcas", "xPilot: Toggle TCAS Control");
    XPLMRegisterCommandHandler(ToggleTcasCommand, ToggleTcasCommandHandler, 1, (void*)0);

    ToggleTextMessageConsoleCommand = XPLMCreateCommand("xpilot/toggle_text_message_console", "xPilot: Toggle Text Message Console");
    XPLMRegisterCommandHandler(ToggleTextMessageConsoleCommand, ToggleTextMessageConsoleCommandHandler, 1, (void*)0);

    ToggleAircraftLabelsCommand = XPLMCreateCommand("xpilot/toggle_aircraft_labels", "xPilot: Toggle Aircraft Labels");
    XPLMRegisterCommandHandler(ToggleAircraftLabelsCommand, ToggleAircraftLabelsCommandHandler, 1, (void*)0);

    XPLMRegisterCommandHandler(ContactAtcCommand, ContactAtcCommandHandler, 1, (void*)0);

    PluginMenuIdx = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "xPilot", nullptr, 0);
    PluginMenu = XPLMCreateMenu("xPilot", XPLMFindPluginsMenu(), PluginMenuIdx, nullptr, nullptr);

    MenuPreferences = XPLMAppendMenuItemWithCommand(PluginMenu, "Preferences", TogglePreferencesCommand);
    MenuNearbyAtc = XPLMAppendMenuItemWithCommand(PluginMenu, "Nearby ATC", ToggleNearbyATCWindowCommand);
    MenuTextMessageConsole = XPLMAppendMenuItemWithCommand(PluginMenu, "Text Message Console", ToggleTextMessageConsoleCommand);
    MenuNotificationPanel = XPLMAppendMenuItemWithCommand(PluginMenu, "Notification Panel", ToggleNotificationPanelCommand);
    MenuDefaultAtis = XPLMAppendMenuItemWithCommand(PluginMenu, "Default ATIS", ToggleDefaultAtisCommand);
    MenuToggleTcas = XPLMAppendMenuItemWithCommand(PluginMenu, "Toggle TCAS", ToggleTcasCommand);
    MenuToggleAircraftLabels = XPLMAppendMenuItemWithCommand(PluginMenu, "Toggle Aircraft Labels", ToggleAircraftLabelsCommand);
}

void UpdateMenuItems()
{
    XPLMSetMenuItemName(PluginMenu, MenuToggleTcas, XPMPHasControlOfAIAircraft() ? "Release TCAS Control" : "Request TCAS Control", 0);
}

#ifdef _WIN32
#include <Windows.h>
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    return TRUE;
}
#endif // _WIN32