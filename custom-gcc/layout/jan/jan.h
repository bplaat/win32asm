// Jan Win32 Widget / Layout Library Header
#ifndef JAN_H
#define JAN_H

#define WIN32_MALLOC
#define WIN32_REALLOC
#define WIN32_FREE
#define WIN32_WCSLEN
#define WIN32_WCSDUP
#include "win32.h"

// Jan Globals
extern HWND jan_hwnd;
extern int32_t jan_width;
extern int32_t jan_height;

// JanColor
typedef uint32_t JanColor;

// JanRect
typedef struct JanRect {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
} JanRect;

// JanOrientation
#define JAN_ORIENTATION_HORIZONTAL 0
#define JAN_ORIENTATION_VERTICAL 1
typedef int32_t JanOrientation;

// JanAlign
#define JAN_ALIGN_HORIZONTAL_LEFT 0
#define JAN_ALIGN_HORIZONTAL_CENTER 1
#define JAN_ALIGN_HORIZONTAL_RIGHT 2
#define JAN_ALIGN_VERTICAL_TOP 0
#define JAN_ALIGN_VERTICAL_CENTER 4
#define JAN_ALIGN_VERTICAL_BOTTOM 8
typedef int32_t JanAlign;

// JanUnit
#define JAN_UNIT_TYPE_UNDEFINED 0
#define JAN_UNIT_TYPE_WRAP 1
#define JAN_UNIT_TYPE_PX 2
#define JAN_UNIT_TYPE_DP 3
#define JAN_UNIT_TYPE_SP 4
#define JAN_UNIT_TYPE_VW 5
#define JAN_UNIT_TYPE_VH 6
#define JAN_UNIT_TYPE_VMIN 7
#define JAN_UNIT_TYPE_VMAX 8
#define JAN_UNIT_TYPE_PERCENT 9
typedef int32_t JanUnitType;

typedef struct JanUnit {
    float value;
    JanUnitType type;
} JanUnit;

int32_t jan_unit_to_pixels(JanUnit unit, int32_t size);

// JanOffset
typedef struct JanOffset {
    JanUnit top;
    JanUnit right;
    JanUnit bottom;
    JanUnit left;
} JanOffset;

// JanList
typedef struct JanList {
    void **items;
    size_t capacity;
    size_t size;
} JanList;

JanList *list_new(size_t capacity);

void jan_list_init(JanList *list, size_t capacity);

void jan_list_add(JanList *list, void *item);

void jan_list_free(JanList *list, void (*free_function)(void *item));

// JanWidget
#define JAN_WIDGET(ptr) ((JanWidget *)ptr)
#define JAN_PARAM(param) ((void *)(size_t)param)
#define JAN_TYPE_WIDGET 1

#define JAN_ATTRIBUTE_ID 1
#define JAN_ATTRIBUTE_WIDTH JAN_ATTRIBUTE_ID + 1
#define JAN_ATTRIBUTE_HEIGHT JAN_ATTRIBUTE_WIDTH + 1
#define JAN_ATTRIBUTE_BACKGROUND_COLOR JAN_ATTRIBUTE_HEIGHT + 1
#define JAN_ATTRIBUTE_VISIBLE JAN_ATTRIBUTE_BACKGROUND_COLOR + 1
#define JAN_ATTRIBUTE_MARGIN JAN_ATTRIBUTE_VISIBLE + 1
#define JAN_ATTRIBUTE_MARGIN_TOP JAN_ATTRIBUTE_MARGIN + 1
#define JAN_ATTRIBUTE_MARGIN_LEFT JAN_ATTRIBUTE_MARGIN_TOP + 1
#define JAN_ATTRIBUTE_MARGIN_RIGHT JAN_ATTRIBUTE_MARGIN_LEFT + 1
#define JAN_ATTRIBUTE_MARGIN_BOTTOM JAN_ATTRIBUTE_MARGIN_RIGHT + 1
#define JAN_ATTRIBUTE_PADDING JAN_ATTRIBUTE_MARGIN_BOTTOM + 1
#define JAN_ATTRIBUTE_PADDING_TOP JAN_ATTRIBUTE_PADDING + 1
#define JAN_ATTRIBUTE_PADDING_LEFT JAN_ATTRIBUTE_PADDING_TOP + 1
#define JAN_ATTRIBUTE_PADDING_RIGHT JAN_ATTRIBUTE_PADDING_LEFT + 1
#define JAN_ATTRIBUTE_PADDING_BOTTOM JAN_ATTRIBUTE_PADDING_RIGHT + 1

#define JAN_EVENT_FREE 1
#define JAN_EVENT_MEASURE JAN_EVENT_FREE + 1
#define JAN_EVENT_PLACE JAN_EVENT_MEASURE + 1
#define JAN_EVENT_DRAW JAN_EVENT_PLACE + 1
#define JAN_EVENT_ID_CHANGED JAN_EVENT_DRAW + 1
#define JAN_EVENT_WIDTH_CHANGED JAN_EVENT_ID_CHANGED + 1
#define JAN_EVENT_HEIGHT_CHANGED JAN_EVENT_WIDTH_CHANGED + 1
#define JAN_EVENT_BACKGROUND_COLOR_CHANGED JAN_EVENT_HEIGHT_CHANGED + 1
#define JAN_EVENT_VISIBLE_CHANGED JAN_EVENT_BACKGROUND_COLOR_CHANGED + 1
#define JAN_EVENT_MARGIN_CHANGED JAN_EVENT_VISIBLE_CHANGED + 1
#define JAN_EVENT_PADDING_CHANGED JAN_EVENT_MARGIN_CHANGED + 1

typedef struct JanWidget {
    uint32_t id;
    JanUnit width;
    JanUnit height;
    JanColor background_color;
    bool visible;
    JanOffset margin;
    JanOffset padding;
    void *(*event_function)(struct JanWidget *widget, uint32_t event, void *param1, void *param2);
    int32_t parent_width;
    int32_t parent_height;
    JanRect content_rect;
    JanRect padding_rect;
    JanRect margin_rect;
} JanWidget;

JanWidget *jan_widget_new(void);

void jan_widget_init(JanWidget *widget);

uint32_t jan_widget_get_id(JanWidget *widget);

void jan_widget_set_id(JanWidget *widget, uint32_t id);

JanUnit widget_get_width(JanWidget *widget);

void jan_widget_set_width(JanWidget *widget, JanUnit width);

JanUnit widget_get_height(JanWidget *widget);

void jan_widget_set_height(JanWidget *widget, JanUnit height);

JanColor jan_widget_get_background_color(JanWidget *widget);

void jan_widget_set_background_color(JanWidget *widget, JanColor background_color);

bool jan_widget_get_visible(JanWidget *widget);

void jan_widget_set_visible(JanWidget *widget, bool visible);

JanOffset *widget_get_margin(JanWidget *widget);

void jan_widget_set_margin(JanWidget *widget, JanUnit top, JanUnit right, JanUnit bottom, JanUnit left);

JanUnit widget_get_margin_top(JanWidget *widget);

void jan_widget_set_margin_top(JanWidget *widget, JanUnit top);

JanUnit widget_get_margin_right(JanWidget *widget);

void jan_widget_set_margin_right(JanWidget *widget, JanUnit right);

JanUnit widget_get_margin_bottom(JanWidget *widget);

void jan_widget_set_margin_bottom(JanWidget *widget, JanUnit bottom);

JanUnit widget_get_margin_left(JanWidget *widget);

void jan_widget_set_margin_left(JanWidget *widget, JanUnit left);

JanOffset *widget_get_padding(JanWidget *widget);

void jan_widget_set_padding(JanWidget *widget, JanUnit top, JanUnit right, JanUnit bottom, JanUnit left);

JanUnit widget_get_padding_top(JanWidget *widget);

void jan_widget_set_padding_top(JanWidget *widget, JanUnit top);

JanUnit widget_get_padding_right(JanWidget *widget);

void jan_widget_set_padding_right(JanWidget *widget, JanUnit right);

JanUnit widget_get_padding_bottom(JanWidget *widget);

void jan_widget_set_padding_bottom(JanWidget *widget, JanUnit bottom);

JanUnit widget_get_padding_left(JanWidget *widget);

void jan_widget_set_padding_left(JanWidget *widget, JanUnit left);

void *jan_widget_event(JanWidget *widget, uint32_t event, void *param1, void *param2);

// JanContainer
#define CONTAINER_WIDGETS_INIT_CAPACITY 4

#define JAN_CONTAINER(ptr) ((JanContainer *)ptr)
#define JAN_TYPE_CONTAINER JAN_TYPE_WIDGET + 1

#define JAN_ATTRIBUTE_WIDGETS JAN_ATTRIBUTE_PADDING_BOTTOM + 1

#define JAN_EVENT_WIDGETS_CHANGED JAN_EVENT_PADDING_CHANGED + 1
#define JAN_EVENT_FIND JAN_EVENT_WIDGETS_CHANGED + 1

typedef struct JanContainer {
    JanWidget super;
    JanList widgets;
} JanContainer;

void jan_container_init(JanContainer *container);

void jan_container_add(JanContainer *container, JanWidget *ptr);

JanWidget *jan_container_find(JanContainer *container, uint32_t id);

void jan_container_free(JanWidget *widget);

// JanStack
#define JAN_STACK(ptr) ((JanStack *)ptr)
#define JAN_TYPE_STACK JAN_TYPE_CONTAINER + 1

#define JAN_ATTRIBUTE_ALIGN JAN_ATTRIBUTE_WIDGETS + 1

#define JAN_EVENT_ALIGN_CHANGED JAN_EVENT_FIND + 1

typedef struct JanStack {
    JanContainer super;
    JanAlign align;
} JanStack;

JanStack *jan_stack_new(void);

void jan_stack_init(JanStack *stack);

JanAlign jan_stack_get_align(JanStack *stack);

void jan_stack_set_align(JanStack *stack, JanAlign align);

void *jan_stack_event(JanWidget *widget, uint32_t event, void *param1, void *param2);

// JanBox
#define JAN_BOX(ptr) ((JanBox *)ptr)
#define JAN_TYPE_BOX JAN_TYPE_STACK + 1

#define JAN_ATTRIBUTE_ORIENTATION JAN_ATTRIBUTE_ALIGN + 1

#define JAN_EVENT_ORIENTATION_CHANGED JAN_EVENT_ALIGN_CHANGED + 1

typedef struct JanBox {
    JanContainer super;
    JanOrientation orientation;
    JanAlign align;
} JanBox;

JanBox *jan_box_new(void);

JanBox *jan_box_new_with_orientation(JanOrientation orientation);

void jan_box_init(JanBox *box);

JanOrientation jan_box_get_orientation(JanBox *box);

void jan_box_set_orientation(JanBox *box, JanOrientation orientation);

JanAlign jan_box_get_align(JanBox *box);

void jan_box_set_align(JanBox *box, JanAlign align);

void *jan_box_event(JanWidget *widget, uint32_t event, void *param1, void *param2);

// JanLabel
#define JAN_LABEL(ptr) ((JanLabel *)ptr)
#define JAN_TYPE_LABEL JAN_TYPE_BOX + 1

#define JAN_ATTRIBUTE_TEXT JAN_ATTRIBUTE_ALIGN + 1
#define JAN_ATTRIBUTE_FONT_NAME JAN_ATTRIBUTE_TEXT + 1
#define JAN_ATTRIBUTE_FONT_WEIGHT JAN_ATTRIBUTE_FONT_NAME + 1
#define JAN_ATTRIBUTE_FONT_ITALIC JAN_ATTRIBUTE_FONT_WEIGHT + 1
#define JAN_ATTRIBUTE_FONT_UNDERLINE JAN_ATTRIBUTE_FONT_ITALIC + 1
#define JAN_ATTRIBUTE_FONT_LINE_THROUGH JAN_ATTRIBUTE_FONT_UNDERLINE + 1
#define JAN_ATTRIBUTE_FONT_SIZE JAN_ATTRIBUTE_FONT_LINE_THROUGH + 1
#define JAN_ATTRIBUTE_TEXT_COLOR JAN_ATTRIBUTE_FONT_SIZE + 1
#define JAN_ATTRIBUTE_SINGLE_LINE JAN_ATTRIBUTE_TEXT_COLOR + 1

#define JAN_EVENT_TEXT_CHANGED JAN_EVENT_ORIENTATION_CHANGED + 1
#define JAN_EVENT_FONT_NAME_CHANGED JAN_EVENT_TEXT_CHANGED + 1
#define JAN_EVENT_FONT_WEIGHT_CHANGED JAN_EVENT_FONT_NAME_CHANGED + 1
#define JAN_EVENT_FONT_ITALIC_CHANGED JAN_EVENT_FONT_WEIGHT_CHANGED + 1
#define JAN_EVENT_FONT_UNDERLINE_CHANGED JAN_EVENT_FONT_ITALIC_CHANGED + 1
#define JAN_EVENT_FONT_LINE_THROUGH_CHANGED JAN_EVENT_FONT_UNDERLINE_CHANGED + 1
#define JAN_EVENT_FONT_SIZE_CHANGED JAN_EVENT_FONT_LINE_THROUGH_CHANGED + 1
#define JAN_EVENT_TEXT_COLOR_CHANGED JAN_EVENT_FONT_SIZE_CHANGED + 1
#define JAN_EVENT_SINGLE_LINE_CHANGED JAN_EVENT_TEXT_COLOR_CHANGED + 1

#define JAN_FONT_WEIGHT_NORMAL 400
#define JAN_FONT_WEIGHT_BOLD 700

typedef struct JanLabel {
    JanWidget super;
    wchar_t *text;
    wchar_t *font_name;
    uint32_t font_weight;
    bool font_italic;
    bool font_underline;
    bool font_line_through;
    JanUnit font_size;
    JanColor text_color;
    bool single_line;
    JanAlign align;
} JanLabel;

JanLabel *jan_label_new(void);

JanLabel *jan_label_new_with_text(wchar_t *text);

void jan_label_init(JanLabel *label);

wchar_t *jan_label_get_text(JanLabel *label);

void jan_label_set_text(JanLabel *label, wchar_t *text);

wchar_t *jan_label_get_font_name(JanLabel *label);

void jan_label_set_font_name(JanLabel *label, wchar_t *font_name);

uint32_t jan_label_get_font_weight(JanLabel *label);

void jan_label_set_font_weight(JanLabel *label, uint32_t font_weight);

JanUnit jan_label_get_font_size(JanLabel *label);

void jan_label_set_font_size(JanLabel *label, JanUnit font_size);

bool jan_label_get_font_italic(JanLabel *label);

void jan_label_set_font_italic(JanLabel *label, bool font_italic);

bool jan_label_get_font_underlne(JanLabel *label);

void jan_label_set_font_underline(JanLabel *label, bool font_underline);

bool jan_label_get_font_line_through(JanLabel *label);

void jan_label_set_font_line_through(JanLabel *label, bool font_line_through);

HFONT jan_label_get_hfont(JanLabel *label);

JanColor jan_label_get_text_color(JanLabel *label);

void jan_label_set_text_color(JanLabel *label, JanColor text_color);

bool jan_label_get_single_line(JanLabel *label);

void jan_label_set_single_line(JanLabel *label, bool single_line);

JanAlign jan_label_get_align(JanLabel *label);

void jan_label_set_align(JanLabel *label, JanAlign align);

void *jan_label_event(JanWidget *widget, uint32_t event, void *param1, void *param2);

// JanButton
#define JAN_BUTTON(ptr) ((JanButton *)ptr)
#define JAN_TYPE_BUTTON JAN_TYPE_LABEL + 1

typedef struct JanButton {
    JanLabel super;
    HWND hwnd;
    HFONT hfont;
} JanButton;

JanButton *jan_button_new(void);

JanButton *jan_button_new_with_text(wchar_t *text);

void jan_button_init(JanButton *button);

void *jan_button_event(JanWidget *widget, uint32_t event, void *param1, void *param2);

// JanLoad
uint8_t *jan_load(uint8_t *data, JanWidget **widget);

#endif
