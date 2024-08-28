// ### Win32 Helpers v0.1.0-dev ###
// Some random Win32 Helper Functions
// ~ Made by Bastiaan van der Plaat (https://bastiaan.ml/)

// Define options:
// HELPERS_IMPLEMENTATION: Use this once in your project for the c code
// HELPERS_ENABLE_DPI: Enable DPI aware helpers
// HELPERS_ENABLE_VERSION: Enable Windows version helpers
// HELPERS_ENABLE_LANGUAGE: Enable resource language helper (needs HELPERS_ENABLE_VERSION)
// HELPERS_ENABLE_FULLSCREEN: Enable fullscreen window helper
// HELPERS_ENABLE_IMMERSIVE_DARK_MODE: Enable dark window decoration helper for Windows 10+

#pragma once

#include <windows.h>
#include <stdint.h>

// Some general stuff that is not in the Tiny C Compiler / MinGW includes
#ifndef MAX
    #define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif
#ifndef MIN
    #define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef GET_X_LPARAM
    #define GET_X_LPARAM(lParam) ((int32_t)(int16_t)LOWORD(lParam))
#endif
#ifndef GET_Y_LPARAM
    #define GET_Y_LPARAM(lParam) ((int32_t)(int16_t)HIWORD(lParam))
#endif

#ifndef WM_DPICHANGED
    #define WM_DPICHANGED 0x02e0
#endif

int32_t GetPrimaryDesktopDpi(void);

typedef BOOL (STDMETHODCALLTYPE *_AdjustWindowRectExForDpi)(RECT *lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi);

BOOL AdjustWindowRectExForDpi(RECT *lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi);

bool IsVistaOrHigher(void);

typedef LANGID (STDMETHODCALLTYPE *_SetThreadUILanguage)(LANGID LangId);

void SetThreadLanguage(LANGID language);

extern WINDOWPLACEMENT previousPlacement;

void SetWindowFullscreen(HWND hwnd, BOOL enabled);

#define DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 19
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20

typedef HRESULT (STDMETHODCALLTYPE *_DwmSetWindowAttribute)(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute);

void SetWindowImmersiveDarkMode(HWND hwnd, BOOL enabled);

// ##########################################################################################################
// ##########################################################################################################
// ##########################################################################################################
// ##########################################################################################################
// ##########################################################################################################

#ifdef HELPERS_IMPLEMENTATION

#ifdef HELPERS_ENABLE_DPI

int32_t GetPrimaryDesktopDpi(void) {
    HDC hdc = GetDC(HWND_DESKTOP);
    int32_t dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(HWND_DESKTOP, hdc);
    return dpi;
}

BOOL AdjustWindowRectExForDpi(RECT *lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi) {
    HMODULE huser32 = LoadLibraryW(L"user32.dll");
    _AdjustWindowRectExForDpi AdjustWindowRectExForDpi = (_AdjustWindowRectExForDpi)GetProcAddress(huser32, "AdjustWindowRectExForDpi");
    if (AdjustWindowRectExForDpi) {
        return AdjustWindowRectExForDpi(lpRect, dwStyle, bMenu, dwExStyle, dpi);
    }
    return AdjustWindowRectEx(lpRect, dwStyle, bMenu, dwExStyle);
}

#endif

#ifdef HELPERS_ENABLE_VERSION

bool IsVistaOrHigher(void) {
    OSVERSIONINFOW osver = {0};
    osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
    GetVersionExW(&osver);
    return osver.dwMajorVersion >= 6;
}

#ifdef HELPERS_ENABLE_LANGUAGE

void SetThreadLanguage(LANGID language) {
    if (IsVistaOrHigher()) {
        HMODULE kernel32 = LoadLibraryW(L"kernel32.dll");
        _SetThreadUILanguage SetThreadUILanguage = (_SetThreadUILanguage)GetProcAddress(kernel32, "SetThreadUILanguage");
        SetThreadUILanguage(language);
    } else {
        SetThreadLocale(language);
    }
}

#endif

#endif

#ifdef HELPERS_ENABLE_FULLSCREEN

WINDOWPLACEMENT previousPlacement = { sizeof(previousPlacement) };

void SetWindowFullscreen(HWND hwnd, BOOL enabled) {
    DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
    if (enabled) {
        MONITORINFO monitorInfo = { sizeof(monitorInfo) };
        if (
            GetWindowPlacement(hwnd, &previousPlacement) &&
            GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &monitorInfo)
        ) {
            SetWindowLong(hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(hwnd, HWND_TOP, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
                monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    } else {
        SetWindowLong(hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(hwnd, &previousPlacement);
        SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

#endif

#ifdef HELPERS_ENABLE_IMMERSIVE_DARK_MODE

void SetWindowImmersiveDarkMode(HWND hwnd, BOOL enabled) {
    HMODULE hdwmapi = LoadLibraryW(L"dwmapi.dll");
    _DwmSetWindowAttribute DwmSetWindowAttribute = (_DwmSetWindowAttribute)GetProcAddress(hdwmapi, "DwmSetWindowAttribute");
    if (DwmSetWindowAttribute) {
        DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1, &enabled, sizeof(BOOL));
        DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &enabled, sizeof(BOOL));
    }
}

#endif

#endif
