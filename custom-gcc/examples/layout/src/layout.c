#include "win32.h"
#include "jan.h"

wchar_t *window_class_name = L"jan-test";

#ifdef WIN64
    wchar_t *window_title = L"This is a Jan Test window 😊 (64-bit)";
#else
    wchar_t *window_title = L"This is a Jan Test window 😊 (32-bit)";
#endif

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_STYLE WS_OVERLAPPEDWINDOW

typedef struct {
    JanWidget *root;
} WindowData;

int32_t __stdcall WndProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam) {
    WindowData *window = GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    if (msg == WM_CREATE) {
        // Create window data
        window = malloc(sizeof(WindowData));
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, window);
        jan_hwnd = hwnd;
        jan_width = WINDOW_WIDTH;
        jan_height = WINDOW_HEIGHT;
        jan_label_default_font_name = L"Georgia";
        jan_label_default_text_color = RGB(255, 255, 255);

        // Load widgets
        HRSRC main_layout_resource = FindResourceW(GetModuleHandleW(NULL), (wchar_t *)1, (wchar_t *)RT_RCDATA);
        HGLOBAL main_layout = LoadResource(GetModuleHandleW(NULL), main_layout_resource);
        jan_load(LockResource(main_layout), &window->root);

        // Insert items into combobox
        JanComboBox *combobox = JAN_COMBOBOX(jan_container_find(JAN_CONTAINER(window->root), 202));
        jan_comboxbox_add(combobox, L"Ik ben dom!");
        jan_comboxbox_add(combobox, L"Ik ben stom!");
        jan_comboxbox_add(combobox, L"Ik ben dinga!");
        jan_comboxbox_add(combobox, L"Ik ben slim!");

        return 0;
    }

    if (msg == WM_COMMAND) {
        uint16_t id = LOWORD(wParam);

        if (id == 1) {
            JanWidget *menu_page = jan_container_find(JAN_CONTAINER(window->root), 100);
            jan_widget_set_visible(menu_page, false);
            JanWidget *settings_page = jan_container_find(JAN_CONTAINER(window->root), 101);
            jan_widget_set_visible(settings_page, true);

            // Redraw window
            jan_widget_measure(window->root);
            InvalidateRect(hwnd, NULL, true);
        }

        if (id == 2) {
            MessageBoxW(hwnd, L"Made by Bastiaan van der Plaat", L"About", MB_OK);
        }

        if (id == 3) {
            DestroyWindow(hwnd);
        }

        if (id == 4) {
            JanWidget *menu_page = jan_container_find(JAN_CONTAINER(window->root), 100);
            jan_widget_set_visible(menu_page, true);
            JanWidget *settings_page = jan_container_find(JAN_CONTAINER(window->root), 101);
            jan_widget_set_visible(settings_page, false);

            // Redraw window
            jan_widget_measure(window->root);
            InvalidateRect(hwnd, NULL, true);
        }

        if (id == 201) {
            JanEdit *name_edit = JAN_EDIT(jan_container_find(JAN_CONTAINER(window->root), 200));
            wchar_t string_buffer[128];
            wsprintfW(string_buffer, L"Hello %s!", jan_label_get_text(JAN_LABEL(name_edit)));
            MessageBoxW(hwnd, string_buffer, string_buffer, MB_OK);
        }
    }

    if (msg == WM_SIZE) {
        // Save new window size
        jan_width = LOWORD(lParam);
        jan_height = HIWORD(lParam);

        // Resize widgets
        jan_widget_measure(window->root);
        return 0;
    }

    if (msg == WM_GETMINMAXINFO) {
        // Set window min size
        MINMAXINFO *minMaxInfo = (MINMAXINFO *)lParam;
        RECT window_rect = { 0, 0, 320, 240 };
        AdjustWindowRectEx(&window_rect, WINDOW_STYLE, false, 0);
        minMaxInfo->ptMinTrackSize.x = window_rect.right - window_rect.left;
        minMaxInfo->ptMinTrackSize.y = window_rect.bottom - window_rect.top;
        return 0;
    }

    if (msg == WM_PAINT) {
        PAINTSTRUCT paint_struct;
        HDC hdc = BeginPaint(hwnd, &paint_struct);
        jan_widget_draw(window->root, hdc);
        EndPaint(hwnd, &paint_struct);
        return 0;
    }

    if (msg == WM_DESTROY) {
        // Free window data
        jan_widget_free(window->root);
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
    gdiplusStartupInput.SuppressBackgroundThread = false;
    gdiplusStartupInput.SuppressExternalCodecs = false;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.hIcon = LoadIconW(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(17, 17, 17));
    wc.lpszClassName = window_class_name;
    wc.hIconSm = wc.hIcon;
    RegisterClassExW(&wc);

    RECT window_rect;
    window_rect.left = (GetSystemMetrics(SM_CXSCREEN) - WINDOW_WIDTH) / 2;
    window_rect.top = (GetSystemMetrics(SM_CYSCREEN) - WINDOW_HEIGHT) / 2;
    window_rect.right = window_rect.left + WINDOW_WIDTH;
    window_rect.bottom = window_rect.top + WINDOW_HEIGHT;
    AdjustWindowRectEx(&window_rect, WINDOW_STYLE, false, 0);

    HWND hwnd = CreateWindowExW(0, window_class_name, window_title,
        WINDOW_STYLE, window_rect.left, window_rect.top,
        window_rect.right - window_rect.left, window_rect.bottom - window_rect.top,
        NULL, NULL, wc.hInstance, NULL);
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
