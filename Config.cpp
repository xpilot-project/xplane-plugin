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

#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include "Config.h"
#include "Utilities.h"
#include "XPMPMultiplayer.h"
#include "Lib/json.hpp"
using json = nlohmann::json;

namespace xpilot
{
    void to_json(json& j, const CslPackage& p) 
    {
        j = json{ {"Path", RemoveSystemPath(p.path)},{"Enabled",p.enabled} };
    }

    void from_json(const json& j, CslPackage& p) 
    {
        j.at("Path").get_to(p.path);
        j.at("Enabled").get_to(p.enabled);
    }

    Config& Config::Instance()
    {
        static auto&& config = Config();
        return config;
    }

    bool Config::LoadConfig()
    {
        std::string configPath(GetPluginPath() + "Resources/Config.json");
        std::ifstream ifs(configPath);
        if (!ifs)
        {
            SaveConfig();
        }

        try
        {
            json jf = json::parse(ifs);
            if (!jf.empty())
            {
                if (jf.contains("ShowAircraftLabels"))
                {
                    SetShowHideLabels(jf["ShowAircraftLabels"]);
                }
                if (jf.contains("DefaultIcaoType"))
                {
                    SetDefaultAcIcaoType(jf["DefaultIcaoType"]);
                }
                if (jf.contains("PluginPort"))
                {
                    SetTcpPort(jf["PluginPort"]);
                }
                if (jf.contains("DebugModelMatching"))
                {
                    SetDebugModelMatching(jf["DebugModelMatching"]);
                }
                if (jf.contains("EnableDefaultAtis"))
                {
                    SetDefaultAtisEnabled(jf["EnableDefaultAtis"]);
                }
                if (jf.contains("ShowNotificationBar"))
                {
                    SetShowNotificationPanel(jf["ShowNotificationBar"]);
                }
                if (jf.contains("NotificationBarDisappearTime"))
                {
                    SetNotificationPanelDisappearTime(jf["NotificationBarDisappearTime"]);
                }
                if (jf.contains("OverrideContactAtc"))
                {
                    SetOverrideContactAtcCommand(jf["OverrideContactAtc"]);
                }
                if (jf.contains("DisableTcas"))
                {
                    SetDisableTcas(jf["DisableTcas"]);
                }
                if (jf.contains("LabelColor"))
                {
                    SetAircraftLabelColor(jf["LabelColor"]);
                }
                if (jf.contains("MaxLabelDist"))
                {
                    SetMaxLabelDistance(jf["MaxLabelDist"]);
                }
                if (jf.contains("LabelCutoffVis"))
                {
                    SetLabelCutoffVis(jf["LabelCutoffVis"]);
                }
                if (jf.contains("CSL"))
                {
                    json cslpackages = jf["CSL"];
                    for (auto& p : cslpackages)
                    {
                        auto csl = p.get<CslPackage>();
                        if (std::find(mCslPackages.begin(), mCslPackages.end(), csl.path) == mCslPackages.end())
                        {
                            mCslPackages.emplace_back(csl);
                        }
                    }
                }
                SaveConfig();
            }
        }
        catch (const std::string& e)
        {
            return false;
        }
        catch (...)
        {
            return false;
        }
        return true;
    }

    bool Config::SaveConfig()
    {
        std::string configPath(GetPluginPath() + "Resources/Config.json");
        std::ofstream file(configPath);

        json j;

        j["ShowAircraftLabels"] = GetShowHideLabels();
        j["DefaultIcaoType"] = GetDefaultAcIcaoType();
        j["PluginPort"] = GetTcpPort();
        j["DebugModelMatching"] = GetDebugModelMatching();
        j["EnableDefaultAtis"] = GetDefaultAtisEnabled();
        j["ShowNotificationBar"] = GetShowNotificationBar();
        j["NotificationBarDisappearTime"] = GetNotificationBarDisappaerTime();
        j["OverrideContactAtc"] = GetOverrideContactAtcCommand();
        j["LabelColor"] = GetAircraftLabelColor();
        j["DisableTcas"] = GetDisableTcas();
        j["MaxLabelDist"] = GetMaxLabelDistance();
        j["LabelCutoffVis"] = GetLabelCutoffVis();

        if (!mCslPackages.empty())
        {
            auto jsonObjects = json::array();
            for (CslPackage& p : mCslPackages)
            {
                if (!p.path.empty())
                {
                    json j = p;
                    jsonObjects.push_back(j);
                }
            }
            j["CSL"] = jsonObjects;
        }

        file << j;

        if (file.fail()) 
            return false;

        file.flush();
        file.close();
        return true;
    }

    bool Config::HasValidPaths() const
    {
        return (std::count_if(mCslPackages.begin(), mCslPackages.end(), [](const CslPackage& p) {
            return !p.path.empty() && p.enabled && CountFilesInPath(p.path) > 0;
        }) > 0);
    }

    void Config::SaveCSLPath(int idx, std::string path)
    {
        while (size_t(idx) >= mCslPackages.size()) 
        {
            mCslPackages.push_back({});
        }
        std::replace(path.begin(), path.end(), '/', '\\');
        mCslPackages[idx].path = path;
    }

    void Config::SaveCSLEnabled(int idx, bool enabled)
    {
        while (size_t(idx) >= mCslPackages.size()) 
        {
            mCslPackages.push_back({});
        }
        mCslPackages[idx].enabled = enabled;
    }

    // Load a CSL package interactively
    bool Config::LoadCSLPackage(int idx)
    {
        if (size_t(idx) < mCslPackages.size())
        {
            const std::string path = GetPluginPath() + mCslPackages[idx].path;

            if (CountFilesInPath(path) > 1)
            {
                LOG_INFO("Found CSL Path: %s", path.c_str());
                if (!path.empty())
                {
                    auto err = XPMPLoadCSLPackage(path.c_str());
                    if (*err)
                    {
                        LOG_ERROR("Error loading CSL package (%s): %s", path.c_str(), err);
                    }
                    else
                    {
                        LOG_INFO("CSL package successfully loaded: %s", path.c_str());
                        return true;
                    }
                }
            }
            else
            {
                LOG_INFO("Skipping CSL path '%s' because it does not exist or the folder is empty.", path.c_str());
            }
        }
        return false;
    }

    bool Config::SetDefaultAcIcaoType(const std::string type)
    {
        mDefaultAcIcaoType = type;
        XPMPSetDefaultPlaneICAO(type.c_str());
        return true;
    }

    bool Config::SetShowHideLabels(bool status)
    {
        mShowHideLabels = status;
        return true;
    }

    bool Config::SetDebugModelMatching(bool status)
    {
        mDebugModelMatching = status;
        return true;
    }

    bool Config::SetTcpPort(int port)
    {
        mTcpPort = port;
        return true;
    }

    bool Config::SetDefaultAtisEnabled(bool status)
    {
        mDefaultAtis = status;
        return true;
    }

    bool Config::SetOverrideContactAtcCommand(bool status)
    {
        mOverrideContactAtcCommand = status;
        return true;
    }

    bool Config::SetAircraftLabelColor(int c)
    {
        if (c > 0 && c <= 0xFFFFFF)
        {
            mLabelColor = c;
        }
        else
        {
            mLabelColor = COLOR_YELLOW;
        }
        return true;
    }

    bool Config::SetDisableTcas(bool status)
    {
        mDisableTcas = status;
        return true;
    }

    bool Config::SetShowNotificationPanel(bool show)
    {
        mShowNotificationBar = show;
        return true;
    }

    bool Config::SetNotificationPanelDisappearTime(int timeout)
    {
        mNotificationBarDisappearTime = timeout;
        return true;
    }
    
    bool Config::SetMaxLabelDistance(int v)
    {
        mMaxLabelDist = v;
        return true;
    }

    bool Config::SetLabelCutoffVis(bool b)
    {
        mLabelCutoffVis = b;
        return true;
    }
}