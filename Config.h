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

#ifndef Config_h
#define Config_h

#include <string>
#include <vector>
#include "Constants.h"

namespace xpilot
{
    struct CslPackage
    {
        std::string path;
        bool enabled;

        inline bool empty() const { return path.empty(); }
        inline bool operator== (const CslPackage& o) const { return path == o.path; }
        inline bool operator== (const std::string& s) const { return path == s; }
    };

    class Config
    {
    public:
        static Config& Instance();
        ~Config() = default;
        Config(const Config&) = delete;
        void operator=(const Config&) = delete;
        Config(Config&&)noexcept = default;
        Config& operator=(Config&&)noexcept = default;

        bool LoadConfig();
        bool SaveConfig();

        typedef std::vector<CslPackage> vecCslPackages;
        std::vector<CslPackage> GetCslPackages()const { return mCslPackages; }
        void SaveCSLPath(int idx, const std::string path);
        void SaveCSLEnabled(int idx, bool enabled);
        bool LoadCSLPackage(int idx);
        bool HasValidPaths() const;

        std::string GetDefaultAcIcaoType() const { return mDefaultAcIcaoType; }
        bool SetDefaultAcIcaoType(const std::string type);

        bool SetShowHideLabels(bool status);
        bool GetShowHideLabels() const { return mShowHideLabels; }

        bool SetDebugModelMatching(bool status);
        bool GetDebugModelMatching() const { return mDebugModelMatching; }

        bool SetTcpPort(int port);
        int GetTcpPort() const { return mTcpPort; }

        bool SetDefaultAtisEnabled(bool status);
        bool GetDefaultAtisEnabled() const { return mDefaultAtis; }

        bool SetShowNotificationPanel(bool enabled);
        bool GetShowNotificationBar()const { return mShowNotificationBar; }

        bool SetNotificationPanelDisappearTime(int timeout);
        int GetNotificationBarDisappaerTime() const { return mNotificationBarDisappearTime; }

        bool SetOverrideContactAtcCommand(bool status);
        bool GetOverrideContactAtcCommand() const { return mOverrideContactAtcCommand; }

        bool SetAircraftLabelColor(int c);
        int GetAircraftLabelColor() { return mLabelColor; }

        bool SetDisableTcas(bool status);
        bool GetDisableTcas()const { return mDisableTcas; }

        bool SetMaxLabelDistance(int d);
        bool GetMaxLabelDistance()const { return mMaxLabelDist; }

        bool SetLabelCutoffVis(bool b);
        bool GetLabelCutoffVis()const { return mLabelCutoffVis; }
    private:
        Config() = default;
        std::vector<CslPackage> mCslPackages;
        std::string mDefaultAcIcaoType = "A320";
        bool mShowHideLabels = true;
        bool mDebugModelMatching = false;
        bool mDefaultAtis = false;
        int mTcpPort = 45001;
        bool mOverrideContactAtcCommand = false;
        int mLabelColor = COLOR_YELLOW;
        bool mDisableTcas = false;
        bool mShowNotificationBar = true;
        int mNotificationBarDisappearTime = 10;
        int mMaxLabelDist = 3;
        bool mLabelCutoffVis = true;
    };
}

#endif // !Config_h
