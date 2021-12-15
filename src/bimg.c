#define UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>
#include <stdint.h>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#define STBI_ONLY_GIF
#include "stb_image.h"

typedef HRESULT (STDMETHODCALLTYPE *_DwmSetWindowAttribute)(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute);
#define DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 19
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20

#define ID_ICON 1
#define ID_MENU_ABOUT 1001

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define WINDOW_MIN_WIDTH 320
#define WINDOW_MIN_HEIGHT 240
#define WINDOW_STYLE WS_OVERLAPPEDWINDOW

LPCWSTR window_class_name = TEXT("bimg");
LPCWSTR app_name = TEXT("BassieImage");
LPCWSTR font_name = TEXT("Segoe UI");
LPCWSTR drag_image_text = TEXT("Drag an image to view it!");
LPCWSTR image_error_text = TEXT("Could not load image!");
LPCWSTR footer_text = TEXT("Made by Bastiaan van der Plaat");

typedef struct Image {
    wchar_t path[MAX_PATH];
    int32_t width;
    int32_t height;
    uint8_t *data;
    HBITMAP bitmap;
} Image;

typedef struct WindowData {
    int32_t dpi;
    int32_t width_dp;
    int32_t height_dp;
    int32_t width_px;
    int32_t height_px;
    Image *image;
} WindowData;

#define DP2PX(dp) MulDiv(dp, window->dpi, 96)
#define PX2DP(px) MulDiv(px, 96, window->dpi)
#define PT2PT(pt) -MulDiv(pt, window->dpi, 72)

// Helper functions
int32_t GetPrimaryDesktopDpi(void) {
    HDC hdc = GetDC(HWND_DESKTOP);
    int32_t dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(HWND_DESKTOP, hdc);
    return dpi;
}

typedef BOOL (STDMETHODCALLTYPE *_AdjustWindowRectExForDpi)(RECT *lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi);

BOOL AdjustWindowRectExForDpi(RECT *lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi) {
    HMODULE huser32 = LoadLibrary(TEXT("user32.dll"));
    _AdjustWindowRectExForDpi AdjustWindowRectExForDpi = (_AdjustWindowRectExForDpi)GetProcAddress(huser32, "AdjustWindowRectExForDpi");
    if (AdjustWindowRectExForDpi) {
        return AdjustWindowRectExForDpi(lpRect, dwStyle, bMenu, dwExStyle, dpi);
    }
    return AdjustWindowRectEx(lpRect, dwStyle, bMenu, dwExStyle);
}

// Image loading code
Image *OpenImage(LPWSTR path) {
    Image *image = malloc(sizeof(Image));

    GetFullPathName(path, MAX_PATH, image->path, NULL);
    char path_ascii[MAX_PATH];
    WideCharToMultiByte(CP_ACP, 0, image->path, -1, path_ascii, MAX_PATH, NULL, FALSE);

    int32_t channels;
    uint8_t *image_data = stbi_load(path_ascii, &image->width, &image->height, &channels, 4);
    if (image_data == NULL) {
        free(image);
        return NULL;
    }

    image->data = calloc(image->width * image->height, 4);
    int32_t in = 0;
    int32_t out = 0;
    for (int32_t y = 0; y < image->height; y++) {
        for (int32_t x = 0; x < image->width; x++) {
            uint8_t alpha = image_data[in + 3];
            image->data[out + 2] = image_data[in] * alpha >> 8;
            image->data[out + 1] = image_data[in + 1] * alpha >> 8;
            image->data[out + 0] = image_data[in + 2] * alpha >> 8;
            in += 4;
            out += 4;
        }
    }
    stbi_image_free(image_data);

    image->bitmap = CreateBitmap(image->width, image->height, 1, 32, image->data);

    return image;
}

void FreeImage(Image *image) {
    DeleteObject(image->bitmap);
    free(image->data);
}

// Window code
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    WindowData *window = (WindowData *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    if (msg == WM_CREATE) {
        // Set window data struct as user data
        window = ((CREATESTRUCTW *)lParam)->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);

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

    // Open dropped image files
    if (msg == WM_DROPFILES) {
        HDROP hdrop = (HDROP)wParam;
        wchar_t path[MAX_PATH];
        DragQueryFile(hdrop, 0, path, MAX_PATH);
        DragFinish(hdrop);

        if (window->image != NULL) {
            FreeImage(window->image);
        }
        window->image = OpenImage(path);

        if (window->image != NULL) {
            wchar_t window_title[512];
            wcscpy(window_title, window->image->path);
            wcscat(window_title, TEXT(" - "));
            wcscat(window_title, app_name);
            SetWindowText(hwnd, window_title);
        }

        InvalidateRect(hwnd, NULL, TRUE);
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
        return 0;
    }

    // Set window min size
    if (msg == WM_GETMINMAXINFO) {
        // Calculate window min size for dpi
        int32_t window_dpi = window != NULL ? window->dpi : GetPrimaryDesktopDpi();
        RECT window_rect = { 0, 0, MulDiv(WINDOW_MIN_WIDTH, window_dpi, 96), MulDiv(WINDOW_MIN_HEIGHT, window_dpi, 96) };
        AdjustWindowRectExForDpi(&window_rect, WINDOW_STYLE, FALSE, 0, window_dpi);

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

    // Draw stuff
    if (msg == WM_PAINT) {
        PAINTSTRUCT paint_struct;
        HDC hdc = BeginPaint(hwnd, &paint_struct);

        // Create back buffer
        HDC hdc_buffer = CreateCompatibleDC(hdc);
        SetBkMode(hdc_buffer, TRANSPARENT);
        HBITMAP bitmap_buffer = CreateCompatibleBitmap(hdc, window->width_px, window->height_px);
        SelectObject(hdc_buffer, bitmap_buffer);

        // Draw background color
        HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
        RECT rect = { 0, 0, DP2PX(window->width_dp), DP2PX(window->height_dp) };
        FillRect(hdc_buffer, &rect, brush);
        DeleteObject(brush);

        if (window->image != NULL) {
            // Draw image bitmap
            int32_t scale_width = window->height_dp * window->image->width / window->image->height;
            int32_t scale_height = window->height_dp;
            if (scale_width > window->width_dp) {
                scale_width = window->width_dp;
                scale_height = window->width_dp * window->image->height / window->image->width;
            }
            if (window->width_dp > window->image->width && window->height_dp > window->image->height) {
                scale_width = window->image->width;
                scale_height = window->image->height;
            }

            HDC hdc_image_buffer = CreateCompatibleDC(hdc_buffer);
            SelectObject(hdc_image_buffer, window->image->bitmap);
            SetStretchBltMode(hdc_buffer, STRETCH_HALFTONE);
            StretchBlt(hdc_buffer, DP2PX((window->width_dp - scale_width) / 2), DP2PX((window->height_dp - scale_height) / 2),
                DP2PX(scale_width), DP2PX(scale_height), hdc_image_buffer, 0, 0, window->image->width, window->image->height, SRCCOPY);
            DeleteDC(hdc_image_buffer);
        } else {
            // Draw drag image text
            HFONT header_font = CreateFont(PT2PT(window->width_dp * 3 / 100), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
            SelectObject(hdc_buffer, header_font);
            SetTextColor(hdc_buffer, RGB(255, 255, 255));
            SetTextAlign(hdc_buffer, TA_CENTER);
            SIZE measure_rect;
            GetTextExtentPoint32(hdc_buffer, drag_image_text, wcslen(drag_image_text), &measure_rect);
            TextOut(hdc_buffer, DP2PX(window->width_dp / 2), DP2PX(window->height_dp / 2) - measure_rect.cy / 2, drag_image_text, lstrlen(drag_image_text));
            DeleteObject(header_font);

            // Draw footer text
            HFONT footer_font = CreateFont(PT2PT(16), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);
            SelectObject(hdc_buffer, footer_font);
            SetTextAlign(hdc_buffer, TA_CENTER | TA_BOTTOM);
            SetTextColor(hdc_buffer, RGB(119, 119, 119));
            TextOut(hdc_buffer, DP2PX(window->width_dp / 2), DP2PX(window->height_dp - 24), footer_text, lstrlen(footer_text));
            DeleteObject(footer_font);
        }

        // Draw and delete back buffer
        BitBlt(hdc, 0, 0, window->width_px, window->height_px, hdc_buffer, 0, 0, SRCCOPY);
        DeleteObject(bitmap_buffer);
        DeleteDC(hdc_buffer);

        EndPaint(hwnd, &paint_struct);
        return 0;
    }

    // Quit application
    if (msg == WM_DESTROY) {
        // Free image bitmap
        if (window->image != NULL) {
            FreeImage(window->image);
        }

        // Free window data
        free(window);

        // Close process
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow) {
    // Parse args
    int32_t argc;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);

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
    wc.hInstance = hInstance;wc.hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(ID_ICON), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_SHARED);
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
    if (argc >= 2) {
        window->image = OpenImage(argv[1]);
    } else {
        window->image = NULL;
    }

    // Create centered window
    RECT window_rect;
    window_rect.left = (GetSystemMetrics(SM_CXSCREEN) - window->width_px) / 2;
    window_rect.top = (GetSystemMetrics(SM_CYSCREEN) - window->height_px) / 2;
    window_rect.right = window_rect.left + window->width_px;
    window_rect.bottom = window_rect.top + window->height_px;
    AdjustWindowRectExForDpi(&window_rect, WINDOW_STYLE, FALSE, 0, window->dpi);

    wchar_t window_title[512];
    if (window->image != NULL) {
        wcscpy(window_title, window->image->path);
        wcscat(window_title, TEXT(" - "));
        wcscat(window_title, app_name);
    }
    HWND hwnd = CreateWindowEx(WS_EX_ACCEPTFILES, window_class_name, window->image != NULL ? window_title : app_name,
        WINDOW_STYLE, window_rect.left, window_rect.top,
        window_rect.right - window_rect.left, window_rect.bottom - window_rect.top,
        HWND_DESKTOP, NULL, wc.hInstance, window);

    // Enable dark window decoration
    HMODULE hdwmapi = LoadLibrary(TEXT("dwmapi.dll"));
    _DwmSetWindowAttribute DwmSetWindowAttribute = (_DwmSetWindowAttribute)GetProcAddress(hdwmapi, "DwmSetWindowAttribute");
    if (DwmSetWindowAttribute) {
        BOOL useImmersiveDarkMode = TRUE;
        DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1, &useImmersiveDarkMode, sizeof(BOOL));
        DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useImmersiveDarkMode, sizeof(BOOL));
    }

    // Show window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Free args not needed anymore
    LocalFree(argv);

    // Main window event loop
    MSG message;
    while (GetMessage(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
    return message.wParam;
}
