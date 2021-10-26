#include "win32.h"
#include "dpi.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_MIN_WIDTH 640
#define WINDOW_MIN_HEIGHT 480
#define WINDOW_STYLE WS_OVERLAPPEDWINDOW

wchar_t *window_class_name = L"window-test";

#ifdef WIN64
    wchar_t *window_title = L"This is a DPI aware GDI window 😍 (64-bit)";
#else
    wchar_t *window_title = L"This is a DPI aware GDI window 😍 (32-bit)";
#endif

wchar_t *font_name = L"Comic Sans MS";

typedef struct WindowData {
    int32_t dpi;
    int32_t width;
    int32_t height;
    int32_t real_width;
    int32_t real_height;
    int32_t background_color;
} WindowData;

#define DP2PX(dp) MulDiv(dp, window->dpi, 96)
#define PX2DP(px) MulDiv(px, 96, window->dpi)
#define PT2PT(pt) -MulDiv(pt, window->dpi, 72)

int32_t __stdcall WndProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam) {
    WindowData *window = GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    if (msg == WM_CREATE) {
        // Set window data struct as user data
        window = ((CREATESTRUCTW *)lParam)->lpCreateParams;
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
        window->real_width = LOWORD(lParam);
        window->real_height = HIWORD(lParam);
        window->width = PX2DP(window->real_width);
        window->height = PX2DP(window->real_height);
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
        SetBkMode(hdc_buffer, TRANSPARENT);
        HBITMAP bitmap_buffer = CreateCompatibleBitmap(hdc, window->real_width, window->real_height);
        SelectObject(hdc_buffer, bitmap_buffer);

        // Draw background color
        HBRUSH brush = CreateSolidBrush(window->background_color);
        RECT rect = { 0, 0, DP2PX(window->width), DP2PX(window->height) };
        FillRect(hdc_buffer, &rect, brush);
        DeleteObject(brush);

        // Draw centered text
        HFONT font = CreateFontW(PT2PT(window->width * 3 / 100), 0, 0, 0, FW_NORMAL, false, false, false, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
        SelectObject(hdc_buffer, font);
        SetTextColor(hdc_buffer, HEX(0xffffff));
        SetTextAlign(hdc_buffer, TA_CENTER);
        SIZE measure_rect;
        GetTextExtentPoint32W(hdc_buffer, window_title, wcslen(window_title), &measure_rect);
        TextOutW(hdc_buffer, DP2PX(window->width / 2), DP2PX(window->height / 2) - measure_rect.cy / 2, window_title, wcslen(window_title));
        DeleteObject(font);

        // Draw footer text
        font = CreateFontW(PT2PT(window->width * 2 / 100), 0, 0, 0, FW_NORMAL, false, false, false, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
        SelectObject(hdc_buffer, font);
        SetTextAlign(hdc_buffer, TA_CENTER | TA_BOTTOM);
        wchar_t string_buffer[64];
        wsprintfW(string_buffer, L"%dx%d at %d dpi is %dx%d", window->width, window->height, window->dpi, window->real_width, window->real_height);
        TextOutW(hdc_buffer, DP2PX(window->width / 2), DP2PX(window->height - 16), string_buffer, wcslen(string_buffer));
        DeleteObject(font);

        // Draw and delete back buffer
        BitBlt(hdc, 0, 0, window->real_width, window->real_height, hdc_buffer, 0, 0, SRCCOPY);
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
    ATOM window_class = RegisterClassExW(&wc);

    // Create window data struct
    WindowData *window = malloc(sizeof(WindowData));
    window->dpi = GetDesktopDpi();
    window->width = WINDOW_WIDTH;
    window->height = WINDOW_HEIGHT;
    window->real_width = DP2PX(window->width);
    window->real_height = DP2PX(window->height);

    // Create centered window
    RECT window_rect;
    window_rect.left = (GetSystemMetrics(SM_CXSCREEN) - window->real_width) / 2;
    window_rect.top = (GetSystemMetrics(SM_CYSCREEN) - window->real_height) / 2;
    window_rect.right = window_rect.left + window->real_width;
    window_rect.bottom = window_rect.top + window->real_height;
    AdjustWindowRectExForDpi(&window_rect, WINDOW_STYLE, false, 0, window->dpi);

    HWND hwnd = CreateWindowExW(0, (wchar_t *)(uintptr_t)window_class, window_title,
        WINDOW_STYLE, window_rect.left, window_rect.top,
        window_rect.right - window_rect.left, window_rect.bottom - window_rect.top,
        HWND_DESKTOP, NULL, wc.hInstance, window);
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
