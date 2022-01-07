#define UNICODE
#include <windows.h>
#include <commctrl.h>

#define CANVAS_IMPLEMENTATION
#define CANVAS_USE_CUSTOM_HEADERS
#define CANVAS_ENABLE_BITMAP
#define CANVAS_ENABLE_STBI_IMAGE
#define CANVAS_ENABLE_TEXT
#define CANVAS_ENABLE_PATH
#include "canvas.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_SIMD
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#define STBI_ONLY_GIF
#include "stb_image.h"

#define HELPERS_IMPLEMENTATION
#define HELPERS_ENABLE_DPI
#define HELPERS_ENABLE_VERSION
#define HELPERS_ENABLE_FULLSCREEN
#define HELPERS_ENABLE_IMMERSIVE_DARK_MODE
#include "helpers.h"

#include "../res/resource.h"
#define ID_MENU_ABOUT 0x1001

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define WINDOW_MIN_WIDTH 320
#define WINDOW_MIN_HEIGHT 240
#define WINDOW_EX_STYLE (WS_EX_ACCEPTFILES | WS_EX_COMPOSITED)
#define WINDOW_STYLE WS_OVERLAPPEDWINDOW

wchar_t *window_class_name = L"bimg";
wchar_t *app_name = L"BassieImage";
wchar_t *font_name;
char *close_icon = "M19,6.41L17.59,5L12,10.59L6.41,5L5,6.41L10.59,12L5,17.59L6.41,19L12,13.41L17.59,19L19,17.59L13.41,12L19,6.41Z";

typedef struct WindowData {
    HINSTANCE instance;
    int32_t dpi;
    int32_t width_dp;
    int32_t height_dp;
    int32_t min_dp;
    int32_t width_px;
    int32_t height_px;
    Canvas *canvas;

    bool error;
    CanvasBitmap *imageBitmap;
    wchar_t *imageBasename;

    bool about;
    CanvasBitmap *bassiebasBitmap;
} WindowData;

#define DP2PX(dp) MulDiv(dp, window->dpi, 96)
#define PX2DP(px) MulDiv(px, 96, window->dpi)

void OpenImage(HWND hwnd, wchar_t *path) {
    WindowData *window = (WindowData *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    if (window->imageBitmap != NULL) {
        CanvasBitmap_Free(window->imageBitmap);
    }
    if (window->imageBasename != NULL) {
        free(window->imageBasename);
        window->imageBasename = NULL;
    }

    if (path != NULL) {
        window->imageBitmap = CanvasBitmap_NewFromFile(window->canvas, path);
        window->error = window->imageBitmap == NULL;
    } else {
        window->imageBitmap = NULL;
        window->error = false;
    }
    window->about = false;

    if (window->imageBitmap != NULL) {
        wchar_t full_path[MAX_PATH];
        GetFullPathName(path, MAX_PATH, full_path, NULL);

        wchar_t *c = full_path + wcslen(full_path);
        while (*c != '\\') c--;
        window->imageBasename = wcsdup(c + 1);

        wchar_t window_title[512];
        wcscpy(window_title, full_path);
        wcscat(window_title, L" - ");
        wcscat(window_title, app_name);
        SetWindowText(hwnd, window_title);
    } else {
        SetWindowText(hwnd, app_name);
    }

    InvalidateRect(hwnd, NULL, FALSE);
}

void OpenAbout(HWND hwnd) {
    WindowData *window = (WindowData *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    window->about = true;
    if (window->bassiebasBitmap == NULL) {
        window->bassiebasBitmap = CanvasBitmap_NewFromResource(window->canvas, L"IMAGE", MAKEINTRESOURCE(ID_IMAGE_BASSIEBASE));
    }
    InvalidateRect(hwnd, NULL, FALSE);
}

void GoBack(HWND hwnd) {
    WindowData *window = (WindowData *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (!(GetWindowLong(hwnd, GWL_STYLE) & WS_OVERLAPPEDWINDOW)) {
        SetWindowFullscreen(hwnd, FALSE);
        return;
    }
    if (window->imageBitmap != NULL) {
        OpenImage(hwnd, NULL);
        return;
    }
    if (window->error || window->about) {
        window->error = false;
        window->about = false;
        InvalidateRect(hwnd, NULL, FALSE);
        return;
    }
    DestroyWindow(hwnd);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    WindowData *window = (WindowData *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    if (msg == WM_CREATE) {
        // Set window data struct as user data
        window = ((CREATESTRUCTW *)lParam)->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);

        // Select right font name
        font_name = IsVistaOrHigher() ? L"Segoe UI" : L"Tahoma";

        // Create canvas
        window->canvas = Canvas_New(hwnd, CANVAS_RENDERER_DEFAULT);
        window->error = false;
        window->imageBitmap = NULL;
        window->imageBasename = NULL;
        window->about = false;
        window->bassiebasBitmap = NULL;

        // Open image from args
        int32_t argc;
        LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
        if (argc >= 2) {
            OpenImage(hwnd, argv[1]);
        }
        LocalFree(argv);

        // Create system about menu
        HMENU sysMenu = GetSystemMenu(hwnd, FALSE);
        InsertMenu(sysMenu, 5, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);

        wchar_t *menu_about_text;
        LoadStringW(window->instance, ID_STRING_MENU_ABOUT, (wchar_t *)&menu_about_text, 0);
        InsertMenu(sysMenu, 6, MF_BYPOSITION, ID_MENU_ABOUT, menu_about_text);

        return 0;
    }

    // Menu commands
    if (msg == WM_SYSCOMMAND) {
        int32_t id = LOWORD(wParam);
        if (id == ID_MENU_ABOUT) {
            OpenAbout(hwnd);
            return 0;
        }
    }

    // Open click listener
    if (msg == WM_LBUTTONUP) {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        CanvasPoint mouse = { PX2DP(x), PX2DP(y) };

        if (window->about) {
            float padding = window->min_dp * 40 / 1000;
            float imageSize = window->min_dp * 350 / 1000;
            CanvasRect dialogRect;
            dialogRect.width = window->min_dp - padding * 2;
            dialogRect.height = imageSize + padding * 2;
            dialogRect.x = (window->width_dp - dialogRect.width) / 2;
            dialogRect.y = (window->height_dp - dialogRect.height) / 2;

            CanvasRect bassiebasRect = { dialogRect.x + padding, dialogRect.y + padding, imageSize, imageSize };
            if (CANVAS_POINT_IN_RECT(mouse, bassiebasRect)) {
                ShellExecute(hwnd, L"open", L"https://bastiaan.ml/", NULL, NULL, SW_SHOWNORMAL);
            }

            CanvasRect closeButtonRect = { dialogRect.x + dialogRect.width - padding, dialogRect.y, padding, padding };
            if (CANVAS_POINT_IN_RECT(mouse, closeButtonRect)) {
                window->about = false;
                InvalidateRect(hwnd, NULL, FALSE);
            }

            return 0;
        }

        if (window->imageBitmap != NULL || window->error) {
            float closeButtonSize = window->min_dp * 40 / 1000;
            CanvasRect closeButtonRect = { window->width_dp - closeButtonSize * 1.25, closeButtonSize / 4, closeButtonSize, closeButtonSize };
            if (CANVAS_POINT_IN_RECT(mouse, closeButtonRect)) {
                GoBack(hwnd);
                return 0;
            }
        }

        if (window->imageBitmap == NULL) {
            // Measure footer height
            CanvasRect footer_rect = { 0, 0, window->width_dp, 0 };
            wchar_t *footer_text;
            LoadStringW(window->instance, ID_STRING_FOOTER, (wchar_t *)&footer_text, 0);
            Canvas_MeasureText(window->canvas, footer_text, -1, &footer_rect,
                &(CanvasFont){ .name = font_name, .size = window->min_dp * 20 / 1000 },
                CANVAS_TEXT_FORMAT_HORIZONTAL_CENTER | CANVAS_TEXT_FORMAT_VERTICAL_BOTTOM);

            if (y < window->height_px - footer_rect.height - (window->min_dp * 30 / 1000) * 2) {
                OPENFILENAME open_file_dialog = { sizeof(OPENFILENAME) };
                wchar_t path[MAX_PATH] = L"";
                open_file_dialog.hwndOwner = hwnd;
                open_file_dialog.lpstrFile = path;
                open_file_dialog.nMaxFile = MAX_PATH;
                open_file_dialog.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

                wchar_t *open_files_filter_text;
                int open_files_filter_size = LoadStringW(window->instance, ID_STRING_OPEN_FILES_FILTER, (wchar_t *)&open_files_filter_text, 0) + 1;
                open_file_dialog.lpstrFilter = malloc(open_files_filter_size * sizeof(wchar_t));
                memcpy((void *)open_file_dialog.lpstrFilter, open_files_filter_text, open_files_filter_size * sizeof(wchar_t));

                if (GetOpenFileName(&open_file_dialog)) {
                    OpenImage(hwnd, path);
                }

                free((void *)open_file_dialog.lpstrFilter);
            } else {
                OpenAbout(hwnd);
            }
            return 0;
        }

        return 0;
    }

    // Keyboard shortcuts
    if (msg == WM_KEYDOWN) {
        if (wParam == VK_ESCAPE || wParam == VK_BACK) {
            GoBack(hwnd);
            return 0;
        }

        if (wParam == VK_F11) {
            SetWindowFullscreen(hwnd, GetWindowLong(hwnd, GWL_STYLE) & WS_OVERLAPPEDWINDOW);
            return 0;
        }
    }

    // Open dropped image files
    if (msg == WM_DROPFILES) {
        HDROP hdrop = (HDROP)wParam;
        wchar_t path[MAX_PATH];
        DragQueryFile(hdrop, 0, path, MAX_PATH);
        DragFinish(hdrop);
        SetForegroundWindow(hwnd);
        OpenImage(hwnd, path);
        return 0;
    }

    // Handle dpi changes
    if (msg == WM_DPICHANGED) {
        window->dpi = HIWORD(wParam);
        RECT *window_rect = (RECT *)lParam;
        SetWindowPos(hwnd, NULL, window_rect->left, window_rect->top, window_rect->right - window_rect->left,
            window_rect->bottom - window_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
        return 0;
    }

    // Save new window size
    if (msg == WM_SIZE) {
        window->width_px = LOWORD(lParam);
        window->height_px = HIWORD(lParam);
        window->width_dp = PX2DP(window->width_px);
        window->height_dp = PX2DP(window->height_px);
        window->min_dp = MIN(window->width_dp, window->height_dp);

        // Resize canvas
        Canvas_Resize(window->canvas, window->width_px, window->height_px, window->dpi);
        return 0;
    }

    // Set window min size
    if (msg == WM_GETMINMAXINFO) {
        // Calculate window min size for dpi
        int32_t window_dpi = window != NULL ? window->dpi : GetPrimaryDesktopDpi();
        RECT window_rect = { 0, 0, MulDiv(WINDOW_MIN_WIDTH, window_dpi, 96), MulDiv(WINDOW_MIN_HEIGHT, window_dpi, 96) };
        AdjustWindowRectExForDpi(&window_rect, WINDOW_STYLE, FALSE, WINDOW_EX_STYLE, window_dpi);

        // Set window min size
        MINMAXINFO *minMaxInfo = (MINMAXINFO *)lParam;
        minMaxInfo->ptMinTrackSize.x = window_rect.right - window_rect.left;
        minMaxInfo->ptMinTrackSize.y = window_rect.bottom - window_rect.top;
        return 0;
    }

    // Draw no background
    if (msg == WM_ERASEBKGND) {
        return TRUE;
    }

    if (msg == WM_PAINT) {
        Canvas_BeginDraw(window->canvas);

        // Draw background
        Canvas_FillRect(window->canvas, &(CanvasRect){ 0, 0, window->width_dp, window->height_dp }, CANVAS_HEX(0x000000));

        if (window->imageBitmap != NULL) {
            // Draw image bitmap
            float scale_width = window->height_dp * window->imageBitmap->width / window->imageBitmap->height;
            float scale_height = window->height_dp;
            if (scale_width > window->width_dp) {
                scale_width = window->width_dp;
                scale_height = window->width_dp * window->imageBitmap->height / window->imageBitmap->width;
            }
            if (window->width_dp > window->imageBitmap->width && window->height_dp > window->imageBitmap->height) {
                scale_width = window->imageBitmap->width;
                scale_height = window->imageBitmap->height;
            }
            Canvas_DrawBitmap(window->canvas, window->imageBitmap, &(CanvasRect){ (window->width_dp - scale_width) / 2,
                (window->height_dp - scale_height) / 2, scale_width, scale_height }, NULL);

            // Draw image basename
            float closeButtonSize = window->min_dp * 40 / 1000;
            Canvas_DrawText(window->canvas, window->imageBasename, -1,
                &(CanvasRect){ closeButtonSize / 4, closeButtonSize / 4, window->width_dp - closeButtonSize * 1.5, closeButtonSize },
                &(CanvasFont){ .name = font_name, .size = window->min_dp * 20 / 1000 },
                CANVAS_TEXT_FORMAT_VERTICAL_CENTER, CANVAS_HEX(0xffffff));
        } else {
            // Draw header text
            wchar_t *image_error_text;
            LoadStringW(window->instance, ID_STRING_IMAGE_ERROR, (wchar_t *)&image_error_text, 0);
            wchar_t *drag_image_text;
            LoadStringW(window->instance, ID_STRING_DRAG_IMAGE, (wchar_t *)&drag_image_text, 0);
            Canvas_DrawText(window->canvas, window->error ? image_error_text : drag_image_text, -1,
                &(CanvasRect){ 0, 0, window->width_dp, window->height_dp },
                &(CanvasFont){ .name = font_name, .size = window->min_dp * 40 / 1000 },
                CANVAS_TEXT_FORMAT_HORIZONTAL_CENTER | CANVAS_TEXT_FORMAT_VERTICAL_CENTER, CANVAS_HEX(0xffffff));

            // Draw footer text
            wchar_t *footer_text;
            LoadStringW(window->instance, ID_STRING_FOOTER, (wchar_t *)&footer_text, 0);
            Canvas_DrawText(window->canvas, footer_text, -1,
                &(CanvasRect){ 0, 0, window->width_dp, window->height_dp - (window->min_dp * 30 / 1000) },
                &(CanvasFont){ .name = font_name, .size = window->min_dp * 20 / 1000 },
                CANVAS_TEXT_FORMAT_HORIZONTAL_CENTER | CANVAS_TEXT_FORMAT_VERTICAL_BOTTOM, CANVAS_HEX(0x888888));
        }

        // Draw close button
        if (window->imageBitmap != NULL || window->error) {
            float closeButtonSize = window->min_dp * 40 / 1000;
            Canvas_FillRect(window->canvas, &(CanvasRect){ window->width_dp - closeButtonSize * 1.25, closeButtonSize / 4, closeButtonSize, closeButtonSize}, CANVAS_HEX(0x222222));
            Canvas_FillPath(window->canvas, close_icon, 24, 24, &(CanvasRect){ window->width_dp - closeButtonSize * 1.25, closeButtonSize / 4, closeButtonSize, closeButtonSize}, CANVAS_HEX(0xffffff));
        }

        // Draw about dialog
        if (window->about) {
            // Draw about dialog shadow
            Canvas_FillRect(window->canvas, &(CanvasRect){ 0, 0, window->width_dp, window->height_dp }, CANVAS_RGBA(0, 0, 0, 200));

            float padding = window->min_dp * 40 / 1000;
            float imageSize = window->min_dp * 350 / 1000;

            // Draw about dialog
            CanvasRect dialogRect;
            dialogRect.width = window->min_dp - padding * 2;
            dialogRect.height = imageSize + padding * 2;
            dialogRect.x = (window->width_dp - dialogRect.width) / 2;
            dialogRect.y = (window->height_dp - dialogRect.height) / 2;
            Canvas_FillRect(window->canvas, &dialogRect, CANVAS_HEX(0x222222));

            // Draw about dialog bassiebas image
            Canvas_DrawBitmap(window->canvas, window->bassiebasBitmap, &(CanvasRect){ dialogRect.x + padding, dialogRect.y + padding, imageSize, imageSize }, NULL);

            // Calculate text height
            float contentHeight = 0;
            CanvasFont headerFont = { .name = font_name, .size = window->min_dp * 25 / 1000 };
            CanvasFont textFont = { .name = font_name, .size = window->min_dp * 15 / 1000 };
            CanvasRect measureRect = { 0, 0, 0, 0 };

            wchar_t *about_header_text;
            LoadStringW(window->instance, ID_STRING_ABOUT_HEADER, (wchar_t *)&about_header_text, 0);
            Canvas_MeasureText(window->canvas, about_header_text, -1, &measureRect, &headerFont, CANVAS_TEXT_FORMAT_DEFAULT);
            contentHeight += measureRect.height + padding / 2;

            measureRect.width = dialogRect.width - imageSize - padding * 3;
            wchar_t *about_description_text;
            LoadStringW(window->instance, ID_STRING_ABOUT_DESCRIPTION, (wchar_t *)&about_description_text, 0);
            Canvas_MeasureText(window->canvas, about_description_text, -1, &measureRect, &textFont, CANVAS_TEXT_FORMAT_WRAP);
            contentHeight += measureRect.height + padding / 2;

            wchar_t *about_copyright_text;
            LoadStringW(window->instance, ID_STRING_ABOUT_COPYRIGHT, (wchar_t *)&about_copyright_text, 0);
            Canvas_MeasureText(window->canvas, about_copyright_text, -1, &measureRect, &textFont, CANVAS_TEXT_FORMAT_DEFAULT);
            contentHeight += measureRect.height;

            wchar_t *footer_text;
            LoadStringW(window->instance, ID_STRING_FOOTER, (wchar_t *)&footer_text, 0);
            Canvas_MeasureText(window->canvas, footer_text, -1, &measureRect, &textFont, CANVAS_TEXT_FORMAT_DEFAULT);
            contentHeight += measureRect.height;

            // Draw about dialog header text
            CanvasRect headerRect = { dialogRect.x + padding + imageSize + padding, dialogRect.y + (dialogRect.height - contentHeight) / 2, 0, 0 };
            Canvas_DrawText(window->canvas, about_header_text, -1, &headerRect, &headerFont,
                CANVAS_TEXT_FORMAT_DEFAULT, CANVAS_HEX(0xffffff));

            // Draw about dialog description text
            CanvasRect descriptionRect = { headerRect.x, headerRect.y + headerRect.height + padding / 2, dialogRect.width - imageSize - padding * 3, 0 };
            Canvas_DrawText(window->canvas, about_description_text, -1, &descriptionRect, &textFont,
                CANVAS_TEXT_FORMAT_WRAP, CANVAS_HEX(0xffffff));

            // Draw about dialog copyright text
            CanvasRect copyrightRect = { headerRect.x, descriptionRect.y + descriptionRect.height + padding / 2, 0, 0 };
            Canvas_DrawText(window->canvas, about_copyright_text, -1, &copyrightRect, &textFont,
                CANVAS_TEXT_FORMAT_DEFAULT, CANVAS_HEX(0xffffff));

            // Draw about dialog footer text
            CanvasRect footerRect = { headerRect.x, copyrightRect.y + copyrightRect.height + padding / 2, 0, 0 };
            Canvas_DrawText(window->canvas, footer_text, -1, &footerRect, &textFont,
                CANVAS_TEXT_FORMAT_DEFAULT, CANVAS_HEX(0xffffff));

            // Draw close button
            Canvas_FillRect(window->canvas, &(CanvasRect){ dialogRect.x + dialogRect.width - padding, dialogRect.y, padding, padding}, CANVAS_HEX(0x444444));
            Canvas_FillPath(window->canvas, close_icon, 24, 24, &(CanvasRect){ dialogRect.x + dialogRect.width - padding, dialogRect.y, padding, padding}, CANVAS_HEX(0xffffff));
        }

        Canvas_EndDraw(window->canvas);
        return 0;
    }

    if (msg == WM_DESTROY) {
        // Free window data
        if (window->imageBitmap != NULL) {
            CanvasBitmap_Free(window->imageBitmap);
        }
        if (window->bassiebasBitmap != NULL) {
            CanvasBitmap_Free(window->bassiebasBitmap);
        }
        Canvas_Free(window->canvas);
        free(window);

        // Close process
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow) {
    // Init common controls
    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icc);

    // Register window class
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(ID_ICON_APP), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_SHARED);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = window_class_name;
    wc.hIconSm = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(ID_ICON_APP), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR | LR_SHARED);
    RegisterClassEx(&wc);

    // Create window data struct
    WindowData *window = malloc(sizeof(WindowData));
    window->instance = hInstance;
    window->dpi = GetPrimaryDesktopDpi();
    window->width_dp = WINDOW_WIDTH;
    window->height_dp = WINDOW_HEIGHT;
    window->width_px = DP2PX(window->width_dp);
    window->height_px = DP2PX(window->height_dp);

    // Create centered window
    RECT window_rect;
    window_rect.left = (GetSystemMetrics(SM_CXSCREEN) - window->width_px) / 2;
    window_rect.top = (GetSystemMetrics(SM_CYSCREEN) - window->height_px) / 2;
    window_rect.right = window_rect.left + window->width_px;
    window_rect.bottom = window_rect.top + window->height_px;
    AdjustWindowRectExForDpi(&window_rect, WINDOW_STYLE, FALSE, WINDOW_EX_STYLE, window->dpi);

    HWND hwnd = CreateWindowEx(WINDOW_EX_STYLE, window_class_name, app_name,
        WINDOW_STYLE, window_rect.left, window_rect.top,
        window_rect.right - window_rect.left, window_rect.bottom - window_rect.top,
        HWND_DESKTOP, NULL, wc.hInstance, window);
    SetWindowImmersiveDarkMode(hwnd, TRUE);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Main window event loop
    MSG message;
    while (GetMessage(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
    return message.wParam;
}
