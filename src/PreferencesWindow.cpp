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

namespace xpilot
{

	static std::string cslPaths[7];
	static bool pathsEnabled[7];
	static int selectedPathIdx;
	static bool showHideLabels;
	static bool debugModelMatching;
	static int logLevel;
	static std::string fallbackTypeCode;
	static std::string tcpPort = "45001";
	static bool overrideContactAtcCommand;
	static bool showMessageConsole = true;
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
		showMessageConsole = xpilot::Config::Instance().getShowNotificationBar();
		notificationPanelTimeoutSeconds = xpilot::Config::Instance().getNotificationBarDisappaerTime();
		overrideContactAtcCommand = xpilot::Config::Instance().getOverrideContactAtcCommand();
		labelMaxDistance = xpilot::Config::Instance().getMaxLabelDistance();
		labelVisibilityCutoff = xpilot::Config::Instance().getLabelCutoffVis();
		logLevel = xpilot::Config::Instance().getLogLevel();
		HexToRgb(xpilot::Config::Instance().getAircraftLabelColor(), lblCol);
	}

	void Save()
	{
		if (!xpilot::Config::Instance().saveConfig())
		{
			ImGui::OpenPopup("Error Saving Preferences");
		}
	}

	static bool is_number(const std::string& s)
	{
		return !s.empty() && std::find_if(s.begin(),
			s.end(), [](unsigned char c)
		{
			return !std::isdigit(c);
		}) == s.end();
	}

	void PreferencesWindow::buildInterface()
	{
		loadConfig();
		ImGui::PushFont(0);

		struct TextFilters
		{
			static int FilterNumbersOnly(ImGuiInputTextCallbackData* data)
			{
				if (data->EventChar >= '0' && data->EventChar <= '9')
					return 0;
				return 1;
			}
		};

		if (ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::BeginTable("##Settings", 2, ImGuiTableFlags_BordersInnerH))
			{
				ImGui::TableSetupColumn("Item", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoSort, 300);
				ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoSort);

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("Plugin Port");
				ImGui::SameLine();
				ImGui::Text("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(300);
					ImGui::TextUnformatted("This port number allows xPilot to communicate with X-Plane.\n\nOnly change this port number if you know what you are doing.\n\nYou must restart X-Plane and xPilot after changing the port number.");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				ImGui::TableSetColumnIndex(1);
				if (ImGui::InputTextStd("##Port", &tcpPort, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::FilterNumbersOnly))
				{
					if (is_number(tcpPort))
					{
						int t = std::stoi(tcpPort);
						if (t > 65535)
						{
							tcpPort = "65535";
						}
						if (t < 1025)
						{
							tcpPort = "1025";
						}
					}
					xpilot::Config::Instance().setTcpPort(tcpPort);
					Save();
				}

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("Automatically Show Message Console");
				ImGui::SameLine();
				ImGui::Text("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(300);
					ImGui::TextUnformatted("If enabled, the Message Console will automatically appear if a new message is received.");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				ImGui::TableSetColumnIndex(1);
				if (ImGui::Checkbox("##AutoShowMessageConsole", &showMessageConsole))
				{
					xpilot::Config::Instance().setAutoShowMessageConsole(showMessageConsole);
					Save();
				}

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("Hide Message Console After");
				ImGui::SameLine();
				ImGui::Text("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(300);
					ImGui::TextUnformatted("Set this option to automatically hide the message console after the specified number of seconds.");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				ImGui::TableSetColumnIndex(1);
				const float cbWidth = ImGui::CalcTextSize("5 Seconds_______").x;
				ImGui::SetNextItemWidth(cbWidth);
				const char* autoHideOptions[] = { "5 seconds", "10 seconds", "15 seconds", "30 seconds", "60 seconds" };
				if (ImGui::Combo("##AutoHide", &notificationPanelTimeoutSeconds, autoHideOptions, IM_ARRAYSIZE(autoHideOptions)))
				{

				}

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("Override \"Contact ATC\" Command");
				ImGui::SameLine();
				ImGui::Text("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(300);
					ImGui::TextUnformatted("If this option is enabled, xPilot will ignore the \"Contact ATC\" X-Plane Command. This is only useful for users who also use PilotEdge.");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				ImGui::TableSetColumnIndex(1);
				if (ImGui::Checkbox("##OverrideContactATC", &overrideContactAtcCommand))
				{
					xpilot::Config::Instance().setOverrideContactAtcCommand(overrideContactAtcCommand);
					Save();
				}

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("Enable Aircraft Labels");
				ImGui::SameLine();
				ImGui::Text("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(300);
					ImGui::TextUnformatted("Enable this option to show the callsign above all aircraft.");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				ImGui::TableSetColumnIndex(1);
				if (ImGui::Checkbox("##EnableAircraftLabels", &showHideLabels))
				{
					xpilot::Config::Instance().setShowHideLabels(showHideLabels);
					Save();
				}

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("Aircraft Label Color");
				ImGui::SameLine();
				ImGui::Text("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(300);
					ImGui::TextUnformatted("Specify the color of the aircraft labels. Choose a custom color or a pre-defined color.");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				ImGui::TableSetColumnIndex(1);
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

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("Max Label Distance (nm)");
				ImGui::SameLine();
				ImGui::Text("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(300);
					ImGui::TextUnformatted("Specify how far away (nautical miles) you want aircraft labels to be visible.");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				ImGui::TableSetColumnIndex(1);
				if (ImGui::SliderInt("##MaxDist", &labelMaxDistance, 1, 50, "%d nm"))
				{
					XPMPSetAircraftLabelDist(float(labelMaxDistance), labelVisibilityCutoff);
					xpilot::Config::Instance().setMaxLabelDistance(labelMaxDistance);
					Save();
				}

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("Hide Labels at Visibility Distance");
				ImGui::SameLine();
				ImGui::Text("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(300);
					ImGui::TextUnformatted("Visibility can oftentimes be less than the \"Max Label Distance\" due to weather conditions.\n\nIf enabled, aircraft labels will not be visible for planes beyond the current visibility range.\n\nIf disabled, labels will show even if the plane is hidden behind fog or clouds.");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				ImGui::TableSetColumnIndex(1);
				if (ImGui::Checkbox("##HideLabelsVisibility", &labelVisibilityCutoff))
				{
					xpilot::Config::Instance().setLabelCutoffVis(labelVisibilityCutoff);
					Save();
				}

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("Log.txt Log Level");
				ImGui::SameLine();
				ImGui::Text("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(300);
					ImGui::TextUnformatted("This option manages the amount of information that is written to the X-Plane Log.txt.\n\n\"Debug\" will write the most information.\n\"Fatal\" will write the least amount of information.\n\nIt is recommended you only change this if you experience odd behavior and need to log additional information to provide to the developer.");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				ImGui::TableSetColumnIndex(1);
				const float logCbWidth = ImGui::CalcTextSize("Warning (default)_____").x;
				ImGui::SetNextItemWidth(logCbWidth);
				if (ImGui::Combo("##LogLevel", &logLevel, "Debug\0Info\0Warning (default)\0Error\0Fatal\0", 5))
				{
					xpilot::Config::Instance().setLogLevel(logLevel);
					Save();
				}

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("Log Model Matching Results");
				ImGui::SameLine();
				ImGui::Text("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(300);
					ImGui::TextUnformatted("If enabled, debug information will be logged to the X-Plane Log.txt about how a CSL model was chosen.\n\nOnly enable this option if you need to determine why planes are rendering as expected.");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				ImGui::TableSetColumnIndex(1);
				if (ImGui::Checkbox("##ModelMatchingLog", &debugModelMatching))
				{
					xpilot::Config::Instance().setDebugModelMatching(debugModelMatching);
					Save();
				}

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("Default Aircraft Type ICAO");
				ImGui::SameLine();
				ImGui::Text("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(300);
					ImGui::TextUnformatted("Fallback aircraft ICAO type designator if no CSL model can be found for a plane.");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				ImGui::TableSetColumnIndex(1);
				if (ImGui::InputTextStd("##Fallback", &fallbackTypeCode, ImGuiInputTextFlags_CharsUppercase))
				{
					xpilot::Config::Instance().setDefaultAcIcaoType(fallbackTypeCode);
					Save();
				}

				ImGui::EndTable();
			}
		}

		if (ImGui::CollapsingHeader("CSL Configuration"))
		{
			ImGui::Text("CSL Model Configuration");
			ImGui::Text("** You must restart X-Plane after making changes to the CSL Paths **");

			if (ImGui::BeginTable("##CSL", 4, ImGuiTableFlags_BordersInnerH))
			{
				ImGui::TableSetupColumn("##Enabled", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoSort, 80);
				ImGui::TableSetupColumn("##Path", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoSort);
				ImGui::TableSetupColumn("##Browse", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoSort, 50);
				ImGui::TableSetupColumn("##Clear", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoSort, 40);

				for (int i = 0; i < 7; i++)
				{
					ImGui::PushID(i);
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					if (ImGui::Checkbox("Enabled", &pathsEnabled[i]))
					{
						xpilot::Config::Instance().saveCSLEnabled(i, pathsEnabled[i]);
						Save();
					}

					ImGui::TableSetColumnIndex(1);
					ImGui::SetNextItemWidth(-1);
					if (ImGui::InputTextStd("", &cslPaths[i]))
					{
						xpilot::Config::Instance().saveCSLPath(i, cslPaths[i]);
						Save();
					}

					ImGui::TableSetColumnIndex(2);
					ImGui::SetNextItemWidth(-1);
					if (ImGui::Button("Browse"))
					{
						fileBrowser.Open();
						selectedPathIdx = i;
					}

					ImGui::TableSetColumnIndex(3);
					ImGui::SetNextItemWidth(-1);
					if (ImGui::Button("Clear"))
					{
						cslPaths[i] = "";
						pathsEnabled[i] = false;

						xpilot::Config::Instance().saveCSLEnabled(i, pathsEnabled[i]);
						xpilot::Config::Instance().saveCSLPath(i, cslPaths[i]);
						Save();
					}

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

					ImGui::PopID();
				}

				ImGui::EndTable();
			}
		}

		ImGui::PopFont();
	}
}