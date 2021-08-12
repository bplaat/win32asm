#define WIN32_MALLOC
#define WIN32_FREE
#define WIN32_RAND
#define WIN32_WCSLEN
#include "win32.h"

// ### Canvas ###
// A simple back bufferd GDI wrapper with transperancy support
typedef struct {
    wchar_t *name;
    int32_t size;
} Font;

HFONT Font_Create(Font *font) {
    return CreateFontW(font->size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font->name);
}

typedef struct {
    HDC hdc;
    int32_t width;
    int32_t height;
    HDC hdc_buffer;
    HBITMAP bitmap_buffer;
} Canvas;

void Canvas_Init(Canvas *canvas, PAINTSTRUCT *ps);

Canvas *Canvas_New(PAINTSTRUCT *ps) {
    Canvas *canvas = malloc(sizeof(Canvas));
    Canvas_Init(canvas, ps);
    return canvas;
}

void Canvas_Init(Canvas *canvas, PAINTSTRUCT *ps) {
    canvas->hdc = ps->hdc;
    canvas->width = ps->rcPaint.right;
    canvas->height= ps->rcPaint.bottom;

    canvas->hdc_buffer = CreateCompatibleDC(canvas->hdc);
    canvas->bitmap_buffer = CreateCompatibleBitmap(canvas->hdc, canvas->width, canvas->height);
    SelectObject(canvas->hdc_buffer, canvas->bitmap_buffer);
}

void Canvas_Free(Canvas *canvas) {
    BitBlt(canvas->hdc, 0, 0, canvas->width, canvas->height, canvas->hdc_buffer, 0, 0, SRCCOPY);
    DeleteObject(canvas->bitmap_buffer);
    DeleteDC(canvas->hdc_buffer);

    free(canvas);
}

void Canvas_FillRect(Canvas *canvas, int32_t x, int32_t y, int32_t width, int32_t height, uint32_t color) {
    HBRUSH brush = CreateSolidBrush(color & 0x00ffffff);
    if ((color >> 24) == 0xff) {
        RECT rect = { x, y, x + width, y + height };
        FillRect(canvas->hdc_buffer, &rect, brush);
    } else {
        HDC hdc_buffer = CreateCompatibleDC(canvas->hdc_buffer);
        HBITMAP bitmap_buffer = CreateCompatibleBitmap(canvas->hdc_buffer, width, height);
        SelectObject(hdc_buffer, bitmap_buffer);

        RECT rect = { 0, 0, width, height };
        FillRect(hdc_buffer, &rect, brush);

        BLENDFUNCTION blend = { AC_SRC_OVER, 0, color >> 24, 0 };
        GdiAlphaBlend(canvas->hdc_buffer, x, y, width, height, hdc_buffer, 0, 0, width, height, blend);
        DeleteObject(bitmap_buffer);
        DeleteDC(hdc_buffer);
    }
    DeleteObject(brush);
}

void Canvas_FillText(Canvas *canvas, Font *font, int32_t x, int32_t y, wchar_t *text, int32_t length, int32_t align, uint32_t color) {
    if (length == -1) length = wcslen(text);
    HFONT hfont = Font_Create(font);
    SelectObject(canvas->hdc_buffer, hfont);
    SIZE measure_size;
    GetTextExtentPoint32W(canvas->hdc_buffer, text, length, &measure_size);
    int32_t dx = x;
    if (align == TA_CENTER) dx -= measure_size.cx / 2;
    if (align == TA_RIGHT) dx -= measure_size.cx;

    if ((color >> 24) == 0xff) {
        SetBkMode(canvas->hdc_buffer, TRANSPARENT);
        SetTextColor(canvas->hdc_buffer, color & 0x00ffffff);
        SetTextAlign(canvas->hdc_buffer, TA_LEFT);
        TextOutW(canvas->hdc_buffer, dx, y, text, length);
    } else {
        HDC hdc_buffer = CreateCompatibleDC(canvas->hdc_buffer);
        HBITMAP bitmap_buffer = CreateCompatibleBitmap(canvas->hdc_buffer, measure_size.cx, measure_size.cy);
        SelectObject(hdc_buffer, bitmap_buffer);
        BitBlt(hdc_buffer, 0, 0, measure_size.cx, measure_size.cy, canvas->hdc_buffer, dx, y, SRCCOPY);

        SelectObject(hdc_buffer, hfont);
        SetBkMode(hdc_buffer, TRANSPARENT);
        SetTextColor(hdc_buffer, color & 0x00ffffff);
        SetTextAlign(hdc_buffer, TA_LEFT);
        TextOutW(hdc_buffer, 0, 0, text, length);

        BLENDFUNCTION blend = { AC_SRC_OVER, 0, color >> 24, 0 };
        GdiAlphaBlend(canvas->hdc_buffer, dx, y, measure_size.cx, measure_size.cy, hdc_buffer, 0, 0, measure_size.cx, measure_size.cy, blend);
        DeleteObject(bitmap_buffer);
        DeleteDC(hdc_buffer);
    }
    DeleteObject(hfont);
}

// ### Window ###
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
        window->background_color = (rand() & 0x007f7f7f) | 0xff000000;
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
        BeginPaint(hwnd, &paint_struct);
        Canvas *canvas = Canvas_New(&paint_struct);

        // Draw background color
        Canvas_FillRect(canvas, 0, 0, window->width, window->height, window->background_color);

        uint32_t active_text_color = RGB(255, 255, 255);
        uint32_t inactive_text_color = RGBA(255, 255, 255, 128);

        // Draw window decoration buttons
        Font button_font = { font_name, 18 };

        // Minimize button
        int32_t x = window->width - TITLEBAR_BUTTON_WIDTH * 3;
        if (window->minimize_hover) {
            Canvas_FillRect(canvas, x, 0, TITLEBAR_BUTTON_WIDTH, TITLEBAR_HEIGHT, RGBA(255, 255, 255, 48));
        }
        Canvas_FillText(canvas, &button_font, x + TITLEBAR_BUTTON_WIDTH / 2, (TITLEBAR_HEIGHT - button_font.size) / 2,
            L"🗕", -1, TA_CENTER, window->active ? active_text_color : inactive_text_color);
        x += TITLEBAR_BUTTON_WIDTH;

        // Maximize button
        if (window->maximize_hover) {
            Canvas_FillRect(canvas, x, 0, TITLEBAR_BUTTON_WIDTH, TITLEBAR_HEIGHT, RGBA(255, 255, 255, 48));
        }
        WINDOWPLACEMENT placement;
        GetWindowPlacement(hwnd, &placement);
        Canvas_FillText(canvas, &button_font, x + TITLEBAR_BUTTON_WIDTH / 2, (TITLEBAR_HEIGHT - button_font.size) / 2,
            placement.showCmd == SW_MAXIMIZE ? L"🗗" : L"🗖", -1, TA_CENTER, window->active ? active_text_color : inactive_text_color);
        x += TITLEBAR_BUTTON_WIDTH;

        // Close button
        if (window->close_hover) {
            Canvas_FillRect(canvas, x, 0, TITLEBAR_BUTTON_WIDTH, TITLEBAR_HEIGHT, RGBA(255, 0, 0, 128));
        }
        Canvas_FillText(canvas, &button_font, x + TITLEBAR_BUTTON_WIDTH / 2, (TITLEBAR_HEIGHT - button_font.size) / 2,
            L"🗙", -1, TA_CENTER, window->active ? active_text_color : inactive_text_color);

        // Draw centered text
        Font title_font = { font_name, window->width / 16 };
        Canvas_FillText(canvas, &title_font, window->width / 2, (window->height - title_font.size) / 2, window_title,  -1, TA_CENTER, active_text_color);

        // Draw footer text
        Font footer_font = { font_name, window->width / 24 };
        wchar_t string_buffer[64];
        wsprintfW(string_buffer, L"(%dx%d)", window->width, window->height);
        Canvas_FillText(canvas, &footer_font, window->width / 2, window->height - footer_font.size - 24, string_buffer, -1, TA_CENTER, active_text_color);

        Canvas_Free(canvas);
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
