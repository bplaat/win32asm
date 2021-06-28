#define WIN32_ALLOC_FUNCTIONS
#define WIN32_RAND_FUNCTIONS
#include "win32.h"

char *window_class_name = "window-test";

#ifdef WIN64
    char *window_title = "This is a test window (64-bit)";
#else
    char *window_title = "This is a test window (32-bit)";
#endif

uint32_t window_width = 800;
uint32_t window_height = 600;

typedef struct {
    uint32_t background_color;
} WindowData;

int32_t __stdcall WndProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_CREATE) {
        // Create window data
        WindowData *window_data = malloc(sizeof(WindowData));
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, window_data);

        // Generate random seed by time and
        SYSTEMTIME time;
        GetLocalTime(&time);
        srand((time.wHour * 60 + time.wMinute) * 60 + time.wSecond);

        // Generate random background color
        window_data->background_color = rand() & 0x007f7f7f;

        // Center window
        RECT window_rect;
        GetClientRect(hwnd, &window_rect);
        uint32_t new_width = window_width * 2 - window_rect.right;
        uint32_t new_height = window_height * 2 - window_rect.bottom;
        SetWindowPos(hwnd, NULL, (GetSystemMetrics(SM_CXSCREEN) - new_width) / 2, (GetSystemMetrics(SM_CYSCREEN) - new_height) / 2, new_width, new_height, SWP_NOZORDER);

        return 0;
    }

    if (msg == WM_SIZE) {
        // Save new window size
        window_width = LOWORD(lParam);
        window_height = HIWORD(lParam);
        return 0;
    }

    if (msg == WM_GETMINMAXINFO) {
        // Set window min size
        MINMAXINFO *minMaxInfo = (MINMAXINFO *)lParam;
        minMaxInfo->ptMinTrackSize.x = 640;
        minMaxInfo->ptMinTrackSize.y = 480;
        return 0;
    }

    if (msg == WM_ERASEBKGND) {
        // Draw no background
        return TRUE;
    }

    if (msg == WM_PAINT) {
        WindowData *window_data = GetWindowLongPtrA(hwnd, GWLP_USERDATA);

        PAINTSTRUCT paint_struct;
        HDC hdc = BeginPaint(hwnd, &paint_struct);

        // Create back buffer
        HDC hdc_buffer = CreateCompatibleDC(hdc);
        HBITMAP bitmap_buffer = CreateCompatibleBitmap(hdc, window_width, window_height);
        SelectObject(hdc_buffer, bitmap_buffer);

        // Draw background color
        HBRUSH brush = CreateSolidBrush(window_data->background_color);
        RECT rect = { 0, 0, window_width, window_height };
        FillRect(hdc_buffer, &rect, brush);
        DeleteObject(brush);

        // Setup text drawing
        uint32_t padding = window_width / 32;
        SetBkMode(hdc_buffer,TRANSPARENT);
        SetTextColor(hdc_buffer, 0x00ffffff);
        SetTextAlign(hdc_buffer, TA_CENTER);

        // Draw header text
        HFONT header_font =  CreateFontA(window_width / 24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Consolas");
        SelectObject(hdc_buffer, header_font);
        char *header = "Battle of Teripa";
        TextOutA(hdc_buffer, window_width / 2, padding, header, lstrlenA(header));
        DeleteObject(header_font);

        // Draw version text
        HFONT version_font =  CreateFontA(window_width / 32, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Tahoma");
        SelectObject(hdc_buffer, version_font);
        SetTextAlign(hdc_buffer, TA_RIGHT);
        char *version = "v0.1.0";
        TextOutA(hdc_buffer, window_width - padding, padding, version, lstrlenA(version));
        DeleteObject(version_font);

        // Draw centered text
        uint32_t font_size = window_width / 16;
        HFONT font =  CreateFontA(font_size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Comic Sans MS");
        SelectObject(hdc_buffer, font);
        SetTextAlign(hdc_buffer, TA_CENTER);
        TextOutA(hdc_buffer, window_width / 2, (window_height - font_size) / 2, window_title, lstrlenA(window_title));
        DeleteObject(font);

        // Draw footer text
        uint32_t footer_font_size = window_width / 32;
        HFONT footer_font =  CreateFontA(footer_font_size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Georgia");
        SelectObject(hdc_buffer, footer_font);
        char *footer = "Made by Bastiaan van der Plaat";
        TextOutA(hdc_buffer, window_width / 2, window_height - footer_font_size - padding, footer, lstrlenA(footer));
        DeleteObject(footer_font);

        // Draw and delete back buffer
        BitBlt(hdc, 0, 0, window_width, window_height, hdc_buffer, 0, 0, SRCCOPY);
        DeleteObject(bitmap_buffer);
        DeleteDC(hdc_buffer);

        EndPaint(hwnd, &paint_struct);
        return 0;
    }

    if (msg == WM_DESTROY) {
        // Free window data
        free(GetWindowLongPtrA(hwnd, GWLP_USERDATA));

        // Close process
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

void _start(void) {
    WNDCLASSEXA wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleA(NULL);
    wc.hIcon = wc.hIconSm = LoadIconA(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wc.lpszClassName = window_class_name;
    RegisterClassExA(&wc);

    HWND hwnd = CreateWindowExA(0, window_class_name, window_title,
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        window_width, window_height, NULL, NULL, wc.hInstance, NULL);
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    MSG message;
    while (GetMessageA(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
    ExitProcess((int32_t)(uintptr_t)message.wParam);
}
