#define WIN32_MALLOC
#define WIN32_REALLOC
#define WIN32_FREE
#define WIN32_WCSLEN
#define WIN32_WCSDUP
#include "win32.h"

#define JAN_DEBUG
#include "jan/jan.c"

wchar_t *window_class_name = L"window-test";

#ifdef WIN64
    wchar_t *window_title = L"This is a test window 😍 (64-bit)";
#else
    wchar_t *window_title = L"This is a test window 😍 (32-bit)";
#endif

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_STYLE WS_OVERLAPPEDWINDOW

typedef struct {
    JanWidget *root;
} WindowData;

int32_t __stdcall WndProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam) {
    WindowData *window = GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    if (msg == WM_CREATE) {
        // Create window data
        window = malloc(sizeof(WindowData));
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, window);
        jan_hwnd = hwnd;
        jan_width = WINDOW_WIDTH;
        jan_height = WINDOW_HEIGHT;

        // Load widgets
        HRSRC main_layout_resource = FindResourceW(GetModuleHandleW(NULL), (wchar_t *)1, (wchar_t *)RT_RCDATA);
        HGLOBAL main_layout = LoadResource(GetModuleHandleW(NULL), main_layout_resource);
        jan_load(LockResource(main_layout), &window->root);
        return 0;
    }

    if (msg == WM_COMMAND) {
        uint16_t id = LOWORD(wParam);
        if (id == 1) {
            MessageBoxW(hwnd, L"Made by Bastiaan van der Plaat", L"About", MB_OK);
        }
        if (id == 2) {
            DestroyWindow(hwnd);
        }
    }

    if (msg == WM_SIZE) {
        // Save new window size
        jan_width = LOWORD(lParam);
        jan_height = HIWORD(lParam);

        // Resize widgets
        window->root->event_function(window->root, JAN_EVENT_MEASURE, JAN_PARAM(jan_width), JAN_PARAM(jan_height));
        window->root->event_function(window->root, JAN_EVENT_PLACE, JAN_PARAM(0), JAN_PARAM(0));
        return 0;
    }

    if (msg == WM_GETMINMAXINFO) {
        // Set window min size
        MINMAXINFO *minMaxInfo = (MINMAXINFO *)lParam;
        RECT window_rect = { 0, 0, 320, 240 };
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
        HDC hdc = BeginPaint(hwnd, &paint_struct);

        // Create back buffer
        HDC hdc_buffer = CreateCompatibleDC(hdc);
        HBITMAP bitmap_buffer = CreateCompatibleBitmap(hdc, jan_width, jan_height);
        SelectObject(hdc_buffer, bitmap_buffer);

        // Draw widgets
        window->root->event_function(window->root, JAN_EVENT_DRAW, hdc_buffer, NULL);

        // Draw and delete back buffer
        BitBlt(hdc, 0, 0, jan_width, jan_height, hdc_buffer, 0, 0, SRCCOPY);
        DeleteObject(bitmap_buffer);
        DeleteDC(hdc_buffer);

        EndPaint(hwnd, &paint_struct);
        return 0;
    }

    if (msg == WM_DESTROY) {
        // Free window data
        window->root->event_function(window->root, JAN_EVENT_FREE, NULL, NULL);
        free(window);

        // Close process
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void _start(void) {
    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icc.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icc);

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

    uint32_t x = (GetSystemMetrics(SM_CXSCREEN) - WINDOW_WIDTH) / 2;
    uint32_t y = (GetSystemMetrics(SM_CYSCREEN) - WINDOW_HEIGHT) / 2;
    RECT window_rect = { x, y, x + WINDOW_WIDTH, y + WINDOW_HEIGHT };
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
