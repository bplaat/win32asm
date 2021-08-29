#include "win32.h"
#include "dpi.h"

wchar_t *window_class_name = L"window-test";

#ifdef WIN64
    wchar_t *window_title = L"This is a test window 😍 (64-bit)";
#else
    wchar_t *window_title = L"This is a test window 😍 (32-bit)";
#endif

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_MIN_WIDTH 640
#define WINDOW_MIN_HEIGHT 480
#define WINDOW_STYLE WS_OVERLAPPEDWINDOW

typedef struct WindowData {
    int32_t dpi;
    int32_t width;
    int32_t height;
    int32_t background_color;
} WindowData;

int32_t __stdcall WndProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam) {
    WindowData *window = GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    if (msg == WM_CREATE) {
        // Set window data struct as user data
        WindowData *window = ((CREATESTRUCTW *)lParam)->lpCreateParams;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, window);

        // Generate random seed by time
        SYSTEMTIME time;
        GetLocalTime(&time);
        srand((time.wHour * 60 + time.wMinute) * 60 + time.wSecond);

        // Generate random background color
        window->background_color = rand() & 0x007f7f7f;
        return 0;
    }

    if (msg == WM_DPICHANGED) {
        // Update dpi and resize window
        window->dpi = HIWORD(wParam);
        RECT *window_rect = lParam;
        SetWindowPos(hwnd, NULL, window_rect->left, window_rect->top, window_rect->right - window_rect->left,
            window_rect->bottom - window_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
        return 0;
    }

    if (msg == WM_SIZE) {
        // Save new window size
        window->width = LOWORD(lParam);
        window->height = HIWORD(lParam);
        return 0;
    }

    if (msg == WM_GETMINMAXINFO) {
        // Calculate window min size for dpi
        int32_t window_dpi = window != NULL ? window->dpi : GetDesktopDpi();
        RECT window_rect = { 0, 0, MulDiv(WINDOW_MIN_WIDTH, window_dpi, 96), MulDiv(WINDOW_MIN_HEIGHT, window_dpi, 96) };
        AdjustWindowRectExForDpi(&window_rect, WINDOW_STYLE, false, 0, window_dpi);

        // Set window min size
        MINMAXINFO *minMaxInfo = (MINMAXINFO *)lParam;
        minMaxInfo->ptMinTrackSize.x = window_rect.right - window_rect.left;
        minMaxInfo->ptMinTrackSize.y = window_rect.bottom - window_rect.top;
        return 0;
    }

    if (msg == WM_ERASEBKGND) {
        return true;
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

        // Draw centered text
        wchar_t *font_name = L"Comic Sans MS";
        int32_t font_size = window->width / 16;
        HFONT font = CreateFontW(font_size, 0, 0, 0, FW_NORMAL, false, false, false, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
        SelectObject(hdc_buffer, font);
        SetBkMode(hdc_buffer, TRANSPARENT);
        SetTextColor(hdc_buffer, 0x00ffffff);
        SetTextAlign(hdc_buffer, TA_CENTER);
        TextOutW(hdc_buffer, window->width / 2, (window->height - font_size) / 2, window_title, wcslen(window_title));
        DeleteObject(font);

        // Draw footer text
        font_size = window->width / 24;
        font = CreateFontW(font_size, 0, 0, 0, FW_NORMAL, false, false, false, ANSI_CHARSET,
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
    // Set process dpi aware
    SetDpiAware();

    // Register window class
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

    // Create window data struct
    WindowData *window = malloc(sizeof(WindowData));
    window->dpi = GetDesktopDpi();
    window->width = MulDiv(WINDOW_WIDTH, window->dpi, 96);
    window->height = MulDiv(WINDOW_HEIGHT, window->dpi, 96);

    // Create centered window
    RECT window_rect;
    window_rect.left = (GetSystemMetrics(SM_CXSCREEN) - window->width) / 2;
    window_rect.top = (GetSystemMetrics(SM_CYSCREEN) - window->height) / 2;
    window_rect.right = window_rect.left + window->width;
    window_rect.bottom = window_rect.top + window->height;
    AdjustWindowRectExForDpi(&window_rect, WINDOW_STYLE, false, 0, window->dpi);

    HWND hwnd = CreateWindowExW(0, window_class_name, window_title,
        WINDOW_STYLE, window_rect.left, window_rect.top,
        window_rect.right - window_rect.left, window_rect.bottom - window_rect.top,
        NULL, NULL, wc.hInstance, window);
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // Main message loop
    MSG message;
    while (GetMessageW(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    ExitProcess((uintptr_t)message.wParam);
}
