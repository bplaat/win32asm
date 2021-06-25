#define WIN32_USE_STDLIB_HELPERS
#define WIN32_USE_STRING_HELPERS
#include "win32.h"

char *window_class_name = "redsquare";
char *window_title = "RedSquare";
char *font_name = "Georgia";
char *button_class = "BUTTON";
uint32_t window_width = 800;
uint32_t window_height = 600;
uint32_t min_window_width = 640;
uint32_t min_window_height = 480;
float vw;
float vh;

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
    float x;
    float y;
    float width;
    float height;
    float vx;
    float vy;
} Square;

typedef struct {
    HBITMAP background_image;
    Page page;
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

        window_data->background_image = LoadImageA(NULL, "paper.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTCOLOR);
        if (window_data->background_image == NULL) {
            MessageBoxA(HWND_DESKTOP, "Can't load paper.bmp", "Image Load Error!", MB_OK);
            ExitProcess(1);
        }

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
        window_data->blue_squares[0].width = 72;
        window_data->blue_squares[0].height = 72;
        window_data->blue_squares[0].x = 0;
        window_data->blue_squares[0].y = 0;
        window_data->blue_squares[0].vx = 2;
        window_data->blue_squares[0].vy = 2;

        window_data->blue_squares[1].width = 72;
        window_data->blue_squares[1].height = 96;
        window_data->blue_squares[1].x = min_window_width - window_data->blue_squares[1].width;
        window_data->blue_squares[1].y = 0;
        window_data->blue_squares[1].vx = -2;
        window_data->blue_squares[1].vy = 2;

        window_data->blue_squares[2].width = 96;
        window_data->blue_squares[2].height = 72;
        window_data->blue_squares[2].x = 0;
        window_data->blue_squares[2].y = min_window_height - window_data->blue_squares[2].height;
        window_data->blue_squares[2].vx = 2;
        window_data->blue_squares[2].vy = -2;

        window_data->blue_squares[3].width = 96;
        window_data->blue_squares[3].height = 96;
        window_data->blue_squares[3].x = min_window_width - window_data->blue_squares[3].width;
        window_data->blue_squares[3].y = min_window_height - window_data->blue_squares[3].height;
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
        vw = (float)window_width / (float)min_window_width;
        vh = (float)window_height / (float)min_window_height;

        // Fonts
        if (window_data->button_font != NULL) DeleteObject(window_data->button_font);
        window_data->button_font = CreateFontA(32 * vw, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
        SendMessageA(window_data->menu_play_button, WM_SETFONT, window_data->button_font, (void *)TRUE);
        SendMessageA(window_data->menu_help_button, WM_SETFONT, window_data->button_font, (void *)TRUE);
        SendMessageA(window_data->menu_exit_button, WM_SETFONT, window_data->button_font, (void *)TRUE);
        SendMessageA(window_data->help_back_button, WM_SETFONT, window_data->button_font, (void *)TRUE);

        // Menu page widgets
        float padding = 16 * vw;
        float y = (window_height - (48 * vw + padding + (72 * vh + padding) * 3)) / 2;
        y += 48 * vw  + padding;
        SetWindowPos(window_data->menu_play_button, NULL, window_width / 4, y, window_width / 2, 72 * vh, SWP_NOZORDER);
        y += 72 * vh + padding;
        SetWindowPos(window_data->menu_help_button, NULL, window_width / 4, y, window_width / 2, 72 * vh, SWP_NOZORDER);
        y += 72 * vh + padding;
        SetWindowPos(window_data->menu_exit_button, NULL, window_width / 4, y, window_width / 2, 72 * vh, SWP_NOZORDER);

        // Help page widgets
        y = (window_height - (48 * vw + padding + 72 * vh)) / 2;
        y += 48 * vw + padding;
        SetWindowPos(window_data->help_back_button, NULL, window_width / 4, y, window_width / 2, 72 * vh, SWP_NOZORDER);
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
        minMaxInfo->ptMinTrackSize.x = min_window_width;
        minMaxInfo->ptMinTrackSize.y = min_window_height;
        return 0;
    }

    if (msg == WM_TIMER) {
        WindowData *window_data = GetWindowLongPtrA(hwnd, GWLP_USERDATA);

        if ((uintptr_t)wParam == TIMER_ID) {
            if (window_data->page == PAGE_MENU || window_data->page == PAGE_HELP) {
                for (int32_t i = 0; i < 4; i++) {
                    Square *square = &window_data->blue_squares[i];
                    square->x += square->vx;
                    square->y += square->vy;

                    if (square->x < 0 || square->x + square->width > min_window_width) {
                        square->vx = -square->vx;
                    }
                    if (square->y < 0 || square->y + square->height > min_window_height) {
                        square->vy = -square->vy;
                    }
                }

                InvalidateRect(hwnd, NULL, TRUE);
            }
        }
    }

    if (msg == WM_ERASEBKGND) {
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

        // Draw background image
        HDC hdc_bitmap_buffer = CreateCompatibleDC(hdc_buffer);
        SelectObject(hdc_bitmap_buffer, window_data->background_image);
        uint32_t cols = window_width / 256 + 1;
        uint32_t rows = window_height / 256 + 1;
        for (int32_t y = 0; y <= rows; y ++) {
            for (int32_t x = 0; x <= cols; x ++) {
                BitBlt(hdc_buffer, (window_width / 2) + (x - cols / 2 - 1) * 256, (window_height / 2) + (y - rows / 2 - 1) * 256, window_width, window_height, hdc_bitmap_buffer, 0, 0, SRCCOPY);
            }
        }
        DeleteDC(hdc_bitmap_buffer);

        // Create graphics object
        GpGraphics *graphics;
        GdipCreateFromHDC(hdc_buffer, &graphics);
        GdipSetSmoothingMode(graphics, SmoothingModeAntiAlias);

        // Setup text drawing
        float padding = 16 * vw;
        SetBkMode(hdc_buffer,TRANSPARENT);
        SetTextColor(hdc_buffer, 0x00111111);

        // Draw menu & help page
        if (window_data->page == PAGE_MENU || window_data->page == PAGE_HELP) {
            // Draw blue squares
            GpBrush *brush;
            GdipCreateSolidFill(0xaa0000ff, (GpSolidFill **)&brush);
            for (int32_t i = 0; i < 4; i++) {
                Square *square = &window_data->blue_squares[i];
                GdipFillRectangle(graphics, brush, square->x * vw, square->y * vh, square->width * vw, square->height * vh);
            }
            GdipDeleteBrush(brush);
        }

        // Draw menu page
        if (window_data->page == PAGE_MENU) {
            // Draw version text
            HFONT text_font = CreateFontA(24 * vw, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
            SelectObject(hdc_buffer, text_font);
            SetTextAlign(hdc_buffer, TA_RIGHT);
            #ifdef WIN64
                char *version_text = "v0.1.0 (x64)";
            #else
                char *version_text = "v0.1.0 (x86)";
            #endif
            TextOutA(hdc_buffer, window_width - padding, padding, version_text, strlen(version_text));

            // Draw title text
            HFONT title_font =  CreateFontA(48 * vw, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
            SelectObject(hdc_buffer, title_font);
            SetTextAlign(hdc_buffer, TA_CENTER);
            float y = (window_height - (48 * vw + padding + (72 * vh + padding) * 3)) / 2;
            TextOutA(hdc_buffer, window_width / 2, y, window_title, strlen(window_title));
            DeleteObject(title_font);

            // Draw footer text
            SelectObject(hdc_buffer, text_font);
            SetTextAlign(hdc_buffer, TA_CENTER);
            char *footer_text = "Made by Bastiaan van der Plaat";
            TextOutA(hdc_buffer, window_width / 2, window_height - 24 * vw - padding, footer_text, strlen(footer_text));
            DeleteObject(text_font);
        }

        // Page game
        if (window_data->page == PAGE_GAME) {
            HFONT status_font = CreateFontA(24 * vw, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
            SelectObject(hdc_buffer, status_font);
            SetTextAlign(hdc_buffer, TA_CENTER);
            char *temp_text = "RedSquare";
            TextOutA(hdc_buffer, window_width / 2, padding, temp_text, strlen(temp_text));
            DeleteObject(status_font);
        }

        // Page help
        if (window_data->page == PAGE_HELP) {
            // Draw title text
            HFONT title_font =  CreateFontA(48 * vw, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
            SelectObject(hdc_buffer, title_font);
            SetTextAlign(hdc_buffer, TA_CENTER);
            char *title_text = "Help";
            float y = (window_height - (48 * vw + padding + 72 * vh)) / 2;
            TextOutA(hdc_buffer, window_width / 2, y, title_text, strlen(title_text));
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
        DeleteObject(window_data->background_image);
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
