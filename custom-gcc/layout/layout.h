#ifndef LAYOUT_H
#define LAYOUT_H

#define WIN32_MALLOC
#define WIN32_REALLOC
#define WIN32_FREE
#define WIN32_WCSLEN
#define WIN32_WCSDUP
#include "win32.h"

// Globals
HWND global_hwnd;
int32_t global_width;
int32_t global_height;

// Color
typedef uint32_t Color;

// Rect
typedef struct Rect {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
} Rect;

// Orientation
#define ORIENTATION_HORIZONTAL 0
#define ORIENTATION_VERTICAL 1
typedef uint32_t Orientation;

// Align
#define ALIGN_HORIZONTAL_LEFT (1 << 0)
#define ALIGN_HORIZONTAL_CENTER (1 << 1)
#define ALIGN_HORIZONTAL_RIGHT (1 << 2)
#define ALIGN_VERTICAL_TOP (1 << 3)
#define ALIGN_VERTICAL_CENTER (1 << 4)
#define ALIGN_VERTICAL_BOTTOM (1 << 5)
typedef uint32_t Align;

// Unit
#define UNIT_TYPE_UNDEFINED 0
#define UNIT_TYPE_PX 1
#define UNIT_TYPE_DP 2
#define UNIT_TYPE_SP 3
#define UNIT_TYPE_VW 4
#define UNIT_TYPE_VH 5
#define UNIT_TYPE_PERCENT 6
#define UNIT_TYPE_WRAP 7
typedef uint32_t UnitType;

typedef struct Unit {
    float value;
    UnitType type;
} Unit;

int32_t unit_to_pixels(Unit *unit, int32_t size) {
    if (unit->type == UNIT_TYPE_PX) return unit->value;
    if (unit->type == UNIT_TYPE_DP) return unit->value * 1.25; // Todo: DPI-aware & Get window DPI
    if (unit->type == UNIT_TYPE_SP) return unit->value * 1.25;
    if (unit->type == UNIT_TYPE_VW) return unit->value * ((float)global_width / 100);
    if (unit->type == UNIT_TYPE_VH) return unit->value * ((float)global_height / 100);
    if (unit->type == UNIT_TYPE_PERCENT) return unit->value * ((float)size / 100);
    return 0;
}

// Offset
typedef struct Offset {
    Unit top;
    Unit right;
    Unit bottom;
    Unit left;
} Offset;

// List
typedef struct List {
    void **items;
    size_t capacity;
    size_t size;
} List;

void list_init(List *list, size_t capacity);

List *list_new(size_t capacity) {
    List *list = malloc(sizeof(List));
    list_init(list, capacity);
    return list;
}

void list_init(List *list, size_t capacity) {
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

// Widget
#define WIDGET(ptr) ((Widget *)ptr)
#define TYPE_WIDGET 1
#define ATTRIBUTE_WIDTH 1
#define ATTRIBUTE_HEIGHT ATTRIBUTE_WIDTH + 1
#define ATTRIBUTE_BACKGROUND_COLOR ATTRIBUTE_HEIGHT + 1
#define ATTRIBUTE_MARGIN ATTRIBUTE_BACKGROUND_COLOR + 1
#define ATTRIBUTE_MARGIN_TOP ATTRIBUTE_MARGIN + 1
#define ATTRIBUTE_MARGIN_LEFT ATTRIBUTE_MARGIN_TOP + 1
#define ATTRIBUTE_MARGIN_RIGHT ATTRIBUTE_MARGIN_LEFT + 1
#define ATTRIBUTE_MARGIN_BOTTOM ATTRIBUTE_MARGIN_RIGHT + 1
#define ATTRIBUTE_PADDING ATTRIBUTE_MARGIN_BOTTOM + 1
#define ATTRIBUTE_PADDING_TOP ATTRIBUTE_PADDING + 1
#define ATTRIBUTE_PADDING_LEFT ATTRIBUTE_PADDING_TOP + 1
#define ATTRIBUTE_PADDING_RIGHT ATTRIBUTE_PADDING_LEFT + 1
#define ATTRIBUTE_PADDING_BOTTOM ATTRIBUTE_PADDING_RIGHT + 1
typedef struct Widget {
    Unit width;
    Unit height;
    Color background_color;
    Offset margin;
    Offset padding;

    int32_t parent_width;
    int32_t parent_height;
    Rect content_rect;
    Rect padding_rect;
    Rect margin_rect;
    bool rect_changed;
    void (*measure_function)(struct Widget *widget, int32_t parent_width, int32_t parent_height);
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

void widget_measure(Widget *widget, int32_t parent_width, int32_t parent_height);
void widget_place(Widget *widget, int32_t x, int32_t y);
void widget_draw(Widget *widget, HDC hdc);
void widget_free(Widget *widget);

void widget_init(Widget *widget) {
    widget->width.type = UNIT_TYPE_UNDEFINED;
    widget->height.type = UNIT_TYPE_UNDEFINED;
    widget->background_color = 0;

    widget->margin.top.value = 0;
    widget->margin.top.type = UNIT_TYPE_PX;
    widget->margin.right.value = 0;
    widget->margin.right.type = UNIT_TYPE_PX;
    widget->margin.bottom.value = 0;
    widget->margin.bottom.type = UNIT_TYPE_PX;
    widget->margin.left.value = 0;
    widget->margin.left.type = UNIT_TYPE_PX;

    widget->padding.top.value = 0;
    widget->padding.top.type = UNIT_TYPE_PX;
    widget->padding.right.value = 0;
    widget->padding.right.type = UNIT_TYPE_PX;
    widget->padding.bottom.value = 0;
    widget->padding.bottom.type = UNIT_TYPE_PX;
    widget->padding.left.value = 0;
    widget->padding.left.type = UNIT_TYPE_PX;

    widget->measure_function = widget_measure;
    widget->place_function = widget_place;
    widget->draw_function = widget_draw;
    widget->free_function = widget_free;
}

Unit *widget_get_width(Widget *widget) {
    return &widget->width;
}

void widget_set_width(Widget *widget, Unit *width) {
    widget->width = *width;
}

Unit *widget_get_height(Widget *widget) {
    return &widget->height;
}

void widget_set_height(Widget *widget, Unit *height) {
    widget->height = *height;
}

Color widget_get_background_color(Widget *widget) {
    return widget->background_color;
}

void widget_set_background_color(Widget *widget, Color background_color) {
    widget->background_color = background_color;
}

Offset *widget_get_margin(Widget *widget) {
    return &widget->margin;
}

void widget_set_margin(Widget *widget, Unit *top, Unit *right, Unit *bottom, Unit *left) {
    widget->margin.top = *top;
    widget->margin.right = *right;
    widget->margin.bottom = *bottom;
    widget->margin.left = *left;
}

Unit *widget_get_margin_top(Widget *widget) {
    return &widget->margin.top;
}

void widget_set_margin_top(Widget *widget, Unit *top) {
    widget->margin.top = *top;
}

Unit *widget_get_margin_right(Widget *widget) {
    return &widget->margin.right;
}

void widget_set_margin_right(Widget *widget, Unit *right) {
    widget->margin.right = *right;
}

Unit *widget_get_margin_bottom(Widget *widget) {
    return &widget->margin.bottom;
}

void widget_set_margin_bottom(Widget *widget, Unit *bottom) {
    widget->margin.bottom = *bottom;
}

Unit *widget_get_margin_left(Widget *widget) {
    return &widget->margin.left;
}

void widget_set_margin_left(Widget *widget, Unit *left) {
    widget->margin.left = *left;
}

Offset *widget_get_padding(Widget *widget) {
    return &widget->padding;
}

void widget_set_padding(Widget *widget, Unit *top, Unit *right, Unit *bottom, Unit *left) {
    widget->padding.top = *top;
    widget->padding.right = *right;
    widget->padding.bottom = *bottom;
    widget->padding.left = *left;
}

Unit *widget_get_padding_top(Widget *widget) {
    return &widget->padding.top;
}

void widget_set_padding_top(Widget *widget, Unit *top) {
    widget->padding.top = *top;
}

Unit *widget_get_padding_right(Widget *widget) {
    return &widget->padding.right;
}

void widget_set_padding_right(Widget *widget, Unit *right) {
    widget->padding.right = *right;
}

Unit *widget_get_padding_bottom(Widget *widget) {
    return &widget->padding.bottom;
}

void widget_set_padding_bottom(Widget *widget, Unit *bottom) {
    widget->padding.bottom = *bottom;
}

Unit *widget_get_padding_left(Widget *widget) {
    return &widget->padding.left;
}

void widget_set_padding_left(Widget *widget, Unit *left) {
    widget->padding.left = *left;
}

void widget_measure(Widget *widget, int32_t parent_width, int32_t parent_height) {
    widget->parent_width = parent_width;
    if (widget->width.type == UNIT_TYPE_WRAP) {
        widget->content_rect.width = 0;
    } else {
        widget->content_rect.width = unit_to_pixels(&widget->width, parent_width - unit_to_pixels(&widget->padding.left, parent_width) -
            unit_to_pixels(&widget->padding.right, parent_width) - unit_to_pixels(&widget->margin.left, parent_width) - unit_to_pixels(&widget->margin.right, parent_width));
    }
    widget->padding_rect.width = unit_to_pixels(&widget->padding.left, parent_width) + widget->content_rect.width + unit_to_pixels(&widget->padding.right, parent_width);
    widget->margin_rect.width = unit_to_pixels(&widget->margin.left, parent_width) + widget->padding_rect.width + unit_to_pixels(&widget->margin.right, parent_width);

    widget->parent_height = parent_height;
    if (widget->height.type == UNIT_TYPE_WRAP) {
        widget->content_rect.height = 0;
    } else {
        widget->content_rect.height = unit_to_pixels(&widget->height, parent_height - unit_to_pixels(&widget->padding.top, parent_height) - unit_to_pixels(&widget->padding.bottom, parent_height) -
            unit_to_pixels(&widget->margin.top, parent_height) - unit_to_pixels(&widget->margin.bottom, parent_height));
    }
    widget->padding_rect.height = unit_to_pixels(&widget->padding.top, parent_height) + widget->content_rect.height + unit_to_pixels(&widget->padding.bottom, parent_height);
    widget->margin_rect.height = unit_to_pixels(&widget->margin.top, parent_height) + widget->padding_rect.height + unit_to_pixels(&widget->margin.bottom, parent_height);

    widget->rect_changed = true;
}

void widget_place(Widget *widget, int32_t x, int32_t y) {
    widget->margin_rect.x = x;
    widget->margin_rect.y = y;
    widget->padding_rect.x = widget->margin_rect.x + unit_to_pixels(&widget->margin.left, widget->parent_width);
    widget->padding_rect.y = widget->margin_rect.y + unit_to_pixels(&widget->margin.top, widget->parent_height);
    widget->content_rect.x = widget->padding_rect.x + unit_to_pixels(&widget->padding.left, widget->parent_width);
    widget->content_rect.y = widget->padding_rect.y + unit_to_pixels(&widget->padding.top, widget->parent_height);
    widget->rect_changed = true;
}

void widget_draw(Widget *widget, HDC hdc) {
    if (widget->background_color != 0) {
        HBRUSH brush = CreateSolidBrush(widget->background_color);
        RECT rect = { widget->padding_rect.x, widget->padding_rect.y,
            widget->padding_rect.x + widget->padding_rect.width,
            widget->padding_rect.y + widget->padding_rect.height };
        FillRect(hdc, &rect, brush);
        DeleteObject(brush);
    }

    #ifdef LAYOUT_DEBUG
        HBRUSH brush = CreateSolidBrush(0x000000ff);
        RECT content_rect = { widget->content_rect.x, widget->content_rect.y,
            widget->content_rect.x + widget->content_rect.width,
            widget->content_rect.y + widget->content_rect.height };
        FrameRect(hdc, &content_rect, brush);

        RECT padding_rect = { widget->padding_rect.x, widget->padding_rect.y,
            widget->padding_rect.x + widget->padding_rect.width,
            widget->padding_rect.y + widget->padding_rect.height };
        FrameRect(hdc, &padding_rect, brush);

        RECT margin_rect = { widget->margin_rect.x, widget->margin_rect.y,
            widget->margin_rect.x + widget->margin_rect.width,
            widget->margin_rect.y + widget->margin_rect.height };
        FrameRect(hdc, &margin_rect, brush);
        DeleteObject(brush);
    #endif

    widget->rect_changed = false;
}

void widget_free(Widget *widget) {
    free(widget);
}

// Container
#define CONTAINER_WIDGETS_INIT_CAPACITY 4

#define CONTAINER(ptr) ((Container *)ptr)
#define TYPE_CONTAINER TYPE_WIDGET + 1
#define ATTRIBUTE_WIDGETS ATTRIBUTE_PADDING_BOTTOM + 1
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
    for (size_t i = 0; i < container->widgets.size; i++) {
        Widget *other_widget = container->widgets.items[i];
        other_widget->free_function(other_widget);
    }
    widget_free(widget);
}

// Box
#define BOX(ptr) ((Box *)ptr)
#define TYPE_BOX TYPE_CONTAINER + 1
#define ATTRIBUTE_ORIENTATION ATTRIBUTE_WIDGETS + 1
#define ATTRIBUTE_ALIGN ATTRIBUTE_ORIENTATION + 1
typedef struct Box {
    Container super;
    Orientation orientation;
    Align align;
} Box;

void box_init(Box *box);
void box_measure(Widget *widget, int32_t parent_width, int32_t parent_height);
void box_place(Widget *widget, int32_t x, int32_t y);
void box_draw(Widget *widget, HDC hdc);
void box_free(Widget *widget);

Box *box_new(void) {
    Box *box = malloc(sizeof(Box));
    box_init(box);
    return box;
}

Box *box_new_with_orientation(Orientation orientation) {
    Box *box = box_new();
    box->orientation = orientation;
    return box;
}

void box_init(Box *box) {
    Container *container = &box->super;
    Widget *widget = &container->super;
    container_init(container);

    box->orientation = ORIENTATION_HORIZONTAL;
    box->align = ALIGN_HORIZONTAL_LEFT || ALIGN_VERTICAL_TOP;

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

Align box_get_align(Box *box) {
    return box->align;
}

void box_set_align(Box *box, Align align) {
    box->align = align;
}

void box_measure(Widget *widget, int32_t parent_width, int32_t parent_height) {
    Box *box = BOX(widget);
    Container *container = CONTAINER(widget);

    widget->parent_width = parent_width;
    if (widget->width.type != UNIT_TYPE_WRAP) {
        widget->content_rect.width = unit_to_pixels(&widget->width, parent_width - unit_to_pixels(&widget->padding.left, parent_width) - unit_to_pixels(&widget->padding.right, parent_width) -
            unit_to_pixels(&widget->margin.left, parent_width) - unit_to_pixels(&widget->margin.right, parent_width));
    }

    widget->parent_height = parent_height;
    if (widget->height.type != UNIT_TYPE_WRAP) {
        widget->content_rect.height = unit_to_pixels(&widget->height, parent_height - unit_to_pixels(&widget->padding.top, parent_height) - unit_to_pixels(&widget->padding.bottom, parent_height) -
            unit_to_pixels(&widget->margin.top, parent_height) - unit_to_pixels(&widget->margin.bottom, parent_height));
    }

    int32_t sum_width = 0;
    int32_t max_width = 0;
    int32_t sum_height = 0;
    int32_t max_height = 0;
    for (size_t i = 0; i < container->widgets.size; i++) {
        Widget *other_widget = container->widgets.items[i];
        if (other_widget->width.type == UNIT_TYPE_UNDEFINED) {
            if (box->orientation == ORIENTATION_HORIZONTAL) {
                other_widget->width.type = UNIT_TYPE_WRAP;
            }
            if (box->orientation == ORIENTATION_VERTICAL) {
                other_widget->width.value = 100;
                other_widget->width.type = UNIT_TYPE_PERCENT;
            }
        }
        if (other_widget->height.type == UNIT_TYPE_UNDEFINED) {
            if (box->orientation == ORIENTATION_HORIZONTAL) {
                other_widget->height.value = 100;
                other_widget->height.type = UNIT_TYPE_PERCENT;
            }
            if (box->orientation == ORIENTATION_VERTICAL) {
                other_widget->height.type = UNIT_TYPE_WRAP;
            }
        }

        other_widget->measure_function(other_widget, widget->content_rect.width, widget->content_rect.height);
        sum_width += other_widget->margin_rect.width;
        max_width = MAX(max_width, other_widget->margin_rect.width);
        sum_height += other_widget->margin_rect.height;
        max_height = MAX(max_height, other_widget->margin_rect.height);
    }

    if (widget->width.type == UNIT_TYPE_WRAP) {
        if (box->orientation == ORIENTATION_HORIZONTAL) {
            widget->content_rect.width = sum_width;
        }
        if (box->orientation == ORIENTATION_VERTICAL) {
            widget->content_rect.width = max_width;
        }
    }
    widget->padding_rect.width = unit_to_pixels(&widget->padding.left, parent_width) + widget->content_rect.width + unit_to_pixels(&widget->padding.right, parent_width);
    widget->margin_rect.width = unit_to_pixels(&widget->margin.left, parent_width) + widget->padding_rect.width + unit_to_pixels(&widget->margin.right, parent_width);

    if (widget->height.type == UNIT_TYPE_WRAP) {
        if (box->orientation == ORIENTATION_HORIZONTAL) {
            widget->content_rect.height = max_height;
        }
        if (box->orientation == ORIENTATION_VERTICAL) {
            widget->content_rect.height = sum_height;
        }
    }
    widget->padding_rect.height = unit_to_pixels(&widget->padding.top, parent_height) + widget->content_rect.height + unit_to_pixels(&widget->padding.bottom, parent_height);
    widget->margin_rect.height = unit_to_pixels(&widget->margin.top, parent_height) + widget->padding_rect.height + unit_to_pixels(&widget->margin.bottom, parent_height);

    widget->rect_changed = true;
}

void box_place(Widget *widget, int32_t x, int32_t y) {
    Box *box = BOX(widget);
    Container *container = CONTAINER(widget);

    widget->margin_rect.x = x;
    widget->margin_rect.y = y;
    x += unit_to_pixels(&widget->margin.left, widget->parent_width);
    widget->padding_rect.x = x;
    y += unit_to_pixels(&widget->margin.top, widget->parent_height);
    widget->padding_rect.y = y;
    x += unit_to_pixels(&widget->padding.left, widget->parent_width);
    widget->content_rect.x = x;
    y += unit_to_pixels(&widget->padding.top, widget->parent_height);
    widget->content_rect.y = y;

    int32_t sum_width = 0;
    int32_t sum_height = 0;
    for (size_t i = 0; i < container->widgets.size; i++) {
        Widget *other_widget = container->widgets.items[i];
        sum_width += other_widget->margin_rect.width;
        sum_height += other_widget->margin_rect.height;
    }
    if ((box->align & ALIGN_HORIZONTAL_CENTER) != 0) {
        x = MAX(widget->content_rect.x + (widget->content_rect.width - sum_width) / 2, widget->content_rect.x);
    }
    if ((box->align & ALIGN_HORIZONTAL_RIGHT) != 0) {
        x = MAX(widget->content_rect.x + widget->content_rect.width - sum_width, widget->content_rect.x);
    }
    if ((box->align & ALIGN_VERTICAL_CENTER) != 0) {
        y = MAX(widget->content_rect.y + (widget->content_rect.height - sum_height) / 2, widget->content_rect.y);
    }
    if ((box->align & ALIGN_VERTICAL_BOTTOM) != 0) {
        y = MAX(widget->content_rect.y + widget->content_rect.height - sum_height, widget->content_rect.y);
    }

    for (size_t i = 0; i < container->widgets.size; i++) {
        Widget *other_widget = container->widgets.items[i];

        other_widget->place_function(other_widget, x, y);

        if (box->orientation == ORIENTATION_HORIZONTAL) {
            x += other_widget->margin_rect.width;
        }
        if (box->orientation == ORIENTATION_VERTICAL) {
            y += other_widget->margin_rect.height;
        }
    }

    widget->rect_changed = true;
}

void box_draw(Widget *widget, HDC hdc) {
    Box *box = BOX(widget);
    Container *container = CONTAINER(widget);

    widget_draw(widget, hdc);

    for (size_t i = 0; i < container->widgets.size; i++) {
        Widget *other_widget = container->widgets.items[i];
        HRGN padding_region = CreateRectRgn(widget->padding_rect.x, widget->padding_rect.y,
            widget->padding_rect.x + widget->padding_rect.width,
            widget->padding_rect.y + widget->padding_rect.height);
        SelectClipRgn(hdc, &padding_region);
        DeleteObject(padding_region);
        other_widget->draw_function(other_widget, hdc);
        SelectClipRgn(hdc, NULL);
    }
}

void box_free(Widget *widget) {
    container_free(widget);
}

// Label
#define LABEL(ptr) ((Label *)ptr)
#define TYPE_LABEL TYPE_BOX + 1
#define ATTRIBUTE_TEXT ATTRIBUTE_ALIGN + 1
#define ATTRIBUTE_FONT_NAME ATTRIBUTE_TEXT + 1
#define ATTRIBUTE_FONT_WEIGHT ATTRIBUTE_FONT_NAME + 1
#define ATTRIBUTE_FONT_ITALIC ATTRIBUTE_FONT_WEIGHT + 1
#define ATTRIBUTE_FONT_UNDERLINE ATTRIBUTE_FONT_ITALIC + 1
#define ATTRIBUTE_FONT_LINE_THROUGH ATTRIBUTE_FONT_UNDERLINE + 1
#define ATTRIBUTE_TEXT_SIZE ATTRIBUTE_FONT_LINE_THROUGH + 1
#define ATTRIBUTE_TEXT_COLOR ATTRIBUTE_TEXT_SIZE + 1
#define ATTRIBUTE_SINGLE_LINE ATTRIBUTE_TEXT_COLOR + 1
#define FONT_WEIGHT_NORMAL 400
#define FONT_WEIGHT_BOLD 700
typedef struct Label {
    Widget super;

    wchar_t *text;
    bool text_changed;

    wchar_t *font_name;
    uint32_t font_weight;
    bool font_italic;
    bool font_underline;
    bool font_line_through;
    Unit text_size;
    bool font_changed;

    Color text_color;
    bool single_line;
    Align align;
} Label;

void label_init(Label *label);
void label_measure(Widget *widget, int32_t parent_width, int32_t parent_height);
void label_draw(Widget *widget, HDC hdc);
void label_free(Widget *widget);

Label *label_new(void) {
    Label *label = malloc(sizeof(Label));
    label_init(label);
    return label;
}

Label *label_new_with_text(wchar_t *text) {
    Label *label = label_new();
    label->text = wcsdup(text);
    label->text_changed = true;
    return label;
}

void label_init(Label *label) {
    Widget *widget = WIDGET(label);
    widget_init(widget);

    label->font_name = wcsdup(L"Tamoha");
    label->font_weight = FONT_WEIGHT_NORMAL;
    label->text_size.value = 16;
    label->text_size.type = UNIT_TYPE_SP;
    label->font_changed = true;

    label->text_color = 0x00111111;
    label->single_line = false;
    label->align = ALIGN_HORIZONTAL_LEFT || ALIGN_VERTICAL_TOP;

    widget->measure_function = label_measure;
    widget->draw_function = label_draw;
    widget->free_function = label_free;
}

wchar_t *label_get_text(Label *label) {
    return label->text;
}

void label_set_text(Label *label, wchar_t *text) {
    label->text = wcsdup(text);
    label->text_changed = true;
}

wchar_t *label_get_font_name(Label *label) {
    return label->font_name;
}

void label_set_font_name(Label *label, wchar_t *font_name) {
    label->font_name = wcsdup(font_name);
    label->font_changed = true;
}

uint32_t label_get_font_weight(Label *label) {
    return label->font_weight;
}

void label_set_font_weight(Label *label, uint32_t font_weight) {
    label->font_weight = font_weight;
    label->font_changed = true;
}

Unit *label_get_text_size(Label *label) {
    return &label->text_size;
}

void label_set_text_size(Label *label, Unit *text_size) {
    label->text_size = *text_size;
    label->font_changed = true;
}

bool label_get_font_italic(Label *label) {
    return label->font_italic;
}

void label_set_font_italic(Label *label, bool font_italic) {
    label->font_italic = font_italic;
}

bool label_get_font_underlne(Label *label) {
    return label->font_underline;
}

void label_set_font_underline(Label *label, bool font_underline) {
    label->font_underline = font_underline;
}

bool label_get_font_line_through(Label *label) {
    return label->font_line_through;
}

void label_set_font_line_through(Label *label, bool font_line_through) {
    label->font_line_through = font_line_through;
}

HFONT label_get_hfont(Label *label) {
    return CreateFontW(unit_to_pixels(&label->text_size, 0), 0, 0, 0, label->font_weight, label->font_italic,
        label->font_underline, label->font_line_through, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, label->font_name);
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

Align label_get_align(Label *label) {
    return label->align;
}

void label_set_align(Label *label, Align align) {
    label->align = align;
}

void label_measure(Widget *widget, int32_t parent_width, int32_t parent_height) {
    Label *label = LABEL(widget);

    widget->parent_width = parent_width;
    if (widget->width.type == UNIT_TYPE_WRAP) {
        HDC hdc = GetDC(NULL);
        HFONT font = label_get_hfont(label);
        SelectObject(hdc, font);
        RECT measure_rect = { 0, 0, 0, 0 };
        DrawTextW(hdc, label->text, -1, &measure_rect, DT_CALCRECT);
        DeleteObject(font);
        widget->content_rect.width = measure_rect.right - measure_rect.left;
    } else {
        widget->content_rect.width = unit_to_pixels(&widget->width, parent_width - unit_to_pixels(&widget->padding.left, parent_width) - unit_to_pixels(&widget->padding.right, parent_width) -
            unit_to_pixels(&widget->margin.left, parent_width) - unit_to_pixels(&widget->margin.right, parent_width));
    }
    widget->padding_rect.width = unit_to_pixels(&widget->padding.left, parent_width) + widget->content_rect.width + unit_to_pixels(&widget->padding.right, parent_width);
    widget->margin_rect.width = unit_to_pixels(&widget->margin.left, parent_width) + widget->padding_rect.width + unit_to_pixels(&widget->margin.right, parent_width);

    widget->parent_height = parent_height;
    if (widget->height.type == UNIT_TYPE_WRAP) {
        if (label->single_line) {
            widget->content_rect.height = unit_to_pixels(&label->text_size, 0);
        } else {
            HDC hdc = GetDC(NULL);
            HFONT font = label_get_hfont(label);
            SelectObject(hdc, font);
            RECT measure_rect = { 0, 0, widget->content_rect.width, 0 };
            widget->content_rect.height = DrawTextW(hdc, label->text, -1, &measure_rect, DT_CALCRECT | DT_WORDBREAK);
            DeleteObject(font);
        }
    } else {
        widget->content_rect.height = unit_to_pixels(&widget->height, parent_height - unit_to_pixels(&widget->padding.top, parent_height) - unit_to_pixels(&widget->padding.bottom, parent_height) -
            unit_to_pixels(&widget->margin.top, parent_height) - unit_to_pixels(&widget->margin.bottom, parent_height));
    }
    widget->padding_rect.height = unit_to_pixels(&widget->padding.top, parent_height) + widget->content_rect.height + unit_to_pixels(&widget->padding.bottom, parent_height);
    widget->margin_rect.height = unit_to_pixels(&widget->margin.top, parent_height) + widget->padding_rect.height + unit_to_pixels(&widget->margin.bottom, parent_height);

    widget->rect_changed = true;
}

void label_draw(Widget *widget, HDC hdc) {
    Label *label = LABEL(widget);
    widget_draw(widget, hdc);

    HFONT font = label_get_hfont(label);
    SelectObject(hdc, font);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, label->text_color);
    if (label->single_line) {
        int32_t x = widget->content_rect.x;
        int32_t y = widget->content_rect.y;
        if ((label->align & ALIGN_HORIZONTAL_LEFT) != 0) {
            SetTextAlign(hdc, TA_LEFT);
        }
        if ((label->align & ALIGN_HORIZONTAL_CENTER) != 0) {
            x += widget->content_rect.width / 2;
            SetTextAlign(hdc, TA_CENTER);
        }
        if ((label->align & ALIGN_HORIZONTAL_RIGHT) != 0) {
            x += widget->content_rect.width;
            SetTextAlign(hdc, TA_RIGHT);
        }
        if ((label->align & ALIGN_VERTICAL_CENTER) != 0) {
            y += (widget->content_rect.height - unit_to_pixels(&label->text_size, 0)) / 2;
        }
        if ((label->align & ALIGN_VERTICAL_BOTTOM) != 0) {
            y += widget->content_rect.height - unit_to_pixels(&label->text_size, 0);
        }
        TextOutW(hdc, x, y, label->text, wcslen(label->text));
    } else {
        SetTextAlign(hdc, TA_LEFT);
        RECT content_rect = { widget->content_rect.x, widget->content_rect.y,
            widget->content_rect.x + widget->content_rect.width,
            widget->content_rect.y + widget->content_rect.height };
        uint32_t style = DT_WORDBREAK;
        if ((label->align & ALIGN_HORIZONTAL_CENTER) != 0) {
            style |= DT_CENTER;
        }
        if ((label->align & ALIGN_HORIZONTAL_RIGHT) != 0) {
            style |= DT_RIGHT;
        }
        if ((label->align & ALIGN_HORIZONTAL_LEFT) != 0) {
            style |= DT_LEFT;
        }
        if ((label->align & ALIGN_VERTICAL_CENTER) != 0) {
            HDC hdc = GetDC(NULL);
            HFONT font = label_get_hfont(label);
            SelectObject(hdc, font);
            RECT measure_rect = { 0, 0, widget->content_rect.width, 0 };
            content_rect.top += (widget->content_rect.height - DrawTextW(hdc, label->text, -1, &measure_rect, DT_CALCRECT | DT_WORDBREAK)) / 2;
            DeleteObject(font);
        }
        if ((label->align & ALIGN_VERTICAL_BOTTOM) != 0) {
            HDC hdc = GetDC(NULL);
            HFONT font = label_get_hfont(label);
            SelectObject(hdc, font);
            RECT measure_rect = { 0, 0, widget->content_rect.width, 0 };
            content_rect.top += widget->content_rect.height - DrawTextW(hdc, label->text, -1, &measure_rect, DT_CALCRECT | DT_WORDBREAK);
            DeleteObject(font);
        }
        DrawTextW(hdc, label->text, -1, &content_rect, style);
    }
    DeleteObject(font);

    label->text_changed = false;
    label->font_changed = false;
}

void label_free(Widget *widget) {
    Label *label = LABEL(widget);
    free(label->text);
    free(label->font_name);
    widget_free(widget);
}

// Button
#define BUTTON(ptr) ((Button *)ptr)
#define TYPE_BUTTON TYPE_LABEL + 1
typedef struct Button {
    Label super;
    HWND hwnd;
    HFONT hfont;
} Button;

void button_init(Button *button);
void button_draw(Widget *widget, HDC hdc);
void button_free(Widget *widget);

Button *button_new(void) {
    Button *button = malloc(sizeof(Button));
    button_init(button);
    return button;
}

Button *button_new_with_text(wchar_t *text) {
    Button *button = malloc(sizeof(Button));
    Label *label = LABEL(button);
    button_init(button);
    label->text = wcsdup(text);
    label->text_changed = true;
    return button;
}

void button_init(Button *button) {
    Label *label = LABEL(button);
    Widget *widget = WIDGET(button);
    label_init(label);
    label->single_line = true;

    button->hwnd = CreateWindowExW(0, L"BUTTON", NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, global_hwnd, NULL, NULL, NULL);
    button->hfont = NULL;

    widget->draw_function = button_draw;
    widget->free_function = button_free;
}

void button_draw(Widget *widget, HDC hdc) {
    Button *button = BUTTON(widget);
    Label *label = LABEL(widget);

    if (widget->rect_changed) {
        SetWindowPos(button->hwnd, NULL, widget->padding_rect.x, widget->padding_rect.y, widget->padding_rect.width, widget->padding_rect.height, SWP_NOZORDER);
        widget->rect_changed = false;
    }

    if (label->text_changed) {
        SendMessageW(button->hwnd, WM_SETTEXT, NULL, label->text);
        label->text_changed = false;
    }

    if (label->font_changed) {
        if (button->hfont != NULL) {
            DeleteObject(button->hfont);
        }
        button->hfont = label_get_hfont(label);
        SendMessageW(button->hwnd, WM_SETFONT, button->hfont, (LPARAM)TRUE);
        label->font_changed = false;
    }
}

void button_free(Widget *widget) {
    Button *button = BUTTON(widget);
    Label *label = LABEL(widget);
    if (button->hfont != NULL) {
        DeleteObject(button->hfont);
    }
    DestroyWindow(button->hwnd);
    label_free(widget);
}

// Loader
void *layout_load(void *data, Widget **widget) {
    uint16_t widget_type = *(uint16_t *)data;
    data += sizeof(uint16_t);
    if (widget_type == TYPE_WIDGET) *widget = widget_new();
    if (widget_type == TYPE_BOX) *widget = WIDGET(box_new());
    if (widget_type == TYPE_LABEL) *widget = WIDGET(label_new());
    if (widget_type == TYPE_BUTTON) *widget = WIDGET(button_new());

    uint16_t attributes_count = *(uint16_t *)data;
    data += sizeof(uint16_t);
    for (size_t i = 0; i < attributes_count; i++) {
        uint16_t attribute = *(uint16_t *)data;
        data += sizeof(uint16_t);

        // Widget attributes
        if (attribute == ATTRIBUTE_WIDTH) {
            Unit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            widget_set_width(*widget, &unit);
        }
        if (attribute == ATTRIBUTE_HEIGHT) {
            Unit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            widget_set_height(*widget, &unit);
        }
        if (attribute == ATTRIBUTE_BACKGROUND_COLOR) {
            widget_set_background_color(*widget, *(Color *)data);
            data += sizeof(Color);
        }
        if (attribute == ATTRIBUTE_MARGIN) {
            Unit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            widget_set_margin_top(*widget, &unit);

            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            widget_set_margin_right(*widget, &unit);

            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            widget_set_margin_bottom(*widget, &unit);

            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            widget_set_margin_left(*widget, &unit);
        }
        if (attribute == ATTRIBUTE_MARGIN_TOP) {
            Unit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            widget_set_margin_top(*widget, &unit);
        }
        if (attribute == ATTRIBUTE_MARGIN_RIGHT) {
            Unit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            widget_set_margin_right(*widget, &unit);
        }
        if (attribute == ATTRIBUTE_MARGIN_BOTTOM) {
            Unit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            widget_set_margin_bottom(*widget, &unit);
        }
        if (attribute == ATTRIBUTE_MARGIN_LEFT) {
            Unit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            widget_set_margin_left(*widget, &unit);
        }
        if (attribute == ATTRIBUTE_PADDING) {
            Unit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            widget_set_padding_top(*widget, &unit);

            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            widget_set_padding_right(*widget, &unit);

            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            widget_set_padding_bottom(*widget, &unit);

            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            widget_set_padding_left(*widget, &unit);
        }
        if (attribute == ATTRIBUTE_PADDING_TOP) {
            Unit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            widget_set_padding_top(*widget, &unit);
        }
        if (attribute == ATTRIBUTE_PADDING_RIGHT) {
            Unit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            widget_set_padding_right(*widget, &unit);
        }
        if (attribute == ATTRIBUTE_PADDING_BOTTOM) {
            Unit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            widget_set_padding_bottom(*widget, &unit);
        }
        if (attribute == ATTRIBUTE_PADDING_LEFT) {
            Unit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            widget_set_padding_left(*widget, &unit);
        }

        // Container attributes
        if (widget_type == TYPE_CONTAINER || widget_type == TYPE_BOX) {
            if (attribute == ATTRIBUTE_WIDGETS) {
                uint16_t widgets_size = *(uint16_t *)data;
                data += sizeof(uint16_t);
                for (size_t j = 0; j < widgets_size; j++) {
                    Widget *other_widget;
                    data = layout_load(data, &other_widget);
                    container_add(CONTAINER(*widget), other_widget);
                }
            }
        }

        // Box attributes
        if (widget_type == TYPE_BOX) {
            if (attribute == ATTRIBUTE_ORIENTATION) {
                box_set_orientation(BOX(*widget), *(uint8_t *)data);
                data += sizeof(uint8_t);
            }
            if (attribute == ATTRIBUTE_ALIGN) {
                box_set_align(BOX(*widget), *(uint8_t *)data);
                data += sizeof(uint8_t);
            }
        }

        // Label attributes
        if (widget_type == TYPE_BUTTON || widget_type == TYPE_LABEL) {
            if (attribute == ATTRIBUTE_TEXT) {
                uint16_t string_size = *(uint16_t *)data;
                data += sizeof(uint16_t);
                label_set_text(LABEL(*widget), (wchar_t *)data);
                data += (string_size + 1) * sizeof(wchar_t);
            }
            if (attribute == ATTRIBUTE_FONT_NAME) {
                uint16_t string_size = *(uint16_t *)data;
                data += sizeof(uint16_t);
                label_set_font_name(LABEL(*widget), (wchar_t *)data);
                data += (string_size + 1) * sizeof(wchar_t);
            }
            if (attribute == ATTRIBUTE_FONT_WEIGHT) {
                label_set_font_weight(LABEL(*widget), *(uint16_t *)data);
                data += sizeof(uint16_t);
            }
            if (attribute == ATTRIBUTE_FONT_ITALIC) {
                label_set_font_italic(LABEL(*widget), *(uint8_t *)data);
                data += sizeof(uint8_t);
            }
            if (attribute == ATTRIBUTE_FONT_UNDERLINE) {
                label_set_font_underline(LABEL(*widget), *(uint8_t *)data);
                data += sizeof(uint8_t);
            }
            if (attribute == ATTRIBUTE_FONT_LINE_THROUGH) {
                label_set_font_line_through(LABEL(*widget), *(uint8_t *)data);
                data += sizeof(uint8_t);
            }
            if (attribute == ATTRIBUTE_TEXT_SIZE) {
                Unit unit;
                unit.value = *(float *)data;
                data += sizeof(float);
                unit.type = *(uint8_t *)data;
                data += sizeof(uint8_t);
                label_set_text_size(LABEL(*widget), &unit);
            }
            if (attribute == ATTRIBUTE_TEXT_COLOR) {
                label_set_text_color(LABEL(*widget), *(Color *)data);
                data += sizeof(Color);
            }
            if (attribute == ATTRIBUTE_SINGLE_LINE) {
                label_set_single_line(LABEL(*widget), *(uint8_t *)data);
                data += sizeof(uint8_t);
            }
            if (attribute == ATTRIBUTE_ALIGN) {
                label_set_align(LABEL(*widget), *(uint8_t *)data);
                data += sizeof(uint8_t);
            }
        }
    }

    return data;
}

#endif
