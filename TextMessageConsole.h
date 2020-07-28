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

#ifndef TextMessageConsole_h
#define TextMessageConsole_h

#include "XPImgWindow.h"

namespace xpilot {

	class XPilot;

	class ConsoleMessage
	{
	private:
		std::string message;
		double red;
		double green;
		double blue;
	public:
		std::string getMessage() { return message; }
		float getRed() { return red / 255; }
		float getGreen() { return green / 255; }
		float getBlue() { return blue / 255; }
		void setMessage(std::string value) { message = value; }
		void setRed(double value) { red = value; }
		void setGreen(double value) { green = value; }
		void setBlue(double value) { blue = value; }
	};

	struct Tab 
	{
		std::string TabName;
		std::string InputValue;
		bool Open;
		bool ScrollToBottom;
		std::list<ConsoleMessage> Messages;
	};

	enum class ConsoleTabType
	{
		Incoming,
		Outgoing
	};

	class TextMessageConsole : public XPImgWindow 
	{
	public:
		TextMessageConsole(XPilot* instance);
		void CreateTabIfNotExists(const std::string& tabName);
		void AddMessageToTab(const std::string& msg, const std::string& recipient, ConsoleTabType tabType);
		void AddIncomingMessage(std::string message, double red = 255, double green = 255, double blue = 255);
		void AddOutgoingMessage(std::string message);
		void SendSocketMsg(const std::string& msg);
		void SendPrivateMessage(const std::string& tabName, const std::string& msg);
	protected:
		void buildInterface() override;
		void errorMessage(std::string error);
	private:
		bool mScrollToBottom;
		XPilot* env;
	};

}

#endif // !TextMessageConsole_h
