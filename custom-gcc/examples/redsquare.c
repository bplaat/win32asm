#define WIN32_USE_STDLIB_HELPERS
#define WIN32_USE_STRING_HELPERS
#include "win32.h"

char *window_class_name = "redsquare";
char *window_title = "RedSquare";
char *font_name = "Georgia";
char *button_class = "BUTTON";
uint32_t window_width = 800;
uint32_t window_height = 600;

#define TIMER_ID 1

#define MENU_PLAY_BUTTON_ID 1
#define MENU_HELP_BUTTON_ID 2
#define MENU_EXIT_BUTTON_ID 3

#define HELP_BACK_BUTTON_ID 4

typedef enum Page {
    PAGE_MENU,
    PAGE_GAME,
    PAGE_HELP
} Page;

typedef struct Square {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
    int32_t vx;
    int32_t vy;
} Square;

typedef struct {
    Page page;
    uint32_t background_color;
    HFONT button_font;
    HWND menu_play_button;
    HWND menu_help_button;
    HWND menu_exit_button;
    HWND help_back_button;
    Square blue_squares[4];
} WindowData;

void __stdcall ChangePage(HWND hwnd, Page page) {
    WindowData *window_data = GetWindowLongPtrA(hwnd, GWLP_USERDATA);
    window_data->page = page;

    int32_t menu_visible = page == PAGE_MENU ? SW_SHOW : SW_HIDE;
    ShowWindow(window_data->menu_play_button, menu_visible);
    ShowWindow(window_data->menu_help_button, menu_visible);
    ShowWindow(window_data->menu_exit_button, menu_visible);

    int32_t help_visible = page == PAGE_HELP ? SW_SHOW : SW_HIDE;
    ShowWindow(window_data->help_back_button, help_visible);

    InvalidateRect(hwnd, NULL, TRUE);
}

int32_t __stdcall WndProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_CREATE) {
        // Create window data
        WindowData *window_data = malloc(sizeof(WindowData));
        window_data->button_font = NULL;
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, window_data);

        // Generate random seed by time and
        SYSTEMTIME time;
        GetLocalTime(&time);
        srand((time.wHour * 60 + time.wMinute) * 60 + time.wSecond);

        // Generate random background color
        window_data->background_color = (rand() & 0x007f7f7f) | 0xff000000;

        // Center window
        RECT window_rect;
        GetClientRect(hwnd, &window_rect);
        uint32_t new_width = window_width * 2 - window_rect.right;
        uint32_t new_height = window_height * 2 - window_rect.bottom;
        SetWindowPos(hwnd, NULL, (GetSystemMetrics(SM_CXSCREEN) - new_width) / 2, (GetSystemMetrics(SM_CYSCREEN) - new_height) / 2, new_width, new_height, SWP_NOZORDER);

        // Menu page widgets
        window_data->menu_play_button = CreateWindowExA(0, button_class, "Play", WS_CHILD, 0, 0, 0, 0, hwnd, (HMENU)MENU_PLAY_BUTTON_ID, NULL, NULL);
        window_data->menu_help_button = CreateWindowExA(0, button_class, "Help", WS_CHILD, 0, 0, 0, 0, hwnd, (HMENU)MENU_HELP_BUTTON_ID, NULL, NULL);
        window_data->menu_exit_button = CreateWindowExA(0, button_class, "Exit", WS_CHILD, 0, 0, 0, 0, hwnd, (HMENU)MENU_EXIT_BUTTON_ID, NULL, NULL);

        // Help page widgets
        window_data->help_back_button = CreateWindowExA(0, button_class, "Back", WS_CHILD, 0, 0, 0, 0, hwnd, (HMENU)HELP_BACK_BUTTON_ID, NULL, NULL);

        // Blue squares
        window_data->blue_squares[0].width = 100;
        window_data->blue_squares[0].height = 100;
        window_data->blue_squares[0].x = 0;
        window_data->blue_squares[0].y = 0;
        window_data->blue_squares[0].vx = 2;
        window_data->blue_squares[0].vy = 2;

        window_data->blue_squares[1].width = 100;
        window_data->blue_squares[1].height = 150;
        window_data->blue_squares[1].x = window_width - window_data->blue_squares[1].width;
        window_data->blue_squares[1].y = 0;
        window_data->blue_squares[1].vx = -2;
        window_data->blue_squares[1].vy = 2;

        window_data->blue_squares[2].width = 150;
        window_data->blue_squares[2].height = 100;
        window_data->blue_squares[2].x = 0;
        window_data->blue_squares[2].y = window_height - window_data->blue_squares[2].height;
        window_data->blue_squares[2].vx = 2;
        window_data->blue_squares[2].vy = -2;

        window_data->blue_squares[3].width = 150;
        window_data->blue_squares[3].height = 150;
        window_data->blue_squares[3].x = window_width - window_data->blue_squares[3].width;
        window_data->blue_squares[3].y = window_height - window_data->blue_squares[3].height;
        window_data->blue_squares[3].vx = -2;
        window_data->blue_squares[3].vy = -2;

        ChangePage(hwnd, PAGE_MENU);

        SetTimer(hwnd, TIMER_ID, 10, NULL);
        return 0;
    }

    if (msg == WM_SIZE) {
        WindowData *window_data = GetWindowLongPtrA(hwnd, GWLP_USERDATA);

        // Save new window size
        window_width = LOWORD(lParam);
        window_height = HIWORD(lParam);

        // Fonts
        if (window_data->button_font != NULL) DeleteObject(window_data->button_font);
        window_data->button_font = CreateFontA(window_width / 24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
        SendMessageA(window_data->menu_play_button, WM_SETFONT, window_data->button_font, (void *)TRUE);
        SendMessageA(window_data->menu_help_button, WM_SETFONT, window_data->button_font, (void *)TRUE);
        SendMessageA(window_data->menu_exit_button, WM_SETFONT, window_data->button_font, (void *)TRUE);
        SendMessageA(window_data->help_back_button, WM_SETFONT, window_data->button_font, (void *)TRUE);

        // Menu page widgets
        uint32_t padding = window_height / 24;
        uint32_t y = (window_height - (window_height / 16 + padding + (window_height / 6 + padding) * 3)) / 2;
        y += window_height / 16 + padding;
        SetWindowPos(window_data->menu_play_button, NULL, window_width / 4, y, window_width / 2, window_height / 6, SWP_NOZORDER);
        y += window_height / 6 + padding;
        SetWindowPos(window_data->menu_help_button, NULL, window_width / 4, y, window_width / 2, window_height / 6, SWP_NOZORDER);
        y += window_height / 6 + padding;
        SetWindowPos(window_data->menu_exit_button, NULL, window_width / 4, y, window_width / 2, window_height / 6, SWP_NOZORDER);

        // Help page widgets
        y = (window_height - (window_height / 16 + padding + (window_height / 6 + padding) * 2)) / 2;
        y += window_height / 16 + padding;
        y += window_height / 6 + padding;
        SetWindowPos(window_data->help_back_button, NULL, window_width / 4, y, window_width / 2, window_height / 6, SWP_NOZORDER);
        return 0;
    }

    if (msg == WM_COMMAND) {
        uint16_t id = LOWORD(wParam);

        // Menu widgets
        if (id == MENU_PLAY_BUTTON_ID) {
            ChangePage(hwnd, PAGE_GAME);
        }
        if (id == MENU_HELP_BUTTON_ID) {
            ChangePage(hwnd, PAGE_HELP);
        }
        if (id == MENU_EXIT_BUTTON_ID) {
            DestroyWindow(hwnd);
        }

        // Help widgets
        if (id == HELP_BACK_BUTTON_ID) {
            ChangePage(hwnd, PAGE_MENU);
        }
    }

    if (msg == WM_GETMINMAXINFO) {
        // Set window min size
        MINMAXINFO *minMaxInfo = (MINMAXINFO *)lParam;
        minMaxInfo->ptMinTrackSize.x = 320;
        minMaxInfo->ptMinTrackSize.y = 240;
        return 0;
    }

    if (msg == WM_TIMER) {
        WindowData *window_data = GetWindowLongPtrA(hwnd, GWLP_USERDATA);

        if ((uintptr_t)wParam == TIMER_ID) {
            if (window_data->page == PAGE_MENU || window_data->page == PAGE_HELP) {
                for (uint32_t i = 0; i < 4; i++) {
                    Square *square = &window_data->blue_squares[i];
                    square->x += square->vx;
                    square->y += square->vy;

                    if (square->x < 0 || square->x + square->width > window_width) {
                        square->vx = -square->vx;
                    }
                    if (square->y < 0 || square->y + square->height > window_height) {
                        square->vy = -square->vy;
                    }
                }

                InvalidateRect(hwnd, NULL, TRUE);
            }
        }
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

        // Create graphics object
        GpGraphics *graphics;
        GdipCreateFromHDC(hdc_buffer, &graphics);
        GdipSetSmoothingMode(graphics, SmoothingModeAntiAlias);

        // Draw background color
        GdipGraphicsClear(graphics, window_data->background_color);

        // Setup text drawing
        uint32_t padding = window_width / 24;
        SetBkMode(hdc_buffer,TRANSPARENT);
        SetTextColor(hdc_buffer, 0x00ffffff);

        // Draw menu & help page
        if (window_data->page == PAGE_MENU || window_data->page == PAGE_HELP) {
            // Draw blue squares
            GpBrush *brush;
            GdipCreateSolidFill(0xaa0000ff, (GpSolidFill **)&brush);
            for (uint32_t i = 0; i < 4; i++) {
                Square *square = &window_data->blue_squares[i];
                GdipFillRectangleI(graphics, brush, square->x, square->y, square->width, square->height);
            }
            GdipDeleteBrush(brush);
        }

        // Draw menu page
        if (window_data->page == PAGE_MENU) {
            // Draw version text
            HFONT version_font = CreateFontA(window_width / 32, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
            SelectObject(hdc_buffer, version_font);
            SetTextAlign(hdc_buffer, TA_RIGHT);
            #ifdef WIN64
                char *version = "v0.1.0 (x64)";
            #else
                char *version = "v0.1.0 (x86)";
            #endif
            TextOutA(hdc_buffer, window_width - padding, padding, version, strlen(version));
            DeleteObject(version_font);

            // Draw title text
            HFONT title_font =  CreateFontA(window_width / 16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
            SelectObject(hdc_buffer, title_font);
            SetTextAlign(hdc_buffer, TA_CENTER);
            char *header = window_title;
            uint32_t y = (window_height - (window_height / 16 + padding + (window_height / 6 + padding) * 3)) / 2;
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
        }

        // Page game
        if (window_data->page == PAGE_GAME) {
            HFONT status_font = CreateFontA(window_width / 48, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
            SelectObject(hdc_buffer, status_font);
            SetTextAlign(hdc_buffer, TA_CENTER);
            char *version = "RedSquare";
            TextOutA(hdc_buffer, window_width / 2, 16, version, strlen(version));
            DeleteObject(status_font);
        }

        // Page help
        if (window_data->page == PAGE_HELP) {
            // Draw title text
            HFONT title_font =  CreateFontA(window_width / 16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
            SelectObject(hdc_buffer, title_font);
            SetTextAlign(hdc_buffer, TA_CENTER);
            char *header = "Help";
            uint32_t y = (window_height - (window_height / 16 + padding + (window_height / 6 + padding) * 3)) / 2;
            TextOutA(hdc_buffer, window_width / 2, y, header, strlen(header));
            DeleteObject(title_font);
        }

        // Delete GDI+ graphics object
        GdipDeleteGraphics(graphics);

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
        DeleteObject(window_data->button_font);
        free(window_data);

        // Close process
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

void _start(void) {
    uint32_t gdiplusToken;
    GdiplusStartupInput gdiplusStartupInput;
    gdiplusStartupInput.GdiplusVersion = 1;
    gdiplusStartupInput.DebugEventCallback = NULL;
    gdiplusStartupInput.SuppressBackgroundThread = FALSE;
    gdiplusStartupInput.SuppressExternalCodecs = FALSE;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

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
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT,
        window_width, window_height, NULL, NULL, wc.hInstance, NULL);
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    MSG message;
    while (GetMessageA(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    GdiplusShutdown(&gdiplusToken);

    ExitProcess((int32_t)(uintptr_t)message.wParam);
}
