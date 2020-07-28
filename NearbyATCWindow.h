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

#ifndef NearbyATCWindow_h
#define NearbyATCWindow_h

#include "Lib/json.hpp"
using json = nlohmann::json;

namespace xpilot 
{
	class XPilot;

	class NearbyATCList
	{
	private:
		std::string mCallsign;
		std::string mFrequency;
		std::string mRealName;
		int mXPFrequency;
	public:
		std::string getCallsign() { return mCallsign; }
		std::string getFrequency() { return mFrequency; }
		std::string getRealName() { return mRealName; }
		int getXplaneFrequency() { return mXPFrequency; }
		void setCallsign(std::string value) { mCallsign = value; }
		void setFrequency(std::string value) { mFrequency = value; }
		void setRealName(std::string value) { mRealName = value; }
		void setXplaneFrequency(int value) { mXPFrequency = value; }
	};

	class NearbyATCWindow : public XPImgWindow {
	public:
		NearbyATCWindow(XPilot* instance);
		~NearbyATCWindow() final = default;
		void UpdateList(const nlohmann::json& data);
	protected:
		void buildInterface() override;
	private:
		XPilot* env;
		std::mutex mListMutex;
		DataRefAccess<int> mCom1Freq;
	};

}

#endif // !NearbyATCWindow_h
