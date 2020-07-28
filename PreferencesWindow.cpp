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

#include <string>
#include "imfilebrowser.h"
#include "imgui_stdlib.h"
#include "XPImgWindow.h"
#include "Utilities.h"
#include "Config.h"
#include "PreferencesWindow.h"
#include "XPMPMultiplayer.h"

namespace xpilot {

	static std::string cslPaths[7];
	static bool pathsEnabled[7];
	static int selectedPathIdx;
	static bool showHideLabels;
	static bool debugModelMatching;
	static std::string fallbackTypeCode;
	static int tcpPort = 45001;
	static bool overrideContactAtcCommand;
	static bool showNotificationPanel = true;
	static int notificationPanelTimeoutSeconds = 10;
	ImGui::FileBrowser fileBrowser(ImGuiFileBrowserFlags_SelectDirectory);

	PreferencesWindow::PreferencesWindow(WndMode _mode) :
		XPImgWindow(_mode, WND_STYLE_SOLID, WndRect(0, 415, 600, 0))
	{
		SetWindowTitle(string_format("xPilot %s Preferences", PLUGIN_VERSION_STRING));
		SetWindowResizingLimits(600, 415, 600, 415);

		fileBrowser.SetTitle("Browse...");
		fileBrowser.SetWindowSize(450, 250);
	}

	void PreferencesWindow::LoadConfig()
	{
		const Config::vecCslPackages& paths = xpilot::Config::Instance().GetCslPackages();
		for (int i = 0; i < 7; i++) 
		{
			if (i < paths.size()) 
			{
				if (!paths[i].path.empty()) 
				{
					cslPaths[i] = paths[i].path;
					pathsEnabled[i] = paths[i].enabled;
				}
			}
		}

		debugModelMatching = xpilot::Config::Instance().GetDebugModelMatching();
		showHideLabels = xpilot::Config::Instance().GetShowHideLabels();
		fallbackTypeCode = xpilot::Config::Instance().GetDefaultAcIcaoType();
		tcpPort = xpilot::Config::Instance().GetTcpPort();
		showNotificationPanel = xpilot::Config::Instance().GetShowNotificationBar();
		notificationPanelTimeoutSeconds = xpilot::Config::Instance().GetNotificationBarDisappaerTime();
		overrideContactAtcCommand = xpilot::Config::Instance().GetOverrideContactAtcCommand();
	}

	void Save() 
	{
		if (!xpilot::Config::Instance().SaveConfig()) 
		{
			ImGui::OpenPopup("Error Saving Preferences");
		}
	}

	void PreferencesWindow::buildInterface() 
	{
		LoadConfig();
		ImGui::PushFont(0);

		ImGui::Text("General");
		ImGui::PushItemWidth(125);
		if (ImGui::InputTextStd("Fallback Aircraft Type ICAO", &fallbackTypeCode, ImGuiInputTextFlags_CharsUppercase))
		{
			xpilot::Config::Instance().SetDefaultAcIcaoType(fallbackTypeCode);
			Save();
		}
		ImGui::PopItemWidth();

		ImGui::PushItemWidth(125);
		if (ImGui::InputInt("xPilot Plugin Port Number (Requires X-Plane Restart)", &tcpPort))
		{
			if (tcpPort < 1025) 
			{
				tcpPort = 1025;
			}
			else if (tcpPort > 65535) 
			{
				tcpPort = 65535;
			}
			xpilot::Config::Instance().SetTcpPort(tcpPort);
			Save();
		}
		ImGui::PopItemWidth();

		if (ImGui::Checkbox("Show Aircraft Labels", &showHideLabels))
		{
			XPMPEnableAircraftLabels(showHideLabels);
			xpilot::Config::Instance().SetShowHideLabels(showHideLabels);
			Save();
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Yellow", Config::Instance().IsYellow()))
		{
			xpilot::Config::Instance().SetAircraftLabelColor(1.0f, 1.0f, 0.0f);
			Save();
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Red", Config::Instance().IsRed()))
		{
			xpilot::Config::Instance().SetAircraftLabelColor(1.0f, 0.0f, 0.0f);
			Save();
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Green", Config::Instance().IsGreen()))
		{
			xpilot::Config::Instance().SetAircraftLabelColor(0.0f, 1.0f, 0.0f);
			Save();
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Blue", Config::Instance().IsBlue()))
		{
			xpilot::Config::Instance().SetAircraftLabelColor(0.0f, 0.94f, 0.94f);
			Save();
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Black", Config::Instance().IsBlack()))
		{
			xpilot::Config::Instance().SetAircraftLabelColor(0.0f, 0.0f, 0.0f);
			Save();
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("White", Config::Instance().IsWhite()))
		{
			xpilot::Config::Instance().SetAircraftLabelColor(1.0f, 1.0f, 1.0f);
			Save();
		}

		if (ImGui::Checkbox("Override \"Contact ATC\" Command", &overrideContactAtcCommand))
		{
			xpilot::Config::Instance().SetOverrideContactAtcCommand(overrideContactAtcCommand);
			Save();
		}

		if (ImGui::Checkbox("Model Matching Debug Logging", &debugModelMatching))
		{
			xpilot::Config::Instance().SetDebugModelMatching(debugModelMatching);
			Save();
		}

		if (ImGui::Checkbox("Show Notification Panel", &showNotificationPanel))
		{
			xpilot::Config::Instance().SetShowNotificationPanel(showNotificationPanel);
			Save();
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(125);
		if (ImGui::InputInt("Disappear Time (seconds)", &notificationPanelTimeoutSeconds))
		{
			if (notificationPanelTimeoutSeconds < 1) 
			{
				notificationPanelTimeoutSeconds = 1;
			}
			if (notificationPanelTimeoutSeconds > 60)
			{
				notificationPanelTimeoutSeconds = 60;
			}
			xpilot::Config::Instance().SetNotificationPanelDisappearTime(notificationPanelTimeoutSeconds);
			Save();
		}
		ImGui::PopItemWidth();

		ImGui::Separator();

		ImGui::Text("CSL Model Configuration");
		ImGui::Text("** You must restart X-Plane after making changes to the CSL configuration **");

		for (int i = 0; i < 7; i++) 
		{
			ImGui::PushID(i);
			if (ImGui::Checkbox("Enabled", &pathsEnabled[i])) 
			{
				xpilot::Config::Instance().SaveCSLEnabled(i, pathsEnabled[i]);
				Save();
			}
			ImGui::SameLine();
			if (ImGui::InputTextStd("", &cslPaths[i])) 
			{
				xpilot::Config::Instance().SaveCSLPath(i, cslPaths[i]);
				Save();
			}
			ImGui::SameLine();
			if (ImGui::Button("Browse"))
			{
				fileBrowser.Open();
				selectedPathIdx = i;
			}
			ImGui::SameLine();
			if (ImGui::Button("Clear"))
			{
				cslPaths[i] = "";
				pathsEnabled[i] = false;

				xpilot::Config::Instance().SaveCSLEnabled(i, pathsEnabled[i]);
				xpilot::Config::Instance().SaveCSLPath(i, cslPaths[i]);
				Save();
			}
			ImGui::PopID();
		}
		ImGui::PopFont();

		fileBrowser.Display();
		if (fileBrowser.HasSelected())
		{
			cslPaths[selectedPathIdx] = fileBrowser.GetSelected().string();
			pathsEnabled[selectedPathIdx] = true;

			xpilot::Config::Instance().SaveCSLEnabled(selectedPathIdx, pathsEnabled[selectedPathIdx]);
			xpilot::Config::Instance().SaveCSLPath(selectedPathIdx, cslPaths[selectedPathIdx]);
			Save();

			fileBrowser.ClearSelected();
		}

		ImGui::SetNextWindowSize(ImVec2(400, 150), ImGuiCond_FirstUseEver);
		if (ImGui::BeginPopupModal("Error Saving Preferences", NULL, ImGuiWindowFlags_NoResize))
		{
			ImGui::TextWrapped("An error occured while trying to save the preferences.\n\nMake sure read/write permissions are set properly for the \n\"Resources > Plugins > xPilot > Resources\" folder.\n\n");

			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}
	}
}