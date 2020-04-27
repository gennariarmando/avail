#ifndef AVAIL_SCREEN
#define AVAIL_SCREEN

#include <WTypes.h>

#include <stdio.h>
#include <cstdint>
#include <tuple>
#include <cmath>

namespace avail {
    // Internal use only.
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

#ifndef _WIN64
    static float fAspectRatio;
    static float fWidth;
    static float fHeight;

    static float fFOV;
    static float fFOVMult;
#else
    static float& fAspectRatio = Trampoline::MakeTrampoline(GetModuleHandle(nullptr))->Reference<float>();
    static float& fWidth = Trampoline::MakeTrampoline(GetModuleHandle(nullptr))->Reference<float>();
    static float& fHeight = Trampoline::MakeTrampoline(GetModuleHandle(nullptr))->Reference<float>();

    static float& fFOV = Trampoline::MakeTrampoline(GetModuleHandle(nullptr))->Reference<float>();
    static float& fFOVMult = Trampoline::MakeTrampoline(GetModuleHandle(nullptr))->Reference<float>();
#endif

    // Set aspect ratio based on window size.
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
        fWidth = fCustomAspectRatioHor;
        fHeight = fCustomAspectRatioVer;
    }

    // Set FOV based on original FOV/Aspect Ratio.
    inline void SetFOV(float originalFOV, float originalAR) {
        fFOV = std::round((2.0f * atan(((fAspectRatio) / (originalAR)) * tan(originalFOV / 2.0f * ((float)M_PI / 180.0f)))) * (180.0f / (float)M_PI) * 100.0f) / 100.0f;
    }

    // Set FOV based on a FOV multiplier and original Aspect Ratio.
    inline void SetFOVMult(float originalFOVMult, float originalAR) {
        float f = std::round((2.0f * atan(((fAspectRatio) / (originalAR)) * tan((originalFOVMult * 10000.0f) / 2.0f * ((float)M_PI / 180.0f)))) * (180.0f / (float)M_PI) * 100.0f) / 100.0f;
        fFOVMult = f / 10000.0f;
    }

    // After initialization, call this to get respective aspect ratio and fov.
    inline float &GetAspectRatio() { return fAspectRatio; }
    inline float &GetFieldOfView() { return fFOV; }
    inline float &GetFOVMult() { return fFOVMult; }
};

#endif