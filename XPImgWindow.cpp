/*
 * Adapted from LiveTraffic's LTImgWindow
 * (c) 2018-2020 Birger Hoppe
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:\n
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.\n
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

#include "XPImgWindow.h"

XPImgWindow::XPImgWindow(WndMode _mode, WndStyle _style, WndRect _initPos) :
    ImgWindow(_initPos.left(), _initPos.top(),
        _initPos.right(), _initPos.bottom(),
        toDeco(_style), toLayer(_style)),
    wndStyle(_style),
    rectFloat(_initPos),
    mIsVREnabled("sim/graphics/VR/enabled")
{
    // Disable reading/writing of "imgui.ini"
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

    // Create a flight loop id, but don't schedule it yet
    XPLMCreateFlightLoop_t flDef = {
        sizeof(flDef),                              // structSize
        xplm_FlightLoop_Phase_BeforeFlightModel,    // phase
        cbChangeWndMode,                               // callbackFunc
        (void*)this,                                // refcon
    };
    flChangeWndMode = XPLMCreateFlightLoop(&flDef);

    // Set the positioning mode
    SetMode(_mode);
}

XPImgWindow::~XPImgWindow()
{
    if (flChangeWndMode)
        XPLMDestroyFlightLoop(flChangeWndMode);
    flChangeWndMode = nullptr;
}

/// Set the window mode, move the window if needed
void XPImgWindow::SetMode(WndMode _mode)
{
    // auto-set VR mode if requested
    if (_mode == WND_MODE_FLOAT_OR_VR)
        _mode = IsVREnabled() ? WND_MODE_VR : WND_MODE_FLOAT;
    else if (_mode == WND_MODE_FLOAT_CNT_VR)
        _mode = IsVREnabled() ? WND_MODE_VR : WND_MODE_FLOAT_CENTERED;

    // Floating: Save current geometry to have a chance to get back there
    if (GetMode() == WND_MODE_FLOAT && _mode != WND_MODE_FLOAT)
        rectFloat = GetCurrentWindowGeometry();

    // Do set the XP window positioning mode
    SetWindowPositioningMode(toPosMode(_mode, IsVREnabled()));

    // reset a wish to re-position
    nextWinMode = WND_MODE_NONE;

    // If we pop in, then we need to explicitely set a position for the window to appear
    if (_mode == WND_MODE_FLOAT && !rectFloat.empty()) {
        SetWindowGeometry(rectFloat.left(), rectFloat.top(),
            rectFloat.right(), rectFloat.bottom());
        rectFloat.clear();
    }
    // if we set any of the "centered" modes
    // we shall set it back to floating a few flight loops later
    else if (_mode == WND_MODE_FLOAT_CENTERED)
    {
        nextWinMode = WND_MODE_FLOAT;           // to floating
        rectFloat.clear();                      // but don't move the window!
        XPLMScheduleFlightLoop(flChangeWndMode, -5.0, 1);  // in 5 flight loops time
    }
}

/// Get current window mode
WndMode XPImgWindow::GetMode() const
{
    if (IsInVR())
        return WND_MODE_VR;
    if (IsPoppedOut())
        return WND_MODE_POPOUT;
    return WND_MODE_FLOAT;
}

// Get current window geometry as an WndRect structure
WndRect XPImgWindow::GetCurrentWindowGeometry() const
{
    WndRect r;
    ImgWindow::GetCurrentWindowGeometry(r.left(), r.top(), r.right(), r.bottom());
    return r;
}

// Loose keyboard foucs, ie. return focus to X-Plane proper, if I have it now
bool XPImgWindow::ReturnKeyboardFocus()
{
    if (XPLMHasKeyboardFocus(GetWindowId())) {
        XPLMTakeKeyboardFocus(0);
        return true;
    }
    return false;
}

// flight loop callback for stuff we cannot do during drawing callback
// Outside all rendering we can change things like window mode
float XPImgWindow::cbChangeWndMode(float, float, int, void* inRefcon)
{
    // refcon is pointer to ImguiWidget
    XPImgWindow& wnd = *reinterpret_cast<XPImgWindow*>(inRefcon);

    // Has user requested to close the window?
    if (wnd.nextWinMode == WND_MODE_CLOSE)
        delete& wnd;

    // Has user requested a change in window mode?
    else if (wnd.nextWinMode > WND_MODE_NONE)
        wnd.SetMode(wnd.nextWinMode);

    return 0.0f;
}

bool XPImgWindowInit()
{
    if (!ImgWindow::sFontAtlas)
    {
        ImgWindow::sFontAtlas = std::make_shared<ImgFontAtlas>();
    }
    if (!ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Roboto-Regular.ttf", 16.0f))
    {
        return false;
    }
    return true;
}

void XPImgWindowCleanup()
{
    ImgWindow::sFontAtlas.reset();
}