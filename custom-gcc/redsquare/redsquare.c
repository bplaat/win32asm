#define WIN32_MALLOC
#define WIN32_REALLOC
#define WIN32_FREE
#define WIN32_STRLEN
#define WIN32_STRCPY
#define WIN32_STRCAT
#include "win32.h"
#include "redsquare.h"

#define FRAME_TIMER_ID 1

#define FPS 50

#define THEME_LIGHT 0
#define THEME_DARK 1

char *window_class_name = "redsquare";
char *settings_file = "\\redsquare-settings.bin";

#define VIEWPORT_WIDTH 640
#define VIEWPORT_HEIGHT 480
uint32_t window_width = 800;
uint32_t window_height = 600;
float vw, vh, vx;
HINSTANCE instance;

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
    char name[SETTINGS_NAME_SIZE];
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
    HBITMAP paper_bitmap;
    HBITMAP paper_dark_bitmap;
    uint32_t page;
    Control *controls;
    HANDLE *controls_handles;
    uint32_t controls_size;

    char name[SETTINGS_NAME_SIZE];
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
    OSVERSIONINFOA osver = {0};
    osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
    GetVersionExA(&osver);
    return osver.dwMajorVersion >= 6;
}

typedef uint32_t (__stdcall *_SetThreadUILanguage)(uint32_t LangId);

void __stdcall SetLanguage(uint32_t language) {
    if (IsVistaOrHigher()) {
        HMODULE kernel32 = LoadLibraryA("kernel32.dll");
        _SetThreadUILanguage SetThreadUILanguage = GetProcAddress(kernel32, "SetThreadUILanguage");
        SetThreadUILanguage(language);
    } else {
        SetThreadLocale(language);
    }
}

void __stdcall LoadSettings(HWND hwnd) {
    WindowData *window_data = GetWindowLongPtrA(hwnd, GWLP_USERDATA);

    char settings_path[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, settings_path);
    strcat(settings_path, settings_file);

    HANDLE settings_file = CreateFileA(settings_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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
            ReadFile(settings_file, &window_data->name, SETTINGS_NAME_SIZE, &bytes_read, NULL);

            // Read language
            SetFilePointer(settings_file, settings_header.language_address, 0, FILE_BEGIN);
            ReadFile(settings_file, &window_data->language, sizeof(uint32_t), &bytes_read, NULL);

            // Read theme
            SetFilePointer(settings_file, settings_header.theme_address, 0, FILE_BEGIN);
            ReadFile(settings_file, &window_data->theme, sizeof(uint32_t), &bytes_read, NULL);

            // Read highscores
            SetFilePointer(settings_file, settings_header.highscores_address, 0, FILE_BEGIN);
            ReadFile(settings_file, &window_data->highscores_size, sizeof(uint32_t), &bytes_read, NULL);
            if (window_data->highscores_size > 0) {
                window_data->highscores = malloc(window_data->highscores_size * sizeof(HighScore));
                for (uint32_t i = 0; i < window_data->highscores_size; i++) {
                    ReadFile(settings_file, &window_data->highscores[i], sizeof(HighScore), &bytes_read, NULL);
                }
            } else {
                window_data->highscores = NULL;
            }
        }

        CloseHandle(settings_file);
    }
}

void __stdcall SaveSettings(HWND hwnd) {
    WindowData *window_data = GetWindowLongPtrA(hwnd, GWLP_USERDATA);

    char settings_path[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, settings_path);
    strcat(settings_path, settings_file);

    HANDLE settings_file = CreateFileA(settings_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // Write settings file header
    SettingsHeader settings_header;
    settings_header.signature = SETTINGS_SIGNATURE;
    settings_header.version.fields.major = APP_VERSION_MAJOR;
    settings_header.version.fields.minor = APP_VERSION_MINOR;
    settings_header.version.fields.patch = APP_VERSION_PATCH;
    settings_header.version.fields.zero = 0;
    settings_header.name_address = sizeof(SettingsHeader);
    settings_header.language_address = settings_header.name_address + SETTINGS_NAME_SIZE;
    settings_header.theme_address = settings_header.language_address + sizeof(uint32_t);
    settings_header.highscores_address = settings_header.theme_address + sizeof(uint32_t);
    uint32_t bytes_written;
    WriteFile(settings_file, &settings_header, sizeof(SettingsHeader), &bytes_written, NULL);

    // Write name
    WriteFile(settings_file, &window_data->name, SETTINGS_NAME_SIZE, &bytes_written, NULL);

    // Write language
    WriteFile(settings_file, &window_data->language, sizeof(uint32_t), &bytes_written, NULL);

    // Write theme
    WriteFile(settings_file, &window_data->theme, sizeof(uint32_t), &bytes_written, NULL);

    // Write high scores
    WriteFile(settings_file, &window_data->highscores_size, sizeof(uint32_t), &bytes_written, NULL);
    for (uint32_t i = 0; i < window_data->highscores_size; i++) {
        WriteFile(settings_file, &window_data->highscores[i], sizeof(HighScore), &bytes_written, NULL);
    }

    CloseHandle(settings_file);
}

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
    window_data->blue_squares[1].x = VIEWPORT_WIDTH - window_data->blue_squares[1].width;
    window_data->blue_squares[1].y = 0;
    window_data->blue_squares[1].vx = -speed;
    window_data->blue_squares[1].vy = speed;

    window_data->blue_squares[2].width = 96;
    window_data->blue_squares[2].height = 72;
    window_data->blue_squares[2].x = 0;
    window_data->blue_squares[2].y = VIEWPORT_HEIGHT - window_data->blue_squares[2].height;
    window_data->blue_squares[2].vx = speed;
    window_data->blue_squares[2].vy = -speed;

    window_data->blue_squares[3].width = 96;
    window_data->blue_squares[3].height = 96;
    window_data->blue_squares[3].x = VIEWPORT_WIDTH - window_data->blue_squares[3].width;
    window_data->blue_squares[3].y = VIEWPORT_HEIGHT - window_data->blue_squares[3].height;
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
    window_data->red_square.x = (VIEWPORT_WIDTH - window_data->red_square.width) / 2;
    window_data->red_square.y = (VIEWPORT_HEIGHT - window_data->red_square.height) / 2;

    InitBlueSquares(hwnd, 1);
}

int32_t __cdecl SortHighScores(const void *a, const void *b) {
    return ((HighScore *)a)->score - ((HighScore *)b)->score;
}

void __stdcall UpdateControlsTexts(HWND hwnd) {
    WindowData *window_data = GetWindowLongPtrA(hwnd, GWLP_USERDATA);

    // Update control strings
    char string_buffer[64];
    for (uint32_t i = 0; i < window_data->controls_size; i++) {
        Control *control = &window_data->controls[i];
        if (control->type == CONTROL_TYPE_BUTTON && control->string != 0) {
            LoadStringA(instance, control->string, string_buffer, sizeof(string_buffer));
            SetWindowTextA(window_data->controls_handles[i], string_buffer);
        }
    }

    // Settings theme select
    HWND settings_theme_select = GetDlgItem(hwnd, SETTINGS_THEME_SELECT_ID);
    SendMessageA(settings_theme_select, CB_RESETCONTENT, NULL, NULL);
    LoadStringA(instance, SETTINGS_THEME_LIGHT_STRING_ID, string_buffer, sizeof(string_buffer));
    SendMessageA(settings_theme_select, CB_ADDSTRING, NULL, string_buffer);
    LoadStringA(instance, SETTINGS_THEME_DARK_STRING_ID, string_buffer, sizeof(string_buffer));
    SendMessageA(settings_theme_select, CB_ADDSTRING, NULL, string_buffer);
    if (window_data->theme == THEME_LIGHT) {
        SendMessageA(settings_theme_select, CB_SETCURSEL, (WPARAM)0, NULL);
    }
    if (window_data->theme == THEME_DARK) {
        SendMessageA(settings_theme_select, CB_SETCURSEL, (WPARAM)1, NULL);
    }
}

void __stdcall ChangePage(HWND hwnd, uint32_t page) {
    WindowData *window_data = GetWindowLongPtrA(hwnd, GWLP_USERDATA);
    uint32_t old_page = window_data->page;
    window_data->page = page;

    // Update controls visibility
    for (uint32_t i = 0; i < window_data->controls_size; i++) {
        Control *control = &window_data->controls[i];
        if (control->type == CONTROL_TYPE_BUTTON || control->type == CONTROL_TYPE_EDIT || control->type == CONTROL_TYPE_COMBOBOX || control->type == CONTROL_TYPE_LIST) {
            ShowWindow(window_data->controls_handles[i], page == control->page ? SW_SHOW : SW_HIDE);
        }
    }

    if (page == PAGE_GAME) {
        StartGame(hwnd);
    }

    if (page == PAGE_GAMEOVER) {
        if (window_data->highscores == NULL) {
            window_data->highscores = malloc(sizeof(HighScore));
        } else {
            window_data->highscores = realloc(window_data->highscores, (window_data->highscores_size + 1) * sizeof(HighScore));
        }
        HighScore *highscore = &window_data->highscores[window_data->highscores_size];
        for (int32_t i = 0; i < SETTINGS_NAME_SIZE; i++) highscore->name[i] = '\0';
        strcpy(highscore->name, window_data->name);
        highscore->score = window_data->score;
        window_data->highscores_size++;

        qsort(window_data->highscores, window_data->highscores_size, sizeof(HighScore), SortHighScores);

        SaveSettings(hwnd);

        PlaySoundA((char *)GAMEOVER_WAVE_ID, instance, SND_RESOURCE | SND_ASYNC);
    }

    if (old_page == PAGE_GAMEOVER) {
        for (int32_t i = 0; i < 4; i++) {
            Square *square = &window_data->blue_squares[i];
            square->vx = square->vx > 0 ? 4 : -4;
            square->vy = square->vy > 0 ? 4 : -4;
        }
    }

    if (page == PAGE_HIGHSCORES) {
        HWND *highscores_list = GetDlgItem(hwnd, HIGHSCORES_LIST_ID);
        SendMessageA(highscores_list, LVM_DELETEALLITEMS, NULL, NULL);
        if (window_data->highscores_size > 0) {
            for (uint32_t i = 0; i < window_data->highscores_size; i++) {
                HighScore *highscore = &window_data->highscores[i];
                LVITEMA item = {0};
                item.mask = LVIF_TEXT;
                char string_buffer[64];
                wsprintfA(string_buffer, "%s: %d", highscore->name, highscore->score);
                item.pszText = string_buffer;
                SendMessageA(highscores_list, LVM_INSERTITEMA, NULL, &item);
            }
        } else {
            LVITEMA item = {0};
            item.mask = LVIF_TEXT;
            char string_buffer[64];
            LoadStringA(instance, HIGHSCORES_EMPTY_STRING_ID, string_buffer, sizeof(string_buffer));
            item.pszText = string_buffer;
            SendMessageA(highscores_list, LVM_INSERTITEMA, NULL, &item);
        }
    }

    if (page == PAGE_SETTINGS) {
        HWND settings_name_edit = GetDlgItem(hwnd, SETTINGS_NAME_EDIT_ID);
        SetWindowTextA(settings_name_edit, window_data->name);

        HWND settings_language_select = GetDlgItem(hwnd, SETTINGS_LANGUAGE_SELECT_ID);
        if (window_data->language == MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)) {
            SendMessageA(settings_language_select, CB_SETCURSEL, (WPARAM)0, NULL);
        }
        if (window_data->language == MAKELANGID(LANG_DUTCH, SUBLANG_DUTCH)) {
            SendMessageA(settings_language_select, CB_SETCURSEL, (WPARAM)1, NULL);
        }

        HWND settings_theme_select = GetDlgItem(hwnd, SETTINGS_THEME_SELECT_ID);
        if (window_data->theme == THEME_LIGHT) {
            SendMessageA(settings_theme_select, CB_SETCURSEL, (WPARAM)0, NULL);
        }
        if (window_data->theme == THEME_DARK) {
            SendMessageA(settings_theme_select, CB_SETCURSEL, (WPARAM)1, NULL);
        }
    }

    if (old_page == PAGE_SETTINGS) {
        HWND settings_name_edit = GetDlgItem(hwnd, SETTINGS_NAME_EDIT_ID);
        GetWindowTextA(settings_name_edit, window_data->name, SETTINGS_NAME_SIZE);
        SaveSettings(hwnd);
    }

    InvalidateRect(hwnd, NULL, TRUE);
}

int32_t __stdcall WndProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam) {
    WindowData *window_data = GetWindowLongPtrA(hwnd, GWLP_USERDATA);

    if (msg == WM_CREATE) {
        // Create window data
        window_data = malloc(sizeof(WindowData));
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, window_data);

        // Load background image resource
        window_data->paper_bitmap = LoadImageA(instance, (char *)PAPER_BITMAP_ID, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
        window_data->paper_dark_bitmap = LoadImageA(instance, (char *)PAPER_DARK_BITMAP_ID, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);

        // Read layout
        HRSRC main_layout_resource = FindResourceA(instance, (char *)MAIN_LAYOUT_ID, (char *)RT_RCDATA);
        HGLOBAL main_layout = LoadResource(instance, main_layout_resource);
        window_data->controls = LockResource(main_layout);
        window_data->controls_size = SizeofResource(instance, main_layout_resource) / sizeof(Control);
        window_data->controls_handles = malloc(window_data->controls_size * sizeof(HWND));

        // Create layout controls
        for (uint32_t i = 0; i < window_data->controls_size; i++) {
            window_data->controls_handles[i] = NULL;
            Control *control = &window_data->controls[i];
            if (control->type == CONTROL_TYPE_BUTTON) {
                window_data->controls_handles[i] = CreateWindowExA(0, "BUTTON", NULL, WS_CHILD | control->style, 0, 0, 0, 0, hwnd, (HMENU)(size_t)control->id, NULL, NULL);
            }
            if (control->type == CONTROL_TYPE_EDIT) {
                window_data->controls_handles[i] = CreateWindowExA(0, "EDIT", NULL, WS_CHILD | control->style, 0, 0, 0, 0, hwnd, (HMENU)(size_t)control->id, NULL, NULL);
            }
            if (control->type == CONTROL_TYPE_COMBOBOX) {
                window_data->controls_handles[i] = CreateWindowExA(0, "COMBOBOX", NULL, WS_CHILD | control->style, 0, 0, 0, 0, hwnd, (HMENU)(size_t)control->id, NULL, NULL);
            }
            if (control->type == CONTROL_TYPE_LIST) {
                window_data->controls_handles[i] = CreateWindowExA(0, "SysListView32", NULL, WS_CHILD | control->style, 0, 0, 0, 0, hwnd, (HMENU)(size_t)control->id, NULL, NULL);
            }
        }

        // Settings page widgets
        HWND settings_name_edit = GetDlgItem(hwnd, SETTINGS_NAME_EDIT_ID);
        SendMessageA(settings_name_edit, EM_SETLIMITTEXT, (WPARAM)(SETTINGS_NAME_SIZE - 1), 0);

        HWND settings_language_select = GetDlgItem(hwnd, SETTINGS_LANGUAGE_SELECT_ID);
        SendMessageA(settings_language_select, CB_ADDSTRING, NULL, "English");
        SendMessageA(settings_language_select, CB_ADDSTRING, NULL, "Nederlands");

        // Center window
        RECT window_rect;
        GetClientRect(hwnd, &window_rect);
        uint32_t new_width = window_width * 2 - window_rect.right;
        uint32_t new_height = window_height * 2 - window_rect.bottom;
        SetWindowPos(hwnd, NULL, (GetSystemMetrics(SM_CXSCREEN) - new_width) / 2, (GetSystemMetrics(SM_CYSCREEN) - new_height) / 2, new_width, new_height, SWP_NOZORDER);

        // Load settings
        for (int32_t i = 0; i < SETTINGS_NAME_SIZE; i++) window_data->name[i] = '\0';
        uint32_t size = SETTINGS_NAME_SIZE;
        GetUserNameA(window_data->name, &size);
        window_data->language = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
        window_data->theme = THEME_LIGHT;
        window_data->highscores = NULL;
        window_data->highscores_size = 0;
        LoadSettings(hwnd);

        // Go to menu page
        InitBlueSquares(hwnd, 4);
        SetLanguage(window_data->language);
        UpdateControlsTexts(hwnd);
        ChangePage(hwnd, PAGE_MENU);

        // Start frame timer
        SetTimer(hwnd, FRAME_TIMER_ID, 1000 / FPS, NULL);
        return 0;
    }

    if (msg == WM_SIZE) {
        // Save new window size
        window_width = LOWORD(lParam);
        window_height = HIWORD(lParam);
        vw = (float)window_width / (float)VIEWPORT_WIDTH;
        vh = (float)window_height / (float)VIEWPORT_HEIGHT;
        vx = MIN(vw, vh);

        // Resize font controls
        for (uint32_t i = 0; i < window_data->controls_size; i++) {
            Control *control = &window_data->controls[i];
            if (control->type == CONTROL_TYPE_FONT) {
                float height = control->height;
                if (control->height_unit == CONTROL_UNIT_VW) height *= vw;
                if (control->height_unit == CONTROL_UNIT_VH) height *= vh;
                if (control->height_unit == CONTROL_UNIT_VX) height *= vx;

                char string_buffer[64];
                LoadStringA(instance, control->string, string_buffer, sizeof(string_buffer));

                if (window_data->controls_handles[i] != NULL) {
                    DeleteObject(window_data->controls_handles[i]);
                }
                window_data->controls_handles[i] = CreateFontA(height, 0, 0, 0, control->width, FALSE, FALSE, FALSE, ANSI_CHARSET,
                    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, string_buffer);
            }
        }

        // Resize controls
        for (uint32_t i = 0; i < window_data->controls_size; i++) {
            Control *control = &window_data->controls[i];
            if (control->type == CONTROL_TYPE_BUTTON || control->type == CONTROL_TYPE_EDIT || control->type == CONTROL_TYPE_COMBOBOX || control->type == CONTROL_TYPE_LIST) {
                for (uint32_t j = 0; j < window_data->controls_size; j++) {
                    Control *other_control = &window_data->controls[j];
                    if (control->font == other_control->id) {
                        SendMessageA(window_data->controls_handles[i], WM_SETFONT, window_data->controls_handles[j], (LPARAM)TRUE);
                        break;
                    }
                }

                float x = control->x;
                if (control->x_unit == CONTROL_UNIT_VW) x *= vw;
                if (control->x_unit == CONTROL_UNIT_VH) x *= vh;
                if (control->x_unit == CONTROL_UNIT_VX) x *= vx;

                float y = control->y;
                if (control->y_unit == CONTROL_UNIT_VW) y *= vw;
                if (control->y_unit == CONTROL_UNIT_VH) y *= vh;
                if (control->y_unit == CONTROL_UNIT_VX) y *= vx;

                float width = control->width;
                if (control->width_unit == CONTROL_UNIT_VW) width *= vw;
                if (control->width_unit == CONTROL_UNIT_VH) width *= vh;
                if (control->width_unit == CONTROL_UNIT_VX) width *= vx;

                float height = control->height;
                if (control->height_unit == CONTROL_UNIT_VW) height *= vw;
                if (control->height_unit == CONTROL_UNIT_VH) height *= vh;
                if (control->height_unit == CONTROL_UNIT_VX) height *= vx;

                SetWindowPos(window_data->controls_handles[i], NULL, x, y, width, height, SWP_NOZORDER);
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
            uint32_t old_language = window_data->language;
            HWND settings_language_select = GetDlgItem(hwnd, SETTINGS_LANGUAGE_SELECT_ID);
            int32_t selected = SendMessageA(settings_language_select, CB_GETCURSEL, NULL, NULL);
            if (selected == 0) window_data->language = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
            if (selected == 1) window_data->language = MAKELANGID(LANG_DUTCH, SUBLANG_DUTCH);
            if (old_language != window_data->language) {
                SetLanguage(window_data->language);
                UpdateControlsTexts(hwnd);
                SaveSettings(hwnd);
            }
        }

        if (id == SETTINGS_THEME_SELECT_ID && notification == CBN_SELCHANGE) {
            uint32_t old_theme = window_data->theme;
            HWND settings_theme_select = GetDlgItem(hwnd, SETTINGS_THEME_SELECT_ID);
            int32_t selected = SendMessageA(settings_theme_select, CB_GETCURSEL, NULL, NULL);
            if (selected == 0) window_data->theme = THEME_LIGHT;
            if (selected == 1) window_data->theme = THEME_DARK;
            if (old_theme != window_data->theme) {
                SaveSettings(hwnd);
            }
        }

        if (id == SETTINGS_BACK_BUTTON_ID) {
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
            float x = LOWORD(lParam) / vw;
            float y = HIWORD(lParam) / vh;

            if (y > 440) {
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
        minMaxInfo->ptMinTrackSize.x = VIEWPORT_WIDTH;
        minMaxInfo->ptMinTrackSize.y = VIEWPORT_HEIGHT;
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
                    red_square->x < 16 * vx ||
                    red_square->y < 16 * vx + 20 * vx + 16 * vx ||
                    red_square->x + red_square->width > VIEWPORT_WIDTH - 16 * vx ||
                    red_square->y + red_square->height > VIEWPORT_HEIGHT - 16 * vx
                ) {
                    is_gameover = true;
                }
            }

            if (
                window_data->page == PAGE_MENU || window_data->page == PAGE_GAME || window_data->page == PAGE_HIGHSCORES ||
                window_data->page == PAGE_HELP || window_data->page == PAGE_SETTINGS
            ) {
                Square *red_square = &window_data->red_square;
                for (int32_t i = 0; i < 4; i++) {
                    Square *square = &window_data->blue_squares[i];
                    square->x += square->vx;
                    square->y += square->vy;

                    if (square->x < 0 || square->x + square->width > VIEWPORT_WIDTH) {
                        square->vx = -square->vx;
                    }
                    if (square->y < 0 || square->y + square->height > VIEWPORT_HEIGHT) {
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
        HBITMAP bitmap_buffer = CreateCompatibleBitmap(hdc, window_width, window_height);
        SelectObject(hdc_buffer, bitmap_buffer);

        // Draw background image
        if (window_data->paper_bitmap != NULL) {
            HDC hdc_bitmap_buffer = CreateCompatibleDC(hdc_buffer);
            SelectObject(hdc_bitmap_buffer, window_data->theme == THEME_DARK ? window_data->paper_dark_bitmap : window_data->paper_bitmap);
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

        if (window_data->paper_bitmap == NULL) {
            GdipGraphicsClear(graphics, window_data->theme == THEME_DARK ? 0xff222222 : 0xffffffff);
        }

        // Setup text drawing
        SetBkMode(hdc_buffer, TRANSPARENT);
        SetTextColor(hdc_buffer, window_data->theme == THEME_DARK ? 0x00ffffff : 0x00111111);

        // Draw border
        if (window_data->page == PAGE_GAME || window_data->page == PAGE_GAMEOVER) {
            GpBrush *brush;
            GdipCreateSolidFill(window_data->theme == THEME_DARK ? 0x33ffffff : 0x33000000, (GpSolidFill **)&brush);
            GdipFillRectangle(graphics, brush, 16 * vx, 16 * vx + 20 * vx + 16 * vx, window_width - 16 * vx - 16 * vx, window_height - 16 * vx - 20 * vx - 16 * vx - 16 * vx);
            GdipDeleteBrush(brush);
        }

        // Draw blue squares
        if (
            window_data->page == PAGE_MENU || window_data->page == PAGE_GAME || window_data->page == PAGE_GAMEOVER ||
            window_data->page == PAGE_HIGHSCORES || window_data->page == PAGE_HELP || window_data->page == PAGE_SETTINGS
        ) {
            GpBrush *brush;
            GdipCreateSolidFill(window_data->page == PAGE_GAME || window_data->page == PAGE_GAMEOVER ? 0xaa0000ff : 0x550000ff, (GpSolidFill **)&brush);
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

        // Page game
        if (window_data->page == PAGE_GAME || window_data->page == PAGE_GAMEOVER) {
            // Draw game stats
            char string_buffer[64];
            LoadStringA(instance, FONT_STRING_ID, string_buffer, sizeof(string_buffer));
            HFONT stats_font = CreateFontA(20 * vx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, string_buffer);
            SelectObject(hdc_buffer, stats_font);

            char format_buffer[64];
            LoadStringA(instance, GAME_SCORE_STRING_ID, format_buffer, sizeof(format_buffer));
            wsprintfA(string_buffer, format_buffer, window_data->score);
            SetTextAlign(hdc_buffer, TA_LEFT);
            TextOutA(hdc_buffer, 16 * vx, 16 * vx, string_buffer, strlen(string_buffer));

            LoadStringA(instance, GAME_TIME_STRIND_ID, format_buffer, sizeof(format_buffer));
            uint32_t seconds = window_data->time / FPS;
            wsprintfA(string_buffer, format_buffer, seconds / 60, seconds % 60);
            SetTextAlign(hdc_buffer, TA_CENTER);
            TextOutA(hdc_buffer, window_width / 2, 16 * vx, string_buffer, strlen(string_buffer));

            LoadStringA(instance, GAME_LEVEL_STRING_ID, format_buffer, sizeof(format_buffer));
            wsprintfA(string_buffer, format_buffer, window_data->level);
            SetTextAlign(hdc_buffer, TA_RIGHT);
            TextOutA(hdc_buffer, window_width - 16 * vx, 16 * vx, string_buffer, strlen(string_buffer));

            DeleteObject(stats_font);
        }

        // Page gameover
        if (window_data->page == PAGE_GAMEOVER) {
            // Draw overlay
            GpBrush *brush;
            GdipCreateSolidFill(window_data->theme == THEME_DARK ? 0x88000000 : 0x88ffffff, (GpSolidFill **)&brush);
            GdipFillRectangleI(graphics, brush, 0, 0, window_width, window_height);
            GdipDeleteBrush(brush);

            // Draw stats lines
            char string_buffer[64];
            LoadStringA(instance, FONT_STRING_ID, string_buffer, sizeof(string_buffer));
            HFONT text_font = CreateFontA(24 * vx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, string_buffer);
            SelectObject(hdc_buffer, text_font);
            SetTextAlign(hdc_buffer, TA_CENTER);

            char format_buffer[64];
            LoadStringA(instance, GAME_SCORE_STRING_ID, format_buffer, sizeof(format_buffer));
            wsprintfA(string_buffer, format_buffer, window_data->score);
            float y = ((480 - (48 + 16 + (24 + 16) * 3 + 52)) / 2) * vh;
            y += 48 * vh + 16 * vh;
            TextOutA(hdc_buffer, window_width / 2, y, string_buffer, strlen(string_buffer));
            y += 24 * vh + 16 * vh;

            LoadStringA(instance, GAME_TIME_STRIND_ID, format_buffer, sizeof(format_buffer));
            uint32_t seconds = window_data->time / FPS;
            wsprintfA(string_buffer, format_buffer, seconds / 60, seconds % 60);
            TextOutA(hdc_buffer, window_width / 2, y, string_buffer, strlen(string_buffer));
            y += 24 * vh + 16 * vh;

            LoadStringA(instance, GAME_LEVEL_STRING_ID, format_buffer, sizeof(format_buffer));
            wsprintfA(string_buffer, format_buffer, window_data->level);
            TextOutA(hdc_buffer, window_width / 2, y, string_buffer, strlen(string_buffer));

            DeleteObject(text_font);
        }

        // Draw controls
        for (uint32_t i = 0; i < window_data->controls_size; i++) {
            Control *control = &window_data->controls[i];
            if (control->type == CONTROL_TYPE_LABEL && control->page == window_data->page) {
                for (uint32_t j = 0; j < window_data->controls_size; j++) {
                    Control *other_control = &window_data->controls[j];
                    if (control->font == other_control->id) {
                        SelectObject(hdc_buffer, window_data->controls_handles[j]);
                        break;
                    }
                }

                float x = control->x;
                if (control->x_unit == CONTROL_UNIT_VW) x *= vw;
                if (control->x_unit == CONTROL_UNIT_VH) x *= vh;
                if (control->x_unit == CONTROL_UNIT_VX) x *= vx;

                float y = control->y;
                if (control->y_unit == CONTROL_UNIT_VW) y *= vw;
                if (control->y_unit == CONTROL_UNIT_VH) y *= vh;
                if (control->y_unit == CONTROL_UNIT_VX) y *= vx;

                float width = control->width;
                if (control->width_unit == CONTROL_UNIT_VW) width *= vw;
                if (control->width_unit == CONTROL_UNIT_VH) width *= vh;
                if (control->width_unit == CONTROL_UNIT_VX) width *= vx;

                float height = control->height;
                if (control->height_unit == CONTROL_UNIT_VW) height *= vw;
                if (control->height_unit == CONTROL_UNIT_VH) height *= vh;
                if (control->height_unit == CONTROL_UNIT_VX) height *= vx;

                char string_buffer[256];
                LoadStringA(instance, control->string, string_buffer, sizeof(string_buffer));

                if ((control->style & ES_MULTILINE) != 0) {
                    RECT rect = { x, y, x + width, y + height };
                    SetTextAlign(hdc_buffer, TA_LEFT);
                    DrawTextA(hdc_buffer, string_buffer, -1, &rect, control->style & 3);
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
                    TextOutA(hdc_buffer, x, y, string_buffer, strlen(string_buffer));
                }
            }
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
        DeleteObject(window_data->paper_bitmap);
        DeleteObject(window_data->paper_dark_bitmap);
        for (uint32_t i = 0; i < window_data->controls_size; i++) {
            Control *control = &window_data->controls[i];
            if (control->type == CONTROL_TYPE_FONT && window_data->controls_handles[i] != NULL) {
                DeleteObject(window_data->controls_handles[i]);
            }
        }
        free(window_data->controls_handles);
        free(window_data);

        // Close process
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
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

    WNDCLASSEXA wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    instance = GetModuleHandleA(NULL);
    wc.hInstance = instance;
    wc.hIcon = LoadImageA(wc.hInstance, (char *)APP_ICON_ID, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_SHARED);
    wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wc.lpszClassName = window_class_name;
    wc.hIconSm = LoadImageA(wc.hInstance, (char *)APP_ICON_ID, IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR | LR_SHARED);
    RegisterClassExA(&wc);

    char string_buffer[64];
    LoadStringA(instance, MENU_TITLE_STRING_ID, string_buffer, sizeof(string_buffer));
    HWND hwnd = CreateWindowExA(0, window_class_name, string_buffer,
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
