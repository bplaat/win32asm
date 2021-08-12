#define WIN32_MALLOC
#define WIN32_FREE
#define WIN32_RAND
#define WIN32_WCSLEN
#include "win32.h"

wchar_t *window_class_name = L"window-borderless";

#ifdef WIN64
    wchar_t *window_title = L"This is a test borderless window 😍 (64-bit)";
#else
    wchar_t *window_title = L"This is a test borderless window 😍 (32-bit)";
#endif

wchar_t *font_name = L"Comic Sans MS";

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_STYLE (WS_POPUP | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CAPTION | WS_SYSMENU)
#define TITLEBAR_HEIGHT 32
#define TITLEBAR_BUTTON_WIDTH 48

typedef struct {
    int32_t width;
    int32_t height;
    uint32_t background_color;
    uint32_t hover_background_color;
    bool active;
    bool minimize_hover;
    bool maximize_hover;
    bool close_hover;
} WindowData;

int32_t __stdcall WndProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam) {
    WindowData *window = GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    if (msg == WM_CREATE) {
        // Create window data
        window = malloc(sizeof(WindowData));
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, window);
        window->width = WINDOW_WIDTH;
        window->height = WINDOW_HEIGHT;

        // Generate random seed by time
        SYSTEMTIME time;
        GetLocalTime(&time);
        srand((time.wHour * 60 + time.wMinute) * 60 + time.wSecond);

        // Generate random background color
        window->background_color = rand() & 0x007f7f7f;
        window->hover_background_color = ((window->background_color & 0xff) + 0x17) |
            ((((window->background_color >> 8) & 0xff) + 0x17) << 8) |
            ((((window->background_color >> 16) & 0xff) + 0x17) << 16);

        window->active = true;
        window->minimize_hover = false;
        window->maximize_hover = false;
        window->close_hover = false;
        return 0;
    }

    if (msg == WM_ACTIVATE) {
        MARGINS borderless = { 1, 1, 1, 1 };
        DwmExtendFrameIntoClientArea(hwnd, &borderless);
        return 0;
    }

    if (msg == WM_NCACTIVATE) {
        window->active = wParam;
        InvalidateRect(hwnd, NULL, false);
    }

    if (msg == WM_NCCALCSIZE) {
        if (wParam) {
            WINDOWPLACEMENT placement;
            GetWindowPlacement(hwnd, &placement);
            if (placement.showCmd == SW_MAXIMIZE) {
                NCCALCSIZE_PARAMS *params = lParam;
                HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
                if (!monitor) return 0;
                MONITORINFO monitor_info;
                monitor_info.cbSize = sizeof(MONITORINFO);
                if (!GetMonitorInfoW(monitor, &monitor_info)) {
                    return 0;
                }
                params->rgrc[0] = monitor_info.rcWork;
            }
        }
        return 0;
    }

    if (msg == WM_NCHITTEST) {
        int32_t x = GET_X_LPARAM(lParam);
        int32_t y = GET_Y_LPARAM(lParam);

        RECT window_rect;
        GetWindowRect(hwnd, &window_rect);

        int32_t border_horizontal = GetSystemMetrics(SM_CXSIZEFRAME);
        int32_t border_vertical = GetSystemMetrics(SM_CYSIZEFRAME);

        if (y >= window_rect.top && y < window_rect.bottom) {
            if (x >= window_rect.left && x < window_rect.left + border_horizontal) {
                if (y < window_rect.top + border_vertical) {
                    return HTTOPLEFT;
                }
                if (y > window_rect.bottom - border_vertical) {
                    return HTBOTTOMLEFT;
                }
                return HTLEFT;
            }
            if (x >= window_rect.right - border_horizontal && x < window_rect.right) {
                if (y < window_rect.top + border_vertical) {
                    return HTTOPRIGHT;
                }
                if (y > window_rect.bottom - border_vertical) {
                    return HTBOTTOMRIGHT;
                }
                return HTRIGHT;
            }
        }

        if (x >= window_rect.left && x < window_rect.right) {
            if (y >= window_rect.top && y < window_rect.top + border_vertical) {
                return HTTOP;
            }
            if (y >= window_rect.bottom - border_vertical && y < window_rect.bottom) {
                return HTBOTTOM;
            }
        }

        if (x >= window_rect.left && y >= window_rect.top && x < window_rect.right && y < window_rect.bottom) {
            x -= window_rect.left;
            y -= window_rect.top;
            if (y < TITLEBAR_HEIGHT && x < window->width - TITLEBAR_BUTTON_WIDTH * 3) {
                return HTCAPTION;
            }
            return HTCLIENT;
        }

        return HTNOWHERE;
    }

    if (msg == WM_SIZE) {
        // Save new window size
        window->width = LOWORD(lParam);
        window->height = HIWORD(lParam);
        return 0;
    }

    if (msg == WM_GETMINMAXINFO) {
        // Set window min size
        MINMAXINFO *minMaxInfo = (MINMAXINFO *)lParam;
        minMaxInfo->ptMinTrackSize.x = 640;
        minMaxInfo->ptMinTrackSize.y = 480;
        return 0;
    }

    if (msg == WM_LBUTTONUP) {
        int32_t x = GET_X_LPARAM(lParam);
        int32_t y = GET_Y_LPARAM(lParam);

        // Window decoration buttons
        if (y >= 0 && y < TITLEBAR_HEIGHT) {
            if (
                x >= window->width - TITLEBAR_BUTTON_WIDTH * 3 &&
                x < window->width - TITLEBAR_BUTTON_WIDTH  * 2
            ) {
                ShowWindow(hwnd, SW_MINIMIZE);
                window->minimize_hover = false;
                ReleaseCapture();
            }

            if (
                x >= window->width - TITLEBAR_BUTTON_WIDTH  * 2 &&
                x < window->width - TITLEBAR_BUTTON_WIDTH  * 1
            ) {
                WINDOWPLACEMENT placement;
                GetWindowPlacement(hwnd, &placement);
                if (placement.showCmd == SW_MAXIMIZE) {
                    ShowWindow(hwnd, SW_RESTORE);
                } else {
                    ShowWindow(hwnd, SW_MAXIMIZE);
                }
                window->maximize_hover = false;
                ReleaseCapture();
            }

            if (
                x >= window->width - TITLEBAR_BUTTON_WIDTH  * 1 &&
                x < window->width
            ) {
                DestroyWindow(hwnd);
                window->close_hover = false;
                ReleaseCapture();
            }
        }
        return 0;
    }

    if (msg == WM_MOUSEMOVE) {
        int32_t x = GET_X_LPARAM(lParam);
        int32_t y = GET_Y_LPARAM(lParam);

        // Window decoration buttons
        bool new_minimize_hover = window->minimize_hover;
        bool new_maximize_hover = window->minimize_hover;
        bool new_close_hover = window->close_hover;
        if (y >= 0 && y < TITLEBAR_HEIGHT) {
            new_minimize_hover = x >= window->width - TITLEBAR_BUTTON_WIDTH * 3 &&
                x < window->width - TITLEBAR_BUTTON_WIDTH  * 2;

            new_maximize_hover = x >= window->width - TITLEBAR_BUTTON_WIDTH  * 2 &&
                x < window->width - TITLEBAR_BUTTON_WIDTH  * 1;

            new_close_hover = x >= window->width - TITLEBAR_BUTTON_WIDTH  * 1 &&
                    x < window->width;
        } else {
            new_minimize_hover = false;
            new_maximize_hover = false;
            new_close_hover = false;
        }

        if (
            new_minimize_hover != window->minimize_hover ||
            new_maximize_hover != window->maximize_hover ||
            new_close_hover != window->close_hover
        ) {
            window->minimize_hover = new_minimize_hover;
            window->maximize_hover = new_maximize_hover;
            window->close_hover = new_close_hover;

            if (window->minimize_hover || window->maximize_hover || window->close_hover) {
                SetCapture(hwnd);
            } else {
                ReleaseCapture();
            }
            InvalidateRect(hwnd, NULL, false);
        }
        return 0;
    }

    if (msg == WM_ERASEBKGND) {
        // Draw no background
        return TRUE;
    }

    if (msg == WM_PAINT) {
        PAINTSTRUCT paint_struct;
        HDC hdc = BeginPaint(hwnd, &paint_struct);

        // Create back buffer
        HDC hdc_buffer = CreateCompatibleDC(hdc);
        HBITMAP bitmap_buffer = CreateCompatibleBitmap(hdc, window->width, window->height);
        SelectObject(hdc_buffer, bitmap_buffer);

        // Draw background color
        HBRUSH brush = CreateSolidBrush(window->background_color);
        RECT rect = { 0, 0, window->width, window->height };
        FillRect(hdc_buffer, &rect, brush);
        DeleteObject(brush);

        // Draw window decoration buttons
        int32_t font_size = 18;
        HFONT font = CreateFontW(font_size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
        SelectObject(hdc_buffer, font);
        SetBkMode(hdc_buffer, TRANSPARENT);
        SetTextAlign(hdc_buffer, TA_CENTER);
        SetTextColor(hdc_buffer, window->active ? 0x00ffffff : 0x00aaaaaa);

        // Minimize button
        int32_t x = window->width - TITLEBAR_BUTTON_WIDTH * 3;
        if (window->minimize_hover) {
            HBRUSH brush = CreateSolidBrush(window->hover_background_color);
            RECT rect = { x, 0, x + TITLEBAR_BUTTON_WIDTH, TITLEBAR_HEIGHT };
            FillRect(hdc_buffer, &rect, brush);
            DeleteObject(brush);
        }
        wchar_t *text = L"🗕";
        TextOutW(hdc_buffer, x + TITLEBAR_BUTTON_WIDTH / 2, (TITLEBAR_HEIGHT - font_size) / 2, text, wcslen(text));
        x += TITLEBAR_BUTTON_WIDTH;

        // Maximize button
        if (window->maximize_hover) {
            HBRUSH brush = CreateSolidBrush(window->hover_background_color);
            RECT rect = { x, 0, x + TITLEBAR_BUTTON_WIDTH, TITLEBAR_HEIGHT };
            FillRect(hdc_buffer, &rect, brush);
            DeleteObject(brush);
        }
        WINDOWPLACEMENT placement;
        GetWindowPlacement(hwnd, &placement);
        text = placement.showCmd == SW_MAXIMIZE ? L"🗗" : L"🗖";
        TextOutW(hdc_buffer, x + TITLEBAR_BUTTON_WIDTH / 2, (TITLEBAR_HEIGHT - font_size) / 2, text, wcslen(text));
        x += TITLEBAR_BUTTON_WIDTH;

        // Close button
        if (window->close_hover) {
            HBRUSH brush = CreateSolidBrush(window->hover_background_color);
            RECT rect = { x, 0, x + TITLEBAR_BUTTON_WIDTH, TITLEBAR_HEIGHT };
            FillRect(hdc_buffer, &rect, brush);
            DeleteObject(brush);
        }
        text = L"🗙";
        TextOutW(hdc_buffer, x + TITLEBAR_BUTTON_WIDTH / 2, (TITLEBAR_HEIGHT - font_size) / 2, text, wcslen(text));

        DeleteObject(font);

        // Draw centered text
        font_size = window->width / 16;
        font = CreateFontW(font_size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
        SelectObject(hdc_buffer, font);
        SetTextColor(hdc_buffer, 0x00ffffff);
        TextOutW(hdc_buffer, window->width / 2, (window->height - font_size) / 2, window_title, wcslen(window_title));
        DeleteObject(font);

        // Draw footer text
        font_size = window->width / 24;
        font = CreateFontW(font_size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
        SelectObject(hdc_buffer, font);
        wchar_t string_buffer[64];
        wsprintfW(string_buffer, L"(%dx%d)", window->width, window->height);
        TextOutW(hdc_buffer, window->width / 2, window->height - font_size - 24, string_buffer, wcslen(string_buffer));
        DeleteObject(font);

        // Draw and delete back buffer
        BitBlt(hdc, 0, 0, window->width, window->height, hdc_buffer, 0, 0, SRCCOPY);
        DeleteObject(bitmap_buffer);
        DeleteDC(hdc_buffer);

        EndPaint(hwnd, &paint_struct);
        return 0;
    }

    if (msg == WM_DESTROY) {
        // Free window data
        free(window);

        // Close process
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void _start(void) {
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.hIcon = LoadIconW(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.lpszClassName = window_class_name;
    wc.hIconSm = wc.hIcon;
    RegisterClassExW(&wc);

    RECT window_rect;
    window_rect.left = (GetSystemMetrics(SM_CXSCREEN) - WINDOW_WIDTH) / 2;
    window_rect.top = (GetSystemMetrics(SM_CYSCREEN) - WINDOW_HEIGHT) / 2;
    window_rect.right = window_rect.left + WINDOW_WIDTH;
    window_rect.bottom = window_rect.top + WINDOW_HEIGHT;

    HWND hwnd = CreateWindowExW(0, window_class_name, window_title,
        WINDOW_STYLE, window_rect.left, window_rect.top,
        window_rect.right - window_rect.left, window_rect.bottom - window_rect.top,
        NULL, NULL, wc.hInstance, NULL);
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    MSG message;
    while (GetMessageW(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    ExitProcess((uintptr_t)message.wParam);
}
