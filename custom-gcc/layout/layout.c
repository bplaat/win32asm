#define WIN32_MALLOC
#define WIN32_REALLOC
#define WIN32_FREE
#define WIN32_RAND
#define WIN32_WCSLEN
#define WIN32_WCSDUP
#include "win32.h"
#define LAYOUT_DEBUG
#include "layout.h"

wchar_t *window_class_name = L"window-test";

#ifdef WIN64
    wchar_t *window_title = L"This is a test window 😍 (64-bit)";
#else
    wchar_t *window_title = L"This is a test window 😍 (32-bit)";
#endif

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_STYLE WS_OVERLAPPEDWINDOW

typedef struct {
    Widget *root;
} WindowData;

int32_t __stdcall WndProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam) {
    WindowData *window = GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    if (msg == WM_CREATE) {
        // Create window data
        window = malloc(sizeof(WindowData));
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, window);
        global_hwnd = hwnd;
        global_width = WINDOW_WIDTH;
        global_height = WINDOW_HEIGHT;

        // Generate random seed by time
        SYSTEMTIME time;
        GetLocalTime(&time);
        srand((time.wHour * 60 + time.wMinute) * 60 + time.wSecond);

        // Create widgets
        #if TRUE
            HRSRC main_layout_resource = FindResourceW(GetModuleHandleW(NULL), (wchar_t *)1, (wchar_t *)RT_RCDATA);
            HGLOBAL main_layout = LoadResource(GetModuleHandleW(NULL), main_layout_resource);
            layout_load(LockResource(main_layout), &window->root);
        #else
            Box *root = box_new_with_orientation(ORIENTATION_VERTICAL);
            widget_set_width(WIDGET(root), &(Unit){ 100, UNIT_TYPE_PERCENT });
            widget_set_height(WIDGET(root), &(Unit){ 100, UNIT_TYPE_PERCENT });
            widget_set_background_color(WIDGET(root), rand() & 0x007f7f7f);
            Unit padding = { 16, UNIT_TYPE_DP };
            widget_set_padding(WIDGET(root), &padding, &padding, &padding, &padding);
            window->root = WIDGET(root);

            Label *header = label_new_with_text(L"Lorem ipsum dolor sit amet consectetur adipisicing elit. Odit, ipsa? Recusandae, aut impedit illum ducimus odit porro necessitatibus exercitationem iusto eaque voluptatum ipsam, magnam similique quia consequatur vel repudiandae perspiciatis minima. Doloribus, blanditiis totam sint fugiat alias magni recusandae nulla odit natus, ut quo at doloremque voluptas sequi autem! Iste!");
            label_set_text_size(header, &(Unit){ 1.75, UNIT_TYPE_VW });
            label_set_text_color(header, 0x00ffffff);
            widget_set_background_color(WIDGET(header), 0x00ff00ff);
            label_set_align(header, ALIGN_HORIZONTAL_CENTER);

            widget_set_padding(WIDGET(header), &padding, &padding, &padding, &padding);
            Unit zero = { 0, UNIT_TYPE_PX };
            widget_set_margin(WIDGET(header), &zero, &zero, &padding, &zero);
            container_add(CONTAINER(root), WIDGET(header));

            for (int32_t y = 1; y <= 4; y++) {
                Box *row = box_new_with_orientation(ORIENTATION_HORIZONTAL);
                widget_set_margin(WIDGET(row), &zero, &padding, &padding, &padding);
                box_set_align(row, ALIGN_HORIZONTAL_CENTER);
                container_add(CONTAINER(root), WIDGET(row));

                for (int32_t x = 1; x <= 10; x++) {
                    wchar_t string_buffer[64];
                    wsprintfW(string_buffer, L"Item %dx%d", x, y);
                    Label *item = x % 2 == y % 2 ? label_new_with_text(string_buffer) : LABEL(button_new_with_text(string_buffer));
                    widget_set_width(WIDGET(item), &(Unit){ 96, UNIT_TYPE_DP });
                    widget_set_height(WIDGET(item), &(Unit){ 0, UNIT_TYPE_WRAP });
                    widget_set_background_color(WIDGET(item), 0x0000ffff);
                    label_set_font_underline(item, true);
                    label_set_align(item, ALIGN_HORIZONTAL_CENTER);
                    label_set_single_line(item, true);
                    Unit item_padding = { 8, UNIT_TYPE_DP };
                    widget_set_padding(WIDGET(item), &item_padding, &zero, &item_padding, &zero);
                    container_add(CONTAINER(row), WIDGET(item));
                }
            }

            Button *button = button_new_with_text(L"Click me to!");
            widget_set_padding(WIDGET(button), &padding, &padding, &padding, &padding);
            label_set_font_name(LABEL(button), L"Georgia");
            label_set_font_line_through(LABEL(button), true);
            widget_set_margin(WIDGET(button), &zero, &zero, &padding, &zero);
            container_add(CONTAINER(root), WIDGET(button));

            Label *footer = label_new_with_text(L"Lorem ipsum dolor sit amet consectetur adipisicing elit. Odit, ipsa? Recusandae, aut impedit illum ducimus odit porro necessitatibus exercitationem iusto eaque voluptatum ipsam, magnam similique quia consequatur vel repudiandae perspiciatis minima. Doloribus, blanditiis totam sint fugiat alias magni recusandae nulla odit natus, ut quo at doloremque voluptas sequi autem! Iste!");
            label_set_font_weight(footer, FONT_WEIGHT_BOLD);
            label_set_font_italic(footer, true);
            label_set_text_color(footer, 0x00ffffff);
            widget_set_background_color(WIDGET(footer), 0x00ff0000);
            label_set_align(footer, ALIGN_HORIZONTAL_CENTER);
            container_add(CONTAINER(root), WIDGET(footer));
        #endif
        return 0;
    }

    if (msg == WM_COMMAND) {
        uint16_t id = LOWORD(wParam);
        if (id == 1) {
            MessageBoxW(hwnd, L"Made by Bastiaan van der Plaat", L"About", MB_OK);
        }
        if (id == 2) {
            DestroyWindow(hwnd);
        }
    }

    if (msg == WM_SIZE) {
        // Save new window size
        global_width = LOWORD(lParam);
        global_height = HIWORD(lParam);

        // Resize widgets
        window->root->measure_function(window->root, global_width, global_height);
        window->root->place_function(window->root, 0, 0);
        return 0;
    }

    if (msg == WM_GETMINMAXINFO) {
        // Set window min size
        MINMAXINFO *minMaxInfo = (MINMAXINFO *)lParam;
        RECT window_rect = { 0, 0, 640, 480 };
        AdjustWindowRectEx(&window_rect, WINDOW_STYLE, false, 0);
        minMaxInfo->ptMinTrackSize.x = window_rect.right - window_rect.left;
        minMaxInfo->ptMinTrackSize.y = window_rect.bottom - window_rect.top;
        return 0;
    }

    if (msg == WM_ERASEBKGND) {
        // Draw no background
        return TRUE;
    }

    if (msg == WM_PAINT) {
        PAINTSTRUCT paint_struct;
        HDC hdc = BeginPaint(hwnd, &paint_struct);

        // Create back buffer
        HDC hdc_buffer = CreateCompatibleDC(hdc);
        HBITMAP bitmap_buffer = CreateCompatibleBitmap(hdc, global_width, global_height);
        SelectObject(hdc_buffer, bitmap_buffer);

        // Draw widgets
        window->root->draw_function(window->root, hdc_buffer);

        // Draw and delete back buffer
        BitBlt(hdc, 0, 0, global_width, global_height, hdc_buffer, 0, 0, SRCCOPY);
        DeleteObject(bitmap_buffer);
        DeleteDC(hdc_buffer);

        EndPaint(hwnd, &paint_struct);
        return 0;
    }

    if (msg == WM_DESTROY) {
        // Free window data
        window->root->free_function(window->root);
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

    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.hIcon = LoadIconW(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.lpszClassName = window_class_name;
    wc.hIconSm = wc.hIcon;
    RegisterClassExW(&wc);

    uint32_t x = (GetSystemMetrics(SM_CXSCREEN) - WINDOW_WIDTH) / 2;
    uint32_t y = (GetSystemMetrics(SM_CYSCREEN) - WINDOW_HEIGHT) / 2;
    RECT window_rect = { x, y, x + WINDOW_WIDTH, y + WINDOW_HEIGHT };
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
    ExitProcess((int32_t)(uintptr_t)message.wParam);
}
