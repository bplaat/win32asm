#define WIN32_MALLOC
#define WIN32_REALLOC
#define WIN32_FREE
#define WIN32_RAND
#define WIN32_WCSLEN
#define WIN32_WCSDUP
#include "win32.h"

// Types
typedef uint32_t Color;

typedef struct Rect {
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
} Rect;

typedef struct Offset {
    int32_t top;
    int32_t left;
    int32_t right;
    int32_t bottom;
} Offset;

typedef enum Orientation {
    ORIENTATION_HORIZONTAL,
    ORIENTATION_VERTICAL
} Orientation;

typedef enum HorizontalAlign {
    HORIZONTAL_ALIGN_LEFT,
    HORIZONTAL_ALIGN_CENTER,
    HORIZONTAL_ALIGN_RIGHT
} HorizontalAlign;

typedef enum VerticalAlign {
    VERTICAL_ALIGN_TOP,
    VERTICAL_ALIGN_CENTER,
    VERTICAL_ALIGN_BOTTOM
} VerticalAlign;

// List
typedef struct List {
    void **items;
    uint32_t capacity;
    uint32_t size;
} List;

void list_init(List *list, uint32_t capacity);

List *list_new(uint32_t capacity) {
    List *list = malloc(sizeof(List));
    list_init(list, capacity);
    return list;
}

void list_init(List *list, uint32_t capacity) {
    list->items = malloc(capacity * sizeof(void *));
    list->capacity = capacity;
    list->size = 0;
}

void list_add(List *list, void *item) {
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->items = realloc(list->items, list->capacity * sizeof(void *));
    }
    list->items[list->size++] = item;
}

// Font
typedef struct Font {
    wchar_t *name;
    uint32_t size;
    uint32_t weight;
} Font;

Font *global_font;

void font_init(Font *font, wchar_t *name, uint32_t size, uint32_t weight);

Font *font_new(wchar_t *name, uint32_t size, uint32_t weight) {
    Font *font = malloc(sizeof(Font));
    font_init(font, name, size, weight);
    return font;
}

void font_init(Font *font, wchar_t *name, uint32_t size, uint32_t weight) {
    font->name = wcsdup(name);
    font->size = size;
    font->weight = weight;
}

Font *font_copy(Font *font) {
    return font_new(font->name, font->size, font->weight);
}

HFONT font_get_handle(Font *font) {
    return CreateFontW(font->size, 0, 0, 0, font->weight, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font->name);
}

void font_free(Font *font) {
    free(font->name);
    free(font);
}

// Widget
#define WIDGET_UNDEFINED -1
#define WIDGET_WRAP_CONTENT -2
#define WIDGET_MATCH_PARENT -3

#define WIDGET(ptr) ((Widget *)ptr)
typedef struct Widget {
    int32_t width;
    int32_t height;
    Color background_color;
    // Offset margin;
    // Offset padding;

    Rect content_rect;
    // Rect padding_rect;
    // Rect margin_rect;
    void (*measure_function)(struct Widget *widget, uint32_t parent_width, uint32_t parent_height);
    void (*place_function)(struct Widget *widget, int32_t x, int32_t y);
    void (*draw_function)(struct Widget *widget, HDC hdc);
    void (*free_function)(struct Widget *widget);
} Widget;

void widget_init(Widget *widget);

Widget *widget_new(void) {
    Widget *widget = malloc(sizeof(Widget));
    widget_init(widget);
    return widget;
}

void widget_measure(Widget *widget, uint32_t parent_width, uint32_t parent_height);
void widget_place(Widget *widget, int32_t x, int32_t y);
void widget_draw(Widget *widget, HDC hdc);
void widget_free(Widget *widget);

void widget_init(Widget *widget) {
    widget->width = WIDGET_UNDEFINED;
    widget->height = WIDGET_UNDEFINED;
    widget->background_color = 0;
    widget->measure_function = widget_measure;
    widget->place_function = widget_place;
    widget->draw_function = widget_draw;
    widget->free_function = widget_free;
}

int32_t widget_get_width(Widget *widget) {
    return widget->width;
}

void widget_set_width(Widget *widget, int32_t width) {
    widget->width = width;
}

int32_t widget_get_height(Widget *widget) {
    return widget->height;
}

void widget_set_height(Widget *widget, int32_t height) {
    widget->height = height;
}

Color widget_get_background_color(Widget *widget) {
    return widget->background_color;
}

void widget_set_background_color(Widget *widget, Color background_color) {
    widget->background_color = background_color;
}

void widget_measure(Widget *widget, uint32_t parent_width, uint32_t parent_height) {
    if (widget->width == WIDGET_WRAP_CONTENT) {
        widget->content_rect.width = 0;
    } else if (widget->width == WIDGET_MATCH_PARENT) {
        widget->content_rect.width = parent_width;
    } else {
        widget->content_rect.width = widget->width;
    }

    if (widget->height == WIDGET_WRAP_CONTENT) {
        widget->content_rect.height = 0;
    } else if (widget->height == WIDGET_MATCH_PARENT) {
        widget->content_rect.height = parent_height;
    } else {
        widget->content_rect.height = widget->height;
    }
}

void widget_place(Widget *widget, int32_t x, int32_t y) {
    widget->content_rect.x = x;
    widget->content_rect.y = y;
}

void widget_draw(Widget *widget, HDC hdc) {
    if (widget->background_color != 0) {
        HBRUSH brush = CreateSolidBrush(widget->background_color);
        RECT rect = { widget->content_rect.x, widget->content_rect.y,
            widget->content_rect.x + widget->content_rect.width,
            widget->content_rect.y + widget->content_rect.height };
        FillRect(hdc, &rect, brush);
        DeleteObject(brush);
    }
}

void widget_free(Widget *widget) {
    free(widget);
}

// Container
#define CONTAINER_WIDGETS_INIT_CAPACITY 4

#define CONTAINER(ptr) ((Container *)ptr)
typedef struct Container {
    Widget super;
    List widgets;
} Container;

void container_init(Container *container) {
    Widget *widget = &container->super;
    widget_init(widget);
    list_init(&container->widgets, CONTAINER_WIDGETS_INIT_CAPACITY);
}

void container_add(Container *container, Widget *ptr) {
    list_add(&container->widgets, ptr);
}

void container_free(Widget *widget) {
    Container *container = CONTAINER(widget);
    for (uint32_t i = 0; i < container->widgets.size; i++) {
        Widget *other_widget = container->widgets.items[i];
        other_widget->free_function(other_widget);
    }
    widget_free(widget);
}

// Box
#define BOX(ptr) ((Box *)ptr)
typedef struct Box {
    Container super;
    Orientation orientation;
    // HorizontalAlign horizontal_align;
    // VerticalAlign vertical_align;
} Box;

void box_init(Box *box, Orientation orientation);
void box_measure(Widget *widget, uint32_t parent_width, uint32_t parent_height);
void box_place(Widget *widget, int32_t x, int32_t y);
void box_draw(Widget *widget, HDC hdc);
void box_free(Widget *widget);

Box *box_new(Orientation orientation) {
    Box *box = malloc(sizeof(Box));
    box_init(box, orientation);
    return box;
}

void box_init(Box *box, Orientation orientation) {
    Container *container = &box->super;
    Widget *widget = &container->super;
    container_init(container);

    box->orientation = orientation;

    widget->measure_function = box_measure;
    widget->place_function = box_place;
    widget->draw_function = box_draw;
    widget->free_function = box_free;
}

Orientation box_get_orientation(Box *box) {
    return box->orientation;
}

void box_set_orientation(Box *box, Orientation orientation) {
    box->orientation = orientation;
}

void box_measure(Widget *widget, uint32_t parent_width, uint32_t parent_height) {
    Box *box = BOX(widget);
    Container *container = CONTAINER(widget);

    uint32_t sum_width = 0;
    uint32_t max_width = 0;
    uint32_t sum_height = 0;
    uint32_t max_height = 0;
    for (uint32_t i = 0; i < container->widgets.size; i++) {
        Widget *other_widget = container->widgets.items[i];
        if (other_widget->width == WIDGET_UNDEFINED) {
            if (box->orientation == ORIENTATION_HORIZONTAL) {
                other_widget->width = WIDGET_WRAP_CONTENT;
            }
            if (box->orientation == ORIENTATION_VERTICAL) {
                other_widget->width = WIDGET_MATCH_PARENT;
            }
        }
        if (other_widget->height == WIDGET_UNDEFINED) {
            if (box->orientation == ORIENTATION_HORIZONTAL) {
                other_widget->height = WIDGET_MATCH_PARENT;
            }
            if (box->orientation == ORIENTATION_VERTICAL) {
                other_widget->height = WIDGET_WRAP_CONTENT;
            }
        }

        other_widget->measure_function(other_widget, widget->content_rect.width, widget->content_rect.height);
        sum_width += other_widget->content_rect.width;
        max_width = MAX(max_width, other_widget->content_rect.width);
        sum_height += other_widget->content_rect.height;
        max_height = MAX(max_height, other_widget->content_rect.height);
    }

    if (widget->width == WIDGET_WRAP_CONTENT) {
        if (box->orientation == ORIENTATION_HORIZONTAL) {
            widget->content_rect.width = sum_width;
        }
        if (box->orientation == ORIENTATION_VERTICAL) {
            widget->content_rect.width = max_width;
        }
    } else if (widget->width == WIDGET_MATCH_PARENT) {
        widget->content_rect.width = parent_width;
    } else {
        widget->content_rect.width = widget->width;
    }

    if (widget->height == WIDGET_WRAP_CONTENT) {
        if (box->orientation == ORIENTATION_HORIZONTAL) {
            widget->content_rect.height = max_height;
        }
        if (box->orientation == ORIENTATION_VERTICAL) {
            widget->content_rect.height = sum_height;
        }
    } else if (widget->height == WIDGET_MATCH_PARENT) {
        widget->content_rect.height = parent_height;
    } else {
        widget->content_rect.height = widget->height;
    }
}

void box_place(Widget *widget, int32_t x, int32_t y) {
    Box *box = BOX(widget);
    Container *container = CONTAINER(widget);

    widget->content_rect.x = x;
    widget->content_rect.y = y;

    for (uint32_t i = 0; i < container->widgets.size; i++) {
        Widget *other_widget = container->widgets.items[i];

        other_widget->place_function(other_widget, x, y);

        if (box->orientation == ORIENTATION_HORIZONTAL) {
            x += other_widget->content_rect.width;
        }
        if (box->orientation == ORIENTATION_VERTICAL) {
            y += other_widget->content_rect.height;
        }
    }
}

void box_draw(Widget *widget, HDC hdc) {
    Box *box = BOX(widget);
    Container *container = CONTAINER(widget);

    widget_draw(widget, hdc);

    for (uint32_t i = 0; i < container->widgets.size; i++) {
        Widget *other_widget = container->widgets.items[i];
        other_widget->draw_function(other_widget, hdc);
    }
}

void box_free(Widget *widget) {
    container_free(widget);
}

// Label
#define LABEL(ptr) ((Label *)ptr)
typedef struct Label {
    Widget super;
    wchar_t *text;
    Font *font;
    Color text_color;
    bool single_line;
    HorizontalAlign horizontal_align;
    // VerticalAlign vertical_align;
} Label;

void label_init(Label *label, wchar_t *text);
void label_measure(Widget *widget, uint32_t parent_width, uint32_t parent_height);
void label_draw(Widget *widget, HDC hdc);
void label_free(Widget *widget);

Label *label_new(wchar_t *text) {
    Label *label = malloc(sizeof(Label));
    label_init(label, text);
    return label;
}

void label_init(Label *label, wchar_t *text) {
    Widget *widget = WIDGET(label);
    widget_init(widget);

    label->text = wcsdup(text);
    if (global_font == NULL) {
        global_font = font_new(L"Tamoha", 20, 400);
    }
    label->font = global_font;
    label->text_color = 0x00111111;
    label->single_line = false;
    label->horizontal_align = HORIZONTAL_ALIGN_LEFT;

    widget->measure_function = label_measure;
    widget->draw_function = label_draw;
    widget->free_function = label_free;
}

wchar_t *label_get_text(Label *label) {
    return label->text;
}

void label_set_text(Label *label, wchar_t *text) {
    label->text = text;
}

Font *label_get_font(Label *label) {
    return label->font;
}

void label_set_font(Label *label, Font *font) {
    label->font = font_copy(font);
}

Color label_get_text_color(Label *label) {
    return label->text_color;
}

void label_set_text_color(Label *label, Color text_color) {
    label->text_color = text_color;
}

bool label_get_single_line(Label *label) {
    return label->single_line;
}

void label_set_single_line(Label *label, bool single_line) {
    label->single_line = single_line;
}

HorizontalAlign label_get_horizontal_align(Label *label) {
    return label->horizontal_align;
}

void label_set_horizontal_align(Label *label, HorizontalAlign horizontal_align) {
    label->horizontal_align = horizontal_align;
}

void label_measure(Widget *widget, uint32_t parent_width, uint32_t parent_height) {
    Label *label = LABEL(widget);

    if (widget->width == WIDGET_WRAP_CONTENT) {
        HDC hdc = GetDC(NULL);
        HFONT font = font_get_handle(label->font);
        SelectObject(hdc, font);
        RECT rect = { 0, 0, 0, 0 };
        DrawTextW(hdc, label->text, -1, &rect, DT_CALCRECT);
        DeleteObject(font);
        widget->content_rect.width = rect.right - rect.left;
    } else if (widget->width == WIDGET_MATCH_PARENT) {
        widget->content_rect.width = parent_width;
    } else {
        widget->content_rect.width = widget->width;
    }

    if (widget->height == WIDGET_WRAP_CONTENT) {
        if (label->single_line) {
            widget->content_rect.height = label->font->size;
        } else {
            HDC hdc = GetDC(NULL);
            HFONT font = font_get_handle(label->font);
            SelectObject(hdc, font);
            RECT rect = { 0, 0, widget->content_rect.width, 0 };
            widget->content_rect.height = DrawTextW(hdc, label->text, -1, &rect, DT_CALCRECT | DT_WORDBREAK);
            DeleteObject(font);
        }
    } else if (widget->height == WIDGET_MATCH_PARENT) {
        widget->content_rect.height = parent_height;
    } else {
        widget->content_rect.height = widget->height;
    }
}

void label_draw(Widget *widget, HDC hdc) {
    Label *label = LABEL(widget);
    widget_draw(widget, hdc);

    HFONT font = font_get_handle(label->font);
    SelectObject(hdc, font);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, label->text_color);
    RECT rect = { widget->content_rect.x, widget->content_rect.y,
        widget->content_rect.x + widget->content_rect.width,
        widget->content_rect.y + widget->content_rect.height };
    if (label->single_line) {
        if (label->horizontal_align == HORIZONTAL_ALIGN_LEFT) {
            SetTextAlign(hdc, TA_LEFT);
        }
        if (label->horizontal_align == HORIZONTAL_ALIGN_CENTER) {
            rect.left += widget->content_rect.width / 2;
            SetTextAlign(hdc, TA_CENTER);
        }
        if (label->horizontal_align == HORIZONTAL_ALIGN_RIGHT) {
            rect.left += widget->content_rect.width;
            SetTextAlign(hdc, TA_RIGHT);
        }
        TextOutW(hdc, rect.left, rect.top, label->text, wcslen(label->text));
    } else {
        SetTextAlign(hdc, TA_LEFT);
        DrawTextW(hdc, label->text, -1, &rect, label->horizontal_align | DT_WORDBREAK);
    }
    DeleteObject(font);
}

void label_free(Widget *widget) {
    Label *label = LABEL(widget);
    free(label->text);
    font_free(label->font);
    widget_free(widget);
}

// Button
#define BUTTON(ptr) ((Button *)ptr)
typedef struct Button {
    Label super;
    HWND hwnd;
    HFONT hfont;
} Button;

HWND global_hwnd;

void button_init(Button *button, wchar_t *text);
void button_measure(Widget *widget, uint32_t parent_width, uint32_t parent_height);
void button_place(Widget *widget, int32_t x, int32_t y);
void button_draw(Widget *widget, HDC hdc);
void button_free(Widget *widget);

Button *button_new(wchar_t *text) {
    Button *button = malloc(sizeof(Button));
    button_init(button, text);
    return button;
}

void button_init(Button *button, wchar_t *text) {
    Label *label = LABEL(button);
    Widget *widget = WIDGET(button);
    label_init(label, text);
    label->single_line = true;
    label->horizontal_align = HORIZONTAL_ALIGN_CENTER;

    button->hwnd = CreateWindowExW(0, L"BUTTON", label->text, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, global_hwnd, NULL, NULL, NULL);
    button->hfont = NULL;

    widget->measure_function = button_measure;
    widget->place_function = button_place;
    widget->draw_function = button_draw;
    widget->free_function = button_free;
}

void button_measure(Widget *widget, uint32_t parent_width, uint32_t parent_height) {
    Button *button = BUTTON(widget);
    label_measure(widget, parent_width, parent_height);
    SetWindowPos(button->hwnd, NULL, 0, 0, widget->content_rect.width, widget->content_rect.height, SWP_NOMOVE | SWP_NOZORDER);
}

void button_place(Widget *widget, int32_t x, int32_t y) {
    Button *button = BUTTON(widget);
    widget_place(widget, x, y);
    SetWindowPos(button->hwnd, NULL, widget->content_rect.x, widget->content_rect.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void button_draw(Widget *widget, HDC hdc) {
    Button *button = BUTTON(widget);
    Label *label = LABEL(widget);

    if (button->hfont == NULL) {
        button->hfont = font_get_handle(label->font);
        SendMessageW(button->hwnd, WM_SETFONT, button->hfont, (LPARAM)TRUE);
    }
}

void button_free(Widget *widget) {
    Button *button = BUTTON(widget);
    Label *label = LABEL(widget);
    DeleteObject(button->hfont);
    DestroyWindow(button->hwnd);
    label_free(WIDGET(label));
}

// ####################################################################################################

wchar_t *window_class_name = L"window-test";

#ifdef WIN64
    wchar_t *window_title = L"This is a test window 😍 (64-bit)";
#else
    wchar_t *window_title = L"This is a test window 😍 (32-bit)";
#endif

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

typedef struct {
    uint32_t width;
    uint32_t height;
    Widget *root;
} WindowData;

int32_t __stdcall WndProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam) {
    WindowData *window = GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    if (msg == WM_CREATE) {
        global_hwnd = hwnd;

        // Create window data
        window = malloc(sizeof(WindowData));
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, window);
        window->width = WINDOW_WIDTH;
        window->height = WINDOW_HEIGHT;

        // Generate random seed by time
        SYSTEMTIME time;
        GetLocalTime(&time);
        srand((time.wHour * 60 + time.wMinute) * 60 + time.wSecond);

        // Create widgets
        Box *root = box_new(ORIENTATION_VERTICAL);
        widget_set_width(WIDGET(root), WIDGET_MATCH_PARENT);
        widget_set_height(WIDGET(root), WIDGET_MATCH_PARENT);
        widget_set_background_color(WIDGET(root), rand() & 0x007f7f7f);
        window->root = WIDGET(root);

        Font *font = font_new(L"Georgia", 24, 400);

        Label *header = label_new(L"Lorem ipsum dolor sit amet consectetur adipisicing elit. Odit, ipsa? Recusandae, aut impedit illum ducimus odit porro necessitatibus exercitationem iusto eaque voluptatum ipsam, magnam similique quia consequatur vel repudiandae perspiciatis minima. Doloribus, blanditiis totam sint fugiat alias magni recusandae nulla odit natus, ut quo at doloremque voluptas sequi autem! Iste!");
        label_set_font(header, font);
        label_set_text_color(header, 0x00ffffff);
        widget_set_background_color(WIDGET(header), 0x000000ff);
        label_set_horizontal_align(header, HORIZONTAL_ALIGN_CENTER);
        container_add(CONTAINER(root), WIDGET(header));

        for (int32_t y = 1; y <= 10; y++) {
            Box *row = box_new(ORIENTATION_HORIZONTAL);
            widget_set_width(WIDGET(row), WIDGET_WRAP_CONTENT);
            widget_set_background_color(WIDGET(row), 0x0000ffff);
            container_add(CONTAINER(root), WIDGET(row));

            for (int32_t x = 1; x <= 10; x++) {
                wchar_t string_buffer[64];
                wsprintfW(string_buffer, L"Item %dx%d", x, y);
                Label *item = x % 2 == y % 2 ? label_new(string_buffer) : LABEL(button_new(string_buffer));
                widget_set_width(WIDGET(item), 125);
                widget_set_height(WIDGET(item), WIDGET_WRAP_CONTENT);
                label_set_horizontal_align(item, HORIZONTAL_ALIGN_CENTER);
                label_set_single_line(item, true);
                container_add(CONTAINER(row), WIDGET(item));
            }
        }

        Button *button = button_new(L"Click me to!");
        label_set_font(LABEL(button), font);
        container_add(CONTAINER(root), WIDGET(button));

        Label *footer = label_new(L"Lorem ipsum dolor sit amet consectetur adipisicing elit. Odit, ipsa? Recusandae, aut impedit illum ducimus odit porro necessitatibus exercitationem iusto eaque voluptatum ipsam, magnam similique quia consequatur vel repudiandae perspiciatis minima. Doloribus, blanditiis totam sint fugiat alias magni recusandae nulla odit natus, ut quo at doloremque voluptas sequi autem! Iste!");
        label_set_font(footer, font);
        label_set_text_color(footer, 0x00ffffff);
        widget_set_background_color(WIDGET(footer), 0x00ff0000);
        label_set_horizontal_align(footer, HORIZONTAL_ALIGN_RIGHT);
        container_add(CONTAINER(root), WIDGET(footer));

        font_free(font);

        // Resize window to right size and center it
        RECT window_rect;
        GetClientRect(hwnd, &window_rect);
        uint32_t new_width = window->width * 2 - window_rect.right;
        uint32_t new_height = window->height * 2 - window_rect.bottom;
        SetWindowPos(hwnd, NULL, (GetSystemMetrics(SM_CXSCREEN) - new_width) / 2, (GetSystemMetrics(SM_CYSCREEN) - new_height) / 2, new_width, new_height, SWP_NOZORDER);
        return 0;
    }

    if (msg == WM_SIZE) {
        // Save new window size
        window->width = LOWORD(lParam);
        window->height = HIWORD(lParam);

        // Resize widgets
        window->root->measure_function(window->root, window->width, window->height);
        window->root->place_function(window->root, 0, 0);
        return 0;
    }

    if (msg == WM_GETMINMAXINFO) {
        // Set window min size
        MINMAXINFO *minMaxInfo = (MINMAXINFO *)lParam;
        minMaxInfo->ptMinTrackSize.x = 640;
        minMaxInfo->ptMinTrackSize.y = 480;
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
        HBITMAP bitmap_buffer = CreateCompatibleBitmap(hdc, window->width, window->height);
        SelectObject(hdc_buffer, bitmap_buffer);

        // Draw widgets
        window->root->draw_function(window->root, hdc_buffer);

        // Draw and delete back buffer
        BitBlt(hdc, 0, 0, window->width, window->height, hdc_buffer, 0, 0, SRCCOPY);
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

    HWND hwnd = CreateWindowExW(0, window_class_name, window_title,
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, wc.hInstance, NULL);
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    MSG message;
    while (GetMessageW(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    ExitProcess((int32_t)(uintptr_t)message.wParam);
}
