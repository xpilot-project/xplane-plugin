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
	static int labelMaxDistance = 3;
	static bool labelVisibilityCutoff = true;
	static float lblCol[4];
	ImGui::FileBrowser fileBrowser(ImGuiFileBrowserFlags_SelectDirectory);

	PreferencesWindow::PreferencesWindow(WndMode _mode) :
		XPImgWindow(_mode, WND_STYLE_SOLID, WndRect(0, 445, 600, 0))
	{
		SetWindowTitle(string_format("xPilot %s Preferences", PLUGIN_VERSION_STRING));
		SetWindowResizingLimits(600, 445, 600, 445);

		fileBrowser.SetTitle("Browse...");
		fileBrowser.SetWindowSize(450, 250);
	}

	void PreferencesWindow::loadConfig()
	{
		const Config::vecCslPackages& paths = xpilot::Config::Instance().getCSLPackages();
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

		debugModelMatching = xpilot::Config::Instance().getDebugModelMatching();
		showHideLabels = xpilot::Config::Instance().getShowHideLabels();
		fallbackTypeCode = xpilot::Config::Instance().getDefaultAcIcaoType();
		tcpPort = xpilot::Config::Instance().getTcpPort();
		showNotificationPanel = xpilot::Config::Instance().getShowNotificationBar();
		notificationPanelTimeoutSeconds = xpilot::Config::Instance().getNotificationBarDisappaerTime();
		overrideContactAtcCommand = xpilot::Config::Instance().getOverrideContactAtcCommand();
		labelMaxDistance = xpilot::Config::Instance().getMaxLabelDistance();
		labelVisibilityCutoff = xpilot::Config::Instance().getLabelCutoffVis();
		HexToRgb(xpilot::Config::Instance().getAircraftLabelColor(), lblCol);
	}

	void Save() 
	{
		if (!xpilot::Config::Instance().saveConfig()) 
		{
			ImGui::OpenPopup("Error Saving Preferences");
		}
	}

	void PreferencesWindow::buildInterface() 
	{
		loadConfig();
		ImGui::PushFont(0);

		ImGui::Text("General");
		ImGui::PushItemWidth(125);
		if (ImGui::InputTextStd("Fallback Aircraft Type ICAO", &fallbackTypeCode, ImGuiInputTextFlags_CharsUppercase))
		{
			xpilot::Config::Instance().setDefaultAcIcaoType(fallbackTypeCode);
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
			xpilot::Config::Instance().setTcpPort(tcpPort);
			Save();
		}
		ImGui::PopItemWidth();

		if (ImGui::Checkbox("Show Aircraft Labels", &showHideLabels))
		{
			XPMPEnableAircraftLabels(showHideLabels);
			xpilot::Config::Instance().setShowHideLabels(showHideLabels);
			Save();
		}
		ImGui::SameLine();
		ImGui::TextUnformatted("Label Color:");
		ImGui::SameLine();
		if (ImGui::ColorButton("Pick Label Color", ImVec4(lblCol[0], lblCol[1], lblCol[2], lblCol[3]), ImGuiColorEditFlags_NoAlpha))
		{
			ImGui::OpenPopup("Label Color Picker");
		}
		if (ImGui::BeginPopup("Label Color Picker"))
		{
			if (ImGui::ColorPicker3("Label Color Picker", lblCol,
				ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoLabel |
				ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_NoSidePreview))
			{
				const int col = (int)((std::lround(lblCol[0] * 255.0f) << 16)
					+ (std::lround(lblCol[1] * 255.0f) << 8)
					+ (std::lround(lblCol[2] * 255.0f) << 0));

				xpilot::Config::Instance().setAircraftLabelColor(col);
				Save();
			}
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		ImGui::TextUnformatted("Or Choose Color:");
		ImGui::SameLine();
		if (ImGui::ColorButton("Yellow", ImVec4(1.0f, 1.0f, 0.0f, 1.0f), ImGuiColorEditFlags_NoTooltip))
		{
			HexToRgb(COLOR_YELLOW, lblCol);
			xpilot::Config::Instance().setAircraftLabelColor(COLOR_YELLOW);
			Save();
		}
		ImGui::SameLine();
		if (ImGui::ColorButton("Red", ImVec4(1.0f, 0.0f, 0.0f, 1.0f), ImGuiColorEditFlags_NoTooltip))
		{
			HexToRgb(COLOR_RED, lblCol);
			xpilot::Config::Instance().setAircraftLabelColor(COLOR_RED);
			Save();
		}
		ImGui::SameLine();
		if (ImGui::ColorButton("Green", ImVec4(0.0f, 1.0f, 0.0f, 1.0f), ImGuiColorEditFlags_NoTooltip))
		{
			HexToRgb(COLOR_GREEN, lblCol);
			xpilot::Config::Instance().setAircraftLabelColor(COLOR_GREEN);
			Save();
		}
		ImGui::SameLine();
		if (ImGui::ColorButton("Blue", ImVec4(0.0f, 0.94f, 0.94f, 1.0f), ImGuiColorEditFlags_NoTooltip))
		{
			HexToRgb(COLOR_BLUE, lblCol);
			xpilot::Config::Instance().setAircraftLabelColor(COLOR_BLUE);
			Save();
		}
		ImGui::PushItemWidth(100);
		if (ImGui::SliderInt("Max Label Distance", &labelMaxDistance, 1, 50, "%d nm"))
		{
			XPMPSetAircraftLabelDist(float(labelMaxDistance), labelVisibilityCutoff);
			xpilot::Config::Instance().setMaxLabelDistance(labelMaxDistance);
			Save();
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::Checkbox("Cutoff at Visibility", &labelVisibilityCutoff))
		{
			XPMPSetAircraftLabelDist(float(labelMaxDistance), labelVisibilityCutoff);
			xpilot::Config::Instance().setLabelCutoffVis(labelVisibilityCutoff);
			Save();
		}
		if (ImGui::Checkbox("Override \"Contact ATC\" Command", &overrideContactAtcCommand))
		{
			xpilot::Config::Instance().setOverrideContactAtcCommand(overrideContactAtcCommand);
			Save();
		}

		if (ImGui::Checkbox("Model Matching Debug Logging", &debugModelMatching))
		{
			xpilot::Config::Instance().setDebugModelMatching(debugModelMatching);
			Save();
		}

		if (ImGui::Checkbox("Show Notification Panel", &showNotificationPanel))
		{
			xpilot::Config::Instance().setShowNotificationPanel(showNotificationPanel);
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
			xpilot::Config::Instance().setNotificationPanelDisappearTime(notificationPanelTimeoutSeconds);
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
				xpilot::Config::Instance().saveCSLEnabled(i, pathsEnabled[i]);
				Save();
			}
			ImGui::SameLine();
			if (ImGui::InputTextStd("", &cslPaths[i])) 
			{
				xpilot::Config::Instance().saveCSLPath(i, cslPaths[i]);
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

				xpilot::Config::Instance().saveCSLEnabled(i, pathsEnabled[i]);
				xpilot::Config::Instance().saveCSLPath(i, cslPaths[i]);
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

			xpilot::Config::Instance().saveCSLEnabled(selectedPathIdx, pathsEnabled[selectedPathIdx]);
			xpilot::Config::Instance().saveCSLPath(selectedPathIdx, cslPaths[selectedPathIdx]);
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