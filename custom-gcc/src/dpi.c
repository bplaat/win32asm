#include "dpi.h"

void SetDPIAware(void) {
    HMODULE hshcore = LoadLibraryW(L"shcore");
    if (hshcore) {
        _SetProcessDpiAwareness SetProcessDpiAwareness = GetProcAddress(hshcore, "SetProcessDpiAwareness");
        if (SetProcessDpiAwareness) {
            SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
        }
    } else {
        HMODULE huser = LoadLibraryW(L"user");
        _SetProcessDPIAware SetProcessDPIAware = GetProcAddress(huser, "SetProcessDPIAware");
        if (SetProcessDPIAware) {
            SetProcessDPIAware();
        }
    }
}

int32_t GetWindowDPI(HWND hwnd) {
    HMODULE hShcore = LoadLibraryW(L"shcore");
    if (hShcore) {
        _GetDpiForMonitor GetDpiForMonitor = GetProcAddress(hShcore, "GetDpiForMonitor");
        if (GetDpiForMonitor) {
            HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
            uint32_t uiDpiX, uiDpiY;
            GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &uiDpiX, &uiDpiY);
            return uiDpiY;
        }
    }

    HDC hdc = GetDC(hwnd);
    int32_t dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    DeleteDC(hdc);
    return dpi;
}
