#include "dpi.h"

void SetDpiAware(void) {
    HMODULE huser32 = LoadLibraryW(L"user32.dll");
    _SetProcessDpiAwarenessContext SetProcessDpiAwarenessContext = GetProcAddress(huser32, "SetProcessDpiAwarenessContext");
    if (SetProcessDpiAwarenessContext) {
        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        return;
    }

    HMODULE hshcore = LoadLibraryW(L"shcore.dll");
    if (hshcore) {
        _SetProcessDpiAwareness SetProcessDpiAwareness = GetProcAddress(hshcore, "SetProcessDpiAwareness");
        if (SetProcessDpiAwareness) {
            SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
            return;
        }
    }

    _SetProcessDPIAware SetProcessDPIAware = GetProcAddress(huser32, "SetProcessDPIAware");
    if (SetProcessDPIAware) {
        SetProcessDPIAware();
    }
}

int32_t GetDesktopDpi(void) {
    HDC hdc = GetDC(HWND_DESKTOP);
    int32_t dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(HWND_DESKTOP, hdc);
    return dpi;
}

bool AdjustWindowRectExForDpi(RECT *lpRect, uint32_t dwStyle, bool bMenu, uint32_t dwExStyle, uint32_t dpi) {
    HMODULE huser32 = LoadLibraryW(L"user32.dll");
    _AdjustWindowRectExForDpi AdjustWindowRectExForDpi = GetProcAddress(huser32, "AdjustWindowRectExForDpi");
    if (AdjustWindowRectExForDpi) {
        return AdjustWindowRectExForDpi(lpRect, dwStyle, bMenu, dwExStyle, dpi);
    }
    return AdjustWindowRectEx(lpRect, dwStyle, bMenu, dwExStyle);
}
