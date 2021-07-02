#define WIN32_MALLOC
#define WIN32_REALLOC
#define WIN32_FREE
#define WIN32_WCSLEN
#define WIN32_WCSCPY
#define WIN32_WCSCAT
#include "win32.h"
#include "redsquare.h"

#define FRAME_TIMER_ID 1

#define FPS 50

#define THEME_LIGHT 0
#define THEME_DARK 1

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define VIEWPORT_WIDTH 640
#define VIEWPORT_HEIGHT 480

wchar_t *window_class_name = L"redsquare";
wchar_t *settings_file = L"\\redsquare-settings.bin";

#define SETTINGS_SIGNATURE 0x56415352
#define SETTINGS_NAME_SIZE 32

typedef union SettingsVersion {
    struct {
        uint16_t major;
        uint16_t minor;
        uint16_t patch;
        uint16_t zero;
    } fields;
    uint64_t bits;
} SettingsVersion;

typedef struct SettingsHeader {
    uint32_t signature;
    SettingsVersion version;
    uint32_t name_address;
    uint32_t language_address;
    uint32_t theme_address;
    uint32_t highscores_address;
} SettingsHeader;

typedef struct HighScore {
    wchar_t name[SETTINGS_NAME_SIZE];
    uint32_t score;
} HighScore;

typedef struct Control {
    uint8_t type;
    uint8_t page;
    uint16_t id;
    uint16_t string;
    uint16_t font;
    uint32_t style;
    float x;
    float y;
    float width;
    float height;
    uint8_t x_unit;
    uint8_t y_unit;
    uint8_t width_unit;
    uint8_t height_unit;
} Control;

typedef struct Square {
    float x;
    float y;
    float width;
    float height;
    float vx;
    float vy;
} Square;

typedef struct {
    uint32_t width;
    uint32_t height;
    float vw;
    float vh;
    float vx;
    HINSTANCE instance;
    HBITMAP paper_bitmap;
    HBITMAP paper_dark_bitmap;
    uint32_t page;
    Control *controls;
    HANDLE *controls_handles;
    uint32_t controls_size;

    wchar_t name[SETTINGS_NAME_SIZE];
    uint32_t language;
    uint32_t theme;
    HighScore *highscores;
    uint32_t highscores_size;

    uint32_t time;
    uint32_t level;
    uint32_t score;
    bool is_dragging;
    Square red_square;
    Square blue_squares[4];
} WindowData;

bool __stdcall IsVistaOrHigher(void) {
    OSVERSIONINFOW osver = {0};
    osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
    GetVersionExW(&osver);
    return osver.dwMajorVersion >= 6;
}

typedef uint32_t (__stdcall *_SetThreadUILanguage)(uint32_t LangId);

void __stdcall SetLanguage(uint32_t language) {
    if (IsVistaOrHigher()) {
        HMODULE kernel32 = LoadLibraryW(L"kernel32.dll");
        _SetThreadUILanguage SetThreadUILanguage = GetProcAddress(kernel32, "SetThreadUILanguage");
        SetThreadUILanguage(language);
    } else {
        SetThreadLocale(language);
    }
}

void __stdcall LoadSettings(HWND hwnd) {
    WindowData *window = GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    wchar_t settings_path[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, settings_path);
    wcscat(settings_path, settings_file);

    HANDLE settings_file = CreateFileW(settings_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (settings_file != NULL) {
        // Read settings file header
        SettingsHeader settings_header;
        uint32_t bytes_read;
        ReadFile(settings_file, &settings_header, sizeof(SettingsHeader), &bytes_read, NULL);

        SettingsVersion version = { APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_PATCH, 0 };
        if (
            settings_header.signature == SETTINGS_SIGNATURE &&
            settings_header.version.bits <= version.bits
        ) {
            // Read name
            SetFilePointer(settings_file, settings_header.name_address, 0, FILE_BEGIN);
            ReadFile(settings_file, &window->name, SETTINGS_NAME_SIZE * sizeof(wchar_t), &bytes_read, NULL);

            // Read language
            SetFilePointer(settings_file, settings_header.language_address, 0, FILE_BEGIN);
            ReadFile(settings_file, &window->language, sizeof(uint32_t), &bytes_read, NULL);

            // Read theme
            SetFilePointer(settings_file, settings_header.theme_address, 0, FILE_BEGIN);
            ReadFile(settings_file, &window->theme, sizeof(uint32_t), &bytes_read, NULL);

            // Read highscores
            SetFilePointer(settings_file, settings_header.highscores_address, 0, FILE_BEGIN);
            ReadFile(settings_file, &window->highscores_size, sizeof(uint32_t), &bytes_read, NULL);
            if (window->highscores_size > 0) {
                window->highscores = malloc(window->highscores_size * sizeof(HighScore));
                for (uint32_t i = 0; i < window->highscores_size; i++) {
                    ReadFile(settings_file, &window->highscores[i], sizeof(HighScore), &bytes_read, NULL);
                }
            } else {
                window->highscores = NULL;
            }
        }

        CloseHandle(settings_file);
    }
}

void __stdcall SaveSettings(HWND hwnd) {
    WindowData *window = GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    wchar_t settings_path[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, settings_path);
    wcscat(settings_path, settings_file);

    HANDLE settings_file = CreateFileW(settings_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // Write settings file header
    SettingsHeader settings_header;
    settings_header.signature = SETTINGS_SIGNATURE;
    settings_header.version.fields.major = APP_VERSION_MAJOR;
    settings_header.version.fields.minor = APP_VERSION_MINOR;
    settings_header.version.fields.patch = APP_VERSION_PATCH;
    settings_header.version.fields.zero = 0;
    settings_header.name_address = sizeof(SettingsHeader);
    settings_header.language_address = settings_header.name_address + SETTINGS_NAME_SIZE * sizeof(wchar_t);
    settings_header.theme_address = settings_header.language_address + sizeof(uint32_t);
    settings_header.highscores_address = settings_header.theme_address + sizeof(uint32_t);
    uint32_t bytes_written;
    WriteFile(settings_file, &settings_header, sizeof(SettingsHeader), &bytes_written, NULL);

    // Write name
    WriteFile(settings_file, &window->name, SETTINGS_NAME_SIZE * sizeof(wchar_t), &bytes_written, NULL);

    // Write language
    WriteFile(settings_file, &window->language, sizeof(uint32_t), &bytes_written, NULL);

    // Write theme
    WriteFile(settings_file, &window->theme, sizeof(uint32_t), &bytes_written, NULL);

    // Write high scores
    WriteFile(settings_file, &window->highscores_size, sizeof(uint32_t), &bytes_written, NULL);
    for (uint32_t i = 0; i < window->highscores_size; i++) {
        WriteFile(settings_file, &window->highscores[i], sizeof(HighScore), &bytes_written, NULL);
    }

    CloseHandle(settings_file);
}

void __stdcall InitBlueSquares(HWND hwnd, float speed) {
    WindowData *window = GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    window->blue_squares[0].width = 72;
    window->blue_squares[0].height = 72;
    window->blue_squares[0].x = 0;
    window->blue_squares[0].y = 0;
    window->blue_squares[0].vx = speed;
    window->blue_squares[0].vy = speed;

    window->blue_squares[1].width = 72;
    window->blue_squares[1].height = 96;
    window->blue_squares[1].x = VIEWPORT_WIDTH - window->blue_squares[1].width;
    window->blue_squares[1].y = 0;
    window->blue_squares[1].vx = -speed;
    window->blue_squares[1].vy = speed;

    window->blue_squares[2].width = 96;
    window->blue_squares[2].height = 72;
    window->blue_squares[2].x = 0;
    window->blue_squares[2].y = VIEWPORT_HEIGHT - window->blue_squares[2].height;
    window->blue_squares[2].vx = speed;
    window->blue_squares[2].vy = -speed;

    window->blue_squares[3].width = 96;
    window->blue_squares[3].height = 96;
    window->blue_squares[3].x = VIEWPORT_WIDTH - window->blue_squares[3].width;
    window->blue_squares[3].y = VIEWPORT_HEIGHT - window->blue_squares[3].height;
    window->blue_squares[3].vx = -speed;
    window->blue_squares[3].vy = -speed;
}

void __stdcall StartGame(HWND hwnd) {
    WindowData *window = GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    window->time = 0;
    window->level = 1;
    window->score = 0;

    window->is_dragging = false;
    window->red_square.width = 48;
    window->red_square.height = 48;
    window->red_square.x = (VIEWPORT_WIDTH - window->red_square.width) / 2;
    window->red_square.y = (VIEWPORT_HEIGHT - window->red_square.height) / 2;

    InitBlueSquares(hwnd, 1);
}

void __stdcall UpdateControlsTexts(HWND hwnd) {
    WindowData *window = GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    // Update control strings
    wchar_t *string_buffer;
    for (uint32_t i = 0; i < window->controls_size; i++) {
        Control *control = &window->controls[i];
        if (control->type == CONTROL_TYPE_BUTTON && control->string != 0) {
            LoadStringW(window->instance, control->string, (wchar_t *)&string_buffer, 0);
            SendMessageW(window->controls_handles[i], WM_SETTEXT, NULL, string_buffer);
        }
    }

    // Settings theme select
    HWND settings_theme_select = GetDlgItem(hwnd, SETTINGS_THEME_SELECT_ID);
    SendMessageW(settings_theme_select, CB_RESETCONTENT, NULL, NULL);
    LoadStringW(window->instance, SETTINGS_THEME_LIGHT_STRING_ID, (wchar_t *)&string_buffer, 0);
    SendMessageW(settings_theme_select, CB_ADDSTRING, NULL, string_buffer);
    LoadStringW(window->instance, SETTINGS_THEME_DARK_STRING_ID, (wchar_t *)&string_buffer, 0);
    SendMessageW(settings_theme_select, CB_ADDSTRING, NULL, string_buffer);
    if (window->theme == THEME_LIGHT) {
        SendMessageW(settings_theme_select, CB_SETCURSEL, (WPARAM)0, NULL);
    }
    if (window->theme == THEME_DARK) {
        SendMessageW(settings_theme_select, CB_SETCURSEL, (WPARAM)1, NULL);
    }
}

void __stdcall AddHighScore(HWND hwnd) {
    WindowData *window = GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    if (window->highscores == NULL) {
        window->highscores = malloc(sizeof(HighScore));
    } else {
        window->highscores = realloc(window->highscores, (window->highscores_size + 1) * sizeof(HighScore));
        for (uint32_t i = 0; i < window->highscores_size; i++) {
            HighScore *highscore = &window->highscores[i];
            if (window->score >= highscore->score) {
                // Move rest highscores one to right
                for (int32_t j = (int32_t)window->highscores_size; j > i; j--) {
                    for (int32_t k = 0; k < SETTINGS_NAME_SIZE; k++) window->highscores[j].name[k] = '\0';
                    wcscpy(window->highscores[j].name, window->highscores[j - 1].name);
                    window->highscores[j].score = window->highscores[j - 1].score;
                }

                // Replace this highscore
                for (int32_t j = 0; j < SETTINGS_NAME_SIZE; j++) highscore->name[j] = '\0';
                wcscpy(highscore->name, window->name);
                highscore->score = window->score;
                window->highscores_size++;
                return;
            }
        }
    }

    // Add highscore add the end
    HighScore *highscore = &window->highscores[window->highscores_size];
    for (int32_t i = 0; i < SETTINGS_NAME_SIZE; i++) highscore->name[i] = '\0';
    wcscpy(highscore->name, window->name);
    highscore->score = window->score;
    window->highscores_size++;
}

void __stdcall ChangePage(HWND hwnd, uint32_t page) {
    WindowData *window = GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    uint32_t old_page = window->page;
    window->page = page;

    // Update controls visibility
    for (uint32_t i = 0; i < window->controls_size; i++) {
        Control *control = &window->controls[i];
        if (control->type == CONTROL_TYPE_BUTTON || control->type == CONTROL_TYPE_EDIT || control->type == CONTROL_TYPE_COMBOBOX || control->type == CONTROL_TYPE_LIST) {
            ShowWindow(window->controls_handles[i], page == control->page ? SW_SHOW : SW_HIDE);
        }
    }

    if (page == PAGE_GAME) {
        StartGame(hwnd);
    }

    if (page == PAGE_GAMEOVER) {
        AddHighScore(hwnd);
        SaveSettings(hwnd);
        PlaySoundW((wchar_t *)GAMEOVER_WAVE_ID, window->instance, SND_RESOURCE | SND_ASYNC);
    }

    if (old_page == PAGE_GAMEOVER) {
        for (int32_t i = 0; i < 4; i++) {
            Square *square = &window->blue_squares[i];
            square->vx = square->vx > 0 ? 4 : -4;
            square->vy = square->vy > 0 ? 4 : -4;
        }
    }

    if (page == PAGE_HIGHSCORES) {
        HWND *highscores_list = GetDlgItem(hwnd, HIGHSCORES_LIST_ID);
        SendMessageW(highscores_list, LVM_DELETEALLITEMS, NULL, NULL);
        if (window->highscores_size > 0) {
            for (int32_t i = (int32_t)window->highscores_size - 1; i >= 0; i--) {
                HighScore *highscore = &window->highscores[i];
                LVITEMW item = {0};
                item.mask = LVIF_TEXT;
                wchar_t string_buffer[64];
                wsprintfW(string_buffer, L"%d. %s: %d", i + 1, highscore->name, highscore->score);
                item.pszText = string_buffer;
                SendMessageW(highscores_list, LVM_INSERTITEMW, NULL, &item);
            }
        } else {
            LVITEMW item = {0};
            item.mask = LVIF_TEXT;
            LoadStringW(window->instance, HIGHSCORES_EMPTY_STRING_ID, (wchar_t *)&item.pszText, 0);
            SendMessageW(highscores_list, LVM_INSERTITEMW, NULL, &item);
        }
    }

    if (page == PAGE_SETTINGS) {
        HWND settings_name_edit = GetDlgItem(hwnd, SETTINGS_NAME_EDIT_ID);
        SendMessageW(settings_name_edit, WM_SETTEXT, NULL, window->name);

        HWND settings_language_select = GetDlgItem(hwnd, SETTINGS_LANGUAGE_SELECT_ID);
        if (window->language == MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)) {
            SendMessageW(settings_language_select, CB_SETCURSEL, (WPARAM)0, NULL);
        }
        if (window->language == MAKELANGID(LANG_DUTCH, SUBLANG_DUTCH)) {
            SendMessageW(settings_language_select, CB_SETCURSEL, (WPARAM)1, NULL);
        }

        HWND settings_theme_select = GetDlgItem(hwnd, SETTINGS_THEME_SELECT_ID);
        if (window->theme == THEME_LIGHT) {
            SendMessageW(settings_theme_select, CB_SETCURSEL, (WPARAM)0, NULL);
        }
        if (window->theme == THEME_DARK) {
            SendMessageW(settings_theme_select, CB_SETCURSEL, (WPARAM)1, NULL);
        }
    }

    if (old_page == PAGE_SETTINGS) {
        HWND settings_name_edit = GetDlgItem(hwnd, SETTINGS_NAME_EDIT_ID);
        SendMessageW(settings_name_edit, WM_GETTEXT, (WPARAM)SETTINGS_NAME_SIZE, window->name);
        SaveSettings(hwnd);
    }

    InvalidateRect(hwnd, NULL, TRUE);
}

int32_t __stdcall WndProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam) {
    WindowData *window = GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    if (msg == WM_CREATE) {
        // Create window data
        window = malloc(sizeof(WindowData));
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, window);
        window->width = WINDOW_WIDTH;
        window->height = WINDOW_HEIGHT;
        window->instance = GetModuleHandleW(NULL);

        // Load background image resource
        window->paper_bitmap = LoadBitmapW(window->instance, (wchar_t *)PAPER_BITMAP_ID);
        window->paper_dark_bitmap = LoadBitmapW(window->instance, (wchar_t *)PAPER_DARK_BITMAP_ID);

        // Read layout
        HRSRC main_layout_resource = FindResourceW(window->instance, (wchar_t *)MAIN_LAYOUT_ID, (wchar_t *)RT_RCDATA);
        HGLOBAL main_layout = LoadResource(window->instance, main_layout_resource);
        window->controls = LockResource(main_layout);
        window->controls_size = SizeofResource(window->instance, main_layout_resource) / sizeof(Control);
        window->controls_handles = malloc(window->controls_size * sizeof(HWND));

        // Create layout controls
        for (uint32_t i = 0; i < window->controls_size; i++) {
            window->controls_handles[i] = NULL;
            Control *control = &window->controls[i];
            if (control->type == CONTROL_TYPE_BUTTON) {
                window->controls_handles[i] = CreateWindowExW(0, L"BUTTON", NULL, WS_CHILD | control->style, 0, 0, 0, 0, hwnd, (HMENU)(size_t)control->id, NULL, NULL);
            }
            if (control->type == CONTROL_TYPE_EDIT) {
                window->controls_handles[i] = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", NULL, WS_CHILD | control->style, 0, 0, 0, 0, hwnd, (HMENU)(size_t)control->id, NULL, NULL);
            }
            if (control->type == CONTROL_TYPE_COMBOBOX) {
                window->controls_handles[i] = CreateWindowExW(0, L"COMBOBOX", NULL, WS_CHILD | control->style, 0, 0, 0, 0, hwnd, (HMENU)(size_t)control->id, NULL, NULL);
            }
            if (control->type == CONTROL_TYPE_LIST) {
                window->controls_handles[i] = CreateWindowExW(WS_EX_CLIENTEDGE, L"SysListView32", NULL, WS_CHILD | control->style, 0, 0, 0, 0, hwnd, (HMENU)(size_t)control->id, NULL, NULL);
            }
        }

        // Settings page widgets
        HWND settings_name_edit = GetDlgItem(hwnd, SETTINGS_NAME_EDIT_ID);
        SendMessageW(settings_name_edit, EM_SETLIMITTEXT, (WPARAM)(SETTINGS_NAME_SIZE - 1), 0);

        HWND settings_language_select = GetDlgItem(hwnd, SETTINGS_LANGUAGE_SELECT_ID);
        SendMessageW(settings_language_select, CB_ADDSTRING, NULL, L"English");
        SendMessageW(settings_language_select, CB_ADDSTRING, NULL, L"Nederlands");

        // Center window
        RECT window_rect;
        GetClientRect(hwnd, &window_rect);
        uint32_t new_width = window->width * 2 - window_rect.right;
        uint32_t new_height = window->height * 2 - window_rect.bottom;
        SetWindowPos(hwnd, NULL, (GetSystemMetrics(SM_CXSCREEN) - new_width) / 2, (GetSystemMetrics(SM_CYSCREEN) - new_height) / 2, new_width, new_height, SWP_NOZORDER);

        // Load settings
        for (int32_t i = 0; i < SETTINGS_NAME_SIZE; i++) window->name[i] = '\0';
        uint32_t size = SETTINGS_NAME_SIZE;
        GetUserNameW(window->name, &size);
        window->language = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
        window->theme = THEME_LIGHT;
        window->highscores = NULL;
        window->highscores_size = 0;
        LoadSettings(hwnd);

        // Go to menu page
        InitBlueSquares(hwnd, 4);
        SetLanguage(window->language);
        UpdateControlsTexts(hwnd);
        ChangePage(hwnd, PAGE_MENU);

        // Start frame timer
        SetTimer(hwnd, FRAME_TIMER_ID, 1000 / FPS, NULL);
        return 0;
    }

    if (msg == WM_SIZE) {
        // Save new window size
        window->width = LOWORD(lParam);
        window->height = HIWORD(lParam);
        window->vw = (float)window->width / (float)VIEWPORT_WIDTH;
        window->vh = (float)window->height / (float)VIEWPORT_HEIGHT;
        window->vx = MIN(window->vw, window->vh);

        // Resize font controls
        for (uint32_t i = 0; i < window->controls_size; i++) {
            Control *control = &window->controls[i];
            if (control->type == CONTROL_TYPE_FONT) {
                float height = control->height;
                if (control->height_unit == CONTROL_UNIT_VW) height *= window->vw;
                if (control->height_unit == CONTROL_UNIT_VH) height *= window->vh;
                if (control->height_unit == CONTROL_UNIT_VX) height *= window->vx;

                wchar_t *font_name;
                LoadStringW(window->instance, control->string, (wchar_t *)&font_name, 0);

                if (window->controls_handles[i] != NULL) {
                    DeleteObject(window->controls_handles[i]);
                }
                window->controls_handles[i] = CreateFontW(height, 0, 0, 0, control->width, FALSE, FALSE, FALSE, ANSI_CHARSET,
                    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
            }
        }

        // Resize controls
        for (uint32_t i = 0; i < window->controls_size; i++) {
            Control *control = &window->controls[i];
            if (control->type == CONTROL_TYPE_BUTTON || control->type == CONTROL_TYPE_EDIT || control->type == CONTROL_TYPE_COMBOBOX || control->type == CONTROL_TYPE_LIST) {
                for (uint32_t j = 0; j < window->controls_size; j++) {
                    Control *other_control = &window->controls[j];
                    if (control->font == other_control->id) {
                        SendMessageW(window->controls_handles[i], WM_SETFONT, window->controls_handles[j], (LPARAM)TRUE);
                        break;
                    }
                }

                float x = control->x;
                if (control->x_unit == CONTROL_UNIT_VW) x *= window->vw;
                if (control->x_unit == CONTROL_UNIT_VH) x *= window->vh;
                if (control->x_unit == CONTROL_UNIT_VX) x *= window->vx;

                float y = control->y;
                if (control->y_unit == CONTROL_UNIT_VW) y *= window->vw;
                if (control->y_unit == CONTROL_UNIT_VH) y *= window->vh;
                if (control->y_unit == CONTROL_UNIT_VX) y *= window->vx;

                float width = control->width;
                if (control->width_unit == CONTROL_UNIT_VW) width *= window->vw;
                if (control->width_unit == CONTROL_UNIT_VH) width *= window->vh;
                if (control->width_unit == CONTROL_UNIT_VX) width *= window->vx;

                float height = control->height;
                if (control->height_unit == CONTROL_UNIT_VW) height *= window->vw;
                if (control->height_unit == CONTROL_UNIT_VH) height *= window->vh;
                if (control->height_unit == CONTROL_UNIT_VX) height *= window->vx;

                SetWindowPos(window->controls_handles[i], NULL, x, y, width, height, SWP_NOZORDER);
            }
        }

        return 0;
    }

    if (msg == WM_COMMAND) {
        uint16_t id = LOWORD(wParam);
        uint16_t notification = HIWORD(wParam);

        // Menu page widgets
        if (id == MENU_PLAY_BUTTON_ID) {
            ChangePage(hwnd, PAGE_GAME);
        }
        if (id == MENU_HIGHSCORES_BUTTON_ID) {
            ChangePage(hwnd, PAGE_HIGHSCORES);
        }
        if (id == MENU_HELP_BUTTON_ID) {
            ChangePage(hwnd, PAGE_HELP);
        }
        if (id == MENU_SETTINGS_BUTTON_ID) {
            ChangePage(hwnd, PAGE_SETTINGS);
        }
        if (id == MENU_EXIT_BUTTON_ID) {
            DestroyWindow(hwnd);
        }

        // Gameover page widgets
        if (id == GAMEOVER_BACK_BUTTON_ID) {
            ChangePage(hwnd, PAGE_MENU);
        }

        // High scores page widgets
        if (id == HIGHSCORES_BACK_BUTTON_ID) {
            ChangePage(hwnd, PAGE_MENU);
        }

        // Help page widgets
        if (id == HELP_BACK_BUTTON_ID) {
            ChangePage(hwnd, PAGE_MENU);
        }

        // Settings page widgets
        if (id == SETTINGS_LANGUAGE_SELECT_ID && notification == CBN_SELCHANGE) {
            uint32_t old_language = window->language;
            HWND settings_language_select = GetDlgItem(hwnd, SETTINGS_LANGUAGE_SELECT_ID);
            int32_t selected = SendMessageW(settings_language_select, CB_GETCURSEL, NULL, NULL);
            if (selected == 0) window->language = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
            if (selected == 1) window->language = MAKELANGID(LANG_DUTCH, SUBLANG_DUTCH);
            if (old_language != window->language) {
                SetLanguage(window->language);
                UpdateControlsTexts(hwnd);
                SaveSettings(hwnd);
            }
        }

        if (id == SETTINGS_THEME_SELECT_ID && notification == CBN_SELCHANGE) {
            uint32_t old_theme = window->theme;
            HWND settings_theme_select = GetDlgItem(hwnd, SETTINGS_THEME_SELECT_ID);
            int32_t selected = SendMessageW(settings_theme_select, CB_GETCURSEL, NULL, NULL);
            if (selected == 0) window->theme = THEME_LIGHT;
            if (selected == 1) window->theme = THEME_DARK;
            if (old_theme != window->theme) {
                SaveSettings(hwnd);
            }
        }

        if (id == SETTINGS_BACK_BUTTON_ID) {
            ChangePage(hwnd, PAGE_MENU);
        }
    }

    if (msg == WM_LBUTTONDOWN) {
        if (window->page == PAGE_GAME) {
            float x = LOWORD(lParam) / window->vw;
            float y = HIWORD(lParam) / window->vh;

            Square *square = &window->red_square;
            if (
                x >= square->x &&
                y >= square->y &&
                x < square->x + square->width &&
                y < square->y + square->height
            ) {
                window->is_dragging = true;
                window->red_square.vx = x - square->x;
                window->red_square.vy = y - square->y;
            }
        }
    }

    if (msg == WM_MOUSEMOVE) {
        if (window->page == PAGE_GAME) {
            float x = LOWORD(lParam) / window->vw;
            float y = HIWORD(lParam) / window->vh;

            if (window->is_dragging) {
                window->red_square.x = x - window->red_square.vx;
                window->red_square.y = y - window->red_square.vy;
            }
        }
    }

    if (msg == WM_LBUTTONUP) {
        if (window->page == PAGE_MENU) {
            float x = LOWORD(lParam) / window->vw;
            float y = HIWORD(lParam) / window->vh;

            if (y > 440) {
                ShellExecuteW(hwnd, L"open", L"https://bastiaan.ml/", NULL, NULL, SW_SHOWNORMAL);
            }
        }

        if (window->page == PAGE_GAME) {
            window->is_dragging = false;
        }
    }

    if (msg == WM_GETMINMAXINFO) {
        // Set window min size
        MINMAXINFO *minMaxInfo = (MINMAXINFO *)lParam;
        minMaxInfo->ptMinTrackSize.x = VIEWPORT_WIDTH;
        minMaxInfo->ptMinTrackSize.y = VIEWPORT_HEIGHT;
        return 0;
    }

    if (msg == WM_TIMER) {
        if ((uintptr_t)wParam == FRAME_TIMER_ID) {
            bool is_leveled = false;
            bool is_gameover = false;
            if (window->page == PAGE_GAME) {
                window->score += window->level;
                window->time++;

                if ((window->time % (10 * FPS)) == 0) {
                    window->level++;
                    is_leveled = true;
                }

                Square *red_square = &window->red_square;
                if (
                    red_square->x * window->vw < 16 * window->vx ||
                    red_square->y * window->vh < 52 * window->vx ||
                    (red_square->x + red_square->width) * window->vw > window->width - 16 * window->vx ||
                    (red_square->y + red_square->height) * window->vh > window->height - 16 * window->vx
                ) {
                    is_gameover = true;
                }
            }

            if (
                window->page == PAGE_MENU || window->page == PAGE_GAME || window->page == PAGE_HIGHSCORES ||
                window->page == PAGE_HELP || window->page == PAGE_SETTINGS
            ) {
                Square *red_square = &window->red_square;
                for (int32_t i = 0; i < 4; i++) {
                    Square *square = &window->blue_squares[i];
                    square->x += square->vx;
                    square->y += square->vy;

                    if (square->x < 0 || square->x + square->width > VIEWPORT_WIDTH) {
                        square->vx = -square->vx;
                    }
                    if (square->y < 0 || square->y + square->height > VIEWPORT_HEIGHT) {
                        square->vy = -square->vy;
                    }

                    if (window->page == PAGE_GAME) {
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

                if (window->page == PAGE_GAME && is_gameover) {
                    ChangePage(hwnd, PAGE_GAMEOVER);
                } else {
                    InvalidateRect(hwnd, NULL, TRUE);
                }
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
        HBITMAP bitmap_buffer = CreateCompatibleBitmap(hdc, window->width, window->height);
        SelectObject(hdc_buffer, bitmap_buffer);

        // Draw background image
        if (window->paper_bitmap != NULL) {
            HDC hdc_bitmap_buffer = CreateCompatibleDC(hdc_buffer);
            SelectObject(hdc_bitmap_buffer, window->theme == THEME_DARK ? window->paper_dark_bitmap : window->paper_bitmap);
            uint32_t cols = window->width / PAPER_BITMAP_SIZE + 1;
            uint32_t rows = window->height / PAPER_BITMAP_SIZE + 1;
            for (int32_t y = 0; y <= rows; y ++) {
                for (int32_t x = 0; x <= cols; x ++) {
                    BitBlt(hdc_buffer, (window->width / 2) + (x - cols / 2 - 1) * PAPER_BITMAP_SIZE, (window->height / 2) + (y - rows / 2 - 1) * PAPER_BITMAP_SIZE,
                        PAPER_BITMAP_SIZE, PAPER_BITMAP_SIZE, hdc_bitmap_buffer, 0, 0, SRCCOPY);
                }
            }
            DeleteDC(hdc_bitmap_buffer);
        }

        // Create graphics object
        GpGraphics *graphics;
        GdipCreateFromHDC(hdc_buffer, &graphics);
        GdipSetSmoothingMode(graphics, SmoothingModeAntiAlias);

        if (window->paper_bitmap == NULL) {
            GdipGraphicsClear(graphics, window->theme == THEME_DARK ? 0xff222222 : 0xffffffff);
        }

        // Setup text drawing
        SetBkMode(hdc_buffer, TRANSPARENT);
        SetTextColor(hdc_buffer, window->theme == THEME_DARK ? 0x00ffffff : 0x00111111);

        // Draw border
        if (window->page == PAGE_GAME || window->page == PAGE_GAMEOVER) {
            GpBrush *brush;
            GdipCreateSolidFill(window->theme == THEME_DARK ? 0x33ffffff : 0x33000000, (GpSolidFill **)&brush);
            GdipFillRectangle(graphics, brush, 16 * window->vx, 52 * window->vx, window->width - 32 * window->vx, window->height - 68 * window->vx);
            GdipDeleteBrush(brush);
        }

        // Draw blue squares
        if (
            window->page == PAGE_MENU || window->page == PAGE_GAME || window->page == PAGE_GAMEOVER ||
            window->page == PAGE_HIGHSCORES || window->page == PAGE_HELP || window->page == PAGE_SETTINGS
        ) {
            GpBrush *brush;
            GdipCreateSolidFill(window->page == PAGE_GAME || window->page == PAGE_GAMEOVER ? 0xaa0000ff : 0x550000ff, (GpSolidFill **)&brush);
            for (int32_t i = 0; i < 4; i++) {
                Square *square = &window->blue_squares[i];
                GdipFillRectangle(graphics, brush, square->x * window->vw, square->y * window->vh,
                    square->width * window->vw, square->height * window->vh);
            }
            GdipDeleteBrush(brush);
        }

        // Draw red square
        if (window->page == PAGE_GAME || window->page == PAGE_GAMEOVER) {
            GpBrush *brush;
            GdipCreateSolidFill(0xaaff0000, (GpSolidFill **)&brush);
            Square *square = &window->red_square;
            GdipFillRectangle(graphics, brush, square->x * window->vw, square->y * window->vh,
                square->width * window->vw, square->height * window->vh);
            GdipDeleteBrush(brush);
        }

        // Page game
        if (window->page == PAGE_GAME || window->page == PAGE_GAMEOVER) {
            // Draw game stats
            wchar_t *font_name;
            LoadStringW(window->instance, FONT_STRING_ID, (wchar_t *)&font_name, 0);
            HFONT stats_font = CreateFontW(20 * window->vx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
            SelectObject(hdc_buffer, stats_font);

            wchar_t *format_buffer;
            wchar_t string_buffer[64];
            LoadStringW(window->instance, GAME_SCORE_STRING_ID, (wchar_t *)&format_buffer, 0);
            wsprintfW(string_buffer, format_buffer, window->score);
            SetTextAlign(hdc_buffer, TA_LEFT);
            TextOutW(hdc_buffer, 16 * window->vx, 16 * window->vx, string_buffer, wcslen(string_buffer));

            LoadStringW(window->instance, GAME_TIME_STRIND_ID, (wchar_t *)&format_buffer, 0);
            uint32_t seconds = window->time / FPS;
            wsprintfW(string_buffer, format_buffer, seconds / 60, seconds % 60);
            SetTextAlign(hdc_buffer, TA_CENTER);
            TextOutW(hdc_buffer, window->width / 2, 16 * window->vx, string_buffer, wcslen(string_buffer));

            LoadStringW(window->instance, GAME_LEVEL_STRING_ID, (wchar_t *)&format_buffer, 0);
            wsprintfW(string_buffer, format_buffer, window->level);
            SetTextAlign(hdc_buffer, TA_RIGHT);
            TextOutW(hdc_buffer, window->width - 16 * window->vx, 16 * window->vx, string_buffer, wcslen(string_buffer));

            DeleteObject(stats_font);
        }

        // Page gameover
        if (window->page == PAGE_GAMEOVER) {
            // Draw overlay
            GpBrush *brush;
            GdipCreateSolidFill(window->theme == THEME_DARK ? 0x88000000 : 0x88ffffff, (GpSolidFill **)&brush);
            GdipFillRectangleI(graphics, brush, 0, 0, window->width, window->height);
            GdipDeleteBrush(brush);

            // Draw stats lines
            wchar_t *font_name;
            LoadStringW(window->instance, FONT_STRING_ID, (wchar_t *)&font_name, 0);
            HFONT text_font = CreateFontW(24 * window->vx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
            SelectObject(hdc_buffer, text_font);
            SetTextAlign(hdc_buffer, TA_CENTER);

            wchar_t *format_buffer;
            wchar_t string_buffer[64];
            LoadStringW(window->instance, GAME_SCORE_STRING_ID, (wchar_t *)&format_buffer, 0);
            wsprintfW(string_buffer, format_buffer, window->score);
            float y = ((480 - (48 + 16 + (24 + 16) * 3 + 52)) / 2) * window->vh;
            y += 48 * window->vh + 16 * window->vh;
            TextOutW(hdc_buffer, window->width / 2, y, string_buffer, wcslen(string_buffer));
            y += 24 * window->vh + 16 * window->vh;

            LoadStringW(window->instance, GAME_TIME_STRIND_ID, (wchar_t *)&format_buffer, 0);
            uint32_t seconds = window->time / FPS;
            wsprintfW(string_buffer, format_buffer, seconds / 60, seconds % 60);
            TextOutW(hdc_buffer, window->width / 2, y, string_buffer, wcslen(string_buffer));
            y += 24 * window->vh + 16 * window->vh;

            LoadStringW(window->instance, GAME_LEVEL_STRING_ID, (wchar_t *)&format_buffer, 0);
            wsprintfW(string_buffer, format_buffer, window->level);
            TextOutW(hdc_buffer, window->width / 2, y, string_buffer, wcslen(string_buffer));

            DeleteObject(text_font);
        }

        // Draw controls
        for (uint32_t i = 0; i < window->controls_size; i++) {
            Control *control = &window->controls[i];
            if (control->type == CONTROL_TYPE_LABEL && control->page == window->page) {
                for (uint32_t j = 0; j < window->controls_size; j++) {
                    Control *other_control = &window->controls[j];
                    if (control->font == other_control->id) {
                        SelectObject(hdc_buffer, window->controls_handles[j]);
                        break;
                    }
                }

                float x = control->x;
                if (control->x_unit == CONTROL_UNIT_VW) x *= window->vw;
                if (control->x_unit == CONTROL_UNIT_VH) x *= window->vh;
                if (control->x_unit == CONTROL_UNIT_VX) x *= window->vx;

                float y = control->y;
                if (control->y_unit == CONTROL_UNIT_VW) y *= window->vw;
                if (control->y_unit == CONTROL_UNIT_VH) y *= window->vh;
                if (control->y_unit == CONTROL_UNIT_VX) y *= window->vx;

                float width = control->width;
                if (control->width_unit == CONTROL_UNIT_VW) width *= window->vw;
                if (control->width_unit == CONTROL_UNIT_VH) width *= window->vh;
                if (control->width_unit == CONTROL_UNIT_VX) width *= window->vx;

                float height = control->height;
                if (control->height_unit == CONTROL_UNIT_VW) height *= window->vw;
                if (control->height_unit == CONTROL_UNIT_VH) height *= window->vh;
                if (control->height_unit == CONTROL_UNIT_VX) height *= window->vx;

                wchar_t *string;
                LoadStringW(window->instance, control->string, (wchar_t *)&string, 0);

                if ((control->style & ES_MULTILINE) != 0) {
                    RECT rect = { x, y, x + width, y + height };
                    SetTextAlign(hdc_buffer, TA_LEFT);
                    DrawTextW(hdc_buffer, string, -1, &rect, control->style & 3);
                } else {
                    if ((control->style & ES_LEFT) != 0) {
                        SetTextAlign(hdc_buffer, TA_LEFT);
                    }
                    if ((control->style & ES_CENTER) != 0) {
                        x += width / 2;
                        SetTextAlign(hdc_buffer, TA_CENTER);
                    }
                    if ((control->style & ES_RIGHT) != 0) {
                        x += width;
                        SetTextAlign(hdc_buffer, TA_RIGHT);
                    }
                    TextOutW(hdc_buffer, x, y, string, wcslen(string));
                }
            }
        }

        // Delete GDI+ graphics object
        GdipDeleteGraphics(graphics);

        // Draw and delete back buffer
        BitBlt(hdc, 0, 0, window->width, window->height, hdc_buffer, 0, 0, SRCCOPY);
        DeleteObject(bitmap_buffer);
        DeleteDC(hdc_buffer);

        EndPaint(hwnd, &paint_struct);
        return 0;
    }

    if (msg == WM_DESTROY) {
        // Free window data
        DeleteObject(window->paper_bitmap);
        DeleteObject(window->paper_dark_bitmap);
        for (uint32_t i = 0; i < window->controls_size; i++) {
            Control *control = &window->controls[i];
            if (control->type == CONTROL_TYPE_FONT && window->controls_handles[i] != NULL) {
                DeleteObject(window->controls_handles[i]);
            }
        }
        free(window->controls_handles);
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

    uint32_t gdiplusToken;
    GdiplusStartupInput gdiplusStartupInput;
    gdiplusStartupInput.GdiplusVersion = 1;
    gdiplusStartupInput.DebugEventCallback = NULL;
    gdiplusStartupInput.SuppressBackgroundThread = FALSE;
    gdiplusStartupInput.SuppressExternalCodecs = FALSE;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.hIcon = LoadImageW(wc.hInstance, (wchar_t *)APP_ICON_ID, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_SHARED);
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.lpszClassName = window_class_name;
    wc.hIconSm = LoadImageW(wc.hInstance, (wchar_t *)APP_ICON_ID, IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR | LR_SHARED);
    RegisterClassExW(&wc);

    wchar_t *window_title;
    LoadStringW(wc.hInstance, MENU_TITLE_STRING_ID, (wchar_t *)&window_title, 0);
    HWND hwnd = CreateWindowExW(0, window_class_name, window_title,
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, wc.hInstance, NULL);
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    MSG message;
    while (GetMessageW(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    GdiplusShutdown(&gdiplusToken);

    ExitProcess((int32_t)(uintptr_t)message.wParam);
}
