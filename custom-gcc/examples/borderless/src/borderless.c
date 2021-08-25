#include "win32.h"
#include "dpi.h"
#include "canvas.h"

wchar_t *window_class_name = L"window-borderless";

#ifdef WIN64
    wchar_t *window_title = L"This is a test borderless window 🤩 (64-bit)";
#else
    wchar_t *window_title = L"This is a test borderless window 🤩 (32-bit)";
#endif

wchar_t *font_name = L"Segoe UI";

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_STYLE (WS_POPUP | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CAPTION | WS_SYSMENU)

typedef struct WindowData {
    int32_t width;
    int32_t height;
    int32_t dpi;
    bool active;
    Canvas *canvas;

    int32_t titlebar_height;
    int32_t titlebar_button_width;
    int32_t titlebar_icon_size;
    CanvasColor background_color;
    bool menu_hover;
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

        // Resize window when dpi is not 96
        window->dpi = GetWindowDPI(hwnd);
        if (window->dpi != 96) {
            window->width = MulDiv(WINDOW_WIDTH, window->dpi, 96);
            window->height = MulDiv(WINDOW_HEIGHT, window->dpi, 96);
            SetWindowPos(hwnd, NULL, (GetSystemMetrics(SM_CXSCREEN) - window->width) / 2,
                (GetSystemMetrics(SM_CYSCREEN) - window->height) / 2,
                window->width, window->height, SWP_NOZORDER | SWP_NOACTIVATE);
        }

        // Create canvas
        window->active = true;
        window->canvas = Canvas_New(hwnd, CANVAS_RENDERER_DEFAULT);

        // Generate random seed by time
        SYSTEMTIME time;
        GetLocalTime(&time);
        srand((time.wHour * 60 + time.wMinute) * 60 + time.wSecond);

        // Generate random background color and fill other window data
        window->background_color = (rand() & 0x007f7f7f) | 0xff000000;
        window->minimize_hover = false;
        window->maximize_hover = false;
        window->close_hover = false;
        return 0;
    }

    if (msg == WM_ACTIVATE) {
        MARGINS borderless = { 0, 0, 0, 1 };
        DwmExtendFrameIntoClientArea(hwnd, &borderless);
        return 0;
    }

    if (msg == WM_NCACTIVATE) {
        window->active = wParam;
        InvalidateRect(hwnd, NULL, false);
        return 0;
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
            if (y < window->titlebar_height) {
                if (x >= window->titlebar_button_width && x < window->width - window->titlebar_button_width * 3) {
                    return HTCAPTION;
                }
            }
            return HTCLIENT;
        }

        return HTNOWHERE;
    }

    if (msg == WM_DPICHANGED) {
        window->dpi = HIWORD(wParam);

        RECT *new_size = lParam;
        SetWindowPos(hwnd, NULL, new_size->left, new_size->top, new_size->right - new_size->left,
            new_size->bottom - new_size->top, SWP_NOZORDER | SWP_NOACTIVATE);
        return 0;
    }

    if (msg == WM_SIZE) {
        // Save new window size
        window->width = LOWORD(lParam);
        window->height = HIWORD(lParam);

        // Resize some controls
        window->titlebar_height = MulDiv(32, window->dpi, 96);
        window->titlebar_button_width = MulDiv(48, window->dpi, 96);
        window->titlebar_icon_size = MulDiv(24, window->dpi, 96);

        // Resize canvas
        Canvas_Resize(window->canvas, window->width, window->height);
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
        if (y >= 0 && y < window->titlebar_height) {
            if (
                x < window->titlebar_button_width
            ) {
                ShellExecuteW(hwnd, L"open", L"https://youtu.be/dQw4w9WgXcQ?t=43", NULL, NULL, SW_SHOWNORMAL);
            }

            if (
                x >= window->width - window->titlebar_button_width * 3 &&
                x < window->width - window->titlebar_button_width  * 2
            ) {
                ShowWindow(hwnd, SW_MINIMIZE);
                window->minimize_hover = false;
                ReleaseCapture();
            }

            if (
                x >= window->width - window->titlebar_button_width  * 2 &&
                x < window->width - window->titlebar_button_width  * 1
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
                x >= window->width - window->titlebar_button_width  * 1 &&
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
        bool new_menu_hover = window->menu_hover;
        bool new_minimize_hover = window->minimize_hover;
        bool new_maximize_hover = window->minimize_hover;
        bool new_close_hover = window->close_hover;
        if (y >= 0 && y < window->titlebar_height) {
            new_menu_hover = x >= 0 && x < window->titlebar_button_width;

            new_minimize_hover = x >= window->width - window->titlebar_button_width * 3 &&
                x < window->width - window->titlebar_button_width  * 2;

            new_maximize_hover = x >= window->width - window->titlebar_button_width  * 2 &&
                x < window->width - window->titlebar_button_width  * 1;

            new_close_hover = x >= window->width - window->titlebar_button_width  * 1 &&
                    x < window->width;
        } else {
            new_menu_hover = false;
            new_minimize_hover = false;
            new_maximize_hover = false;
            new_close_hover = false;
        }

        if (
            new_menu_hover != window->menu_hover ||
            new_minimize_hover != window->minimize_hover ||
            new_maximize_hover != window->maximize_hover ||
            new_close_hover != window->close_hover
        ) {
            window->menu_hover = new_menu_hover;
            window->minimize_hover = new_minimize_hover;
            window->maximize_hover = new_maximize_hover;
            window->close_hover = new_close_hover;

            if (window->menu_hover || window->minimize_hover || window->maximize_hover || window->close_hover) {
                SetCapture(hwnd);
            } else {
                ReleaseCapture();
            }
            InvalidateRect(hwnd, NULL, false);
        }
        return 0;
    }

    if (msg == WM_ERASEBKGND) {
        return true;
    }

    if (msg == WM_PAINT) {
        PAINTSTRUCT paint_struct;
        BeginPaint(hwnd, &paint_struct);
        Canvas_BeginDraw(window->canvas);

        // Draw background color
        CanvasRect background_rect = { 0, 0, window->width, window->height };
        Canvas_FillRect(window->canvas, &background_rect, window->background_color);

        CanvasColor active_text_color = CANVAS_RGB(255, 255, 255);
        CanvasColor inactive_text_color = CANVAS_RGBA(255, 255, 255, 128);

        // Draw window decoration buttons

        // Menu button
        if (window->menu_hover) {
            CanvasRect menu_button_rect = { 0, 0, window->titlebar_button_width, window->titlebar_height };
            Canvas_FillRect(window->canvas, &menu_button_rect, CANVAS_RGBA(255, 255, 255, 48));
        }
        CanvasRect menu_icon_rect = { (window->titlebar_button_width - window->titlebar_icon_size) / 2,
            (window->titlebar_height - window->titlebar_icon_size) / 2, window->titlebar_icon_size, window->titlebar_icon_size };
        Canvas_FillPath(window->canvas, &menu_icon_rect, 24, 24, "M3,6H21V8H3V6M3,11H21V13H3V11M3,16H21V18H3V16Z", window->active ? active_text_color : inactive_text_color);

        // Minimize button
        int32_t x = window->width - window->titlebar_button_width * 3;
        if (window->minimize_hover) {
            CanvasRect minimize_button_rect = { x, 0, window->titlebar_button_width, window->titlebar_height };
            Canvas_FillRect(window->canvas, &minimize_button_rect, CANVAS_RGBA(255, 255, 255, 48));
        }
        CanvasRect minimize_icon_rect = { x + (window->titlebar_button_width - window->titlebar_icon_size) / 2,
            (window->titlebar_height - window->titlebar_icon_size) / 2, window->titlebar_icon_size, window->titlebar_icon_size };
        Canvas_FillPath(window->canvas, &minimize_icon_rect, 24, 24, "M20,14H4V10H20", window->active ? active_text_color : inactive_text_color);
        x += window->titlebar_button_width;

        // Maximize button
        if (window->maximize_hover) {
            CanvasRect maximize_button_rect = { x, 0, window->titlebar_button_width, window->titlebar_height };
            Canvas_FillRect(window->canvas, &maximize_button_rect, CANVAS_RGBA(255, 255, 255, 48));
        }
        WINDOWPLACEMENT placement;
        GetWindowPlacement(hwnd, &placement);
        CanvasRect maximize_icon_rect = { x + (window->titlebar_button_width - window->titlebar_icon_size) / 2,
            (window->titlebar_height - window->titlebar_icon_size) / 2, window->titlebar_icon_size, window->titlebar_icon_size };
        Canvas_FillPath(window->canvas, &maximize_icon_rect, 24, 24,
            placement.showCmd == SW_MAXIMIZE ? "M4,8H8V4H20V16H16V20H4V8M16,8V14H18V6H10V8H16M6,12V18H14V12H6Z" :
            "M4,4H20V20H4V4M6,8V18H18V8H6Z", window->active ? active_text_color : inactive_text_color);
        x += window->titlebar_button_width;

        // Close button
        if (window->close_hover) {
            CanvasRect close_button_rect = { x, 0, window->titlebar_button_width, window->titlebar_height };
            Canvas_FillRect(window->canvas, &close_button_rect, CANVAS_RGBA(255, 0, 0, 128));
        }
        CanvasRect close_icon_rect = { x + (window->titlebar_button_width - window->titlebar_icon_size) / 2,
            (window->titlebar_height - window->titlebar_icon_size) / 2, window->titlebar_icon_size, window->titlebar_icon_size };
        Canvas_FillPath(window->canvas, &close_icon_rect, 24, 24, "M19,6.41L17.59,5L12,10.59L6.41,5L5,6.41L10.59,12L5,17.59L6.41,19L12,13.41L17.59,19L19,17.59L13.41,12L19,6.41Z",
            window->active ? active_text_color : inactive_text_color);

        // Draw centered text
        CanvasFont title_font = { .name = font_name, .size = (float)window->width / 32 };
        CanvasRect title_rect = { 0, (window->height - title_font.size * 2) / 2, window->width, title_font.size * 2 };
        Canvas_DrawText(window->canvas, window_title,  -1, &title_rect, &title_font, CANVAS_ALIGN_HORIZONTAL_CENTER | CANVAS_ALIGN_VERTICAL_CENTER, active_text_color);

        // Draw footer text
        CanvasFont footer_font = { .name = font_name, .size = (float)window->width / 42 };
        CanvasRect footer_rect = { 0, window->height - footer_font.size * 2 - 24, window->width, footer_font.size * 2 };
        wchar_t string_buffer[64];
        wsprintfW(string_buffer, L"Window size: %dx%d at %d dpi", window->width, window->height, window->dpi);
        Canvas_DrawText(window->canvas, string_buffer, -1, &footer_rect, &footer_font, CANVAS_ALIGN_HORIZONTAL_CENTER | CANVAS_ALIGN_VERTICAL_BOTTOM, active_text_color);

        Canvas_EndDraw(window->canvas);
        EndPaint(hwnd, &paint_struct);
        return 0;
    }

    if (msg == WM_DESTROY) {
        // Free window data
        Canvas_Free(window->canvas);
        free(window);

        // Close process
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void _start(void) {
    SetDPIAware();

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

    HWND hwnd = CreateWindowExW(0, window_class_name, window_title, WINDOW_STYLE,
        (GetSystemMetrics(SM_CXSCREEN) - WINDOW_WIDTH) / 2,
        (GetSystemMetrics(SM_CYSCREEN) - WINDOW_HEIGHT) / 2,
        WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, wc.hInstance, NULL);
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    MSG message;
    while (GetMessageW(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    ExitProcess((uintptr_t)message.wParam);
}
