#include "win32.h"

char *window_class_name = "window-test";
uint32_t window_width = 800;
uint32_t window_height = 600;

int32_t __stdcall WndProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_CREATE) {
        RECT window_rect;
        GetClientRect(hwnd, &window_rect);
        uint32_t new_width = window_width * 2 - window_rect.right;
        uint32_t new_height = window_height * 2 - window_rect.bottom;
        SetWindowPos(hwnd, NULL, (GetSystemMetrics(SM_CXSCREEN) - new_width) / 2, (GetSystemMetrics(SM_CYSCREEN) - new_height) / 2, new_width, new_height, SWP_NOZORDER);
        return 0;
    }

    if (msg == WM_SIZE) {
        window_width = LOWORD(lParam);
        window_height = HIWORD(lParam);
        return 0;
    }

    if (msg == WM_GETMINMAXINFO) {
        MINMAXINFO *minMaxInfo = (MINMAXINFO *)lParam;
        minMaxInfo->ptMinTrackSize.x = 320;
        minMaxInfo->ptMinTrackSize.y = 240;
        return 0;
    }

    if (msg == WM_ERASEBKGND) {
        return TRUE;
    }

    if (msg == WM_PAINT) {
        PAINTSTRUCT paint_struct;
        HDC hdc = BeginPaint(hwnd, &paint_struct);
        HDC hdc_buffer = CreateCompatibleDC(hdc);
        HBITMAP bitmap_buffer = CreateCompatibleBitmap(hdc, window_width, window_height);
        SelectObject(hdc_buffer, bitmap_buffer);

        // Draw background color
        HBRUSH brush = CreateSolidBrush(0x00ff00ff);
        RECT rect = { 0, 0, window_width, window_height };
        FillRect(hdc_buffer, &rect, brush);
        DeleteObject(brush);

        BitBlt(hdc, 0, 0, window_width, window_height, hdc_buffer, 0, 0, SRCCOPY);
        DeleteObject(bitmap_buffer);
        DeleteDC(hdc_buffer);

        EndPaint(hwnd, &paint_struct);
        return 0;
    }

    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

void _start(void) {
    WNDCLASSEXA wc;
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandleA(NULL);
    wc.hIcon = wc.hIconSm = LoadIconA(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = window_class_name;
    RegisterClassExA(&wc);

    HWND hwnd = CreateWindowExA(0, window_class_name, "This is a test window (32-bit)",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, window_width, window_height, NULL, NULL, wc.hInstance, NULL);
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    MSG message;
    while (GetMessageA(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
    ExitProcess((int32_t)message.wParam);
}
