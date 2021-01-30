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
#include "json.hpp"
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

    bool Config::loadConfig()
    {
        std::string configPath(GetPluginPath() + "Resources/Config.json");
        std::ifstream ifs(configPath);
        if (!ifs)
        {
            saveConfig();
        }

        try
        {
            json jf = json::parse(ifs);
            if (!jf.empty())
            {
                if (jf.contains("ShowAircraftLabels"))
                {
                    setShowHideLabels(jf["ShowAircraftLabels"]);
                }
                if (jf.contains("DefaultIcaoType"))
                {
                    setDefaultAcIcaoType(jf["DefaultIcaoType"]);
                }
                if (jf.contains("PluginPort"))
                {
                    setTcpPort(jf["PluginPort"]);
                }
                if (jf.contains("DebugModelMatching"))
                {
                    setDebugModelMatching(jf["DebugModelMatching"]);
                }
                if (jf.contains("EnableDefaultAtis"))
                {
                    setDefaultAtisEnabled(jf["EnableDefaultAtis"]);
                }
                if (jf.contains("ShowNotificationBar"))
                {
                    setShowNotificationPanel(jf["ShowNotificationBar"]);
                }
                if (jf.contains("NotificationBarDisappearTime"))
                {
                    setNotificationPanelDisappearTime(jf["NotificationBarDisappearTime"]);
                }
                if (jf.contains("OverrideContactAtc"))
                {
                    setOverrideContactAtcCommand(jf["OverrideContactAtc"]);
                }
                if (jf.contains("DisableTcas"))
                {
                    setDisableTcas(jf["DisableTcas"]);
                }
                if (jf.contains("LabelColor"))
                {
                    setAircraftLabelColor(jf["LabelColor"]);
                }
                if (jf.contains("MaxLabelDist"))
                {
                    setMaxLabelDistance(jf["MaxLabelDist"]);
                }
                if (jf.contains("LabelCutoffVis"))
                {
                    setLabelCutoffVis(jf["LabelCutoffVis"]);
                }
                if (jf.contains("CSL"))
                {
                    json cslpackages = jf["CSL"];
                    for (auto& p : cslpackages)
                    {
                        auto csl = p.get<CslPackage>();
                        if (std::find(m_cslPackages.begin(), m_cslPackages.end(), csl.path) == m_cslPackages.end())
                        {
                            m_cslPackages.emplace_back(csl);
                        }
                    }
                }
                saveConfig();
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

    bool Config::saveConfig()
    {
        std::string configPath(GetPluginPath() + "Resources/Config.json");
        std::ofstream file(configPath);

        json j;

        j["ShowAircraftLabels"] = getShowHideLabels();
        j["DefaultIcaoType"] = getDefaultAcIcaoType();
        j["PluginPort"] = getTcpPort();
        j["DebugModelMatching"] = getDebugModelMatching();
        j["EnableDefaultAtis"] = getDefaultAtisEnabled();
        j["ShowNotificationBar"] = getShowNotificationBar();
        j["NotificationBarDisappearTime"] = getNotificationBarDisappaerTime();
        j["OverrideContactAtc"] = getOverrideContactAtcCommand();
        j["LabelColor"] = getAircraftLabelColor();
        j["DisableTcas"] = getDisableTcas();
        j["MaxLabelDist"] = getMaxLabelDistance();
        j["LabelCutoffVis"] = getLabelCutoffVis();

        if (!m_cslPackages.empty())
        {
            auto jsonObjects = json::array();
            for (CslPackage& p : m_cslPackages)
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

    bool Config::hasValidPaths() const
    {
        return (std::count_if(m_cslPackages.begin(), m_cslPackages.end(), [](const CslPackage& p) {
            return !p.path.empty() && p.enabled && CountFilesInPath(p.path) > 0;
        }) > 0);
    }

    void Config::saveCSLPath(int idx, std::string path)
    {
        while (size_t(idx) >= m_cslPackages.size()) 
        {
            m_cslPackages.push_back({});
        }
        std::replace(path.begin(), path.end(), '/', '\\');
        m_cslPackages[idx].path = path;
    }

    void Config::saveCSLEnabled(int idx, bool enabled)
    {
        while (size_t(idx) >= m_cslPackages.size()) 
        {
            m_cslPackages.push_back({});
        }
        m_cslPackages[idx].enabled = enabled;
    }

    // Load a CSL package interactively
    bool Config::loadCSLPackage(int idx)
    {
        if (size_t(idx) < m_cslPackages.size())
        {
            const std::string path = GetPluginPath() + m_cslPackages[idx].path;

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

    bool Config::setDefaultAcIcaoType(const std::string type)
    {
        m_defaultAcIcaoType = type;
        XPMPSetDefaultPlaneICAO(type.c_str());
        return true;
    }

    bool Config::setShowHideLabels(bool status)
    {
        m_showHideLabels = status;
        return true;
    }

    bool Config::setDebugModelMatching(bool status)
    {
        m_debugModelMatching = status;
        return true;
    }

    bool Config::setTcpPort(int port)
    {
        m_tcpPort = port;
        return true;
    }

    bool Config::setDefaultAtisEnabled(bool status)
    {
        m_defaultAtis = status;
        return true;
    }

    bool Config::setOverrideContactAtcCommand(bool status)
    {
        m_overrideContactAtcCommand = status;
        return true;
    }

    bool Config::setAircraftLabelColor(int c)
    {
        if (c > 0 && c <= 0xFFFFFF)
        {
            m_labelColor = c;
        }
        else
        {
            m_labelColor = COLOR_YELLOW;
        }
        return true;
    }

    bool Config::setDisableTcas(bool status)
    {
        m_disableTcas = status;
        return true;
    }

    bool Config::setShowNotificationPanel(bool show)
    {
        m_showNotificationBar = show;
        return true;
    }

    bool Config::setNotificationPanelDisappearTime(int timeout)
    {
        m_notificationBarDisappearTime = timeout;
        return true;
    }
    
    bool Config::setMaxLabelDistance(int v)
    {
        m_maxLabelDist = v;
        return true;
    }

    bool Config::setLabelCutoffVis(bool b)
    {
        m_labelCutoffVis = b;
        return true;
    }
}