#define WIN32_USE_STDLIB_HELPERS
#define WIN32_USE_STRING_HELPERS
#include "win32.h"

// Jan List
typedef struct JanList {
    void **items;
    size_t size;
    size_t capacity;
} JanList;

JanList *jan_list_new(size_t capacity) {
    JanList *list = malloc(sizeof(JanList));
    list->items = malloc(sizeof(void *) * capacity);
    list->size = 0;
    list->capacity = capacity;
    return list;
}

void jan_list_add(JanList *list, void *item) {
    list->items[list->size++] = item;
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->items = realloc(list->items, sizeof(void *) * list->capacity);
    }
}

void jan_list_free(JanList *list, void (*free_function)(void *item)) {
    if (free_function != NULL) {
        for (size_t i = 0; i < list->size; i++) {
            free_function(list->items[i]);
        }
    }
    free(list->items);
    free(list);
}

// Jan Color
typedef struct JanColor {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
} JanColor;

// Jan Rect
typedef struct JanRect {
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
} JanRect;

// Jan Orientation
typedef enum JanOrientation {
    ORIENTATION_HORIZONTAL = 0,
    ORIENTATION_VERTICAL
} JanOrientation;

// Jan Cavas
typedef struct JanCanvas {
    HDC hdc;
} JanCanvas;

JanCanvas *jan_canvas_new(HDC hdc) {
    JanCanvas *canvas = malloc(sizeof(JanCanvas));
    canvas->hdc = hdc;
    return canvas;
}

void jan_canvas_fill_rect(JanCanvas *canvas, JanRect *rect, JanColor *color) {
    HBRUSH brush = CreateSolidBrush(RGB(color->red, color->green, color->blue));
    RECT other_rect = { rect->x, rect->y, rect->x + rect->width, rect->y + rect->height };
    FillRect(canvas->hdc, &other_rect, brush);
    DeleteObject(brush);
}

void jan_canvas_free(JanCanvas *canvas) {
    free(canvas);
}

// Jan Widget
#define JAN_WIDGET_WRAP_CONTENT -1
#define JAN_WIDGET_MATCH_PARENT -2

typedef struct JanWidget {
    int32_t width;
    int32_t height;
    JanColor backgroundColor;
    JanRect contentRect;
    void (*measure_function)(struct JanWidget *widget, uint32_t parentWidth, uint32_t parentHeight);
    void (*place_function)(struct JanWidget *widget, int32_t x, int32_t y);
    void (*draw_function)(struct JanWidget *widget, JanCanvas *canvas);
    void (*free_function)(struct JanWidget *widget);
} JanWidget;

void jan_widget_width(JanWidget *widget, int32_t width) {
    widget->width = width;
}

void jan_widget_height(JanWidget *widget, int32_t height) {
    widget->height = height;
}

void jan_widget_background_color(JanWidget *widget, JanColor backgroundColor) {
    widget->backgroundColor = backgroundColor;
}

void jan_widget_measure(JanWidget *widget, uint32_t parentWidth, uint32_t parentHeight){
    if (widget->width == JAN_WIDGET_WRAP_CONTENT) {
        widget->contentRect.width = 0;
    } else if (widget->width == JAN_WIDGET_MATCH_PARENT) {
        widget->contentRect.width = parentWidth;
    } else {
        widget->contentRect.width = widget->width;
    }

    if (widget->height == JAN_WIDGET_WRAP_CONTENT) {
        widget->contentRect.height = 0;
    } else if (widget->height == JAN_WIDGET_MATCH_PARENT) {
        widget->contentRect.height = parentHeight;
    } else {
        widget->contentRect.height = widget->height;
    }
}

void jan_widget_place(JanWidget *widget, int32_t x, int32_t y) {
    widget->contentRect.x = x;
    widget->contentRect.y = y;
}

void jan_widget_draw(JanWidget *widget, JanCanvas *canvas) {
    jan_canvas_fill_rect(canvas, &widget->contentRect, &widget->backgroundColor);
}

void jan_widget_free(JanWidget *widget) {
    free(widget);
}

void jan_widget_init(JanWidget *widget) {
    widget->width = JAN_WIDGET_MATCH_PARENT;
    widget->height = JAN_WIDGET_MATCH_PARENT;
    widget->backgroundColor.red = 0;
    widget->backgroundColor.green = 0;
    widget->backgroundColor.blue = 0;
    widget->backgroundColor.alpha = 255;
    widget->measure_function = jan_widget_measure;
    widget->place_function = jan_widget_place;
    widget->draw_function = jan_widget_draw;
    widget->free_function = jan_widget_free;
}

JanWidget *jan_widget_new(void) {
    JanWidget *widget = malloc(sizeof(JanWidget));
    jan_widget_init(widget);
    return widget;
}

// Jan Container
typedef struct JanContainer {
    JanWidget super;
    JanList children;
} JanContainer;

// Jan Box
typedef struct JanBox {
    JanContainer super;
    JanOrientation orientation;
} JanBox;

// Jan Label
typedef struct JanLabel {
    JanWidget super;
    char *text;
} JanLabel;

// =======================================================================

char *window_class_name = "window-test";
char *window_title = "This is a test window (32-bit)";
uint32_t window_width = 800;
uint32_t window_height = 600;

typedef struct {
    uint32_t background_color;
    JanWidget *root;
} WindowData;

int32_t __stdcall WndProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_CREATE) {
        // Create window data
        WindowData *window_data = malloc(sizeof(WindowData));
        SetWindowLongA(hwnd, GWLP_USERDATA, window_data);

        // Generate random seed by time and
        SYSTEMTIME time;
        GetLocalTime(&time);
        srand((time.wHour * 60 + time.wMinute) * 60 + time.wSecond);

        // Generate random background color
        window_data->background_color = rand() & 0x007f7f7f;

        // Center window
        RECT window_rect;
        GetClientRect(hwnd, &window_rect);
        uint32_t new_width = window_width * 2 - window_rect.right;
        uint32_t new_height = window_height * 2 - window_rect.bottom;
        SetWindowPos(hwnd, NULL, (GetSystemMetrics(SM_CXSCREEN) - new_width) / 2, (GetSystemMetrics(SM_CYSCREEN) - new_height) / 2, new_width, new_height, SWP_NOZORDER);

        // Create widget
        window_data->root = jan_widget_new();
        jan_widget_width(window_data->root, JAN_WIDGET_MATCH_PARENT);
        jan_widget_height(window_data->root, 100);
        JanColor color = { 255, 128, 0, 255};
        jan_widget_background_color(window_data->root, color);

        window_data->root->measure_function(window_data->root, window_width, window_height);
        window_data->root->place_function(window_data->root, 0, 0);

        return 0;
    }

    if (msg == WM_SIZE) {
        // Save new window size
        window_width = LOWORD(lParam);
        window_height = HIWORD(lParam);
        return 0;
    }

    if (msg == WM_GETMINMAXINFO) {
        // Set window min size
        MINMAXINFO *minMaxInfo = (MINMAXINFO *)lParam;
        minMaxInfo->ptMinTrackSize.x = 320;
        minMaxInfo->ptMinTrackSize.y = 240;
        return 0;
    }

    if (msg == WM_ERASEBKGND) {
        // Draw no background
        return TRUE;
    }

    if (msg == WM_PAINT) {
        WindowData *window_data = GetWindowLongA(hwnd, GWLP_USERDATA);

        PAINTSTRUCT paint_struct;
        HDC hdc = BeginPaint(hwnd, &paint_struct);

        // Create back buffer
        HDC hdc_buffer = CreateCompatibleDC(hdc);
        HBITMAP bitmap_buffer = CreateCompatibleBitmap(hdc, window_width, window_height);
        SelectObject(hdc_buffer, bitmap_buffer);

        // Draw background color
        HBRUSH brush = CreateSolidBrush(window_data->background_color);
        RECT rect = { 0, 0, window_width, window_height };
        FillRect(hdc_buffer, &rect, brush);
        DeleteObject(brush);

        // Draw centered text
        uint32_t font_size = window_width / 16;
        HFONT font =  CreateFontA(font_size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Comic Sans MS");
        SelectObject(hdc_buffer, font);
        SetTextAlign(hdc_buffer, TA_CENTER);
        SetBkMode(hdc_buffer,TRANSPARENT);
        SetTextColor(hdc_buffer, 0x00ffffff);
        TextOutA(hdc_buffer, window_width / 2, (window_height - font_size) / 2, window_title, strlen(window_title));
        DeleteObject(font);

        // Draw jan widget
        JanCanvas *canvas = jan_canvas_new(hdc_buffer);
        jan_widget_draw(window_data->root, canvas);
        jan_canvas_free(canvas);

        // Draw and delete back buffer
        BitBlt(hdc, 0, 0, window_width, window_height, hdc_buffer, 0, 0, SRCCOPY);
        DeleteObject(bitmap_buffer);
        DeleteDC(hdc_buffer);

        EndPaint(hwnd, &paint_struct);
        return 0;
    }

    if (msg == WM_DESTROY) {
        // Free window data
        free(GetWindowLongA(hwnd, GWLP_USERDATA));

        // Close process
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

void _start(void) {
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
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        window_width, window_height, NULL, NULL, wc.hInstance, NULL);
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    MSG message;
    while (GetMessageA(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
    ExitProcess((int32_t)(uintptr_t)message.wParam);
}
