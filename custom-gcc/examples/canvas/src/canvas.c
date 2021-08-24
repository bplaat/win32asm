#include "win32.h"
#include "canvas.h"

wchar_t *window_class_name = L"canvas-test";

#ifdef WIN64
    wchar_t *window_title = L"This is a test canvas window 😍 (64-bit)";
#else
    wchar_t *window_title = L"This is a test canvas window 😍 (32-bit)";
#endif

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_STYLE WS_OVERLAPPEDWINDOW

typedef struct WindowData {
    uint32_t width;
    uint32_t height;
    uint32_t background_color;
    Canvas *canvas;
    bool renderer;
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
        window->renderer = true;
        window->canvas = Canvas_New(hwnd, window->renderer ? CANVAS_RENDERER_DIRECT2D : CANVAS_RENDERER_GDI);

        SetTimer(hwnd, 1, 500, NULL);
        return 0;
    }

    if (msg == WM_TIMER) {
        if ((uintptr_t)wParam == 1) {
            window->renderer = !window->renderer;
            Canvas_Free(window->canvas);
            window->canvas = Canvas_New(hwnd, window->renderer ? CANVAS_RENDERER_DIRECT2D : CANVAS_RENDERER_GDI);
            Canvas_Resize(window->canvas, window->width, window->height);
            InvalidateRect(hwnd, NULL, true);
            return 0;
        }
    }

    if (msg == WM_SIZE) {
        // Save new window size
        window->width = LOWORD(lParam);
        window->height = HIWORD(lParam);
        Canvas_Resize(window->canvas, window->width, window->height);
        return 0;
    }

    if (msg == WM_GETMINMAXINFO) {
        // Set window min size
        MINMAXINFO *minMaxInfo = (MINMAXINFO *)lParam;
        RECT window_rect = { 0, 0, 640, 480 };
        AdjustWindowRectEx(&window_rect, WINDOW_STYLE, false, 0);
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

        CanvasRect rect1 = { 100, 100, 200, 200 };
        Canvas_FillRect(window->canvas, &rect1, CANVAS_RGBA(255, 0, 0, 128));

        CanvasRect icon_rect1 = { 100, 100, 200, 200 };
        Canvas_FillPath(window->canvas, &icon_rect1, 24, 24, "M3,6H21V8H3V6M3,11H21V13H3V11M3,16H21V18H3V16Z", CANVAS_RGBA(255, 255, 255, 128));

        CanvasRect rect2 = { 200, 200, 200, 200 };
        Canvas_FillRect(window->canvas, &rect2, CANVAS_RGBA(0, 255, 0, 128));

        CanvasRect rect3 = { 300, 300, 200, 200 };
        Canvas_FillRect(window->canvas, &rect3, CANVAS_RGBA(0, 0, 255, 128));

        CanvasRect icon_rect2 = { 300, 300, 200, 200 };
        Canvas_FillPath(window->canvas, &icon_rect2, 24, 24, "M2,2H11V11H2V2M9,4H4V9H9V4M22,13V22H13V13H22M15,20H20V15H15V20M16,8V11H13V8H16M11,16H8V13H11V16Z", CANVAS_RGBA(255, 255, 255, 128));

        CanvasFont small_font = { L"Georgia", 16 };
        CanvasRect text_rect0 = { 16, 16, window->width - 32, window->height - 32 };
        Canvas_DrawText(window->canvas, L"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur sed massa eu risus egestas ornare viverra in nisi. Aenean imperdiet eros sit amet enim ornare fringilla. Praesent iaculis mauris vitae ante commodo tincidunt. Morbi efficitur, lorem vel auctor dignissim, turpis erat rhoncus tortor, vitae suscipit nunc sem bibendum mauris. Mauris vitae tortor at dui rhoncus ullamcorper. Maecenas quis sagittis turpis. Donec a nisl turpis. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Nulla et erat lorem. Integer a neque at nisl sagittis rhoncus rutrum a metus. Aenean sed urna in lorem auctor lobortis. Suspendisse faucibus rhoncus augue vitae blandit. Maecenas tincidunt id nunc vel tempor. Morbi porta ligula finibus odio gravida iaculis.",
            -1, &text_rect0, &small_font, CANVAS_ALIGN_BOTTOM | CANVAS_ALIGN_CENTER, CANVAS_RGBA(255, 255, 255, 128));

        CanvasFont line_font = { L"Segoe UI", 18 };
        CanvasRect line_rect = { 16, 16, window->width - 32, 0 };
        Canvas_DrawText(window->canvas, L"The quick brown fox jumps over the lazy dog.", -1, &line_rect, &line_font, 0, CANVAS_RGB(255, 255, 255));
        line_rect.y += line_rect.height + 8;
        line_rect.height = 0;
        Canvas_DrawText(window->canvas, L"Line 1", -1, &line_rect, &line_font, 0, CANVAS_RGB(255, 255, 255));
        line_rect.y += line_rect.height + 8;
        line_rect.height = 0;
        Canvas_DrawText(window->canvas, L"Line 2", -1, &line_rect, &line_font, 0, CANVAS_RGB(255, 255, 255));
        line_rect.y += line_rect.height + 8;
        line_rect.height = 0;
        Canvas_DrawText(window->canvas, L"Line 3", -1, &line_rect, &line_font, 0, CANVAS_RGB(255, 255, 255));
        line_rect.y += line_rect.height + 8;
        line_rect.height = 0;
        Canvas_DrawText(window->canvas, L"Line 4", -1, &line_rect, &line_font, 0, CANVAS_RGB(255, 255, 255));
        line_rect.y += line_rect.height + 8;
        line_rect.height = 0;
        Canvas_DrawText(window->canvas, L"Line 5", -1, &line_rect, &line_font, 0, CANVAS_RGB(255, 255, 255));
        line_rect.y += line_rect.height + 8;
        line_rect.height = 0;
        if (window->canvas->renderer == CANVAS_RENDERER_GDI) {
            Canvas_DrawText(window->canvas, L"GDI backend", -1, &line_rect, &line_font, 0, CANVAS_RGBA(255, 255, 0, 200));
        }
        if (window->canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
            Canvas_DrawText(window->canvas, L"Direct2D backend", -1, &line_rect, &line_font, 0, CANVAS_RGBA(255, 255, 0, 200));
        }

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
    AdjustWindowRectEx(&window_rect, WINDOW_STYLE, false, 0);

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
    ExitProcess((int32_t)(uintptr_t)message.wParam);
}
