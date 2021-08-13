#define WIN32_MALLOC
#define WIN32_FREE
#define WIN32_RAND
#define WIN32_WCSLEN
#include "win32.h"

wchar_t *window_class_name = L"direct2d-test";

#ifdef WIN64
    wchar_t *window_title = L"Direct 2D Test 🎮 (64-bit)";
#else
    wchar_t *window_title = L"Direct 2D Test 🎮 (32-bit)";
#endif

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_STYLE WS_OVERLAPPEDWINDOW

typedef struct {
    uint32_t width;
    uint32_t height;
    D2D1_COLOR_F background_color;

    ID2D1Factory *pFactory;
    ID2D1HwndRenderTarget *pRenderTarget;
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
        window->background_color.r = (float)(rand() % 128) / 255;
        window->background_color.g = (float)(rand() % 128) / 255;
        window->background_color.b = (float)(rand() % 128) / 255;
        window->background_color.a = 1;

        // Create Direct2D factory
        GUID ID2D1Factory_guid = { 0xbb12d362, 0xdaee, 0x4b9a, { 0xaa, 0x1d, 0x14, 0xba, 0x40, 0x1c, 0xfa, 0x1f } };
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &ID2D1Factory_guid, NULL, &window->pFactory))) {
            return -1;
        }

        // Create Direct2D renderer
        D2D1_RENDER_TARGET_PROPERTIES renderProps = { D2D1_RENDER_TARGET_TYPE_DEFAULT,
            { DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_UNKNOWN },
            0, 0, D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT };
        D2D1_HWND_RENDER_TARGET_PROPERTIES hwndRenderProps = { hwnd, { 0, 0 }, D2D1_PRESENT_OPTIONS_NONE };
        ID2D1Factory_CreateHwndRenderTarget(window->pFactory, &renderProps, &hwndRenderProps, &window->pRenderTarget);

        return 0;
    }

    if (msg == WM_SIZE) {
        // Save new window size
        window->width = LOWORD(lParam);
        window->height = HIWORD(lParam);

        // Resize Direct2D renderer
        D2D1_SIZE_U size = { window->width, window->height };
        ID2D1HwndRenderTarget_Resize(window->pRenderTarget, &size);
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
        // Draw no background
        return TRUE;
    }

    if (msg == WM_PAINT) {
        PAINTSTRUCT paint_struct;
        BeginPaint(hwnd, &paint_struct);
        ID2D1RenderTarget_BeginDraw(window->pRenderTarget);
        ID2D1RenderTarget_Clear(window->pRenderTarget, &window->background_color);

        for (int32_t i = 0; i < 1000; i++) {
            ID2D1Brush *brush;
            D2D1_COLOR_F color = {
                (float)((rand() % 128) + 128) / 255,
                (float)((rand() % 128) + 128) / 255,
                (float)((rand() % 128) + 128) / 255,
                1
            };
            ID2D1RenderTarget_CreateSolidColorBrush(window->pRenderTarget, &color, NULL, &brush);

            D2D1_RECT_F rect;
            rect.left = rand() % window->width;
            rect.top = rand() % window->height;
            rect.right = rect.left + (rand() % (window->width / 10));
            rect.bottom = rect.top + (rand() % (window->height / 10));
            ID2D1RenderTarget_DrawRectangle(window->pRenderTarget, &rect, brush, 2, NULL);

            IUnknown_Release(brush);
        }

        ID2D1RenderTarget_EndDraw(window->pRenderTarget, NULL, NULL);
        EndPaint(hwnd, &paint_struct);
        return 0;
    }

    if (msg == WM_DESTROY) {
        // Free Direct2d stuff
        IUnknown_Release(window->pRenderTarget);
        IUnknown_Release(window->pFactory);

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
