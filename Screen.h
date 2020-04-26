#ifndef __SCREEN
#define __SCREEN

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdio.h>
#include <cstdint>
#include <tuple>
#include <cmath>

struct handle_data {
    unsigned long process_id;
    HWND window_handle;
};

BOOL CALLBACK EnumWindowsCallBack(HWND handle, LPARAM lparam) {
    auto& data = *reinterpret_cast<handle_data*>(lparam);

    unsigned long process_id = 0;
    GetWindowThreadProcessId(handle, &process_id);

    if (data.process_id != process_id) {
        return TRUE;
    }

    data.window_handle = handle;
    return FALSE;
}

HWND FindMainWindow() {
    handle_data data = {};

    data.process_id = GetCurrentProcessId();
    data.window_handle = nullptr;
    EnumWindows(EnumWindowsCallBack, reinterpret_cast<LPARAM>(&data));

    return data.window_handle;
}

std::tuple<int32_t, int32_t> GetWindowRes() {
    WINDOWINFO info = {};
    info.cbSize = sizeof(WINDOWINFO);
    GetWindowInfo(FindMainWindow(), &info);
    int32_t w = info.rcClient.right - info.rcClient.left;
    int32_t h = info.rcClient.bottom - info.rcClient.top;
    return std::make_tuple(w, h);
}

namespace screen {
    static float fAspectRatio;
    static float fFOV;

    inline void SetAspectRatio() {
        uint32_t AspectRatioWidth, AspectRatioHeight;
        uint32_t ViewPortWidth, ViewPortHeight;
        float fCustomAspectRatioHor, fCustomAspectRatioVer;

        std::tie(ViewPortWidth, ViewPortHeight) = GetWindowRes();

        AspectRatioWidth = ViewPortWidth;
        AspectRatioHeight = ViewPortHeight;

        fCustomAspectRatioHor = static_cast<float>(AspectRatioWidth);
        fCustomAspectRatioVer = static_cast<float>(AspectRatioHeight);

        fAspectRatio = fCustomAspectRatioHor / fCustomAspectRatioVer;
    }

    inline void SetFOV(float originalFOV, float originalAR) {
        fFOV = std::round((2.0f * atan(((fAspectRatio) / (originalAR)) * tan(originalFOV / 2.0f * ((float)M_PI / 180.0f)))) * (180.0f / (float)M_PI) * 100.0f) / 100.0f;
    }

    inline float GetAspectRatio() { return fAspectRatio; }
    inline float GetFieldOfView() { return fFOV; }
};

static void InitScreen(float originalFOV, float originalAR) {
    float ar = screen::GetAspectRatio();

    do {
        if (ar != screen::GetAspectRatio()) {
            screen::SetAspectRatio();
            screen::SetFOV(originalFOV, originalAR);
            ar = screen::GetAspectRatio();
        }
    } while (0);
}

#endif