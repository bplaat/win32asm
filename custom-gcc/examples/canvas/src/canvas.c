#include "win32.h"
#include "henk.h"

wchar_t *window_class_name = L"canvas-test";

#ifdef WIN64
    wchar_t *window_title = L"This is a test canvas window 😍 (64-bit)";
#else
    wchar_t *window_title = L"This is a test canvas window 😍 (32-bit)";
#endif

wchar_t *font_name = L"Georgia";

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_STYLE WS_OVERLAPPEDWINDOW

typedef struct WindowData {
    uint32_t width;
    uint32_t height;
    uint32_t background_color;
    Henk *henk;
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
        window->henk = Henk_New(hwnd, HENK_RENDERER_DEFAULT);
        return 0;
    }

    if (msg == WM_SIZE) {
        // Save new window size
        window->width = LOWORD(lParam);
        window->height = HIWORD(lParam);
        Henk_Resize(window->henk, window->width, window->height);
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
        Henk_BeginDraw(window->henk);

        HenkRect background_rect = { 0, 0, window->width, window->height };
        Henk_FillRect(window->henk, &background_rect, window->background_color);

        HenkRect rect1 = { 100, 100, 200, 200 };
        Henk_FillRect(window->henk, &rect1, HENK_RGBA(255, 0, 0, 128));

        HenkRect rect2 = { 200, 200, 200, 200 };
        Henk_FillRect(window->henk, &rect2, HENK_RGBA(0, 255, 0, 128));

        HenkRect rect3 = { 300, 300, 200, 200 };
        Henk_FillRect(window->henk, &rect3, HENK_RGBA(0, 0, 255, 128));

        HenkFont font = { font_name, 32 };

        HenkRect text_rect1 = { 150, 150, 600, font.size * 2 };
        Henk_DrawText(window->henk, L"Henk Canvas Example", -1, &text_rect1, &font, 0, HENK_RGBA(255, 255, 255, 128));

        HenkRect text_rect2 = { 175, 175, 600, font.size * 2 };
        Henk_DrawText(window->henk, L"Hello World!", -1, &text_rect2, &font, 0, HENK_RGBA(255, 255, 255, 128));

        HenkRect icon_rect1 = { 24, 24, 128, 128 };
        Henk_FillPath(window->henk, &icon_rect1, 24, 24, "M3,6H21V8H3V6M3,11H21V13H3V11M3,16H21V18H3V16Z", HENK_RGBA(255, 255, 255, 128));

        HenkRect icon_rect2 = { 300, 300, 200, 200 };
        Henk_FillPath(window->henk, &icon_rect2, 24, 24, "M2,2H11V11H2V2M9,4H4V9H9V4M22,13V22H13V13H22M15,20H20V15H15V20M16,8V11H13V8H16M11,16H8V13H11V16Z", HENK_RGBA(255, 255, 255, 128));

        Henk_EndDraw(window->henk);
        EndPaint(hwnd, &paint_struct);
        return 0;
    }

    if (msg == WM_DESTROY) {
        // Free window data
        Henk_Free(window->henk);
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
