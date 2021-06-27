#define WIN32_USE_STDLIB_HELPERS
#define WIN32_USE_STRING_HELPERS
#include "win32.h"
#include "redsquare.h"

#define FRAME_TIMER_ID 1

#define MENU_PLAY_BUTTON_ID 1
#define MENU_HELP_BUTTON_ID 2
#define MENU_EXIT_BUTTON_ID 3

#define GAMEOVER_BACK_BUTTON_ID 4

#define HELP_BACK_BUTTON_ID 5

#define FPS 100

char *window_class_name = "redsquare";
char *window_title = "RedSquare";
char *font_name = "Georgia";
char *button_class = "BUTTON";
char *back_text = "Back";
char *score_format_text = "Score: %d";
char *time_format_text = "Time: %d:%02d";
char *level_format_text = "Level: %d";
char *help_lines[] = {
    "RedSquare is an classic 2D action game.",
    "Click and hold the red square.",
    "Now move so that you neither touch the wall",
    "nor get hit by any of the blue squares.",
    "If you make it to 60 seconds, you are",
    "doing brilliantly!"
};

uint32_t window_width = 800;
uint32_t window_height = 600;
uint32_t min_window_width = 640;
uint32_t min_window_height = 480;
float vw, vh, vx, padding;

typedef enum Page {
    PAGE_MENU,
    PAGE_GAME,
    PAGE_GAMEOVER,
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
    HWND gameover_back_button;
    HWND help_back_button;

    uint32_t time;
    uint32_t level;
    uint32_t score;
    bool is_dragging;
    Square red_square;
    Square blue_squares[4];
} WindowData;

void __stdcall InitBlueSquares(HWND hwnd, float speed) {
    WindowData *window_data = GetWindowLongPtrA(hwnd, GWLP_USERDATA);

    window_data->blue_squares[0].width = 72;
    window_data->blue_squares[0].height = 72;
    window_data->blue_squares[0].x = 0;
    window_data->blue_squares[0].y = 0;
    window_data->blue_squares[0].vx = speed;
    window_data->blue_squares[0].vy = speed;

    window_data->blue_squares[1].width = 72;
    window_data->blue_squares[1].height = 96;
    window_data->blue_squares[1].x = min_window_width - window_data->blue_squares[1].width;
    window_data->blue_squares[1].y = 0;
    window_data->blue_squares[1].vx = -speed;
    window_data->blue_squares[1].vy = speed;

    window_data->blue_squares[2].width = 96;
    window_data->blue_squares[2].height = 72;
    window_data->blue_squares[2].x = 0;
    window_data->blue_squares[2].y = min_window_height - window_data->blue_squares[2].height;
    window_data->blue_squares[2].vx = speed;
    window_data->blue_squares[2].vy = -speed;

    window_data->blue_squares[3].width = 96;
    window_data->blue_squares[3].height = 96;
    window_data->blue_squares[3].x = min_window_width - window_data->blue_squares[3].width;
    window_data->blue_squares[3].y = min_window_height - window_data->blue_squares[3].height;
    window_data->blue_squares[3].vx = -speed;
    window_data->blue_squares[3].vy = -speed;
}

void __stdcall StartGame(HWND hwnd) {
    WindowData *window_data = GetWindowLongPtrA(hwnd, GWLP_USERDATA);

    window_data->time = 0;
    window_data->level = 1;
    window_data->score = 0;

    window_data->is_dragging = false;
    window_data->red_square.width = 48;
    window_data->red_square.height = 48;
    window_data->red_square.x = (min_window_width - window_data->red_square.width) / 2;
    window_data->red_square.y = (min_window_height - window_data->red_square.height) / 2;

    InitBlueSquares(hwnd, 1);
}

void __stdcall ChangePage(HWND hwnd, Page page) {
    WindowData *window_data = GetWindowLongPtrA(hwnd, GWLP_USERDATA);
    window_data->page = page;

    int32_t menu_visible = page == PAGE_MENU ? SW_SHOW : SW_HIDE;
    ShowWindow(window_data->menu_play_button, menu_visible);
    ShowWindow(window_data->menu_help_button, menu_visible);
    ShowWindow(window_data->menu_exit_button, menu_visible);

    int32_t gameover_visible = page == PAGE_GAMEOVER ? SW_SHOW : SW_HIDE;
    ShowWindow(window_data->gameover_back_button, gameover_visible);

    int32_t help_visible = page == PAGE_HELP ? SW_SHOW : SW_HIDE;
    ShowWindow(window_data->help_back_button, help_visible);

    InvalidateRect(hwnd, NULL, TRUE);
}

int32_t __stdcall WndProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam) {
    WindowData *window_data = GetWindowLongPtrA(hwnd, GWLP_USERDATA);

    if (msg == WM_CREATE) {
        // Create window data
        window_data = malloc(sizeof(WindowData));
        window_data->button_font = NULL;
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, window_data);

        // Generate random seed by time and
        SYSTEMTIME time;
        GetLocalTime(&time);
        srand((time.wHour * 60 + time.wMinute) * 60 + time.wSecond);

        // Load background image resource
        window_data->background_image = LoadImageA(GetModuleHandleA(NULL), (char *)PAPER_BITMAP_ID, IMAGE_BITMAP, 256, 256, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
        // window_data->background_image = LoadImageA(GetModuleHandleA(NULL), "paper.bmp", IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
        // if (window_data->background_image == NULL) {
        //     char error_buffer[255];
        //     wsprintfA(error_buffer, "Can't load paper.bmp from resources: %d", GetLastError());
        //     MessageBoxA(HWND_DESKTOP, error_buffer, "Image Load Error!", MB_OK);
        //     ExitProcess(1);
        // }

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

        // Gameover page widgets
        window_data->gameover_back_button = CreateWindowExA(0, button_class, back_text, WS_CHILD, 0, 0, 0, 0, hwnd, (HMENU)GAMEOVER_BACK_BUTTON_ID, NULL, NULL);

        // Help page widgets
        window_data->help_back_button = CreateWindowExA(0, button_class, back_text, WS_CHILD, 0, 0, 0, 0, hwnd, (HMENU)HELP_BACK_BUTTON_ID, NULL, NULL);

        // Go to menu page
        ChangePage(hwnd, PAGE_MENU);
        InitBlueSquares(hwnd, 2);

        // Start frame timer
        SetTimer(hwnd, FRAME_TIMER_ID, 1000 / FPS, NULL);
        return 0;
    }

    if (msg == WM_SIZE) {
        // Save new window size
        window_width = LOWORD(lParam);
        window_height = HIWORD(lParam);
        vw = (float)window_width / (float)min_window_width;
        vh = (float)window_height / (float)min_window_height;
        vx = MIN(vw, vh);
        padding = 16 * vx;

        // Fonts
        if (window_data->button_font != NULL) DeleteObject(window_data->button_font);
        window_data->button_font = CreateFontA(32 * vx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
        SendMessageA(window_data->menu_play_button, WM_SETFONT, window_data->button_font, (void *)TRUE);
        SendMessageA(window_data->menu_help_button, WM_SETFONT, window_data->button_font, (void *)TRUE);
        SendMessageA(window_data->menu_exit_button, WM_SETFONT, window_data->button_font, (void *)TRUE);
        SendMessageA(window_data->gameover_back_button, WM_SETFONT, window_data->button_font, (void *)TRUE);
        SendMessageA(window_data->help_back_button, WM_SETFONT, window_data->button_font, (void *)TRUE);

        // Menu page widgets
        float y = (window_height - (48 * vx + padding + (72 * vx + padding) * 3)) / 2;
        y += 48 * vx  + padding;
        SetWindowPos(window_data->menu_play_button, NULL, window_width / 4, y, window_width / 2, 72 * vx, SWP_NOZORDER);
        y += 72 * vx + padding;
        SetWindowPos(window_data->menu_help_button, NULL, window_width / 4, y, window_width / 2, 72 * vx, SWP_NOZORDER);
        y += 72 * vx + padding;
        SetWindowPos(window_data->menu_exit_button, NULL, window_width / 4, y, window_width / 2, 72 * vx, SWP_NOZORDER);

        // Gameover page widgets
        y = (window_height - (48 * vx + padding + (24 * vx + padding) * 3 + 72 * vx)) / 2;
        y += 48 * vx + padding + (24 * vx + padding) * 3;
        SetWindowPos(window_data->gameover_back_button, NULL, window_width / 4, y, window_width / 2, 72 * vx, SWP_NOZORDER);

        // Help page widgets
        y = (window_height - (48 * vx + padding + (24 * vx + padding) * (sizeof(help_lines) / sizeof(char *)) + 72 * vx)) / 2;
        y += 48 * vx + padding + (24 * vx + padding) * (sizeof(help_lines) / sizeof(char *));
        SetWindowPos(window_data->help_back_button, NULL, window_width / 4, y, window_width / 2, 72 * vx, SWP_NOZORDER);
        return 0;
    }

    if (msg == WM_COMMAND) {
        uint16_t id = LOWORD(wParam);

        // Menu page widgets
        if (id == MENU_PLAY_BUTTON_ID) {
            ChangePage(hwnd, PAGE_GAME);
            StartGame(hwnd);
        }
        if (id == MENU_HELP_BUTTON_ID) {
            ChangePage(hwnd, PAGE_HELP);
        }
        if (id == MENU_EXIT_BUTTON_ID) {
            DestroyWindow(hwnd);
        }

        // Gameover page widgets
        if (id == GAMEOVER_BACK_BUTTON_ID) {
            ChangePage(hwnd, PAGE_MENU);
            InitBlueSquares(hwnd, 2);
        }

        // Help page widgets
        if (id == HELP_BACK_BUTTON_ID) {
            ChangePage(hwnd, PAGE_MENU);
        }
    }

    if (msg == WM_LBUTTONDOWN) {
        if (window_data->page == PAGE_GAME) {
            float x = LOWORD(lParam) / vw;
            float y = HIWORD(lParam) / vh;

            Square *square = &window_data->red_square;
            if (
                x >= square->x &&
                y >= square->y &&
                x < square->x + square->width &&
                y < square->y + square->height
            ) {
                window_data->is_dragging = true;
                window_data->red_square.vx = x - square->x;
                window_data->red_square.vy = y - square->y;
            }
        }
    }

    if (msg == WM_MOUSEMOVE) {
        if (window_data->page == PAGE_GAME) {
            float x = LOWORD(lParam) / vw;
            float y = HIWORD(lParam) / vh;

            if (window_data->is_dragging) {
                window_data->red_square.x = x - window_data->red_square.vx;
                window_data->red_square.y = y - window_data->red_square.vy;
            }
        }
    }

    if (msg == WM_LBUTTONUP) {
        if (window_data->page == PAGE_MENU) {
            int16_t x = LOWORD(lParam);
            int16_t y = HIWORD(lParam);

            if (y > window_height - padding - 24 * vx - padding) {
                ShellExecuteA(hwnd, "open", "https://bastiaan.ml/", NULL, NULL, SW_SHOWNORMAL);
            }
        }

        if (window_data->page == PAGE_GAME) {
            window_data->is_dragging = false;
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
        if ((uintptr_t)wParam == FRAME_TIMER_ID) {
            bool is_leveled = false;
            bool is_gameover = false;
            if (window_data->page == PAGE_GAME) {
                window_data->score += window_data->level;
                window_data->time++;

                if ((window_data->time % (10 * FPS)) == 0) {
                    window_data->level++;
                    is_leveled = true;
                }

                Square *red_square = &window_data->red_square;
                if (
                    red_square->x < padding ||
                    red_square->y < padding + 20 * vx + padding ||
                    red_square->x + red_square->width > min_window_width - padding ||
                    red_square->y + red_square->height > min_window_height - padding
                ) {
                    is_gameover = true;
                }
            }

            if (window_data->page == PAGE_MENU || window_data->page == PAGE_GAME || window_data->page == PAGE_HELP) {
                Square *red_square = &window_data->red_square;
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

                    if (window_data->page == PAGE_GAME) {
                        if (is_leveled) {
                            square->vx *= 1.5;
                            square->vy *= 1.5;
                        }

                        if (
                            square->x < red_square->x + red_square->width &&
                            square->x + square->width > red_square->x &&
                            square->y < red_square->y + red_square->height &&
                            square->y + square->height > red_square->y
                        ) {
                            is_gameover = true;
                        }
                    }
                }

                if (window_data->page == PAGE_GAME && is_gameover) {
                    ChangePage(hwnd, PAGE_GAMEOVER);
                    PlaySoundA((char *)DEAD_WAVE_ID, GetModuleHandleA(NULL), SND_RESOURCE | SND_ASYNC);
                }

                InvalidateRect(hwnd, NULL, TRUE);
            }
        }
    }

    if (msg == WM_ERASEBKGND) {
        return TRUE;
    }

    if (msg == WM_PAINT) {
        PAINTSTRUCT paint_struct;
        HDC hdc = BeginPaint(hwnd, &paint_struct);

        // Create back buffer
        HDC hdc_buffer = CreateCompatibleDC(hdc);
        HBITMAP bitmap_buffer = CreateCompatibleBitmap(hdc, window_width, window_height);
        SelectObject(hdc_buffer, bitmap_buffer);

        // Draw background image
        if (window_data->background_image != NULL) {
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
        }

        // Create graphics object
        GpGraphics *graphics;
        GdipCreateFromHDC(hdc_buffer, &graphics);
        GdipSetSmoothingMode(graphics, SmoothingModeAntiAlias);

        if (window_data->background_image == NULL) {
            GdipGraphicsClear(graphics, 0xffffffff);
        }

        // Setup text drawing
        SetBkMode(hdc_buffer, TRANSPARENT);
        SetTextColor(hdc_buffer, 0x00111111);

        // Draw border
        if (window_data->page == PAGE_GAME || window_data->page == PAGE_GAMEOVER) {
            GpBrush *brush;
            GdipCreateSolidFill(0x33000000, (GpSolidFill **)&brush);
            GdipFillRectangle(graphics, brush, padding, padding + 20 * vx + padding, window_width - padding - padding, window_height - padding - 20 * vx - padding - padding);
            GdipDeleteBrush(brush);
        }

        // Draw blue squares
        if (window_data->page == PAGE_MENU || window_data->page == PAGE_GAME || window_data->page == PAGE_GAMEOVER || window_data->page == PAGE_HELP) {
            GpBrush *brush;
            GdipCreateSolidFill(0xaa0000ff, (GpSolidFill **)&brush);
            for (int32_t i = 0; i < 4; i++) {
                Square *square = &window_data->blue_squares[i];
                GdipFillRectangle(graphics, brush, square->x * vw, square->y * vh, square->width * vw, square->height * vh);
            }
            GdipDeleteBrush(brush);
        }

        // Draw red square
        if (window_data->page == PAGE_GAME || window_data->page == PAGE_GAMEOVER) {
            GpBrush *brush;
            GdipCreateSolidFill(0xaaff0000, (GpSolidFill **)&brush);
            Square *square = &window_data->red_square;
            GdipFillRectangle(graphics, brush, square->x * vw, square->y * vh, square->width * vw, square->height * vh);
            GdipDeleteBrush(brush);
        }

        // Draw menu page
        if (window_data->page == PAGE_MENU) {
            // Draw version text
            HFONT text_font = CreateFontA(24 * vx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
            SelectObject(hdc_buffer, text_font);
            SetTextAlign(hdc_buffer, TA_RIGHT);
            #ifdef WIN64
                char *version_text = "v" STR(APP_VERSION_MAJOR) "." STR(APP_VERSION_MINOR) " (x64)";
            #else
                char *version_text = "v" STR(APP_VERSION_MAJOR) "." STR(APP_VERSION_MINOR) " (x86)";
            #endif
            TextOutA(hdc_buffer, window_width - padding, padding, version_text, strlen(version_text));

            // Draw title text
            HFONT title_font = CreateFontA(48 * vx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
            SelectObject(hdc_buffer, title_font);
            SetTextAlign(hdc_buffer, TA_CENTER);
            float y = (window_height - (48 * vx + padding + (72 * vx + padding) * 3)) / 2;
            TextOutA(hdc_buffer, window_width / 2, y, window_title, strlen(window_title));
            DeleteObject(title_font);

            // Draw footer text
            SelectObject(hdc_buffer, text_font);
            SetTextAlign(hdc_buffer, TA_CENTER);
            char *footer_text = "Made by Bastiaan van der Plaat";
            TextOutA(hdc_buffer, window_width / 2, window_height - 24 * vx - padding, footer_text, strlen(footer_text));
            DeleteObject(text_font);
        }

        // Page game
        if (window_data->page == PAGE_GAME || window_data->page == PAGE_GAMEOVER) {
            // Draw game stats
            HFONT stats_font = CreateFontA(20 * vx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
            SelectObject(hdc_buffer, stats_font);

            char string_buffer[64];
            wsprintfA(string_buffer, score_format_text, window_data->score);
            SetTextAlign(hdc_buffer, TA_LEFT);
            TextOutA(hdc_buffer, padding, padding, string_buffer, strlen(string_buffer));

            uint32_t seconds = window_data->time / FPS;
            wsprintfA(string_buffer, time_format_text, seconds / 60, seconds % 60);
            SetTextAlign(hdc_buffer, TA_CENTER);
            TextOutA(hdc_buffer, window_width / 2, padding, string_buffer, strlen(string_buffer));

            wsprintfA(string_buffer, level_format_text, window_data->level);
            SetTextAlign(hdc_buffer, TA_RIGHT);
            TextOutA(hdc_buffer, window_width - padding, padding, string_buffer, strlen(string_buffer));

            DeleteObject(stats_font);
        }

        // Page gameover
        if (window_data->page == PAGE_GAMEOVER) {
            // Draw overlay
            GpBrush *brush;
            GdipCreateSolidFill(0x88ffffff, (GpSolidFill **)&brush);
            GdipFillRectangleI(graphics, brush, 0, 0, window_width, window_height);
            GdipDeleteBrush(brush);

            // Draw title text
            HFONT title_font = CreateFontA(48 * vx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
            SelectObject(hdc_buffer, title_font);
            SetTextAlign(hdc_buffer, TA_CENTER);
            char *title_text = "Game Over";
            float y = (window_height - (48 * vx + padding + (24 * vx + padding) * 3 + 72 * vx)) / 2;
            TextOutA(hdc_buffer, window_width / 2, y, title_text, strlen(title_text));
            y += 48 * vx + padding;
            DeleteObject(title_font);

            // Draw stats lines
            HFONT text_font = CreateFontA(24 * vx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
            SelectObject(hdc_buffer, text_font);

            char string_buffer[64];
            wsprintfA(string_buffer, score_format_text, window_data->score);
            TextOutA(hdc_buffer, window_width / 2, y, string_buffer, strlen(string_buffer));
            y += 24 * vx + padding;

            uint32_t seconds = window_data->time / FPS;
            wsprintfA(string_buffer, time_format_text, seconds / 60, seconds % 60);
            TextOutA(hdc_buffer, window_width / 2, y, string_buffer, strlen(string_buffer));
            y += 24 * vx + padding;

            wsprintfA(string_buffer, level_format_text, window_data->level);
            TextOutA(hdc_buffer, window_width / 2, y, string_buffer, strlen(string_buffer));

            DeleteObject(text_font);
        }

        // Page help
        if (window_data->page == PAGE_HELP) {
            // Draw title text
            HFONT title_font = CreateFontA(48 * vx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
            SelectObject(hdc_buffer, title_font);
            SetTextAlign(hdc_buffer, TA_CENTER);
            char *title_text = "Help";
            float y = (window_height - (48 * vx + padding + (24 * vx + padding) * (sizeof(help_lines) / sizeof(char *)) + 72 * vx)) / 2;
            TextOutA(hdc_buffer, window_width / 2, y, title_text, strlen(title_text));
            y += 48 * vx + padding;
            DeleteObject(title_font);

            // Draw help lines
            HFONT text_font = CreateFontA(24 * vx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
            SelectObject(hdc_buffer, text_font);
            for (int32_t i = 0; i < (sizeof(help_lines) / sizeof(char *)); i++) {
                TextOutA(hdc_buffer, window_width / 2, y, help_lines[i], strlen(help_lines[i]));
                y += 24 * vx + padding;
            }
            DeleteObject(text_font);
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
    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icc);

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
    wc.hIcon = LoadImageA(wc.hInstance, (char *)APP_ICON_ID, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_SHARED);
    wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wc.lpszClassName = window_class_name;
    wc.hIconSm = LoadImageA(wc.hInstance, (char *)APP_ICON_ID, IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR | LR_SHARED);
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
