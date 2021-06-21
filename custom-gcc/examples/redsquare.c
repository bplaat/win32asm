#define WIN32_USE_STDLIB_HELPERS
#define WIN32_USE_STRING_HELPERS
#include "win32.h"

char *window_class_name = "redsquare";
char *window_title = "RedSquare";
char *font_name = "Consolas";
uint32_t window_width = 800;
uint32_t window_height = 600;

#define PLAY_BUTTON_ID 1
#define EXIT_BUTTON_ID 2

typedef struct {
    uint32_t background_color;
    HWND play_button;
    HWND exit_button;
    HFONT gui_font;
} WindowData;

int32_t __stdcall SetFont(HWND hwnd, HFONT font) {
    SendMessageA(hwnd, WM_SETFONT, font, TRUE);
    return TRUE;
}

int32_t __stdcall WndProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_CREATE) {
        // Create window data
        WindowData *window_data = malloc(sizeof(WindowData));
        window_data->gui_font = NULL;
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

        // Create buttons
        window_data->play_button = CreateWindowExA(0, "BUTTON", "Play", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)PLAY_BUTTON_ID, NULL, NULL);
        window_data->exit_button = CreateWindowExA(0, "BUTTON", "Exit", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)EXIT_BUTTON_ID, NULL, NULL);

        return 0;
    }

    if (msg == WM_SIZE) {
        WindowData *window_data = GetWindowLongPtrA(hwnd, GWLP_USERDATA);

        // Save new window size
        window_width = LOWORD(lParam);
        window_height = HIWORD(lParam);

        // Resize windows
        if (window_data->gui_font != NULL) {
            DeleteObject(window_data->gui_font);
        }
        window_data->gui_font = CreateFontA(window_width / 24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
        EnumChildWindows(hwnd, SetFont, window_data->gui_font);

        uint32_t padding = window_height / 24;
        uint32_t y = (window_height - (window_height / 16 + padding + window_height / 4 + padding + window_height / 4 + padding)) / 2;
        y += window_height / 16 + padding;
        SetWindowPos(window_data->play_button, NULL, window_width / 4, y, window_width / 2, window_height / 4, SWP_NOZORDER);
        y += window_height / 4 + padding;
        SetWindowPos(window_data->exit_button, NULL, window_width / 4, y, window_width / 2, window_height / 4, SWP_NOZORDER);

        return 0;
    }

    if (msg == WM_COMMAND) {
        uint32_t id = LOWORD(wParam);

        if (id == PLAY_BUTTON_ID) {
            MessageBoxA(hwnd, "Todo", window_title, MB_OK);
        }

        if (id == EXIT_BUTTON_ID) {
            DestroyWindow(hwnd);
        }
    }

    if (msg == WM_GETMINMAXINFO) {
        // Set window min size
        MINMAXINFO *minMaxInfo = (MINMAXINFO *)lParam;
        minMaxInfo->ptMinTrackSize.x = 320;
        minMaxInfo->ptMinTrackSize.y = 240;
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
        uint32_t padding = window_width / 24;
        SetBkMode(hdc_buffer,TRANSPARENT);
        SetTextColor(hdc_buffer, 0x00ffffff);

        // Draw version text
        HFONT version_font = CreateFontA(window_width / 32, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
        SelectObject(hdc_buffer, version_font);
        SetTextAlign(hdc_buffer, TA_RIGHT);
        char *version = "v0.1.0";
        TextOutA(hdc_buffer, window_width - padding, padding, version, strlen(version));
        DeleteObject(version_font);

        // Draw title text
        HFONT title_font =  CreateFontA(window_width / 16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
        SelectObject(hdc_buffer, title_font);
        SetTextAlign(hdc_buffer, TA_CENTER);
        char *header = window_title;
        uint32_t y = (window_height - (window_height / 16 + padding + window_height / 4 + padding + window_height / 4 + padding)) / 2;
        TextOutA(hdc_buffer, window_width / 2, y, header, strlen(header));
        DeleteObject(title_font);

        // Draw footer text
        uint32_t footer_font_size = window_width / 32;
        HFONT footer_font =  CreateFontA(footer_font_size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
        SelectObject(hdc_buffer, footer_font);
        SetTextAlign(hdc_buffer, TA_CENTER);
        char *footer = "Made by Bastiaan van der Plaat";
        TextOutA(hdc_buffer, window_width / 2, window_height - footer_font_size - padding, footer, strlen(footer));
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
        WindowData *window_data = GetWindowLongPtrA(hwnd, GWLP_USERDATA);
        DeleteObject(window_data->gui_font);
        free(window_data);

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
