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
#include <list>
#include "TextMessageConsole.h"
#include "Utilities.h"
#include "XPilot.h"
#include "Lib/json.hpp"

using json = nlohmann::json;

namespace xpilot
{
	static std::string InputValue;
	static std::list<ConsoleMessage> MessageHistory;
	static std::list<Tab> Tabs;

	enum class CommandOptions
	{
		Chat,
		RequestAtis,
		Close,
		CloseAll,
		Clear,
		None
	};

	CommandOptions resolveOption(std::string input)
	{
		std::string v(str_tolower(input));
		if (v == ".chat") return xpilot::CommandOptions::Chat;
		if (v == ".atis") return xpilot::CommandOptions::RequestAtis;
		if (v == ".clear") return xpilot::CommandOptions::Clear;
		if (v == ".close") return xpilot::CommandOptions::Close;
		if (v == ".closeall") return xpilot::CommandOptions::CloseAll;
		return xpilot::CommandOptions::None;
	}

	TextMessageConsole::TextMessageConsole(XPilot* instance) :
		XPImgWindow(WND_MODE_FLOAT_CENTERED, WND_STYLE_SOLID, WndRect(0, 200, 600, 0)),
		m_scrollToBottom(false),
		m_env(instance)
	{
		SetWindowResizingLimits(300, 100, 1024, 1024);
		SetWindowTitle("Text Message Console");
	}

	void TextMessageConsole::sendSocketMessage(const std::string& msg)
	{
		if (m_env->isNetworkConnected())
		{
			json j;
			j["Type"] = "SocketMessage";
			j["Data"]["Message"] = msg.c_str();
			m_env->sendSocketMsg(j.dump());
		}
	}

	void TextMessageConsole::addIncomingMessage(std::string msg, double red, double green, double blue)
	{
		if (!msg.empty())
		{
			ConsoleMessage m;
			m.setMessage(string_format("[%s] %s", UtcTimestamp().c_str(), msg.c_str()));
			m.setRed(red);
			m.setGreen(green);
			m.setBlue(blue);
			MessageHistory.push_back(m);
			m_scrollToBottom = true;
		}
	}

	void TextMessageConsole::addOutgoingMessage(std::string msg)
	{
		ConsoleMessage m;
		m.setMessage(string_format("[%s] %s: %s", UtcTimestamp().c_str(), m_env->ourCallsign(), msg.c_str()));
		m.setRed(255);
		m.setGreen(255);
		m.setBlue(255);
		MessageHistory.push_back(m);
		sendSocketMessage(msg);
		m_scrollToBottom = true;
	}

	void TextMessageConsole::errorMessage(std::string error)
	{
		ConsoleMessage m;
		m.setMessage(string_format("[%s] %s", UtcTimestamp().c_str(), error.c_str()));
		m.setRed(192);
		m.setGreen(57);
		m.setBlue(43);
		MessageHistory.push_back(m);
		m_scrollToBottom = true;
	}

	void tabErrorMessage(std::string tabName, std::string error)
	{
		ConsoleMessage m;
		m.setMessage(string_format("[%s] %s", UtcTimestamp().c_str(), error.c_str()));
		m.setRed(192);
		m.setGreen(57);
		m.setBlue(43);

		auto it = std::find_if(Tabs.begin(), Tabs.end(), [&tabName](const Tab& t)
		{
			return t.tabName == tabName;
		});

		if (it != Tabs.end())
		{
			it->scrollToBottom = true;
		}
	}

	void TextMessageConsole::sendPrivateMessage(const std::string& tabName, const std::string& msg)
	{
		if (!tabName.empty() && !msg.empty())
		{
			if (m_env->isNetworkConnected())
			{
				json j;
				j["Type"] = "PrivateMessageSent";
				j["Data"]["Message"] = msg.c_str();
				j["Data"]["To"] = str_toupper(tabName).c_str();
				m_env->sendSocketMsg(j.dump());
			}
		}
	}

	void TextMessageConsole::createTabIfNotExists(const std::string& tabName)
	{
		auto it = std::find_if(Tabs.begin(), Tabs.end(), [&tabName](const Tab& t)
		{
			return t.tabName == tabName;
		});

		if (it == Tabs.end())
		{
			Tab tab;
			tab.tabName = tabName;
			tab.isOpen = true;
			tab.messageHistory = std::list<ConsoleMessage>();
			Tabs.push_back(tab);
		}
	}

	void TextMessageConsole::addMessageToTab(const std::string& recipient, const std::string& msg, ConsoleTabType tabType)
	{
		switch (tabType)
		{
		case ConsoleTabType::Outgoing:
		{
			ConsoleMessage m;
			m.setMessage(string_format("[%s] %s: %s", UtcTimestamp().c_str(), m_env->ourCallsign(), msg.c_str()));
			m.setRed(255);
			m.setGreen(255);
			m.setBlue(255);

			auto it = std::find_if(Tabs.begin(), Tabs.end(), [&recipient](const Tab& t)
			{
				return t.tabName == recipient;
			});

			if (it != Tabs.end())
			{
				it->messageHistory.push_back(m);
				it->scrollToBottom = true;
			}
			else
			{
				createTabIfNotExists(recipient);
				addMessageToTab(recipient, msg, ConsoleTabType::Outgoing);
			}
		}
		break;
		case ConsoleTabType::Incoming:
		{
			ConsoleMessage m;
			m.setMessage(string_format("[%s] %s: %s", UtcTimestamp().c_str(), recipient.c_str(), msg.c_str()));
			m.setRed(39);
			m.setGreen(174);
			m.setBlue(96);

			auto it = std::find_if(Tabs.begin(), Tabs.end(), [&recipient](const Tab& t)
			{
				return t.tabName == recipient;
			});

			if (it != Tabs.end())
			{
				it->messageHistory.push_back(m);
				it->scrollToBottom = true;
			}
			else
			{
				createTabIfNotExists(recipient);
				addMessageToTab(recipient, msg, ConsoleTabType::Incoming);
			}
		}
		break;
		}
	}

	void CloseTab(const std::string& tabName)
	{
		auto it = std::find_if(Tabs.begin(), Tabs.end(), [&tabName](const Tab& t)
		{
			return t.tabName == tabName;
		});
		if (it != Tabs.end())
		{
			Tabs.erase(it);
		}
	}

	void TextMessageConsole::buildInterface()
	{
		ImGui::PushFont(0);

		if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_NoTooltip))
		{
			if (ImGui::BeginTabItem("Messages"))
			{
				ImGui::BeginChild("##Messages", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false);
				{
					for (auto& e : MessageHistory)
					{
						const ImVec4& color = ImVec4(e.getRed(), e.getGreen(), e.getBlue(), 1.0f);
						ImGui::PushStyleColor(ImGuiCol_Text, color);
						ImGui::TextWrapped(e.getMessage().c_str());
						ImGui::PopStyleColor();
					}
					if (m_scrollToBottom)
					{
						ImGui::SetScrollHere(1.0f);
						m_scrollToBottom = false;
					}
				}
				ImGui::EndChild();
				ImGui::PushItemWidth(-1.0f);
				if (ImGui::InputTextStd("##MessagesInput", &InputValue, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					if (!InputValue.empty())
					{
						std::vector<std::string> args;
						tokenize(InputValue, args, " ", true);
						if (args.size() > 0)
						{
							switch (resolveOption(args.at(0)))
							{
							case xpilot::CommandOptions::Chat:
								if (!m_env->isNetworkConnected()) {
									errorMessage("Not connected to the network.");
								}
								else
								{
									if (args.size() >= 2)
									{
										if (args.size() >= 3)
										{
											std::string m;
											join(args, ' ', m);

											createTabIfNotExists(str_toupper(args.at(1)));
											sendPrivateMessage(str_toupper(args.at(1)), m);
										}
										else
										{
											createTabIfNotExists(str_toupper(args.at(1)));
										}
										InputValue = "";
									}
									else
									{
										errorMessage("Invalid parameters. To open a new chat tab, use the command .chat <callsign> <message>");
										InputValue = "";
									}
								}
								break;
							case xpilot::CommandOptions::RequestAtis:
								if (!m_env->isNetworkConnected())
								{
									errorMessage("Not connected to the network.");
								}
								else
								{
									if (args.size() == 2)
									{
										m_env->requestControllerAtis(args.at(1));
									}
									else
									{
										errorMessage("Invalid parameters. To request an ATIS, use the command .atis <callsign>");
										InputValue = "";
									}
								}
								break;
							case xpilot::CommandOptions::Clear:
								MessageHistory.clear();
								InputValue = "";
								break;
							case xpilot::CommandOptions::CloseAll:
								Tabs.clear();
								InputValue = "";
								break;
							case xpilot::CommandOptions::Close:
							default:
								if (!m_env->isNetworkConnected())
								{
									errorMessage("Not connected to the network.");
								}
								else
								{
									addOutgoingMessage(InputValue);
									InputValue = "";
								}
								break;
							}
						}
					}
				}
				if (ImGui::IsItemDeactivated() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)))
				{
					InputValue = "";
				}
				ImGui::EndTabItem();
			}

			for (auto it = Tabs.begin(); it != Tabs.end(); ++it)
			{
				if (!it->isOpen)
				{
					Tabs.erase(it);
				}
			}

			for (auto it = Tabs.begin(); it != Tabs.end(); ++it)
			{
				std::string key = it->tabName;

				if (ImGui::BeginTabItem(key.c_str(), &it->isOpen))
				{
					ImGui::BeginChild(key.c_str(), ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false);
					{
						for (auto& e : it->messageHistory)
						{
							const ImVec4& color = ImVec4(e.getRed(), e.getGreen(), e.getBlue(), 1.0f);
							ImGui::PushStyleColor(ImGuiCol_Text, color);
							ImGui::TextWrapped(e.getMessage().c_str());
							ImGui::PopStyleColor();
						}
						if (it->scrollToBottom)
						{
							ImGui::SetScrollHere(1.0f);
							it->scrollToBottom = false;
						}
					}
					ImGui::EndChild();
					ImGui::PushID(key.c_str());
					ImGui::PushItemWidth(-1.0f);
					if (ImGui::InputTextStd("##Input", &it->textInput, ImGuiInputTextFlags_EnterReturnsTrue))
					{
						if (!it->textInput.empty())
						{
							std::vector<std::string> args;
							tokenize(it->textInput, args, " ", true);
							if (args.size() > 0)
							{
								switch (resolveOption(args.at(0)))
								{
								case xpilot::CommandOptions::Clear:
									it->messageHistory.clear();
									it->textInput = "";
									break;
								case xpilot::CommandOptions::Close:
									it->textInput = "";
									CloseTab(key);
									break;
								default:
									if (!m_env->isNetworkConnected())
									{
										tabErrorMessage(it->tabName, "Not connected to the network.");
									}
									else
									{
										sendPrivateMessage(it->tabName, it->textInput);
										it->textInput = "";
									}
									break;
								}
							}
						}
					}
					if (ImGui::IsItemDeactivated() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)))
					{
						InputValue = "";
					}
					ImGui::PopID();
					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
			ImGui::PopFont();
		}
	}
}