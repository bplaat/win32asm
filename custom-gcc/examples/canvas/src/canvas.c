#include "win32.h"
#include "dpi.h"
#include "canvas.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_MIN_WIDTH 640
#define WINDOW_MIN_HEIGHT 480
#define WINDOW_STYLE WS_OVERLAPPEDWINDOW
#define WINDOW_TIMER_ID 1

wchar_t *window_class_name = L"window-canvas";

#ifdef WIN64
    wchar_t *window_title = L"This is a DPI aware canvas window 😜 (64-bit)";
#else
    wchar_t *window_title = L"This is a DPI aware canvas window 😜 (32-bit)";
#endif

typedef struct WindowData {
    int32_t dpi;
    uint32_t width;
    uint32_t height;
    int32_t real_width;
    int32_t real_height;
    bool renderer;
    Canvas *canvas;
    uint32_t background_color;
} WindowData;

#define DP2PX(dp) MulDiv(dp, window->dpi, 96)
#define PX2DP(px) MulDiv(px, 96, window->dpi)

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
        window->background_color = (rand() & 0x007f7f7f) | 0xff000000;
        window->renderer = true;
        window->canvas = Canvas_New(hwnd, window->renderer ? CANVAS_RENDERER_DIRECT2D : CANVAS_RENDERER_GDI);

        SetTimer(hwnd, WINDOW_TIMER_ID, 500, NULL);
        return 0;
    }

    if (msg == WM_TIMER) {
        if ((uintptr_t)wParam == WINDOW_TIMER_ID) {
            window->renderer = !window->renderer;
            Canvas_Free(window->canvas);
            window->canvas = Canvas_New(hwnd, window->renderer ? CANVAS_RENDERER_DIRECT2D : CANVAS_RENDERER_GDI);
            Canvas_Resize(window->canvas, window->real_width, window->real_height, window->dpi);
            InvalidateRect(hwnd, NULL, true);
            return 0;
        }
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

        // Resize canvas
        Canvas_Resize(window->canvas, window->real_width, window->real_height, window->dpi);
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
        BeginPaint(hwnd, &paint_struct);
        Canvas_BeginDraw(window->canvas);

        CanvasRect background_rect = { 0, 0, window->width, window->height };
        Canvas_FillRect(window->canvas, &background_rect, window->background_color);

        Canvas_Transform(window->canvas, &(CanvasTransform){ 1, 0, 0, 1, 32, 32 });
        Canvas_Clip(window->canvas, &(CanvasRect){ 32, 32, window->width - 64, window->height - 64 });

        CanvasRect rect1 = { 100, 100, 200, 200 };
        Canvas_FillRect(window->canvas, &rect1, CANVAS_RGBA(255, 0, 0, 128));
        Canvas_FillPath(window->canvas, &rect1, 24, 24, "M3,6H21V8H3V6M3,11H21V13H3V11M3,16H21V18H3V16Z", CANVAS_RGBA(255, 255, 255, 128));

        CanvasRect rect2 = { 200, 200, 200, 200 };
        Canvas_FillRect(window->canvas, &rect2, CANVAS_RGBA(0, 255, 0, 128));
        Canvas_FillPath(window->canvas, &rect2, 24, 24, "M20,11V13H8L13.5,18.5L12.08,19.92L4.16,12L12.08,4.08L13.5,5.5L8,11H20Z", CANVAS_RGBA(255, 255, 255, 128));

        CanvasRect rect3 = { 300, 300, 200, 200 };
        Canvas_FillRect(window->canvas, &rect3, CANVAS_RGBA(0, 0, 255, 128));
        Canvas_FillPath(window->canvas, &rect3, 24, 24, "M2,2H11V11H2V2M9,4H4V9H9V4M22,13V22H13V13H22M15,20H20V15H15V20M16,8V11H13V8H16M11,16H8V13H11V16Z", CANVAS_RGBA(255, 255, 255, 128));

        CanvasFont small_font = { .name = L"Georgia", .size = 16 };
        CanvasRect text_rect0 = { 16, 16, window->width - 32, window->height - 32 };
        Canvas_DrawText(window->canvas, L"Lorem ipsum dolor sit amet,\nconsectetur adipiscing elit. Curabitur sed massa eu risus egestas ornare viverra in nisi. Aenean imperdiet eros sit amet enim ornare fringilla. Praesent iaculis mauris vitae ante commodo tincidunt. Morbi efficitur, lorem vel auctor dignissim, turpis erat rhoncus tortor, vitae suscipit nunc sem bibendum mauris. Mauris vitae tortor at dui rhoncus ullamcorper. Maecenas quis sagittis turpis. Donec a nisl turpis. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Nulla et erat lorem. Integer a neque at nisl sagittis rhoncus rutrum a metus. Aenean sed urna in lorem auctor lobortis. Suspendisse faucibus rhoncus augue vitae blandit. Maecenas tincidunt id nunc vel tempor. Morbi porta ligula finibus odio gravida iaculis.",
            -1, &text_rect0, &small_font, CANVAS_TEXT_FORMAT_VERTICAL_BOTTOM | CANVAS_TEXT_FORMAT_HORIZONTAL_CENTER | CANVAS_TEXT_FORMAT_WRAP, CANVAS_RGBA(255, 255, 255, 128));

        CanvasFont line_font = { .name = L"Segoe UI", .size = 18, .weight = CANVAS_FONT_WEIGHT_BOLD, .italic = true, .underline = true, .line_through = true };
        CanvasRect line_rect = { 16, 16, window->width - 32, 0 };
        Canvas_DrawText(window->canvas, L"The quick brown fox jumps over the lazy dog.", -1, &line_rect, &line_font, CANVAS_TEXT_FORMAT_DEFAULT, CANVAS_RGB(255, 255, 255));
        line_rect.y += line_rect.height + 8;
        line_rect.height = 0;
        Canvas_DrawText(window->canvas, L"Line 1", -1, &line_rect, &line_font, CANVAS_TEXT_FORMAT_DEFAULT, CANVAS_RGB(255, 255, 255));
        line_rect.y += line_rect.height + 8;
        line_rect.height = 0;
        Canvas_DrawText(window->canvas, L"Line 2", -1, &line_rect, &line_font, CANVAS_TEXT_FORMAT_DEFAULT, CANVAS_RGB(255, 255, 255));
        line_rect.y += line_rect.height + 8;
        line_rect.height = 0;
        Canvas_DrawText(window->canvas, L"Line 3", -1, &line_rect, &line_font, CANVAS_TEXT_FORMAT_DEFAULT, CANVAS_RGB(255, 255, 255));
        line_rect.y += line_rect.height + 8;
        line_rect.height = 0;
        CanvasFont emoji_font = { .name = L"Segoe UI", .size = 18 };
        Canvas_DrawText(window->canvas, L"🤢 🤮 🤧 😷 🤒 🤕 🤑 🤠 😈 👿 👹 👺 🤡 💩", -1, &line_rect, &emoji_font, CANVAS_TEXT_FORMAT_DEFAULT, CANVAS_RGB(255, 255, 255));
        line_rect.y += line_rect.height + 8;
        line_rect.height = 0;
        if (window->canvas->renderer == CANVAS_RENDERER_GDI) {
            Canvas_DrawText(window->canvas, L"GDI backend", -1, &line_rect, &line_font, CANVAS_TEXT_FORMAT_DEFAULT, CANVAS_RGBA(255, 255, 0, 200));
        }
        if (window->canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
            Canvas_DrawText(window->canvas, L"Direct2D backend", -1, &line_rect, &line_font, CANVAS_TEXT_FORMAT_DEFAULT, CANVAS_RGBA(255, 255, 0, 200));
        }

        Canvas_Clip(window->canvas, NULL);
        Canvas_Transform(window->canvas, NULL);

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
