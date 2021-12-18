#define UNICODE
#include <windows.h>
#include <commctrl.h>

#define CANVAS_IMPLEMENTATION
#define CANVAS_ENABLE_BITMAP
#define CANVAS_ENABLE_STBI_IMAGE
#define CANVAS_ENABLE_TEXT
#include "canvas.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#define STBI_ONLY_GIF
#include "stb_image.h"

#define HELPERS_IMPLEMENTATION
#define HELPERS_ENABLE_DPI
#define HELPERS_ENABLE_FULLSCREEN
#define HELPERS_ENABLE_IMMERSIVE_DARK_MODE
#include "helpers.h"

#define ID_ICON 1
#define ID_MENU_ABOUT 1001

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define WINDOW_MIN_WIDTH 320
#define WINDOW_MIN_HEIGHT 240
#define WINDOW_EX_STYLE (WS_EX_ACCEPTFILES | WS_EX_COMPOSITED)
#define WINDOW_STYLE WS_OVERLAPPEDWINDOW

wchar_t *window_class_name = L"bimg";
wchar_t *app_name = L"BassieImage";
wchar_t *font_name = L"Segoe UI";
wchar_t *drag_image_text = L"Drag an image to view it!";
wchar_t *image_error_text = L"Could not load that image!";
wchar_t *footer_text = L"Made by Bastiaan van der Plaat";

typedef struct WindowData {
    int32_t dpi;
    int32_t width_dp;
    int32_t height_dp;
    int32_t width_px;
    int32_t height_px;
    Canvas *canvas;
    CanvasBitmap *bitmap;
    bool error;
} WindowData;

#define DP2PX(dp) MulDiv(dp, window->dpi, 96)
#define PX2DP(px) MulDiv(px, 96, window->dpi)

void OpenImage(HWND hwnd, wchar_t *path) {
    WindowData *window = (WindowData *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    if (window->bitmap != NULL) {
        CanvasBitmap_Free(window->bitmap);
    }
    if (path != NULL) {
        window->bitmap = CanvasBitmap_NewFromFile(window->canvas, path);
        window->error = window->bitmap == NULL;
    } else {
        window->bitmap = NULL;
        window->error = false;
    }

    if (window->bitmap != NULL) {
        wchar_t full_path[MAX_PATH];
        GetFullPathName(path, MAX_PATH, full_path, NULL);

        wchar_t window_title[512];
        wcscpy(window_title, full_path);
        wcscat(window_title, TEXT(" - "));
        wcscat(window_title, app_name);
        SetWindowText(hwnd, window_title);
    } else {
        SetWindowText(hwnd, app_name);
    }

    InvalidateRect(hwnd, NULL, FALSE);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    WindowData *window = (WindowData *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    if (msg == WM_CREATE) {
        // Set window data struct as user data
        window = ((CREATESTRUCTW *)lParam)->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);

        // Create canvas
        window->canvas = Canvas_New(hwnd, CANVAS_RENDERER_DEFAULT);
        window->bitmap = NULL;
        window->error = false;

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
        InsertMenu(sysMenu, 6, MF_BYPOSITION, ID_MENU_ABOUT, TEXT("About"));
        return 0;
    }

    // Menu commands
    if (msg == WM_SYSCOMMAND) {
        int32_t id = LOWORD(wParam);
        if (id == ID_MENU_ABOUT) {
            MessageBox(hwnd, TEXT("Made by Bastiaan van der Plaat\nCopyright (c) 2021 PlaatSoft"), TEXT("About BassieImage"), MB_OK | MB_ICONINFORMATION);
            return 0;
        }
    }

    // Open click listener
    if (msg == WM_LBUTTONUP) {
        int y = GET_Y_LPARAM(lParam);

        if (window->bitmap == NULL) {
            // Measure footer height
            CanvasRect footer_rect = { 0, 0, window->width_dp, 0 };
            Canvas_MeasureText(window->canvas, footer_text, -1, &footer_rect,
                &(CanvasFont){ .name = font_name, .size = window->width_dp * 15 / 1000 },
                CANVAS_TEXT_FORMAT_HORIZONTAL_CENTER | CANVAS_TEXT_FORMAT_VERTICAL_BOTTOM);

            if (y < window->height_px - footer_rect.height - DP2PX(24 * 2)) {
                OPENFILENAME open_file_dialog = { sizeof(OPENFILENAME) };
                wchar_t path[MAX_PATH] = L"";
                open_file_dialog.hwndOwner = hwnd;
                open_file_dialog.lpstrFilter = L"All Image Files\0*.jpg;*.jpeg;*.png;*.gif;*.bmp\0"
                    "JPEG Files (*.jpg; *.jpeg)\0*.jpg;*.jpeg\0"
                    "PNG Files (*.png)\0*.png\0"
                    "GIF Files (*.gif)\0*.gif\0"
                    "BMP Files (*.bmp)\0*.bmp\0";
                open_file_dialog.lpstrFile = path;
                open_file_dialog.nMaxFile = MAX_PATH;
                open_file_dialog.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
                if (GetOpenFileName(&open_file_dialog)) {
                    OpenImage(hwnd, path);
                }
            } else {
                ShellExecute(hwnd, TEXT("open"), TEXT("https://bastiaan.ml/"), NULL, NULL, SW_SHOWNORMAL);
            }
        }
        return 0;
    }

    // Keyboard shortcuts
    if (msg == WM_KEYDOWN) {
        if (wParam == VK_ESCAPE || wParam == VK_BACK) {
            if (!(GetWindowLong(hwnd, GWL_STYLE) & WS_OVERLAPPEDWINDOW)) {
                SetWindowFullscreen(hwnd, FALSE);
                return 0;
            }
            if (window->bitmap != NULL) {
                OpenImage(hwnd, NULL);
                return 0;
            }
            if (window->error) {
                window->error = false;
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }
            DestroyWindow(hwnd);
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

        if (window->bitmap != NULL) {
            // Draw bitmap
            int32_t scale_width = window->height_dp * window->bitmap->width / window->bitmap->height;
            int32_t scale_height = window->height_dp;
            if (scale_width > window->width_dp) {
                scale_width = window->width_dp;
                scale_height = window->width_dp * window->bitmap->height / window->bitmap->width;
            }
            if (window->width_dp > window->bitmap->width && window->height_dp > window->bitmap->height) {
                scale_width = window->bitmap->width;
                scale_height = window->bitmap->height;
            }
            Canvas_DrawBitmap(window->canvas, window->bitmap, &(CanvasRect){ (window->width_dp - scale_width) / 2,
                (window->height_dp - scale_height) / 2, scale_width, scale_height }, NULL);
        } else {
            // Draw header text
            Canvas_DrawText(window->canvas, window->error ? image_error_text : drag_image_text, -1,
                &(CanvasRect){ 0, 0, window->width_dp, window->height_dp },
                &(CanvasFont){ .name = font_name, .size = window->width_dp * 30 / 1000 },
                CANVAS_TEXT_FORMAT_HORIZONTAL_CENTER | CANVAS_TEXT_FORMAT_VERTICAL_CENTER, CANVAS_HEX(0xffffff));

            // Draw footer text
            Canvas_DrawText(window->canvas, footer_text, -1,
                &(CanvasRect){ 0, 0, window->width_dp, window->height_dp - 24 },
                &(CanvasFont){ .name = font_name, .size = window->width_dp * 15 / 1000 },
                CANVAS_TEXT_FORMAT_HORIZONTAL_CENTER | CANVAS_TEXT_FORMAT_VERTICAL_BOTTOM, CANVAS_HEX(0x777777));
        }

        Canvas_EndDraw(window->canvas);
        return 0;
    }

    if (msg == WM_DESTROY) {
        // Free window data
        if (window->bitmap != NULL) {
            CanvasBitmap_Free(window->bitmap);
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
    wc.hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(ID_ICON), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_SHARED);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = window_class_name;
    wc.hIconSm = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(ID_ICON), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR | LR_SHARED);
    RegisterClassEx(&wc);

    // Create window data struct
    WindowData *window = malloc(sizeof(WindowData));
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
