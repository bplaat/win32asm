#include "jan.h"

// Jan Globals
HWND jan_hwnd;
int32_t jan_width;
int32_t jan_height;

// Jan Unit
int32_t jan_round(float number) {
    if (number < 0) {
        return (int32_t)(number - 0.5);
    } else {
        return (int32_t)(number + 0.5);
    }
}

int32_t jan_unit_to_pixels(JanUnit *unit, int32_t size) {
    if (unit->type == JAN_UNIT_TYPE_PX) return jan_round(unit->value);
    if (unit->type == JAN_UNIT_TYPE_DP) return jan_round(unit->value * 1.25);
    if (unit->type == JAN_UNIT_TYPE_SP) return jan_round(unit->value * 1.25);
    if (unit->type == JAN_UNIT_TYPE_VW) return jan_round(unit->value * ((float)jan_width / 100));
    if (unit->type == JAN_UNIT_TYPE_VH) return jan_round(unit->value * ((float)jan_height / 100));
    if (unit->type == JAN_UNIT_TYPE_VMIN) return jan_round(unit->value * ((float)MIN(jan_width, jan_height) / 100));
    if (unit->type == JAN_UNIT_TYPE_VMAX) return jan_round(unit->value * ((float)MAX(jan_width, jan_height) / 100));
    if (unit->type == JAN_UNIT_TYPE_PERCENT) return jan_round(unit->value * ((float)size / 100));
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
    return (uintptr_t)widget->event_function(widget, JAN_EVENT_GET_ID, NULL, NULL);
}

void jan_widget_set_id(JanWidget *widget, uint32_t id) {
    widget->event_function(widget, JAN_EVENT_SET_ID, JAN_PARAM(id), NULL);
}

JanUnit *jan_widget_get_width(JanWidget *widget) {
    return widget->event_function(widget, JAN_EVENT_GET_WIDTH, NULL, NULL);
}

void jan_widget_set_width(JanWidget *widget, JanUnit *width) {
    widget->event_function(widget, JAN_EVENT_SET_WIDTH, width, NULL);
}

JanUnit *jan_widget_get_height(JanWidget *widget) {
    return widget->event_function(widget, JAN_EVENT_GET_HEIGHT, NULL, NULL);
}

void jan_widget_set_height(JanWidget *widget, JanUnit *height) {
    widget->event_function(widget, JAN_EVENT_SET_HEIGHT, height, NULL);
}

JanColor jan_widget_get_background_color(JanWidget *widget) {
    return (JanColor)(uintptr_t)widget->event_function(widget, JAN_EVENT_GET_BACKGROUND_COLOR, NULL, NULL);
}

void jan_widget_set_background_color(JanWidget *widget, JanColor background_color) {
    widget->event_function(widget, JAN_EVENT_SET_BACKGROUND_COLOR, JAN_PARAM(background_color), NULL);
}

bool jan_widget_get_visible(JanWidget *widget) {
    return widget->event_function(widget, JAN_EVENT_GET_VISIBLE, NULL, NULL);
}

void jan_widget_set_visible(JanWidget *widget, bool visible) {
    widget->event_function(widget, JAN_EVENT_SET_VISIBLE, JAN_PARAM(visible), NULL);
}

JanOffset *widget_get_margin(JanWidget *widget) {
    return widget->event_function(widget, JAN_EVENT_GET_MARGIN, NULL, NULL);
}

void jan_widget_set_margin(JanWidget *widget, JanUnit *top, JanUnit *right, JanUnit *bottom, JanUnit *left) {
    JanOffset margin = { *top, *right, *bottom, *left };
    widget->event_function(widget, JAN_EVENT_SET_MARGIN, &margin, NULL);
}

JanUnit *jan_widget_get_margin_top(JanWidget *widget) {
    return widget->event_function(widget, JAN_EVENT_GET_MARGIN_TOP, NULL, NULL);
}

void jan_widget_set_margin_top(JanWidget *widget, JanUnit *top) {
    widget->event_function(widget, JAN_EVENT_SET_MARGIN_TOP, top, NULL);
}

JanUnit *jan_widget_get_margin_right(JanWidget *widget) {
    return widget->event_function(widget, JAN_EVENT_GET_MARGIN_RIGHT, NULL, NULL);
}

void jan_widget_set_margin_right(JanWidget *widget, JanUnit *right) {
    widget->event_function(widget, JAN_EVENT_SET_MARGIN_RIGHT, right, NULL);
}

JanUnit *jan_widget_get_margin_bottom(JanWidget *widget) {
    return widget->event_function(widget, JAN_EVENT_GET_MARGIN_BOTTOM, NULL, NULL);
}

void jan_widget_set_margin_bottom(JanWidget *widget, JanUnit *bottom) {
    widget->event_function(widget, JAN_EVENT_SET_MARGIN_BOTTOM, bottom, NULL);
}

JanUnit *jan_widget_get_margin_left(JanWidget *widget) {
    return widget->event_function(widget, JAN_EVENT_GET_MARGIN_LEFT, NULL, NULL);
}

void jan_widget_set_margin_left(JanWidget *widget, JanUnit *left) {
    widget->event_function(widget, JAN_EVENT_SET_MARGIN_LEFT, left, NULL);
}

JanOffset *jan_widget_get_padding(JanWidget *widget) {
    return widget->event_function(widget, JAN_EVENT_GET_PADDING, NULL, NULL);
}

void jan_widget_set_padding(JanWidget *widget, JanUnit *top, JanUnit *right, JanUnit *bottom, JanUnit *left) {
    JanOffset padding = { *top, *right, *bottom, *left };
    widget->event_function(widget, JAN_EVENT_SET_PADDING, &padding, NULL);
}

JanUnit *jan_widget_get_padding_top(JanWidget *widget) {
    return widget->event_function(widget, JAN_EVENT_GET_PADDING_TOP, NULL, NULL);
}

void jan_widget_set_padding_top(JanWidget *widget, JanUnit *top) {
    widget->event_function(widget, JAN_EVENT_SET_PADDING_TOP, top, NULL);
}

JanUnit *jan_widget_get_padding_right(JanWidget *widget) {
    return widget->event_function(widget, JAN_EVENT_GET_PADDING_RIGHT, NULL, NULL);
}

void jan_widget_set_padding_right(JanWidget *widget, JanUnit *right) {
    widget->event_function(widget, JAN_EVENT_SET_PADDING_RIGHT, right, NULL);
}

JanUnit *jan_widget_get_padding_bottom(JanWidget *widget) {
    return widget->event_function(widget, JAN_EVENT_GET_PADDING_BOTTOM, NULL, NULL);
}

void jan_widget_set_padding_bottom(JanWidget *widget, JanUnit *bottom) {
    widget->event_function(widget, JAN_EVENT_SET_PADDING_BOTTOM, bottom, NULL);
}

JanUnit *jan_widget_get_padding_left(JanWidget *widget) {
    return widget->event_function(widget, JAN_EVENT_GET_PADDING_LEFT, NULL, NULL);
}

void jan_widget_set_padding_left(JanWidget *widget, JanUnit *left) {
    widget->event_function(widget, JAN_EVENT_SET_PADDING_LEFT, left, NULL);
}

void *jan_widget_event(JanWidget *widget, uint32_t event, void *param1, void *param2) {
    if (event == JAN_EVENT_FREE) {
        free(widget);
    }

    if (event == JAN_EVENT_MEASURE) {
        int32_t parent_width = (intptr_t)param1;
        int32_t parent_height = (intptr_t)param2;

        widget->parent_width = parent_width;
        widget->content_rect.width = jan_unit_to_pixels(&widget->width, parent_width - jan_unit_to_pixels(&widget->padding.left, parent_width) -
            jan_unit_to_pixels(&widget->padding.right, parent_width) - jan_unit_to_pixels(&widget->margin.left, parent_width) - jan_unit_to_pixels(&widget->margin.right, parent_width));
        widget->padding_rect.width = jan_unit_to_pixels(&widget->padding.left, parent_width) + widget->content_rect.width + jan_unit_to_pixels(&widget->padding.right, parent_width);
        widget->margin_rect.width = jan_unit_to_pixels(&widget->margin.left, parent_width) + widget->padding_rect.width + jan_unit_to_pixels(&widget->margin.right, parent_width);

        widget->parent_height = parent_height;
        widget->content_rect.height = jan_unit_to_pixels(&widget->height, parent_height - jan_unit_to_pixels(&widget->padding.top, parent_height) - jan_unit_to_pixels(&widget->padding.bottom, parent_height) -
            jan_unit_to_pixels(&widget->margin.top, parent_height) - jan_unit_to_pixels(&widget->margin.bottom, parent_height));
        widget->padding_rect.height = jan_unit_to_pixels(&widget->padding.top, parent_height) + widget->content_rect.height + jan_unit_to_pixels(&widget->padding.bottom, parent_height);
        widget->margin_rect.height = jan_unit_to_pixels(&widget->margin.top, parent_height) + widget->padding_rect.height + jan_unit_to_pixels(&widget->margin.bottom, parent_height);
    }

    if (event == JAN_EVENT_PLACE) {
        int32_t x = (intptr_t)param1;
        int32_t y = (intptr_t)param2;

        widget->margin_rect.x = x;
        widget->margin_rect.y = y;

        widget->padding_rect.x = widget->margin_rect.x + jan_unit_to_pixels(&widget->margin.left, widget->parent_width);
        widget->padding_rect.y = widget->margin_rect.y + jan_unit_to_pixels(&widget->margin.top, widget->parent_height);

        widget->content_rect.x = widget->padding_rect.x + jan_unit_to_pixels(&widget->padding.left, widget->parent_width);
        widget->content_rect.y = widget->padding_rect.y + jan_unit_to_pixels(&widget->padding.top, widget->parent_height);
    }

    if (event == JAN_EVENT_DRAW && widget->visible) {
        GpGraphics *graphics = param2;
        if (widget->background_color != 0) {
            GpBrush *background_color_brush;
            GdipCreateSolidFill(widget->background_color, (GpSolidFill **)&background_color_brush);
            GdipFillRectangleI(graphics, background_color_brush, widget->padding_rect.x, widget->padding_rect.y, widget->padding_rect.width, widget->padding_rect.height);
            GdipDeleteBrush(background_color_brush);
        }

        #ifdef JAN_DEBUG
            GpPen *border_pen;
            GdipCreatePen1(0x40ff0000, 2, UnitPixel, &border_pen);
            GdipDrawRectangleI(graphics, border_pen, widget->content_rect.x, widget->content_rect.y, widget->content_rect.width, widget->content_rect.height);
            GdipDrawRectangleI(graphics, border_pen, widget->padding_rect.x, widget->padding_rect.y, widget->padding_rect.width, widget->padding_rect.height);
            GdipDrawRectangleI(graphics, border_pen, widget->margin_rect.x, widget->margin_rect.y, widget->margin_rect.width, widget->margin_rect.height);
            GdipDeletePen(border_pen);
        #endif
    }

    if (event == JAN_EVENT_GET_ID) {
        return JAN_PARAM(widget->id);
    }
    if (event == JAN_EVENT_SET_ID) {
        widget->id = (uintptr_t)param1;
    }

    if (event == JAN_EVENT_GET_WIDTH) {
        return &widget->width;
    }
    if (event == JAN_EVENT_SET_WIDTH) {
        widget->width = *(JanUnit *)param1;
    }

    if (event == JAN_EVENT_GET_HEIGHT) {
        return &widget->height;
    }
    if (event == JAN_EVENT_SET_HEIGHT) {
        widget->height = *(JanUnit *)param1;
    }

    if (event == JAN_EVENT_GET_BACKGROUND_COLOR) {
        return JAN_PARAM(widget->background_color);
    }
    if (event == JAN_EVENT_SET_BACKGROUND_COLOR) {
        widget->background_color = (uintptr_t)param1;
    }

    if (event == JAN_EVENT_GET_VISIBLE) {
        return JAN_PARAM(widget->visible);
    }
    if (event == JAN_EVENT_SET_VISIBLE) {
        widget->visible = (uintptr_t)param1;
    }

    if (event == JAN_EVENT_GET_MARGIN) {
        return &widget->margin;
    }
    if (event == JAN_EVENT_SET_MARGIN) {
        widget->margin = *(JanOffset *)param1;
    }
    if (event == JAN_EVENT_GET_MARGIN_TOP) {
        return &widget->margin.top;
    }
    if (event == JAN_EVENT_SET_MARGIN_TOP) {
        widget->margin.top = *(JanUnit *)param1;
    }
    if (event == JAN_EVENT_GET_MARGIN_RIGHT) {
        return &widget->margin.right;
    }
    if (event == JAN_EVENT_SET_MARGIN_RIGHT) {
        widget->margin.right = *(JanUnit *)param1;
    }
    if (event == JAN_EVENT_GET_MARGIN_BOTTOM) {
        return &widget->margin.bottom;
    }
    if (event == JAN_EVENT_SET_MARGIN_BOTTOM) {
        widget->margin.bottom = *(JanUnit *)param1;
    }
    if (event == JAN_EVENT_GET_MARGIN_LEFT) {
        return &widget->margin.left;
    }
    if (event == JAN_EVENT_SET_MARGIN_LEFT) {
        widget->margin.left = *(JanUnit *)param1;
    }

    if (event == JAN_EVENT_GET_PADDING) {
        return &widget->padding;
    }
    if (event == JAN_EVENT_SET_PADDING) {
        widget->padding = *(JanOffset *)param1;
    }
    if (event == JAN_EVENT_GET_PADDING_TOP) {
        return &widget->padding.top;
    }
    if (event == JAN_EVENT_SET_PADDING_TOP) {
        widget->padding.top = *(JanUnit *)param1;
    }
    if (event == JAN_EVENT_GET_PADDING_RIGHT) {
        return &widget->padding.right;
    }
    if (event == JAN_EVENT_SET_PADDING_RIGHT) {
        widget->padding.right = *(JanUnit *)param1;
    }
    if (event == JAN_EVENT_GET_PADDING_BOTTOM) {
        return &widget->padding.bottom;
    }
    if (event == JAN_EVENT_SET_PADDING_BOTTOM) {
        widget->padding.bottom = *(JanUnit *)param1;
    }
    if (event == JAN_EVENT_GET_PADDING_LEFT) {
        return &widget->padding.left;
    }
    if (event == JAN_EVENT_SET_PADDING_LEFT) {
        widget->padding.left = *(JanUnit *)param1;
    }

    return NULL;
}

void jan_widget_measure(JanWidget *widget) {
    wprintf(L"[JAN] Measure widgets\n");
    widget->event_function(widget, JAN_EVENT_MEASURE, JAN_PARAM(jan_width), JAN_PARAM(jan_height));
    widget->event_function(widget, JAN_EVENT_PLACE, JAN_PARAM(0), JAN_PARAM(0));
}

void jan_widget_draw(JanWidget *widget, HDC hdc) {
    wprintf(L"[JAN] Draw widgets\n");

    // Create back buffer
    HDC hdc_buffer = CreateCompatibleDC(hdc);
    HBITMAP bitmap_buffer = CreateCompatibleBitmap(hdc, jan_width, jan_height);
    SelectObject(hdc_buffer, bitmap_buffer);

    // Create GDI+ context and draw widgets
    GpGraphics *graphics;
    GdipCreateFromHDC(hdc_buffer, &graphics);
    GdipSetSmoothingMode(graphics, SmoothingModeAntiAlias);
    widget->event_function(widget, JAN_EVENT_DRAW, hdc_buffer, graphics);
    GdipDeleteGraphics(graphics);

    // Draw and delete back buffer
    BitBlt(hdc, 0, 0, jan_width, jan_height, hdc_buffer, 0, 0, SRCCOPY);
    DeleteObject(bitmap_buffer);
    DeleteDC(hdc_buffer);
}

void jan_widget_free(JanWidget *widget) {
    widget->event_function(widget, JAN_EVENT_FREE, NULL, NULL);
}

// JanContainer
void jan_container_init(JanContainer *container) {
    JanWidget *widget = JAN_WIDGET(container);
    jan_widget_init(widget);
    jan_list_init(&container->widgets, CONTAINER_WIDGETS_INIT_CAPACITY);
}

void jan_container_add(JanContainer *container, JanWidget *ptr) {
    JanWidget *widget = JAN_WIDGET(container);
    widget->event_function(widget, JAN_EVENT_ADD_WIDGET, ptr, NULL);
}

JanWidget *jan_container_find(JanContainer *container, uint32_t id) {
    for (size_t i = 0; i < container->widgets.size; i++) {
        JanWidget *other_widget = container->widgets.items[i];
        if (other_widget->id == id) {
            return other_widget;
        }
        else {
            JanWidget *find_widget = other_widget->event_function(other_widget, JAN_EVENT_FIND, JAN_PARAM(id), NULL);
            if (find_widget != NULL) {
                return find_widget;
            }
        }
    }
    return NULL;
}

void *jan_container_event(JanWidget *widget, uint32_t event, void *param1, void *param2) {
    JanContainer *container = JAN_CONTAINER(widget);

    if (event == JAN_EVENT_FREE) {
        for (size_t i = 0; i < container->widgets.size; i++) {
            JanWidget *other_widget = container->widgets.items[i];
            other_widget->event_function(other_widget, JAN_EVENT_FREE, NULL, NULL);
        }
        free(container->widgets.items);
    }

    if (event == JAN_EVENT_FIND) {
        uint32_t id = (uintptr_t)param1;
        for (size_t i = 0; i < container->widgets.size; i++) {
            JanWidget *other_widget = container->widgets.items[i];
            if (other_widget->id == id) {
                return other_widget;
            } else {
                JanWidget *find_widget = other_widget->event_function(other_widget, JAN_EVENT_FIND, JAN_PARAM(id), NULL);
                if (find_widget != NULL) {
                    return find_widget;
                }
            }
        }
        return NULL;
    }

    if (event == JAN_EVENT_SET_VISIBLE) {
        for (size_t i = 0; i < container->widgets.size; i++) {
            JanWidget *other_widget = container->widgets.items[i];
            jan_widget_set_visible(other_widget, param1);
        }
    }

    if (event == JAN_EVENT_ADD_WIDGET) {
        jan_list_add(&container->widgets, param1);
        return NULL;
    }

    return jan_widget_event(widget, event, param1, param2);
}

// JanStack
JanStack *jan_stack_new(void) {
    JanStack *stack = malloc(sizeof(JanStack));
    jan_stack_init(stack);
    return stack;
}

void jan_stack_init(JanStack *stack) {
    JanContainer *container = JAN_CONTAINER(stack);
    JanWidget *widget = JAN_WIDGET(stack);
    jan_container_init(container);
    stack->align = JAN_ALIGN_HORIZONTAL_LEFT || JAN_ALIGN_VERTICAL_TOP;
    widget->event_function = jan_stack_event;
}

JanAlign jan_stack_get_align(JanStack *stack) {
    JanWidget *widget = JAN_WIDGET(stack);
    return (JanAlign)(uintptr_t)widget->event_function(widget, JAN_EVENT_GET_ALIGN, NULL, NULL);
}

void jan_stack_set_align(JanStack *stack, JanAlign align) {
    JanWidget *widget = JAN_WIDGET(stack);
    widget->event_function(widget, JAN_EVENT_SET_ALIGN, JAN_PARAM(align), NULL);
}

void *jan_stack_event(JanWidget *widget, uint32_t event, void *param1, void *param2) {
    JanStack *stack = JAN_STACK(widget);
    JanContainer *container = JAN_CONTAINER(widget);

    if (event == JAN_EVENT_MEASURE) {
        int32_t parent_width = (intptr_t)param1;
        int32_t parent_height = (intptr_t)param2;

        widget->parent_width = parent_width;
        if (widget->width.type != JAN_UNIT_TYPE_WRAP) {
            widget->content_rect.width = jan_unit_to_pixels(&widget->width, parent_width - jan_unit_to_pixels(&widget->padding.left, parent_width) - jan_unit_to_pixels(&widget->padding.right, parent_width) -
                jan_unit_to_pixels(&widget->margin.left, parent_width) - jan_unit_to_pixels(&widget->margin.right, parent_width));
        } else {
            widget->content_rect.width = INT32_MAX;
        }

        widget->parent_height = parent_height;
        if (widget->height.type != JAN_UNIT_TYPE_WRAP) {
            widget->content_rect.height = jan_unit_to_pixels(&widget->height, parent_height - jan_unit_to_pixels(&widget->padding.top, parent_height) - jan_unit_to_pixels(&widget->padding.bottom, parent_height) -
                jan_unit_to_pixels(&widget->margin.top, parent_height) - jan_unit_to_pixels(&widget->margin.bottom, parent_height));
        } else {
            widget->content_rect.height = INT32_MAX;
        }

        int32_t max_width = 0;
        int32_t max_height = 0;
        for (size_t i = 0; i < container->widgets.size; i++) {
            JanWidget *other_widget = container->widgets.items[i];
            if (other_widget->visible) {
                if (other_widget->width.type == JAN_UNIT_TYPE_UNDEFINED) {
                    other_widget->width.type = JAN_UNIT_TYPE_WRAP;
                }
                if (other_widget->height.type == JAN_UNIT_TYPE_UNDEFINED) {
                    other_widget->height.type = JAN_UNIT_TYPE_WRAP;
                }
                other_widget->event_function(other_widget, JAN_EVENT_MEASURE, JAN_PARAM(widget->content_rect.width), JAN_PARAM(widget->content_rect.height));
                max_width = MAX(max_width, other_widget->margin_rect.width);
                max_height = MAX(max_height, other_widget->margin_rect.height);
            }
        }

        if (widget->width.type == JAN_UNIT_TYPE_WRAP) {
            widget->content_rect.width = max_width;
        }
        widget->padding_rect.width = jan_unit_to_pixels(&widget->padding.left, parent_width) + widget->content_rect.width + jan_unit_to_pixels(&widget->padding.right, parent_width);
        widget->margin_rect.width = jan_unit_to_pixels(&widget->margin.left, parent_width) + widget->padding_rect.width + jan_unit_to_pixels(&widget->margin.right, parent_width);

        if (widget->height.type == JAN_UNIT_TYPE_WRAP) {
            widget->content_rect.height = max_height;
        }
        widget->padding_rect.height = jan_unit_to_pixels(&widget->padding.top, parent_height) + widget->content_rect.height + jan_unit_to_pixels(&widget->padding.bottom, parent_height);
        widget->margin_rect.height = jan_unit_to_pixels(&widget->margin.top, parent_height) + widget->padding_rect.height + jan_unit_to_pixels(&widget->margin.bottom, parent_height);
        return NULL;
    }

    if (event == JAN_EVENT_PLACE) {
        int32_t x = (intptr_t)param1;
        int32_t y = (intptr_t)param2;

        widget->margin_rect.x = x;
        widget->margin_rect.y = y;
        widget->padding_rect.x = widget->margin_rect.x + jan_unit_to_pixels(&widget->margin.left, widget->parent_width);
        widget->padding_rect.y = widget->margin_rect.y + jan_unit_to_pixels(&widget->margin.top, widget->parent_height);
        widget->content_rect.x = widget->padding_rect.x + jan_unit_to_pixels(&widget->padding.left, widget->parent_width);
        widget->content_rect.y = widget->padding_rect.y + jan_unit_to_pixels(&widget->padding.top, widget->parent_height);

        for (size_t i = 0; i < container->widgets.size; i++) {
            JanWidget *other_widget = container->widgets.items[i];
            if (other_widget->visible) {
                int32_t other_widget_x = widget->content_rect.x;
                if ((stack->align & JAN_ALIGN_HORIZONTAL_CENTER) != 0) {
                    other_widget_x += (widget->content_rect.width - other_widget->margin_rect.width) / 2;
                }
                if ((stack->align & JAN_ALIGN_HORIZONTAL_RIGHT) != 0) {
                    other_widget_x += widget->content_rect.width - other_widget->margin_rect.width;
                }

                int32_t other_widget_y = widget->content_rect.y;
                if ((stack->align & JAN_ALIGN_VERTICAL_CENTER) != 0) {
                    other_widget_y += (widget->content_rect.height - other_widget->margin_rect.height) / 2;
                }
                if ((stack->align & JAN_ALIGN_VERTICAL_BOTTOM) != 0) {
                    other_widget_y += widget->content_rect.height - other_widget->margin_rect.height;
                }

                other_widget->event_function(other_widget, JAN_EVENT_PLACE, JAN_PARAM(other_widget_x), JAN_PARAM(other_widget_y));
            }
        }
        return NULL;
    }

    if (event == JAN_EVENT_DRAW && widget->visible) {
        HDC hdc = param1;
        GpGraphics *graphics = param2;
        jan_widget_event(widget, JAN_EVENT_DRAW, hdc, graphics);

        for (size_t i = 0; i < container->widgets.size; i++) {
            JanWidget *other_widget = container->widgets.items[i];
            if (other_widget->visible) {
                other_widget->event_function(other_widget, JAN_EVENT_DRAW, hdc, graphics);
            }
        }
        return NULL;
    }

    if (event == JAN_EVENT_GET_ALIGN) {
        return JAN_PARAM(stack->align);
    }
    if (event == JAN_EVENT_SET_ALIGN) {
        stack->align = (uintptr_t)param1;
        return NULL;
    }

    return jan_container_event(widget, event, param1, param2);
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
    JanWidget *widget = JAN_WIDGET(box);
    return (JanOrientation)(uintptr_t)widget->event_function(widget, JAN_EVENT_GET_ORIENTATION, NULL, NULL);
}

void jan_box_set_orientation(JanBox *box, JanOrientation orientation) {
    JanWidget *widget = JAN_WIDGET(box);
    widget->event_function(widget, JAN_EVENT_SET_ORIENTATION, JAN_PARAM(orientation), NULL);
}

JanAlign jan_box_get_align(JanBox *box) {
    JanWidget *widget = JAN_WIDGET(box);
    return (JanAlign)(uintptr_t)widget->event_function(widget, JAN_EVENT_GET_ALIGN, NULL, NULL);
}

void jan_box_set_align(JanBox *box, JanAlign align) {
    JanWidget *widget = JAN_WIDGET(box);
    widget->event_function(widget, JAN_EVENT_SET_ALIGN, JAN_PARAM(align), NULL);
}

void *jan_box_event(JanWidget *widget, uint32_t event, void *param1, void *param2) {
    JanBox *box = JAN_BOX(widget);
    JanContainer *container = JAN_CONTAINER(widget);

    if (event == JAN_EVENT_MEASURE) {
        int32_t parent_width = (intptr_t)param1;
        int32_t parent_height = (intptr_t)param2;

        widget->parent_width = parent_width;
        if (widget->width.type != JAN_UNIT_TYPE_WRAP) {
            widget->content_rect.width = jan_unit_to_pixels(&widget->width, parent_width - jan_unit_to_pixels(&widget->padding.left, parent_width) - jan_unit_to_pixels(&widget->padding.right, parent_width) -
                jan_unit_to_pixels(&widget->margin.left, parent_width) - jan_unit_to_pixels(&widget->margin.right, parent_width));
        } else {
            widget->content_rect.width = INT32_MAX;
        }

        widget->parent_height = parent_height;
        if (widget->height.type != JAN_UNIT_TYPE_WRAP) {
            widget->content_rect.height = jan_unit_to_pixels(&widget->height, parent_height - jan_unit_to_pixels(&widget->padding.top, parent_height) - jan_unit_to_pixels(&widget->padding.bottom, parent_height) -
                jan_unit_to_pixels(&widget->margin.top, parent_height) - jan_unit_to_pixels(&widget->margin.bottom, parent_height));
        } else {
            widget->content_rect.height = INT32_MAX;
        }

        int32_t sum_width = 0;
        int32_t max_width = 0;
        int32_t sum_height = 0;
        int32_t max_height = 0;
        for (size_t i = 0; i < container->widgets.size; i++) {
            JanWidget *other_widget = container->widgets.items[i];
            if (other_widget->visible) {
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
                    other_widget->height.type = JAN_UNIT_TYPE_WRAP;
                }

                other_widget->event_function(other_widget, JAN_EVENT_MEASURE, JAN_PARAM(widget->content_rect.width), JAN_PARAM(widget->content_rect.height));
                sum_width += other_widget->margin_rect.width;
                max_width = MAX(max_width, other_widget->margin_rect.width);
                sum_height += other_widget->margin_rect.height;
                max_height = MAX(max_height, other_widget->margin_rect.height);
            }
        }

        if (widget->width.type == JAN_UNIT_TYPE_WRAP) {
            if (box->orientation == JAN_ORIENTATION_HORIZONTAL) {
                widget->content_rect.width = sum_width;
            }
            if (box->orientation == JAN_ORIENTATION_VERTICAL) {
                widget->content_rect.width = max_width;
            }
        }
        widget->padding_rect.width = jan_unit_to_pixels(&widget->padding.left, parent_width) + widget->content_rect.width + jan_unit_to_pixels(&widget->padding.right, parent_width);
        widget->margin_rect.width = jan_unit_to_pixels(&widget->margin.left, parent_width) + widget->padding_rect.width + jan_unit_to_pixels(&widget->margin.right, parent_width);

        if (widget->height.type == JAN_UNIT_TYPE_WRAP) {
            if (box->orientation == JAN_ORIENTATION_HORIZONTAL) {
                widget->content_rect.height = max_height;
            }
            if (box->orientation == JAN_ORIENTATION_VERTICAL) {
                widget->content_rect.height = sum_height;
            }
        }
        widget->padding_rect.height = jan_unit_to_pixels(&widget->padding.top, parent_height) + widget->content_rect.height + jan_unit_to_pixels(&widget->padding.bottom, parent_height);
        widget->margin_rect.height = jan_unit_to_pixels(&widget->margin.top, parent_height) + widget->padding_rect.height + jan_unit_to_pixels(&widget->margin.bottom, parent_height);
        return NULL;
    }

    if (event == JAN_EVENT_PLACE) {
        int32_t x = (intptr_t)param1;
        int32_t y = (intptr_t)param2;

        widget->margin_rect.x = x;
        widget->margin_rect.y = y;
        widget->padding_rect.x = widget->margin_rect.x + jan_unit_to_pixels(&widget->margin.left, widget->parent_width);
        widget->padding_rect.y = widget->margin_rect.y + jan_unit_to_pixels(&widget->margin.top, widget->parent_height);
        widget->content_rect.x = widget->padding_rect.x + jan_unit_to_pixels(&widget->padding.left, widget->parent_width);
        widget->content_rect.y = widget->padding_rect.y + jan_unit_to_pixels(&widget->padding.top, widget->parent_height);

        int32_t sum_width = 0;
        int32_t sum_height = 0;
        for (size_t i = 0; i < container->widgets.size; i++) {
            JanWidget *other_widget = container->widgets.items[i];
            if (other_widget->visible) {
                sum_width += other_widget->margin_rect.width;
                sum_height += other_widget->margin_rect.height;
            }
        }

        x = widget->content_rect.x;
        if (box->orientation == JAN_ORIENTATION_HORIZONTAL) {
            if ((box->align & JAN_ALIGN_HORIZONTAL_CENTER) != 0) {
                x = MAX(widget->content_rect.x + (widget->content_rect.width - sum_width) / 2, widget->content_rect.x);
            }
            if ((box->align & JAN_ALIGN_HORIZONTAL_RIGHT) != 0) {
                x = MAX(widget->content_rect.x + widget->content_rect.width - sum_width, widget->content_rect.x);
            }
        }

        y = widget->content_rect.y;
        if (box->orientation == JAN_ORIENTATION_VERTICAL) {
            if ((box->align & JAN_ALIGN_VERTICAL_CENTER) != 0) {
                y = MAX(widget->content_rect.y + (widget->content_rect.height - sum_height) / 2, widget->content_rect.y);
            }
            if ((box->align & JAN_ALIGN_VERTICAL_BOTTOM) != 0) {
                y = MAX(widget->content_rect.y + widget->content_rect.height - sum_height, widget->content_rect.y);
            }
        }

        for (size_t i = 0; i < container->widgets.size; i++) {
            JanWidget *other_widget = container->widgets.items[i];
            if (other_widget->visible) {
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
        }
        return NULL;
    }

    if (event == JAN_EVENT_DRAW && widget->visible) {
        HDC hdc = param1;
        GpGraphics *graphics = param2;
        jan_widget_event(widget, JAN_EVENT_DRAW, hdc, graphics);

        for (size_t i = 0; i < container->widgets.size; i++) {
            JanWidget *other_widget = container->widgets.items[i];
            if (other_widget->visible) {
                other_widget->event_function(other_widget, JAN_EVENT_DRAW, hdc, graphics);
            }
        }
        return NULL;
    }

    if (event == JAN_EVENT_GET_ORIENTATION) {
        return JAN_PARAM(box->orientation);
    }
    if (event == JAN_EVENT_SET_ORIENTATION) {
        box->orientation = (uintptr_t)param1;
        return NULL;
    }

    if (event == JAN_EVENT_GET_ALIGN) {
        return JAN_PARAM(box->align);
    }
    if (event == JAN_EVENT_SET_ALIGN) {
        box->align = (uintptr_t)param1;
        return NULL;
    }

    return jan_container_event(widget, event, param1, param2);
}

// JanLabel
wchar_t *jan_label_default_font_name = L"Tamoha";
uint32_t jan_label_default_font_weight = JAN_FONT_WEIGHT_NORMAL;
JanUnit jan_label_default_font_size = { 16, JAN_UNIT_TYPE_SP };
JanColor jan_label_default_text_color = RGB(17, 17, 17);

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
    label->font_name = wcsdup(jan_label_default_font_name );
    label->font_weight = jan_label_default_font_weight;
    label->font_italic = false;
    label->font_underline = false;
    label->font_line_through = false;
    label->font_size = jan_label_default_font_size;
    label->text_color = jan_label_default_text_color;
    label->single_line = false;
    label->align = JAN_ALIGN_HORIZONTAL_LEFT || JAN_ALIGN_VERTICAL_TOP;
    widget->event_function = jan_label_event;
}

wchar_t *jan_label_get_text(JanLabel *label) {
    JanWidget *widget = JAN_WIDGET(label);
    return widget->event_function(widget, JAN_EVENT_GET_TEXT, NULL, NULL);
}

void jan_label_set_text(JanLabel *label, wchar_t *text) {
    JanWidget *widget = JAN_WIDGET(label);
    widget->event_function(widget, JAN_EVENT_SET_TEXT, text, NULL);
}

wchar_t *jan_label_get_font_name(JanLabel *label) {
    JanWidget *widget = JAN_WIDGET(label);
    return widget->event_function(widget, JAN_EVENT_GET_FONT_NAME, NULL, NULL);
}

void jan_label_set_font_name(JanLabel *label, wchar_t *font_name) {
    JanWidget *widget = JAN_WIDGET(label);
    widget->event_function(widget, JAN_EVENT_SET_FONT_NAME, font_name, NULL);
}

uint32_t jan_label_get_font_weight(JanLabel *label) {
    JanWidget *widget = JAN_WIDGET(label);
    return (uint32_t)(uintptr_t)widget->event_function(widget, JAN_EVENT_GET_FONT_WEIGHT, NULL, NULL);
}

void jan_label_set_font_weight(JanLabel *label, uint32_t font_weight) {
    JanWidget *widget = JAN_WIDGET(label);
    widget->event_function(widget, JAN_EVENT_SET_FONT_WEIGHT, JAN_PARAM(font_weight), NULL);
}

JanUnit *jan_label_get_font_size(JanLabel *label) {
    JanWidget *widget = JAN_WIDGET(label);
    return widget->event_function(widget, JAN_EVENT_GET_FONT_SIZE, NULL, NULL);
}

void jan_label_set_font_size(JanLabel *label, JanUnit *font_size) {
    JanWidget *widget = JAN_WIDGET(label);
    widget->event_function(widget, JAN_EVENT_SET_FONT_SIZE, JAN_PARAM(font_size), NULL);
}

bool jan_label_get_font_italic(JanLabel *label) {
    JanWidget *widget = JAN_WIDGET(label);
    return widget->event_function(widget, JAN_EVENT_GET_FONT_ITALIC, NULL, NULL);
}

void jan_label_set_font_italic(JanLabel *label, bool font_italic) {
    JanWidget *widget = JAN_WIDGET(label);
    widget->event_function(widget, JAN_EVENT_SET_FONT_ITALIC, JAN_PARAM(font_italic), NULL);
}

bool jan_label_get_font_underlne(JanLabel *label) {
    JanWidget *widget = JAN_WIDGET(label);
    return widget->event_function(widget, JAN_EVENT_GET_FONT_UNDERLINE, NULL, NULL);
}

void jan_label_set_font_underline(JanLabel *label, bool font_underline) {
    JanWidget *widget = JAN_WIDGET(label);
    widget->event_function(widget, JAN_EVENT_SET_FONT_UNDERLINE, JAN_PARAM(font_underline), NULL);
}

bool jan_label_get_font_line_through(JanLabel *label) {
    JanWidget *widget = JAN_WIDGET(label);
    return widget->event_function(widget, JAN_EVENT_GET_FONT_LINE_THROUGH, NULL, NULL);
}

void jan_label_set_font_line_through(JanLabel *label, bool font_line_through) {
    JanWidget *widget = JAN_WIDGET(label);
    widget->event_function(widget, JAN_EVENT_SET_FONT_LINE_THROUGH, JAN_PARAM(font_line_through), NULL);
}

HFONT jan_label_get_hfont(JanLabel *label) {
    return CreateFontW(jan_unit_to_pixels(&label->font_size, 0), 0, 0, 0, label->font_weight, label->font_italic,
        label->font_underline, label->font_line_through, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, label->font_name);
}

JanColor jan_label_get_text_color(JanLabel *label) {
    JanWidget *widget = JAN_WIDGET(label);
    return (JanColor)(uintptr_t)widget->event_function(widget, JAN_EVENT_GET_TEXT_COLOR, NULL, NULL);
}

void jan_label_set_text_color(JanLabel *label, JanColor text_color) {
    JanWidget *widget = JAN_WIDGET(label);
    widget->event_function(widget, JAN_EVENT_SET_TEXT_COLOR, JAN_PARAM(text_color), NULL);
}

bool jan_label_get_single_line(JanLabel *label) {
    JanWidget *widget = JAN_WIDGET(label);
    return widget->event_function(widget, JAN_EVENT_GET_SINGLE_LINE, NULL, NULL);
}

void jan_label_set_single_line(JanLabel *label, bool single_line) {
    JanWidget *widget = JAN_WIDGET(label);
    widget->event_function(widget, JAN_EVENT_SET_SINGLE_LINE, JAN_PARAM(single_line), NULL);
}

JanAlign jan_label_get_align(JanLabel *label) {
    JanWidget *widget = JAN_WIDGET(label);
    return (JanAlign)(uintptr_t)widget->event_function(widget, JAN_EVENT_GET_ALIGN, NULL, NULL);
}

void jan_label_set_align(JanLabel *label, JanAlign align) {
    JanWidget *widget = JAN_WIDGET(label);
    widget->event_function(widget, JAN_EVENT_SET_ALIGN, JAN_PARAM(align), NULL);
}

void *jan_label_event(JanWidget *widget, uint32_t event, void *param1, void *param2) {
    JanLabel *label = JAN_LABEL(widget);

    if (event == JAN_EVENT_FREE) {
        free(label->text);
        free(label->font_name);
    }

    if (event == JAN_EVENT_MEASURE) {
        int32_t parent_width = (intptr_t)param1;
        int32_t parent_height = (intptr_t)param2;

        widget->parent_width = parent_width;
        if (widget->width.type == JAN_UNIT_TYPE_WRAP) {
            HDC hdc = GetDC(NULL);
            HFONT font = jan_label_get_hfont(label);
            SelectObject(hdc, font);
            RECT measure_rect = { 0, 0, 0, 0 };
            DrawTextW(hdc, label->text, -1, &measure_rect, DT_CALCRECT);
            DeleteObject(font);
            widget->content_rect.width = MIN((int32_t)measure_rect.right - (int32_t)measure_rect.left, parent_width);
        } else {
            widget->content_rect.width = jan_unit_to_pixels(&widget->width, parent_width - jan_unit_to_pixels(&widget->padding.left, parent_width) - jan_unit_to_pixels(&widget->padding.right, parent_width) -
                jan_unit_to_pixels(&widget->margin.left, parent_width) - jan_unit_to_pixels(&widget->margin.right, parent_width));
        }
        widget->padding_rect.width = jan_unit_to_pixels(&widget->padding.left, parent_width) + widget->content_rect.width + jan_unit_to_pixels(&widget->padding.right, parent_width);
        widget->margin_rect.width = jan_unit_to_pixels(&widget->margin.left, parent_width) + widget->padding_rect.width + jan_unit_to_pixels(&widget->margin.right, parent_width);

        widget->parent_height = parent_height;
        if (widget->height.type == JAN_UNIT_TYPE_WRAP) {
            if (label->single_line) {
                widget->content_rect.height = MIN(jan_unit_to_pixels(&label->font_size, 0), parent_height);
            } else {
                HDC hdc = GetDC(NULL);
                HFONT font = jan_label_get_hfont(label);
                SelectObject(hdc, font);
                RECT measure_rect = { 0, 0, widget->content_rect.width, 0 };
                widget->content_rect.height = DrawTextW(hdc, label->text, -1, &measure_rect, DT_CALCRECT | DT_WORDBREAK);
                DeleteObject(font);
            }
        } else {
            widget->content_rect.height = jan_unit_to_pixels(&widget->height, parent_height - jan_unit_to_pixels(&widget->padding.top, parent_height) - jan_unit_to_pixels(&widget->padding.bottom, parent_height) -
                jan_unit_to_pixels(&widget->margin.top, parent_height) - jan_unit_to_pixels(&widget->margin.bottom, parent_height));
        }
        widget->padding_rect.height = jan_unit_to_pixels(&widget->padding.top, parent_height) + widget->content_rect.height + jan_unit_to_pixels(&widget->padding.bottom, parent_height);
        widget->margin_rect.height = jan_unit_to_pixels(&widget->margin.top, parent_height) + widget->padding_rect.height + jan_unit_to_pixels(&widget->margin.bottom, parent_height);
        return NULL;
    }

    if (event == JAN_EVENT_DRAW && widget->visible) {
        HDC hdc = param1;
        GpGraphics *graphics = param2;
        jan_widget_event(widget, JAN_EVENT_DRAW, hdc, graphics);

        HFONT font = jan_label_get_hfont(label);
        SelectObject(hdc, font);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, label->text_color);
        SetTextAlign(hdc, TA_LEFT);

        uint32_t style = DT_LEFT;
        if (label->single_line) {
            style |= DT_SINGLELINE;
        } else {
            style |= DT_WORDBREAK;
        }
        if ((label->align & JAN_ALIGN_HORIZONTAL_CENTER) != 0) {
            style |= DT_CENTER;
        }
        if ((label->align & JAN_ALIGN_HORIZONTAL_RIGHT) != 0) {
            style |= DT_RIGHT;
        }

        RECT content_rect = { widget->content_rect.x, widget->content_rect.y,
            widget->content_rect.x + widget->content_rect.width,
            widget->content_rect.y + widget->content_rect.height };

        if ((label->align & JAN_ALIGN_VERTICAL_CENTER) != 0) {
            if (label->single_line) {
                content_rect.top += (widget->content_rect.height - jan_unit_to_pixels(&label->font_size, 0)) / 2;
            } else {
                RECT measure_rect = { 0, 0, widget->content_rect.width, 0 };
                content_rect.top += (widget->content_rect.height - DrawTextW(hdc, label->text, -1, &measure_rect, DT_CALCRECT | DT_WORDBREAK)) / 2;
            }
        }

        if ((label->align & JAN_ALIGN_VERTICAL_BOTTOM) != 0) {
            if (label->single_line) {
                content_rect.top += widget->content_rect.height - jan_unit_to_pixels(&label->font_size, 0);
            } else {
                RECT measure_rect = { 0, 0, widget->content_rect.width, 0 };
                content_rect.top += widget->content_rect.height - DrawTextW(hdc, label->text, -1, &measure_rect, DT_CALCRECT | DT_WORDBREAK);
            }
        }

        DrawTextW(hdc, label->text, -1, &content_rect, style);
        DeleteObject(font);
        return NULL;
    }

    if (event == JAN_EVENT_GET_TEXT) {
        return label->text;
    }
    if (event == JAN_EVENT_SET_TEXT) {
        label->text = wcsdup(param1);
        return NULL;
    }

    if (event == JAN_EVENT_GET_FONT_NAME) {
        return label->font_name;
    }
    if (event == JAN_EVENT_SET_FONT_NAME) {
        label->font_name = wcsdup(param1);
        return NULL;
    }

    if (event == JAN_EVENT_GET_FONT_WEIGHT) {
        return JAN_PARAM(label->font_weight);
    }
    if (event == JAN_EVENT_SET_FONT_WEIGHT) {
        label->font_weight = (uintptr_t)param1;
        return NULL;
    }

    if (event == JAN_EVENT_GET_FONT_ITALIC) {
        return JAN_PARAM(label->font_italic);
    }
    if (event == JAN_EVENT_SET_FONT_ITALIC) {
        label->font_italic = (uintptr_t)param1;
        return NULL;
    }

    if (event == JAN_EVENT_GET_FONT_UNDERLINE) {
        return JAN_PARAM(label->font_underline);
    }
    if (event == JAN_EVENT_SET_FONT_UNDERLINE) {
        label->font_underline = (uintptr_t)param1;
        return NULL;
    }

    if (event == JAN_EVENT_GET_FONT_LINE_THROUGH) {
        return JAN_PARAM(label->font_line_through);
    }
    if (event == JAN_EVENT_SET_FONT_LINE_THROUGH) {
        label->font_line_through = (uintptr_t)param1;
        return NULL;
    }

    if (event == JAN_EVENT_GET_FONT_SIZE) {
        return &label->font_size;
    }
    if (event == JAN_EVENT_SET_FONT_SIZE) {
        label->font_size = *(JanUnit *)param1;
        return NULL;
    }

    if (event == JAN_EVENT_GET_TEXT_COLOR) {
        return JAN_PARAM(label->text_color);
    }
    if (event == JAN_EVENT_SET_TEXT_COLOR) {
        label->text_color = (uintptr_t)param1;
        return NULL;
    }

    if (event == JAN_EVENT_GET_SINGLE_LINE) {
        return JAN_PARAM(label->single_line);
    }
    if (event == JAN_EVENT_SET_SINGLE_LINE) {
        label->single_line = (uintptr_t)param1;
        return NULL;
    }

    if (event == JAN_EVENT_GET_ALIGN) {
        return JAN_PARAM(label->align);
    }
    if (event == JAN_EVENT_SET_ALIGN) {
        label->align = (uintptr_t)param1;
        return NULL;
    }

    return jan_widget_event(widget, event, param1, param2);
}

// JanButton
wchar_t *jan_button_class_name = L"BUTTON";

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

    button->hwnd = CreateWindowExW(0, jan_button_class_name, label->text, WS_CHILD, 0, 0, 0, 0, jan_hwnd, NULL, NULL, NULL);
    button->hfont = jan_label_get_hfont(label);
    SendMessageW(button->hwnd, WM_SETFONT, button->hfont, (LPARAM)TRUE);
    ShowWindow(button->hwnd, widget->visible ? SW_SHOW : SW_HIDE);

    widget->event_function = jan_button_event;
}

void *jan_button_event(JanWidget *widget, uint32_t event, void *param1, void *param2) {
    JanButton *button = JAN_BUTTON(widget);
    JanLabel *label = JAN_LABEL(widget);

    if (event == JAN_EVENT_FREE) {
        DestroyWindow(button->hwnd);
        DeleteObject(button->hfont);
    }

    if (event == JAN_EVENT_MEASURE) {
        jan_label_event(widget, JAN_EVENT_MEASURE, param1, param2);
        SetWindowPos(button->hwnd, NULL, 0, 0, widget->padding_rect.width, widget->padding_rect.height, SWP_NOZORDER | SWP_NOMOVE);
        return NULL;
    }

    if (event == JAN_EVENT_PLACE) {
        jan_label_event(widget, JAN_EVENT_PLACE, param1, param2);
        SetWindowPos(button->hwnd, NULL, widget->padding_rect.x, widget->padding_rect.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
        return NULL;
    }

    if (event == JAN_EVENT_DRAW) {
        #ifdef JAN_DEBUG
            GpGraphics *graphics = param2;
            GpPen *border_pen;
            GdipCreatePen1(0x40ff0000, 2, UnitPixel, &border_pen);
            GdipDrawRectangleI(graphics, border_pen, widget->margin_rect.x, widget->margin_rect.y, widget->margin_rect.width, widget->margin_rect.height);
            GdipDeletePen(border_pen);
        #endif
        return NULL;
    }

    if (event == JAN_EVENT_SET_ID) {
        jan_label_event(widget, event, param1, param2);
        DestroyWindow(button->hwnd);
        button->hwnd = CreateWindowExW(0, jan_button_class_name, label->text, WS_CHILD, 0, 0, 0, 0, jan_hwnd, (HMENU)(size_t)widget->id, NULL, NULL);
        SetWindowPos(button->hwnd, NULL, widget->padding_rect.x, widget->padding_rect.y, widget->padding_rect.width, widget->padding_rect.height, SWP_NOZORDER);
        SendMessageW(button->hwnd, WM_SETFONT, button->hfont, (LPARAM)TRUE);
        ShowWindow(button->hwnd, widget->visible ? SW_SHOW : SW_HIDE);
        return NULL;
    }

    if (event == JAN_EVENT_SET_VISIBLE) {
        jan_label_event(widget, event, param1, param2);
        ShowWindow(button->hwnd, widget->visible ? SW_SHOW : SW_HIDE);
        return NULL;
    }

    if (event == JAN_EVENT_SET_TEXT) {
        jan_label_event(widget, event, param1, param2);
        SendMessageW(button->hwnd, WM_SETTEXT, NULL, label->text);
        return NULL;
    }

    if (
        event == JAN_EVENT_SET_FONT_NAME || event == JAN_EVENT_SET_FONT_WEIGHT ||
        event == JAN_EVENT_SET_FONT_ITALIC || event == JAN_EVENT_SET_FONT_UNDERLINE ||
        event == JAN_EVENT_SET_FONT_LINE_THROUGH || event == JAN_EVENT_SET_FONT_SIZE
    ) {
        jan_label_event(widget, event, param1, param2);
        DeleteObject(button->hfont);
        button->hfont = jan_label_get_hfont(label);
        SendMessageW(button->hwnd, WM_SETFONT, button->hfont, (LPARAM)TRUE);
        return NULL;
    }

    return jan_label_event(widget, event, param1, param2);
}

// JanEdit
wchar_t *jan_edit_class_name = L"EDIT";

JanEdit *jan_edit_new(void) {
    JanEdit *edit = malloc(sizeof(JanEdit));
    jan_edit_init(edit);
    return edit;
}

void jan_edit_init(JanEdit *edit) {
    JanLabel *label = JAN_LABEL(edit);
    JanWidget *widget = JAN_WIDGET(edit);
    jan_label_init(label);
    label->single_line = true;

    edit->hwnd = CreateWindowExW(0, jan_edit_class_name, label->text, WS_CHILD | ES_AUTOHSCROLL, 0, 0, 0, 0, jan_hwnd, NULL, NULL, NULL);
    edit->hfont = jan_label_get_hfont(label);
    SendMessageW(edit->hwnd, WM_SETFONT, edit->hfont, (LPARAM)TRUE);
    ShowWindow(edit->hwnd, widget->visible ? SW_SHOW : SW_HIDE);

    widget->event_function = jan_edit_event;
}

void *jan_edit_event(JanWidget *widget, uint32_t event, void *param1, void *param2) {
    JanEdit *edit = JAN_EDIT(widget);
    JanLabel *label = JAN_LABEL(widget);

    if (event == JAN_EVENT_FREE) {
        DestroyWindow(edit->hwnd);
        DeleteObject(edit->hfont);
    }

    if (event == JAN_EVENT_MEASURE) {
        jan_label_event(widget, JAN_EVENT_MEASURE, param1, param2);
        SetWindowPos(edit->hwnd, NULL, 0, 0, widget->padding_rect.width, widget->padding_rect.height, SWP_NOZORDER | SWP_NOMOVE);
        return NULL;
    }

    if (event == JAN_EVENT_PLACE) {
        jan_label_event(widget, JAN_EVENT_PLACE, param1, param2);
        SetWindowPos(edit->hwnd, NULL, widget->padding_rect.x, widget->padding_rect.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
        return NULL;
    }

    if (event == JAN_EVENT_DRAW) {
        #ifdef JAN_DEBUG
            GpGraphics *graphics = param2;
            GpPen *border_pen;
            GdipCreatePen1(0x40ff0000, 2, UnitPixel, &border_pen);
            GdipDrawRectangleI(graphics, border_pen, widget->margin_rect.x, widget->margin_rect.y, widget->margin_rect.width, widget->margin_rect.height);
            GdipDeletePen(border_pen);
        #endif
        return NULL;
    }

    if (event == JAN_EVENT_SET_ID) {
        jan_label_event(widget, event, param1, param2);
        DestroyWindow(edit->hwnd);
        edit->hwnd = CreateWindowExW(0, jan_edit_class_name, label->text, WS_CHILD | ES_AUTOHSCROLL, 0, 0, 0, 0, jan_hwnd, (HMENU)(size_t)widget->id, NULL, NULL);
        SetWindowPos(edit->hwnd, NULL, widget->padding_rect.x, widget->padding_rect.y, widget->padding_rect.width, widget->padding_rect.height, SWP_NOZORDER);
        SendMessageW(edit->hwnd, WM_SETFONT, edit->hfont, (LPARAM)TRUE);
        ShowWindow(edit->hwnd, widget->visible ? SW_SHOW : SW_HIDE);
        return NULL;
    }

    if (event == JAN_EVENT_SET_VISIBLE) {
        jan_label_event(widget, event, param1, param2);
        ShowWindow(edit->hwnd, widget->visible ? SW_SHOW : SW_HIDE);
        return NULL;
    }

    if (event == JAN_EVENT_GET_TEXT) {
        uint32_t text_length = SendMessageW(edit->hwnd, WM_GETTEXTLENGTH, NULL, NULL);
        wchar_t *string_buffer = malloc((text_length + 1) * sizeof(wchar_t));
        SendMessageW(edit->hwnd, WM_GETTEXT, (WPARAM)(size_t)(text_length + 1), string_buffer);
        return string_buffer;
    }

    if (event == JAN_EVENT_SET_TEXT) {
        jan_label_event(widget, event, param1, param2);
        SendMessageW(edit->hwnd, WM_SETTEXT, NULL, label->text);
        return NULL;
    }

    if (
        event == JAN_EVENT_SET_FONT_NAME || event == JAN_EVENT_SET_FONT_WEIGHT ||
        event == JAN_EVENT_SET_FONT_ITALIC || event == JAN_EVENT_SET_FONT_UNDERLINE ||
        event == JAN_EVENT_SET_FONT_LINE_THROUGH || event == JAN_EVENT_SET_FONT_SIZE
    ) {
        jan_label_event(widget, event, param1, param2);
        DeleteObject(edit->hfont);
        edit->hfont = jan_label_get_hfont(label);
        SendMessageW(edit->hwnd, WM_SETFONT, edit->hfont, (LPARAM)TRUE);
        return NULL;
    }

    return jan_label_event(widget, event, param1, param2);
}

// JanComboBox
wchar_t *jan_combobox_class_name = L"COMBOBOX";

JanComboBox *jan_combobox_new(void) {
    JanComboBox *combobox = malloc(sizeof(JanComboBox));
    jan_combobox_init(combobox);
    return combobox;
}

void jan_combobox_init(JanComboBox *combobox) {
    JanLabel *label = JAN_LABEL(combobox);
    JanWidget *widget = JAN_WIDGET(combobox);
    jan_label_init(label);
    label->single_line = true;

    combobox->hwnd = CreateWindowExW(0, jan_combobox_class_name, label->text, WS_CHILD | CBS_DROPDOWNLIST | CBS_HASSTRINGS, 0, 0, 0, 0, jan_hwnd, NULL, NULL, NULL);
    combobox->hfont = jan_label_get_hfont(label);
    SendMessageW(combobox->hwnd, WM_SETFONT, combobox->hfont, (LPARAM)TRUE);
    ShowWindow(combobox->hwnd, widget->visible ? SW_SHOW : SW_HIDE);

    widget->event_function = jan_combobox_event;
}

void jan_comboxbox_add(JanComboBox *combobox, wchar_t *string) {
    JanWidget *widget = JAN_WIDGET(combobox);
    widget->event_function(widget, JAN_EVENT_ADD_STRING, string, NULL);
}

void *jan_combobox_event(JanWidget *widget, uint32_t event, void *param1, void *param2) {
    JanComboBox *combobox = JAN_COMBOBOX(widget);
    JanLabel *label = JAN_LABEL(widget);

    if (event == JAN_EVENT_FREE) {
        DestroyWindow(combobox->hwnd);
        DeleteObject(combobox->hfont);
    }

    if (event == JAN_EVENT_MEASURE) {
        jan_label_event(widget, JAN_EVENT_MEASURE, param1, param2);
        SetWindowPos(combobox->hwnd, NULL, 0, 0, widget->padding_rect.width, widget->padding_rect.height, SWP_NOZORDER | SWP_NOMOVE);
        return NULL;
    }

    if (event == JAN_EVENT_PLACE) {
        jan_label_event(widget, JAN_EVENT_PLACE, param1, param2);
        SetWindowPos(combobox->hwnd, NULL, widget->padding_rect.x, widget->padding_rect.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
        return NULL;
    }

    if (event == JAN_EVENT_DRAW) {
        #ifdef JAN_DEBUG
            GpGraphics *graphics = param2;
            GpPen *border_pen;
            GdipCreatePen1(0x40ff0000, 2, UnitPixel, &border_pen);
            GdipDrawRectangleI(graphics, border_pen, widget->margin_rect.x, widget->margin_rect.y, widget->margin_rect.width, widget->margin_rect.height);
            GdipDeletePen(border_pen);
        #endif
        return NULL;
    }

    if (event == JAN_EVENT_SET_ID) {
        jan_label_event(widget, event, param1, param2);
        DestroyWindow(combobox->hwnd);
        combobox->hwnd = CreateWindowExW(0, jan_combobox_class_name, label->text, WS_CHILD | CBS_DROPDOWNLIST | CBS_HASSTRINGS, 0, 0, 0, 0, jan_hwnd, (HMENU)(size_t)widget->id, NULL, NULL);
        SetWindowPos(combobox->hwnd, NULL, widget->padding_rect.x, widget->padding_rect.y, widget->padding_rect.width, widget->padding_rect.height, SWP_NOZORDER);
        SendMessageW(combobox->hwnd, WM_SETFONT, combobox->hfont, (LPARAM)TRUE);
        ShowWindow(combobox->hwnd, widget->visible ? SW_SHOW : SW_HIDE);
        return NULL;
    }

    if (event == JAN_EVENT_SET_VISIBLE) {
        jan_label_event(widget, event, param1, param2);
        ShowWindow(combobox->hwnd, widget->visible ? SW_SHOW : SW_HIDE);
        return NULL;
    }

    if (event == JAN_EVENT_GET_TEXT) {
        // TODO
        return NULL;
    }

    if (event == JAN_EVENT_SET_TEXT) {
        // TODO
        return NULL;
    }

    if (
        event == JAN_EVENT_SET_FONT_NAME || event == JAN_EVENT_SET_FONT_WEIGHT ||
        event == JAN_EVENT_SET_FONT_ITALIC || event == JAN_EVENT_SET_FONT_UNDERLINE ||
        event == JAN_EVENT_SET_FONT_LINE_THROUGH || event == JAN_EVENT_SET_FONT_SIZE
    ) {
        jan_label_event(widget, event, param1, param2);
        DeleteObject(combobox->hfont);
        combobox->hfont = jan_label_get_hfont(label);
        SendMessageW(combobox->hwnd, WM_SETFONT, combobox->hfont, (LPARAM)TRUE);
        return NULL;
    }

    if (event == JAN_EVENT_ADD_STRING) {
        SendMessageW(combobox->hwnd, CB_ADDSTRING, NULL, param1);
    }

    return jan_label_event(widget, event, param1, param2);
}

// JanLoader
uint8_t *jan_load(uint8_t *data, JanWidget **widget) {
    uint16_t widget_type = *(uint16_t *)data;
    data += sizeof(uint16_t);
    if (widget_type == JAN_TYPE_WIDGET) *widget = jan_widget_new();
    if (widget_type == JAN_TYPE_STACK) *widget = JAN_WIDGET(jan_stack_new());
    if (widget_type == JAN_TYPE_BOX) *widget = JAN_WIDGET(jan_box_new());
    if (widget_type == JAN_TYPE_LABEL) *widget = JAN_WIDGET(jan_label_new());
    if (widget_type == JAN_TYPE_BUTTON) *widget = JAN_WIDGET(jan_button_new());
    if (widget_type == JAN_TYPE_EDIT) *widget = JAN_WIDGET(jan_edit_new());
    if (widget_type == JAN_TYPE_COMBOBOX) *widget = JAN_WIDGET(jan_combobox_new());

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
            jan_widget_set_width(*widget, &unit);
        }
        if (attribute == JAN_ATTRIBUTE_HEIGHT) {
            JanUnit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_height(*widget, &unit);
        }
        if (attribute == JAN_ATTRIBUTE_BACKGROUND_COLOR) {
            jan_widget_set_background_color(*widget, *(JanColor *)data);
            data += sizeof(JanColor);
        }
        if (attribute == JAN_ATTRIBUTE_VISIBLE) {
            jan_widget_set_visible(*widget, *(uint8_t *)data);
            data += sizeof(uint8_t);
        }
        if (attribute == JAN_ATTRIBUTE_MARGIN) {
            JanUnit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_margin_top(*widget, &unit);

            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_margin_right(*widget, &unit);

            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_margin_bottom(*widget, &unit);

            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_margin_left(*widget, &unit);
        }
        if (attribute == JAN_ATTRIBUTE_MARGIN_TOP) {
            JanUnit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_margin_top(*widget, &unit);
        }
        if (attribute == JAN_ATTRIBUTE_MARGIN_RIGHT) {
            JanUnit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_margin_right(*widget, &unit);
        }
        if (attribute == JAN_ATTRIBUTE_MARGIN_BOTTOM) {
            JanUnit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_margin_bottom(*widget, &unit);
        }
        if (attribute == JAN_ATTRIBUTE_MARGIN_LEFT) {
            JanUnit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_margin_left(*widget, &unit);
        }
        if (attribute == JAN_ATTRIBUTE_PADDING) {
            JanUnit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_padding_top(*widget, &unit);

            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_padding_right(*widget, &unit);

            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_padding_bottom(*widget, &unit);

            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_padding_left(*widget, &unit);
        }
        if (attribute == JAN_ATTRIBUTE_PADDING_TOP) {
            JanUnit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_padding_top(*widget, &unit);
        }
        if (attribute == JAN_ATTRIBUTE_PADDING_RIGHT) {
            JanUnit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_padding_right(*widget, &unit);
        }
        if (attribute == JAN_ATTRIBUTE_PADDING_BOTTOM) {
            JanUnit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_padding_bottom(*widget, &unit);
        }
        if (attribute == JAN_ATTRIBUTE_PADDING_LEFT) {
            JanUnit unit;
            unit.value = *(float *)data;
            data += sizeof(float);
            unit.type = *(uint8_t *)data;
            data += sizeof(uint8_t);
            jan_widget_set_padding_left(*widget, &unit);
        }

        // Container attributes
        if (widget_type == JAN_TYPE_CONTAINER || widget_type == JAN_TYPE_STACK || widget_type == JAN_TYPE_BOX) {
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

        // Stack attributes
        if (widget_type == JAN_TYPE_STACK) {
            if (attribute == JAN_ATTRIBUTE_ALIGN) {
                jan_stack_set_align(JAN_STACK(*widget), *(uint8_t *)data);
                data += sizeof(uint8_t);
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
        if (widget_type == JAN_TYPE_LABEL || widget_type == JAN_TYPE_BUTTON || widget_type == JAN_TYPE_EDIT || widget_type == JAN_TYPE_COMBOBOX) {
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
                jan_label_set_font_size(JAN_LABEL(*widget), &unit);
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
