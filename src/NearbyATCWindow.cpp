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

#include <list>
#include <mutex>
#include "XPImgWindow.h"
#include "NearbyATCWindow.h"
#include "XPilot.h"

namespace xpilot {

	static std::list<NearbyATCList> NearbyList;

	NearbyATCWindow::NearbyATCWindow(XPilot* instance) :
		XPImgWindow(WND_MODE_FLOAT_CENTERED, WND_STYLE_SOLID, WndRect(0, 300, 500, 0)),
		m_com1Frequency("sim/cockpit2/radios/actuators/com1_frequency_hz_833", ReadWrite),
		m_env(instance)
	{
		SetWindowTitle("Nearby ATC");
		SetWindowResizingLimits(500, 300, 500, 300);
	}

	void NearbyATCWindow::UpdateList(xpilot::NearbyControllers data)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		{
			NearbyList.clear();
			for (auto& el : data.list())
			{
				NearbyATCList l;
				l.setCallsign(el.callsign());
				l.setFrequency(el.frequency());
				l.setXplaneFrequency(el.xplane_frequency());
				l.setRealName(el.real_name());
				NearbyList.push_back(l);
			}
		}
	}

	void NearbyATCWindow::ClearList()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		{
			NearbyList.clear();
		}
	}

	void NearbyATCWindow::buildInterface() {

		ImGui::PushFont(0);

		ImGui::Columns(3, "whosonline");
		ImGui::SetColumnWidth(0, 150);
		ImGui::SetColumnWidth(1, 200);
		ImGui::SetColumnWidth(2, 100);

		ImGui::Separator();

		ImGui::Text("Callsign");
		ImGui::NextColumn();
		ImGui::Text("Real Name");
		ImGui::NextColumn();
		ImGui::Text("Frequency");
		ImGui::NextColumn();

		ImGui::Separator();

		std::lock_guard<std::mutex> lock(m_mutex);
		{
			for (auto& e : NearbyList)
			{
				ImGui::Selectable(e.getCallsign().c_str(), false, ImGuiSelectableFlags_SpanAllColumns);
				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
				{
					m_env->requestControllerAtis(e.getCallsign());
				}
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
				{
					m_com1Frequency = e.getXplaneFrequency();
				}
				ImGui::NextColumn();
				ImGui::Text(e.getRealName().c_str());
				ImGui::NextColumn();
				ImGui::Text(e.getFrequency().c_str());
				ImGui::NextColumn();
			}
		}
		ImGui::PopFont();
	}
}