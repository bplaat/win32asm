#include "jan.h"

// Jan Globals
HWND jan_hwnd;
int32_t jan_width;
int32_t jan_height;

// Jan Unit
int32_t jan_unit_to_pixels(JanUnit unit, int32_t size) {
    if (unit.type == JAN_UNIT_TYPE_PX) return unit.value;
    if (unit.type == JAN_UNIT_TYPE_DP) return unit.value * 1.25;
    if (unit.type == JAN_UNIT_TYPE_SP) return unit.value * 1.25;
    if (unit.type == JAN_UNIT_TYPE_VW) return unit.value * ((float)jan_width / 100);
    if (unit.type == JAN_UNIT_TYPE_VH) return unit.value * ((float)jan_height / 100);
    if (unit.type == JAN_UNIT_TYPE_VMIN) return unit.value * ((float)MIN(jan_width, jan_height) / 100);
    if (unit.type == JAN_UNIT_TYPE_VMAX) return unit.value * ((float)MAX(jan_width, jan_height) / 100);
    if (unit.type == JAN_UNIT_TYPE_PERCENT) return unit.value * ((float)size / 100);
    return 0;
}

// JanList
JanList *jan_list_new(size_t capacity) {
    JanList *list = malloc(sizeof(JanList));
    jan_list_init(list, capacity);
    return list;
}

void jan_list_init(JanList *list, size_t capacity) {
    list->items = malloc(capacity * sizeof(void *));
    list->capacity = capacity;
    list->size = 0;
}

void jan_list_add(JanList *list, void *item) {
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->items = realloc(list->items, list->capacity * sizeof(void *));
    }
    list->items[list->size++] = item;
}

void jan_list_free(JanList *list, void (*free_function)(void *item)) {
    for (size_t i = 0; i < list->size; i++) {
        free_function(list->items[i]);
    }
    free(list->items);
    free(list);
}

// JanWidget
JanWidget *jan_widget_new(void) {
    JanWidget *widget = malloc(sizeof(JanWidget));
    jan_widget_init(widget);
    return widget;
}

void jan_widget_init(JanWidget *widget) {
    widget->id = 0;
    widget->width.type = JAN_UNIT_TYPE_UNDEFINED;
    widget->height.type = JAN_UNIT_TYPE_UNDEFINED;
    widget->background_color = 0;
    widget->visible = true;

    widget->margin.top.value = 0;
    widget->margin.top.type = JAN_UNIT_TYPE_PX;
    widget->margin.right.value = 0;
    widget->margin.right.type = JAN_UNIT_TYPE_PX;
    widget->margin.bottom.value = 0;
    widget->margin.bottom.type = JAN_UNIT_TYPE_PX;
    widget->margin.left.value = 0;
    widget->margin.left.type = JAN_UNIT_TYPE_PX;

    widget->padding.top.value = 0;
    widget->padding.top.type = JAN_UNIT_TYPE_PX;
    widget->padding.right.value = 0;
    widget->padding.right.type = JAN_UNIT_TYPE_PX;
    widget->padding.bottom.value = 0;
    widget->padding.bottom.type = JAN_UNIT_TYPE_PX;
    widget->padding.left.value = 0;
    widget->padding.left.type = JAN_UNIT_TYPE_PX;

    widget->event_function = jan_widget_event;
}

uint32_t jan_widget_get_id(JanWidget *widget) {
    return widget->id;
}

void jan_widget_set_id(JanWidget *widget, uint32_t id) {
    widget->id = id;
    widget->event_function(widget, JAN_EVENT_ID_CHANGED, NULL, NULL);
}

JanUnit jan_widget_get_width(JanWidget *widget) {
    return widget->width;
}

void jan_widget_set_width(JanWidget *widget, JanUnit width) {
    widget->width = width;
    widget->event_function(widget, JAN_EVENT_WIDTH_CHANGED, NULL, NULL);
}

JanUnit jan_widget_get_height(JanWidget *widget) {
    return widget->height;
}

void jan_widget_set_height(JanWidget *widget, JanUnit height) {
    widget->height = height;
    widget->event_function(widget, JAN_EVENT_HEIGHT_CHANGED, NULL, NULL);
}

JanColor jan_widget_get_background_color(JanWidget *widget) {
    return widget->background_color;
}

void jan_widget_set_background_color(JanWidget *widget, JanColor background_color) {
    widget->background_color = background_color;
    widget->event_function(widget, JAN_EVENT_BACKGROUND_COLOR_CHANGED, NULL, NULL);
}

bool jan_widget_get_visible(JanWidget *widget) {
    return widget->visible;
}

void jan_widget_set_visible(JanWidget *widget, bool visible) {
    widget->visible = visible;
    widget->event_function(widget, JAN_EVENT_VISIBLE_CHANGED, NULL, NULL);
}

JanOffset *widget_get_margin(JanWidget *widget) {
    return &widget->margin;
}

void jan_widget_set_margin(JanWidget *widget, JanUnit top, JanUnit right, JanUnit bottom, JanUnit left) {
    widget->margin.top = top;
    widget->margin.right = right;
    widget->margin.bottom = bottom;
    widget->margin.left = left;
    widget->event_function(widget, JAN_EVENT_MARGIN_CHANGED, NULL, NULL);
}

JanUnit jan_widget_get_margin_top(JanWidget *widget) {
    return widget->margin.top;
}

void jan_widget_set_margin_top(JanWidget *widget, JanUnit top) {
    widget->margin.top = top;
    widget->event_function(widget, JAN_EVENT_MARGIN_CHANGED, NULL, NULL);
}

JanUnit jan_widget_get_margin_right(JanWidget *widget) {
    return widget->margin.right;
}

void jan_widget_set_margin_right(JanWidget *widget, JanUnit right) {
    widget->margin.right = right;
    widget->event_function(widget, JAN_EVENT_MARGIN_CHANGED, NULL, NULL);
}

JanUnit jan_widget_get_margin_bottom(JanWidget *widget) {
    return widget->margin.bottom;
}

void jan_widget_set_margin_bottom(JanWidget *widget, JanUnit bottom) {
    widget->margin.bottom = bottom;
    widget->event_function(widget, JAN_EVENT_MARGIN_CHANGED, NULL, NULL);
}

JanUnit jan_widget_get_margin_left(JanWidget *widget) {
    return widget->margin.left;
}

void jan_widget_set_margin_left(JanWidget *widget, JanUnit left) {
    widget->margin.left = left;
    widget->event_function(widget, JAN_EVENT_MARGIN_CHANGED, NULL, NULL);
}

JanOffset *jan_widget_get_padding(JanWidget *widget) {
    return &widget->padding;
}

void jan_widget_set_padding(JanWidget *widget, JanUnit top, JanUnit right, JanUnit bottom, JanUnit left) {
    widget->padding.top = top;
    widget->padding.right = right;
    widget->padding.bottom = bottom;
    widget->padding.left = left;
    widget->event_function(widget, JAN_EVENT_PADDING_CHANGED, NULL, NULL);
}

JanUnit jan_widget_get_padding_top(JanWidget *widget) {
    return widget->padding.top;
}

void jan_widget_set_padding_top(JanWidget *widget, JanUnit top) {
    widget->padding.top = top;
    widget->event_function(widget, JAN_EVENT_PADDING_CHANGED, NULL, NULL);
}

JanUnit jan_widget_get_padding_right(JanWidget *widget) {
    return widget->padding.right;
}

void jan_widget_set_padding_right(JanWidget *widget, JanUnit right) {
    widget->padding.right = right;
    widget->event_function(widget, JAN_EVENT_PADDING_CHANGED, NULL, NULL);
}

JanUnit jan_widget_get_padding_bottom(JanWidget *widget) {
    return widget->padding.bottom;
}

void jan_widget_set_padding_bottom(JanWidget *widget, JanUnit bottom) {
    widget->padding.bottom = bottom;
    widget->event_function(widget, JAN_EVENT_PADDING_CHANGED, NULL, NULL);
}

JanUnit jan_widget_get_padding_left(JanWidget *widget) {
    return widget->padding.left;
}

void jan_widget_set_padding_left(JanWidget *widget, JanUnit left) {
    widget->padding.left = left;
    widget->event_function(widget, JAN_EVENT_PADDING_CHANGED, NULL, NULL);
}


void jan_widget_event(JanWidget *widget, uint32_t event, void *param1, void *param2) {
    if (event == JAN_EVENT_FREE) {
        free(widget);
        return;
    }

    if (event == JAN_EVENT_MEASURE) {
        int32_t parent_width = (uintptr_t)param1;
        int32_t parent_height = (uintptr_t)param2;

        widget->parent_width = parent_width;
        widget->content_rect.width = jan_unit_to_pixels(widget->width, parent_width - jan_unit_to_pixels(widget->padding.left, parent_width) -
            jan_unit_to_pixels(widget->padding.right, parent_width) - jan_unit_to_pixels(widget->margin.left, parent_width) - jan_unit_to_pixels(widget->margin.right, parent_width));
        widget->padding_rect.width = jan_unit_to_pixels(widget->padding.left, parent_width) + widget->content_rect.width + jan_unit_to_pixels(widget->padding.right, parent_width);
        widget->margin_rect.width = jan_unit_to_pixels(widget->margin.left, parent_width) + widget->padding_rect.width + jan_unit_to_pixels(widget->margin.right, parent_width);

        widget->parent_height = parent_height;
        widget->content_rect.height = jan_unit_to_pixels(widget->height, parent_height - jan_unit_to_pixels(widget->padding.top, parent_height) - jan_unit_to_pixels(widget->padding.bottom, parent_height) -
            jan_unit_to_pixels(widget->margin.top, parent_height) - jan_unit_to_pixels(widget->margin.bottom, parent_height));
        widget->padding_rect.height = jan_unit_to_pixels(widget->padding.top, parent_height) + widget->content_rect.height + jan_unit_to_pixels(widget->padding.bottom, parent_height);
        widget->margin_rect.height = jan_unit_to_pixels(widget->margin.top, parent_height) + widget->padding_rect.height + jan_unit_to_pixels(widget->margin.bottom, parent_height);
        return;
    }

    if (event == JAN_EVENT_PLACE) {
        int32_t x = (uintptr_t)param1;
        int32_t y = (uintptr_t)param2;

        widget->margin_rect.x = x;
        widget->margin_rect.y = y;

        widget->padding_rect.x = widget->margin_rect.x + jan_unit_to_pixels(widget->margin.left, widget->parent_width);
        widget->padding_rect.y = widget->margin_rect.y + jan_unit_to_pixels(widget->margin.top, widget->parent_height);

        widget->content_rect.x = widget->padding_rect.x + jan_unit_to_pixels(widget->padding.left, widget->parent_width);
        widget->content_rect.y = widget->padding_rect.y + jan_unit_to_pixels(widget->padding.top, widget->parent_height);
        return;
    }

    if (event == JAN_EVENT_DRAW && widget->visible) {
        HDC hdc = param1;
        if (widget->background_color != 0) {
            HBRUSH background_color_brush = CreateSolidBrush(widget->background_color);
            RECT padding_rect = { widget->padding_rect.x, widget->padding_rect.y,
                widget->padding_rect.x + widget->padding_rect.width,
                widget->padding_rect.y + widget->padding_rect.height };
            FillRect(hdc, &padding_rect, background_color_brush);
            DeleteObject(background_color_brush);
        }

        #ifdef JAN_DEBUG
            HBRUSH border_brush = CreateSolidBrush(0x000000ff);
            RECT content_rect = { widget->content_rect.x, widget->content_rect.y,
                widget->content_rect.x + widget->content_rect.width,
                widget->content_rect.y + widget->content_rect.height };
            FrameRect(hdc, &content_rect, border_brush);

            RECT padding_rect = { widget->padding_rect.x, widget->padding_rect.y,
                widget->padding_rect.x + widget->padding_rect.width,
                widget->padding_rect.y + widget->padding_rect.height };
            FrameRect(hdc, &padding_rect, border_brush);

            RECT margin_rect = { widget->margin_rect.x, widget->margin_rect.y,
                widget->margin_rect.x + widget->margin_rect.width,
                widget->margin_rect.y + widget->margin_rect.height };
            FrameRect(hdc, &margin_rect, border_brush);
            DeleteObject(border_brush);
        #endif
        return;
    }
}

// JanContainer
void jan_container_init(JanContainer *container) {
    JanWidget *widget = &container->super;
    jan_widget_init(widget);
    jan_list_init(&container->widgets, CONTAINER_WIDGETS_INIT_CAPACITY);
}

void jan_container_add(JanContainer *container, JanWidget *ptr) {
    JanWidget *widget = JAN_WIDGET(container);
    jan_list_add(&container->widgets, ptr);
    widget->event_function(widget, JAN_EVENT_WIDGETS_CHANGED, NULL, NULL);
}

void jan_container_event(JanWidget *widget, uint32_t event, void *param1, void *param2) {
    JanContainer *container = JAN_CONTAINER(widget);

    if (event == JAN_EVENT_FREE) {
        for (size_t i = 0; i < container->widgets.size; i++) {
            JanWidget *other_widget = container->widgets.items[i];
            other_widget->event_function(other_widget, JAN_EVENT_FREE, NULL, NULL);
        }
        free(container->widgets.items);
    }

    jan_widget_event(widget, event, param1, param2);
}

// JanBox
JanBox *jan_box_new(void) {
    JanBox *box = malloc(sizeof(JanBox));
    jan_box_init(box);
    return box;
}

JanBox *jan_box_new_with_orientation(JanOrientation orientation) {
    JanBox *box = jan_box_new();
    box->orientation = orientation;
    return box;
}

void jan_box_init(JanBox *box) {
    JanContainer *container = &box->super;
    JanWidget *widget = &container->super;
    jan_container_init(container);
    box->orientation = JAN_ORIENTATION_HORIZONTAL;
    box->align = JAN_ALIGN_HORIZONTAL_LEFT || JAN_ALIGN_VERTICAL_TOP;
    widget->event_function = jan_box_event;
}

JanOrientation jan_box_get_orientation(JanBox *box) {
    return box->orientation;
}

void jan_box_set_orientation(JanBox *box, JanOrientation orientation) {
    JanWidget *widget = JAN_WIDGET(box);
    box->orientation = orientation;
    widget->event_function(widget, JAN_EVENT_ORIENTATION_CHANGED, NULL, NULL);
}

JanAlign jan_box_get_align(JanBox *box) {
    return box->align;
}

void jan_box_set_align(JanBox *box, JanAlign align) {
    JanWidget *widget = JAN_WIDGET(box);
    box->align = align;
    widget->event_function(widget, JAN_EVENT_ALIGN_CHANGED, NULL, NULL);
}

void jan_box_event(JanWidget *widget, uint32_t event, void *param1, void *param2) {
    JanBox *box = JAN_BOX(widget);
    JanContainer *container = JAN_CONTAINER(widget);

    if (event == JAN_EVENT_MEASURE) {
        int32_t parent_width = (uintptr_t)param1;
        int32_t parent_height = (uintptr_t)param2;

        widget->parent_width = parent_width;
        if (widget->width.type != JAN_UNIT_TYPE_WRAP) {
            widget->content_rect.width = jan_unit_to_pixels(widget->width, parent_width - jan_unit_to_pixels(widget->padding.left, parent_width) - jan_unit_to_pixels(widget->padding.right, parent_width) -
                jan_unit_to_pixels(widget->margin.left, parent_width) - jan_unit_to_pixels(widget->margin.right, parent_width));
        }

        widget->parent_height = parent_height;
        if (widget->height.type != JAN_UNIT_TYPE_WRAP) {
            widget->content_rect.height = jan_unit_to_pixels(widget->height, parent_height - jan_unit_to_pixels(widget->padding.top, parent_height) - jan_unit_to_pixels(widget->padding.bottom, parent_height) -
                jan_unit_to_pixels(widget->margin.top, parent_height) - jan_unit_to_pixels(widget->margin.bottom, parent_height));
        }

        int32_t sum_width = 0;
        int32_t max_width = 0;
        int32_t sum_height = 0;
        int32_t max_height = 0;
        for (size_t i = 0; i < container->widgets.size; i++) {
            JanWidget *other_widget = container->widgets.items[i];
            if (other_widget->width.type == JAN_UNIT_TYPE_UNDEFINED) {
                if (box->orientation == JAN_ORIENTATION_HORIZONTAL) {
                    other_widget->width.type = JAN_UNIT_TYPE_WRAP;
                }
                if (box->orientation == JAN_ORIENTATION_VERTICAL) {
                    other_widget->width.value = 100;
                    other_widget->width.type = JAN_UNIT_TYPE_PERCENT;
                }
            }
            if (other_widget->height.type == JAN_UNIT_TYPE_UNDEFINED) {
                if (box->orientation == JAN_ORIENTATION_HORIZONTAL) {
                    other_widget->height.value = 100;
                    other_widget->height.type = JAN_UNIT_TYPE_PERCENT;
                }
                if (box->orientation == JAN_ORIENTATION_VERTICAL) {
                    other_widget->height.type = JAN_UNIT_TYPE_WRAP;
                }
            }

            other_widget->event_function(other_widget, JAN_EVENT_MEASURE, JAN_PARAM(widget->content_rect.width), JAN_PARAM(widget->content_rect.height));
            sum_width += other_widget->margin_rect.width;
            max_width = MAX(max_width, other_widget->margin_rect.width);
            sum_height += other_widget->margin_rect.height;
            max_height = MAX(max_height, other_widget->margin_rect.height);
        }

        if (widget->width.type == JAN_UNIT_TYPE_WRAP) {
            if (box->orientation == JAN_ORIENTATION_HORIZONTAL) {
                widget->content_rect.width = sum_width;
            }
            if (box->orientation == JAN_ORIENTATION_VERTICAL) {
                widget->content_rect.width = max_width;
            }
        }
        widget->padding_rect.width = jan_unit_to_pixels(widget->padding.left, parent_width) + widget->content_rect.width + jan_unit_to_pixels(widget->padding.right, parent_width);
        widget->margin_rect.width = jan_unit_to_pixels(widget->margin.left, parent_width) + widget->padding_rect.width + jan_unit_to_pixels(widget->margin.right, parent_width);

        if (widget->height.type == JAN_UNIT_TYPE_WRAP) {
            if (box->orientation == JAN_ORIENTATION_HORIZONTAL) {
                widget->content_rect.height = max_height;
            }
            if (box->orientation == JAN_ORIENTATION_VERTICAL) {
                widget->content_rect.height = sum_height;
            }
        }
        widget->padding_rect.height = jan_unit_to_pixels(widget->padding.top, parent_height) + widget->content_rect.height + jan_unit_to_pixels(widget->padding.bottom, parent_height);
        widget->margin_rect.height = jan_unit_to_pixels(widget->margin.top, parent_height) + widget->padding_rect.height + jan_unit_to_pixels(widget->margin.bottom, parent_height);
        return;
    }

    if (event == JAN_EVENT_PLACE) {
        int32_t x = (uintptr_t)param1;
        int32_t y = (uintptr_t)param2;

        widget->margin_rect.x = x;
        widget->margin_rect.y = y;
        widget->padding_rect.x = widget->margin_rect.x + jan_unit_to_pixels(widget->margin.left, widget->parent_width);
        widget->padding_rect.y = widget->margin_rect.y + jan_unit_to_pixels(widget->margin.top, widget->parent_height);
        widget->content_rect.x = widget->padding_rect.x + jan_unit_to_pixels(widget->padding.left, widget->parent_width);
        widget->content_rect.y = widget->padding_rect.y + jan_unit_to_pixels(widget->padding.top, widget->parent_height);

        int32_t sum_width = 0;
        int32_t sum_height = 0;
        for (size_t i = 0; i < container->widgets.size; i++) {
            JanWidget *other_widget = container->widgets.items[i];
            sum_width += other_widget->margin_rect.width;
            sum_height += other_widget->margin_rect.height;
        }

        if (box->orientation == JAN_ORIENTATION_HORIZONTAL) {
            if ((box->align & JAN_ALIGN_HORIZONTAL_LEFT) != 0) {
                x = widget->content_rect.x;
            }
            if ((box->align & JAN_ALIGN_HORIZONTAL_CENTER) != 0) {
                x = MAX(widget->content_rect.x + (widget->content_rect.width - sum_width) / 2, widget->content_rect.x);
            }
            if ((box->align & JAN_ALIGN_HORIZONTAL_RIGHT) != 0) {
                x = MAX(widget->content_rect.x + widget->content_rect.width - sum_width, widget->content_rect.x);
            }
        } else {
            x = widget->content_rect.x;
        }

        if (box->orientation == JAN_ORIENTATION_VERTICAL) {
            if ((box->align & JAN_ALIGN_VERTICAL_TOP) != 0) {
                y = widget->content_rect.y;
            }
            if ((box->align & JAN_ALIGN_VERTICAL_CENTER) != 0) {
                y = MAX(widget->content_rect.y + (widget->content_rect.height - sum_height) / 2, widget->content_rect.y);
            }
            if ((box->align & JAN_ALIGN_VERTICAL_BOTTOM) != 0) {
                y = MAX(widget->content_rect.y + widget->content_rect.height - sum_height, widget->content_rect.y);
            }
        } else {
            y = widget->content_rect.y;
        }

        for (size_t i = 0; i < container->widgets.size; i++) {
            JanWidget *other_widget = container->widgets.items[i];

            int32_t other_widget_x = x;
            if (box->orientation == JAN_ORIENTATION_VERTICAL) {
                if ((box->align & JAN_ALIGN_HORIZONTAL_CENTER) != 0) {
                    other_widget_x += (widget->content_rect.width - other_widget->margin_rect.width) / 2;
                }
                if ((box->align & JAN_ALIGN_HORIZONTAL_RIGHT) != 0) {
                    other_widget_x += widget->content_rect.width - other_widget->margin_rect.width;
                }
            }

            int32_t other_widget_y = y;
            if (box->orientation == JAN_ORIENTATION_HORIZONTAL) {
                if ((box->align & JAN_ALIGN_VERTICAL_CENTER) != 0) {
                    other_widget_y += (widget->content_rect.height - other_widget->margin_rect.height) / 2;
                }
                if ((box->align & JAN_ALIGN_VERTICAL_BOTTOM) != 0) {
                    other_widget_y += widget->content_rect.height - other_widget->margin_rect.height;
                }
            }

            other_widget->event_function(other_widget, JAN_EVENT_PLACE, JAN_PARAM(other_widget_x), JAN_PARAM(other_widget_y));

            if (box->orientation == JAN_ORIENTATION_HORIZONTAL) {
                x += other_widget->margin_rect.width;
            }
            if (box->orientation == JAN_ORIENTATION_VERTICAL) {
                y += other_widget->margin_rect.height;
            }
        }
        return;
    }

    if (event == JAN_EVENT_DRAW && widget->visible) {
        HDC hdc = param1;
        jan_widget_event(widget, JAN_EVENT_DRAW, hdc, NULL);

        for (size_t i = 0; i < container->widgets.size; i++) {
            JanWidget *other_widget = container->widgets.items[i];
            HRGN padding_region = CreateRectRgn(widget->padding_rect.x, widget->padding_rect.y,
                widget->padding_rect.x + widget->padding_rect.width,
                widget->padding_rect.y + widget->padding_rect.height);
            SelectClipRgn(hdc, &padding_region);
            other_widget->event_function(other_widget, JAN_EVENT_DRAW, hdc, NULL);
            DeleteObject(padding_region);
            SelectClipRgn(hdc, NULL);
        }
        return;
    }

    jan_container_event(widget, event, param1, param2);
}

// JanLabel
JanLabel *jan_label_new(void) {
    JanLabel *label = malloc(sizeof(JanLabel));
    jan_label_init(label);
    return label;
}

JanLabel *jan_label_new_with_text(wchar_t *text) {
    JanLabel *label = jan_label_new();
    label->text = wcsdup(text);
    return label;
}

void jan_label_init(JanLabel *label) {
    JanWidget *widget = JAN_WIDGET(label);
    jan_widget_init(widget);
    label->text = NULL;
    label->font_name = wcsdup(L"Tamoha");
    label->font_weight = JAN_FONT_WEIGHT_NORMAL;
    label->font_italic = false;
    label->font_underline = false;
    label->font_line_through = false;
    label->font_size.value = 16;
    label->font_size.type = JAN_UNIT_TYPE_SP;
    label->text_color = RGB(0x11, 0x11, 0x11);
    label->single_line = false;
    label->align = JAN_ALIGN_HORIZONTAL_LEFT || JAN_ALIGN_VERTICAL_TOP;
    widget->event_function = jan_label_event;
}

wchar_t *jan_label_get_text(JanLabel *label) {
    return label->text;
}

void jan_label_set_text(JanLabel *label, wchar_t *text) {
    JanWidget *widget = JAN_WIDGET(label);
    label->text = wcsdup(text);
    widget->event_function(widget, JAN_EVENT_TEXT_CHANGED, NULL, NULL);
}

wchar_t *jan_label_get_font_name(JanLabel *label) {
    return label->font_name;
}

void jan_label_set_font_name(JanLabel *label, wchar_t *font_name) {
    JanWidget *widget = JAN_WIDGET(label);
    label->font_name = wcsdup(font_name);
    widget->event_function(widget, JAN_EVENT_FONT_NAME_CHANGED, NULL, NULL);
}

uint32_t jan_label_get_font_weight(JanLabel *label) {
    return label->font_weight;
}

void jan_label_set_font_weight(JanLabel *label, uint32_t font_weight) {
    JanWidget *widget = JAN_WIDGET(label);
    label->font_weight = font_weight;
    widget->event_function(widget, JAN_EVENT_FONT_WEIGHT_CHANGED, NULL, NULL);
}

JanUnit jan_label_get_font_size(JanLabel *label) {
    return label->font_size;
}

void jan_label_set_font_size(JanLabel *label, JanUnit font_size) {
    JanWidget *widget = JAN_WIDGET(label);
    label->font_size = font_size;
    widget->event_function(widget, JAN_EVENT_FONT_SIZE_CHANGED, NULL, NULL);
}

bool jan_label_get_font_italic(JanLabel *label) {
    return label->font_italic;
}

void jan_label_set_font_italic(JanLabel *label, bool font_italic) {
    JanWidget *widget = JAN_WIDGET(label);
    label->font_italic = font_italic;
    widget->event_function(widget, JAN_EVENT_FONT_ITALIC_CHANGED, NULL, NULL);
}

bool jan_label_get_font_underlne(JanLabel *label) {
    return label->font_underline;
}

void jan_label_set_font_underline(JanLabel *label, bool font_underline) {
    JanWidget *widget = JAN_WIDGET(label);
    label->font_underline = font_underline;
    widget->event_function(widget, JAN_EVENT_FONT_UNDERLINE_CHANGED, NULL, NULL);
}

bool jan_label_get_font_line_through(JanLabel *label) {
    return label->font_line_through;
}

void jan_label_set_font_line_through(JanLabel *label, bool font_line_through) {
    JanWidget *widget = JAN_WIDGET(label);
    label->font_line_through = font_line_through;
    widget->event_function(widget, JAN_EVENT_FONT_LINE_THROUGH_CHANGED, NULL, NULL);
}

HFONT jan_label_get_hfont(JanLabel *label) {
    return CreateFontW(jan_unit_to_pixels(label->font_size, 0), 0, 0, 0, label->font_weight, label->font_italic,
        label->font_underline, label->font_line_through, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, label->font_name);
}

JanColor jan_label_get_text_color(JanLabel *label) {
    return label->text_color;
}

void jan_label_set_text_color(JanLabel *label, JanColor text_color) {
    JanWidget *widget = JAN_WIDGET(label);
    label->text_color = text_color;
    widget->event_function(widget, JAN_EVENT_TEXT_COLOR_CHANGED, NULL, NULL);
}

bool jan_label_get_single_line(JanLabel *label) {
    return label->single_line;
}

void jan_label_set_single_line(JanLabel *label, bool single_line) {
    JanWidget *widget = JAN_WIDGET(label);
    label->single_line = single_line;
    widget->event_function(widget, JAN_EVENT_SINGLE_LINE_CHANGED, NULL, NULL);
}

JanAlign jan_label_get_align(JanLabel *label) {
    return label->align;
}

void jan_label_set_align(JanLabel *label, JanAlign align) {
    JanWidget *widget = JAN_WIDGET(label);
    label->align = align;
    widget->event_function(widget, JAN_EVENT_ALIGN_CHANGED, NULL, NULL);
}

void jan_label_event(JanWidget *widget, uint32_t event, void *param1, void *param2) {
    JanLabel *label = JAN_LABEL(widget);

    if (event == JAN_EVENT_FREE) {
        free(label->text);
        free(label->font_name);
    }

    if (event == JAN_EVENT_MEASURE) {
        int32_t parent_width = (uintptr_t)param1;
        int32_t parent_height = (uintptr_t)param2;

        widget->parent_width = parent_width;
        if (widget->width.type == JAN_UNIT_TYPE_WRAP) {
            HDC hdc = GetDC(NULL);
            HFONT font = jan_label_get_hfont(label);
            SelectObject(hdc, font);
            RECT measure_rect = { 0, 0, 0, 0 };
            DrawTextW(hdc, label->text, -1, &measure_rect, DT_CALCRECT);
            DeleteObject(font);
            widget->content_rect.width = MIN((int32_t)(measure_rect.right - measure_rect.left), parent_width);
        } else {
            widget->content_rect.width = jan_unit_to_pixels(widget->width, parent_width - jan_unit_to_pixels(widget->padding.left, parent_width) - jan_unit_to_pixels(widget->padding.right, parent_width) -
                jan_unit_to_pixels(widget->margin.left, parent_width) - jan_unit_to_pixels(widget->margin.right, parent_width));
        }
        widget->padding_rect.width = jan_unit_to_pixels(widget->padding.left, parent_width) + widget->content_rect.width + jan_unit_to_pixels(widget->padding.right, parent_width);
        widget->margin_rect.width = jan_unit_to_pixels(widget->margin.left, parent_width) + widget->padding_rect.width + jan_unit_to_pixels(widget->margin.right, parent_width);

        widget->parent_height = parent_height;
        if (widget->height.type == JAN_UNIT_TYPE_WRAP) {
            if (label->single_line) {
                widget->content_rect.height = MIN(jan_unit_to_pixels(label->font_size, 0), parent_height);
            } else {
                HDC hdc = GetDC(NULL);
                HFONT font = jan_label_get_hfont(label);
                SelectObject(hdc, font);
                RECT measure_rect = { 0, 0, widget->content_rect.width, 0 };
                widget->content_rect.height = DrawTextW(hdc, label->text, -1, &measure_rect, DT_CALCRECT | DT_WORDBREAK);
                DeleteObject(font);
            }
        } else {
            widget->content_rect.height = jan_unit_to_pixels(widget->height, parent_height - jan_unit_to_pixels(widget->padding.top, parent_height) - jan_unit_to_pixels(widget->padding.bottom, parent_height) -
                jan_unit_to_pixels(widget->margin.top, parent_height) - jan_unit_to_pixels(widget->margin.bottom, parent_height));
        }
        widget->padding_rect.height = jan_unit_to_pixels(widget->padding.top, parent_height) + widget->content_rect.height + jan_unit_to_pixels(widget->padding.bottom, parent_height);
        widget->margin_rect.height = jan_unit_to_pixels(widget->margin.top, parent_height) + widget->padding_rect.height + jan_unit_to_pixels(widget->margin.bottom, parent_height);
        return;
    }

    if (event == JAN_EVENT_DRAW && widget->visible) {
        HDC hdc = param1;
        jan_widget_event(widget, JAN_EVENT_DRAW, hdc, NULL);

        HFONT font = jan_label_get_hfont(label);
        SelectObject(hdc, font);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, label->text_color);
        if (label->single_line) {
            int32_t x = widget->content_rect.x;
            int32_t y = widget->content_rect.y;
            if ((label->align & JAN_ALIGN_HORIZONTAL_LEFT) != 0) {
                SetTextAlign(hdc, TA_LEFT);
            }
            if ((label->align & JAN_ALIGN_HORIZONTAL_CENTER) != 0) {
                x += widget->content_rect.width / 2;
                SetTextAlign(hdc, TA_CENTER);
            }
            if ((label->align & JAN_ALIGN_HORIZONTAL_RIGHT) != 0) {
                x += widget->content_rect.width;
                SetTextAlign(hdc, TA_RIGHT);
            }
            if ((label->align & JAN_ALIGN_VERTICAL_CENTER) != 0) {
                y += (widget->content_rect.height - jan_unit_to_pixels(label->font_size, 0)) / 2;
            }
            if ((label->align & JAN_ALIGN_VERTICAL_BOTTOM) != 0) {
                y += widget->content_rect.height - jan_unit_to_pixels(label->font_size, 0);
            }
            TextOutW(hdc, x, y, label->text, wcslen(label->text));
        } else {
            SetTextAlign(hdc, TA_LEFT);
            RECT content_rect = { widget->content_rect.x, widget->content_rect.y,
                widget->content_rect.x + widget->content_rect.width,
                widget->content_rect.y + widget->content_rect.height };
            uint32_t style = DT_WORDBREAK;
            if ((label->align & JAN_ALIGN_HORIZONTAL_CENTER) != 0) {
                style |= DT_CENTER;
            }
            if ((label->align & JAN_ALIGN_HORIZONTAL_RIGHT) != 0) {
                style |= DT_RIGHT;
            }
            if ((label->align & JAN_ALIGN_HORIZONTAL_LEFT) != 0) {
                style |= DT_LEFT;
            }
            if ((label->align & JAN_ALIGN_VERTICAL_CENTER) != 0) {
                HDC hdc = GetDC(NULL);
                HFONT font = jan_label_get_hfont(label);
                SelectObject(hdc, font);
                RECT measure_rect = { 0, 0, widget->content_rect.width, 0 };
                content_rect.top += (widget->content_rect.height - DrawTextW(hdc, label->text, -1, &measure_rect, DT_CALCRECT | DT_WORDBREAK)) / 2;
                DeleteObject(font);
            }
            if ((label->align & JAN_ALIGN_VERTICAL_BOTTOM) != 0) {
                HDC hdc = GetDC(NULL);
                HFONT font = jan_label_get_hfont(label);
                SelectObject(hdc, font);
                RECT measure_rect = { 0, 0, widget->content_rect.width, 0 };
                content_rect.top += widget->content_rect.height - DrawTextW(hdc, label->text, -1, &measure_rect, DT_CALCRECT | DT_WORDBREAK);
                DeleteObject(font);
            }
            DrawTextW(hdc, label->text, -1, &content_rect, style);
        }
        DeleteObject(font);
        return;
    }

    jan_widget_event(widget, event, param1, param2);
}

// JanButton
JanButton *jan_button_new(void) {
    JanButton *button = malloc(sizeof(JanButton));
    jan_button_init(button);
    return button;
}

JanButton *jan_button_new_with_text(wchar_t *text) {
    JanButton *button = jan_button_new();
    JanLabel *label = JAN_LABEL(button);
    label->text = wcsdup(text);
    return button;
}

void jan_button_init(JanButton *button) {
    JanLabel *label = JAN_LABEL(button);
    JanWidget *widget = JAN_WIDGET(button);
    jan_label_init(label);
    label->single_line = true;

    button->hwnd = CreateWindowExW(0, L"BUTTON", label->text, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, jan_hwnd, NULL, NULL, NULL);
    button->hfont = jan_label_get_hfont(label);
    SendMessageW(button->hwnd, WM_SETFONT, button->hfont, (LPARAM)TRUE);

    widget->event_function = jan_button_event;
}

void jan_button_event(JanWidget *widget, uint32_t event, void *param1, void *param2) {
    JanButton *button = JAN_BUTTON(widget);
    JanLabel *label = JAN_LABEL(widget);

    if (event == JAN_EVENT_FREE) {
        DestroyWindow(button->hwnd);
        DeleteObject(button->hfont);
    }

    if (event == JAN_EVENT_MEASURE) {
        jan_label_event(widget, JAN_EVENT_MEASURE, param1, param2);
        SetWindowPos(button->hwnd, NULL, 0, 0, widget->padding_rect.width, widget->padding_rect.height, SWP_NOZORDER | SWP_NOMOVE);
        return;
    }

    if (event == JAN_EVENT_PLACE) {
        jan_label_event(widget, JAN_EVENT_PLACE, param1, param2);
        SetWindowPos(button->hwnd, NULL, widget->padding_rect.x, widget->padding_rect.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
        return;
    }

    if (event == JAN_EVENT_DRAW) {
        return;
    }

    if (event == JAN_EVENT_ID_CHANGED) {
        DestroyWindow(button->hwnd);
        button->hwnd = CreateWindowExW(0, L"BUTTON", label->text, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, jan_hwnd, (HMENU)(size_t)widget->id, NULL, NULL);
        SetWindowPos(button->hwnd, NULL, widget->padding_rect.x, widget->padding_rect.y, widget->padding_rect.width, widget->padding_rect.height, SWP_NOZORDER);
        SendMessageW(button->hwnd, WM_SETFONT, button->hfont, (LPARAM)TRUE);
    }

    if (event == JAN_EVENT_VISIBLE_CHANGED) {
        ShowWindow(button->hwnd, widget->visible ? SW_SHOW : SW_HIDE);
    }

    if (event == JAN_EVENT_TEXT_CHANGED) {
        SendMessageW(button->hwnd, WM_SETTEXT, NULL, label->text);
    }

    if (
        event == JAN_EVENT_FONT_NAME_CHANGED || event == JAN_EVENT_FONT_WEIGHT_CHANGED ||
        event == JAN_EVENT_FONT_ITALIC_CHANGED || event == JAN_EVENT_FONT_UNDERLINE_CHANGED ||
        event == JAN_EVENT_FONT_LINE_THROUGH_CHANGED || event == JAN_EVENT_FONT_SIZE_CHANGED
    ) {
        DeleteObject(button->hfont);
        button->hfont = jan_label_get_hfont(label);
        SendMessageW(button->hwnd, WM_SETFONT, button->hfont, (LPARAM)TRUE);
    }

    jan_label_event(widget, event, param1, param2);
}

// JanLoader
uint8_t *jan_load(uint8_t *data, JanWidget **widget) {
    uint16_t widget_type = *(uint16_t *)data;
    data += sizeof(uint16_t);
    if (widget_type == JAN_TYPE_WIDGET) *widget = jan_widget_new();
    if (widget_type == JAN_TYPE_BOX) *widget = JAN_WIDGET(jan_box_new());
    if (widget_type == JAN_TYPE_LABEL) *widget = JAN_WIDGET(jan_label_new());
    if (widget_type == JAN_TYPE_BUTTON) *widget = JAN_WIDGET(jan_button_new());

    uint16_t attributes_count = *(uint16_t *)data;
    data += sizeof(uint16_t);
    for (size_t i = 0; i < attributes_count; i++) {
        uint16_t attribute = *(uint16_t *)data;
        data += sizeof(uint16_t);

        // Widget attributes
        if (attribute == JAN_ATTRIBUTE_ID) {
            jan_widget_set_id(*widget, *(uint16_t *)data);
            data += sizeof(uint16_t);
        }
        if (attribute == JAN_ATTRIBUTE_WIDTH) {
            JanUnit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_width(*widget, unit);
        }
        if (attribute == JAN_ATTRIBUTE_HEIGHT) {
            JanUnit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_height(*widget, unit);
        }
        if (attribute == JAN_ATTRIBUTE_BACKGROUND_COLOR) {
            jan_widget_set_background_color(*widget, *(JanColor *)data);
            data += sizeof(JanColor);
        }
        if (attribute == JAN_ATTRIBUTE_VISIBLE) {
            jan_widget_set_id(*widget, *(uint8_t *)data);
            data += sizeof(uint8_t);
        }
        if (attribute == JAN_ATTRIBUTE_MARGIN) {
            JanUnit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_margin_top(*widget, unit);

            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_margin_right(*widget, unit);

            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_margin_bottom(*widget, unit);

            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_margin_left(*widget, unit);
        }
        if (attribute == JAN_ATTRIBUTE_MARGIN_TOP) {
            JanUnit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_margin_top(*widget, unit);
        }
        if (attribute == JAN_ATTRIBUTE_MARGIN_RIGHT) {
            JanUnit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_margin_right(*widget, unit);
        }
        if (attribute == JAN_ATTRIBUTE_MARGIN_BOTTOM) {
            JanUnit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_margin_bottom(*widget, unit);
        }
        if (attribute == JAN_ATTRIBUTE_MARGIN_LEFT) {
            JanUnit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_margin_left(*widget, unit);
        }
        if (attribute == JAN_ATTRIBUTE_PADDING) {
            JanUnit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_padding_top(*widget, unit);

            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_padding_right(*widget, unit);

            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_padding_bottom(*widget, unit);

            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_padding_left(*widget, unit);
        }
        if (attribute == JAN_ATTRIBUTE_PADDING_TOP) {
            JanUnit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_padding_top(*widget, unit);
        }
        if (attribute == JAN_ATTRIBUTE_PADDING_RIGHT) {
            JanUnit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_padding_right(*widget, unit);
        }
        if (attribute == JAN_ATTRIBUTE_PADDING_BOTTOM) {
            JanUnit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_padding_bottom(*widget, unit);
        }
        if (attribute == JAN_ATTRIBUTE_PADDING_LEFT) {
            JanUnit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_padding_left(*widget, unit);
        }

        // Container attributes
        if (widget_type == JAN_TYPE_CONTAINER || widget_type == JAN_TYPE_BOX) {
            if (attribute == JAN_ATTRIBUTE_WIDGETS) {
                uint16_t widgets_size = *(uint16_t *)data;
                data += sizeof(uint16_t);
                for (size_t j = 0; j < widgets_size; j++) {
                    JanWidget *other_widget;
                    data = jan_load(data, &other_widget);
                    jan_container_add(JAN_CONTAINER(*widget), other_widget);
                }
            }
        }

        // Box attributes
        if (widget_type == JAN_TYPE_BOX) {
            if (attribute == JAN_ATTRIBUTE_ORIENTATION) {
                jan_box_set_orientation(JAN_BOX(*widget), *(uint8_t *)data);
                data += sizeof(uint8_t);
            }
            if (attribute == JAN_ATTRIBUTE_ALIGN) {
                jan_box_set_align(JAN_BOX(*widget), *(uint8_t *)data);
                data += sizeof(uint8_t);
            }
        }

        // Label attributes
        if (widget_type == JAN_TYPE_BUTTON || widget_type == JAN_TYPE_LABEL) {
            if (attribute == JAN_ATTRIBUTE_TEXT) {
                uint16_t string_size = *(uint16_t *)data;
                data += sizeof(uint16_t);
                jan_label_set_text(JAN_LABEL(*widget), (wchar_t *)data);
                data += (string_size + 1) * sizeof(wchar_t);
            }
            if (attribute == JAN_ATTRIBUTE_FONT_NAME) {
                uint16_t string_size = *(uint16_t *)data;
                data += sizeof(uint16_t);
                jan_label_set_font_name(JAN_LABEL(*widget), (wchar_t *)data);
                data += (string_size + 1) * sizeof(wchar_t);
            }
            if (attribute == JAN_ATTRIBUTE_FONT_WEIGHT) {
                jan_label_set_font_weight(JAN_LABEL(*widget), *(uint16_t *)data);
                data += sizeof(uint16_t);
            }
            if (attribute == JAN_ATTRIBUTE_FONT_ITALIC) {
                jan_label_set_font_italic(JAN_LABEL(*widget), *(uint8_t *)data);
                data += sizeof(uint8_t);
            }
            if (attribute == JAN_ATTRIBUTE_FONT_UNDERLINE) {
                jan_label_set_font_underline(JAN_LABEL(*widget), *(uint8_t *)data);
                data += sizeof(uint8_t);
            }
            if (attribute == JAN_ATTRIBUTE_FONT_LINE_THROUGH) {
                jan_label_set_font_line_through(JAN_LABEL(*widget), *(uint8_t *)data);
                data += sizeof(uint8_t);
            }
            if (attribute == JAN_ATTRIBUTE_FONT_SIZE) {
                JanUnit unit;
                unit.value = *(float *)data;
                data += sizeof(float);
                unit.type = *(uint8_t *)data;
                data += sizeof(uint8_t);
                jan_label_set_font_size(JAN_LABEL(*widget), unit);
            }
            if (attribute == JAN_ATTRIBUTE_TEXT_COLOR) {
                jan_label_set_text_color(JAN_LABEL(*widget), *(JanColor *)data);
                data += sizeof(JanColor);
            }
            if (attribute == JAN_ATTRIBUTE_SINGLE_LINE) {
                jan_label_set_single_line(JAN_LABEL(*widget), *(uint8_t *)data);
                data += sizeof(uint8_t);
            }
            if (attribute == JAN_ATTRIBUTE_ALIGN) {
                jan_label_set_align(JAN_LABEL(*widget), *(uint8_t *)data);
                data += sizeof(uint8_t);
            }
        }
    }
    return data;
}
