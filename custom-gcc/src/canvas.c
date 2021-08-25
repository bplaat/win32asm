#include "canvas.h"

Canvas *Canvas_New(HWND hwnd, CanvasRenderer renderer) {
    Canvas *canvas = malloc(sizeof(Canvas));
    canvas->renderer = renderer;
    canvas->width = -1;
    canvas->height = -1;

    HMODULE hd2d1 = LoadLibraryW(L"d2d1");
    HMODULE hdwrite = LoadLibraryW(L"dwrite");
    if (canvas->renderer == CANVAS_RENDERER_DEFAULT) {
        canvas->renderer = hd2d1 && hdwrite ? CANVAS_RENDERER_DIRECT2D : CANVAS_RENDERER_GDI;
    }

    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        canvas->data.gdi.hdc = GetDC(hwnd);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        #ifdef __GNUC__
            __extension__
        #endif
        _D2D1CreateFactory D2D1CreateFactory = GetProcAddress(hd2d1, "D2D1CreateFactory");
        GUID ID2D1Factory_guid = { 0xbb12d362, 0xdaee, 0x4b9a, { 0xaa, 0x1d, 0x14, 0xba, 0x40, 0x1c, 0xfa, 0x1f } };
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &ID2D1Factory_guid, NULL, &canvas->data.d2d.d2d_factory);

        #ifdef __GNUC__
            __extension__
        #endif
        _DWriteCreateFactory DWriteCreateFactory = GetProcAddress(hdwrite, "DWriteCreateFactory");
        GUID IDWriteFactory_guid = { 0xb859ee5a, 0xd838, 0x4b5b, { 0xa2, 0xe8, 0x1a, 0xdc, 0x7d, 0x93, 0xdb, 0x48 } };
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, &IDWriteFactory_guid, &canvas->data.d2d.dwrite_factory);

        D2D1_RENDER_TARGET_PROPERTIES render_props = { D2D1_RENDER_TARGET_TYPE_DEFAULT,
            { DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_UNKNOWN },
            96, 96, D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT };
        D2D1_HWND_RENDER_TARGET_PROPERTIES hwnd_render_props = { hwnd, { 0, 0 }, D2D1_PRESENT_OPTIONS_NONE };
        ID2D1Factory_CreateHwndRenderTarget(canvas->data.d2d.d2d_factory, &render_props, &hwnd_render_props, &canvas->data.d2d.render_target);
    }

    return canvas;
}

void Canvas_Free(Canvas *canvas) {
    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        DeleteObject(canvas->data.gdi.alpha_bitmap);
        DeleteDC(canvas->data.gdi.alpha_hdc);

        DeleteObject(canvas->data.gdi.buffer_bitmap);
        DeleteDC(canvas->data.gdi.buffer_hdc);

        DeleteDC(canvas->data.gdi.hdc);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        IUnknown_Release(canvas->data.d2d.render_target);
        IUnknown_Release(canvas->data.d2d.dwrite_factory);
        IUnknown_Release(canvas->data.d2d.d2d_factory);
    }

    free(canvas);
}

void Canvas_Resize(Canvas *canvas, int32_t width, int32_t height) {
    if (canvas->renderer == CANVAS_RENDERER_GDI && canvas->width != -1 && canvas->height != -1) {
        DeleteObject(canvas->data.gdi.alpha_bitmap);
        DeleteDC(canvas->data.gdi.alpha_hdc);

        DeleteObject(canvas->data.gdi.buffer_bitmap);
        DeleteDC(canvas->data.gdi.buffer_hdc);
    }

    canvas->width = width;
    canvas->height = height;

    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        canvas->data.gdi.buffer_hdc = CreateCompatibleDC(canvas->data.gdi.hdc);
        SetBkMode(canvas->data.gdi.buffer_hdc, TRANSPARENT);
        SetStretchBltMode(canvas->data.gdi.buffer_hdc, STRETCH_HALFTONE);
        canvas->data.gdi.buffer_bitmap = CreateCompatibleBitmap(canvas->data.gdi.hdc, canvas->width, canvas->height);
        SelectObject(canvas->data.gdi.buffer_hdc, canvas->data.gdi.buffer_bitmap);

        canvas->data.gdi.alpha_hdc = CreateCompatibleDC(canvas->data.gdi.hdc);
        SetBkMode(canvas->data.gdi.alpha_hdc, TRANSPARENT);
        SetStretchBltMode(canvas->data.gdi.alpha_hdc, STRETCH_HALFTONE);
        canvas->data.gdi.alpha_bitmap = CreateCompatibleBitmap(canvas->data.gdi.hdc, canvas->width * 2, canvas->height * 2);
        SelectObject(canvas->data.gdi.alpha_hdc, canvas->data.gdi.alpha_bitmap);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        D2D1_SIZE_U size = { canvas->width, canvas->height };
        ID2D1HwndRenderTarget_Resize(canvas->data.d2d.render_target, &size);
    }
}

void Canvas_BeginDraw(Canvas *canvas) {
    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1RenderTarget_BeginDraw(canvas->data.d2d.render_target);
    }
}

void Canvas_EndDraw(Canvas *canvas) {
    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        BitBlt(canvas->data.gdi.hdc, 0, 0, canvas->width, canvas->height, canvas->data.gdi.buffer_hdc, 0, 0, SRCCOPY);
    }
    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1RenderTarget_EndDraw(canvas->data.d2d.render_target, NULL, NULL);
    }
}

void Canvas_FillRect(Canvas *canvas, CanvasRect *rect, CanvasColor color) {
    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        HBRUSH brush = CreateSolidBrush(color & 0x00ffffff);
        if ((color >> 24) == 0xff) {
            RECT real_rect = { rect->x, rect->y, rect->x + rect->width, rect->y + rect->height };
            FillRect(canvas->data.gdi.buffer_hdc, &real_rect, brush);
        } else {
            RECT real_rect = { 0, 0, rect->width, rect->height };
            FillRect(canvas->data.gdi.alpha_hdc, &real_rect, brush);
            BLENDFUNCTION blend = { AC_SRC_OVER, 0, color >> 24, 0 };
            GdiAlphaBlend(canvas->data.gdi.buffer_hdc, rect->x, rect->y, rect->width, rect->height, canvas->data.gdi.alpha_hdc, 0, 0, rect->width, rect->height, blend);
        }
        DeleteObject(brush);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        D2D1_COLOR_F color_float = { (float)(color & 0xff) / 255, (float)((color >> 8) & 0xff) / 255,
            (float)((color >> 16) & 0xff) / 255, (float)((color >> 24) & 0xff) / 255 };
        ID2D1Brush *brush;
        ID2D1RenderTarget_CreateSolidColorBrush(canvas->data.d2d.render_target, &color_float, NULL, &brush);
        D2D1_RECT_F real_rect = { rect->x, rect->y, rect->x + rect->width, rect->y + rect->height };
        ID2D1RenderTarget_FillRectangle(canvas->data.d2d.render_target, &real_rect, brush);
        IUnknown_Release(brush);
    }
}

void Canvas_StrokeRect(Canvas *canvas, CanvasRect *rect, CanvasColor color, float stroke_width) {
    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        HPEN pen = CreatePen(PS_SOLID, stroke_width, color & 0x00ffffff);
        if ((color >> 24) == 0xff) {
            SelectObject(canvas->data.gdi.buffer_hdc, pen);
            SelectObject(canvas->data.gdi.buffer_hdc, GetStockObject(NULL_BRUSH));
            Rectangle(canvas->data.gdi.buffer_hdc, rect->x, rect->y, rect->x + rect->width + 1, rect->y + rect->height + 1);
        } else {
            BitBlt(canvas->data.gdi.alpha_hdc, 0, 0, rect->width + stroke_width * 2, rect->height + stroke_width * 2, canvas->data.gdi.buffer_hdc, rect->x - stroke_width, rect->y - stroke_width, SRCCOPY);
            SelectObject(canvas->data.gdi.alpha_hdc, pen);
            SelectObject(canvas->data.gdi.alpha_hdc, GetStockObject(NULL_BRUSH));
            Rectangle(canvas->data.gdi.alpha_hdc, stroke_width, stroke_width, stroke_width + rect->width + 1, stroke_width + rect->height + 1);
            BLENDFUNCTION blend = { AC_SRC_OVER, 0, color >> 24, 0 };
            GdiAlphaBlend(canvas->data.gdi.buffer_hdc, rect->x - stroke_width, rect->y - stroke_width, rect->width + stroke_width * 2, rect->height + stroke_width * 2,
                canvas->data.gdi.alpha_hdc, 0, 0, rect->width + stroke_width * 2, rect->height + stroke_width * 2, blend);
        }
        DeleteObject(pen);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        D2D1_COLOR_F color_float = { (float)(color & 0xff) / 255, (float)((color >> 8) & 0xff) / 255,
            (float)((color >> 16) & 0xff) / 255, (float)((color >> 24) & 0xff) / 255 };
        ID2D1Brush *brush;
        ID2D1RenderTarget_CreateSolidColorBrush(canvas->data.d2d.render_target, &color_float, NULL, &brush);
        D2D1_RECT_F real_rect = { rect->x, rect->y, rect->x + rect->width, rect->y + rect->height };
        ID2D1RenderTarget_DrawRectangle(canvas->data.d2d.render_target, &real_rect, brush, stroke_width, NULL);
        IUnknown_Release(brush);
    }
}

void Canvas_DrawText(Canvas *canvas, wchar_t *text, int32_t length, CanvasRect *rect, CanvasFont *font, CanvasAlign align, CanvasColor color) {
    if (length == -1) length = wcslen(text);

    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        int32_t weight = FW_NORMAL;
        if (font->weight == CANVAS_FONT_WEIGHT_BOLD) weight = FW_BOLD;
        HFONT hfont = CreateFontW(-MulDiv(font->size, 96, 72), 0, 0, 0, weight, font->italic, font->underline, font->line_through,
            ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font->name);
        SelectObject(canvas->data.gdi.buffer_hdc, hfont);

        RECT measure_rect = { 0, 0, rect->width, 0 };
        if (rect->height == 0 || (align & CANVAS_ALIGN_VCENTER) != 0 || (align & CANVAS_ALIGN_BOTTOM) != 0) {
            DrawTextW(canvas->data.gdi.buffer_hdc, text, length, &measure_rect, DT_CALCRECT | DT_WORDBREAK);
            if (rect->height == 0) rect->height = measure_rect.bottom;
        }

        int32_t options = DT_WORDBREAK;
        int32_t y = 0;
        if ((align & CANVAS_ALIGN_CENTER) != 0) {
            options |= DT_CENTER;
        }
        if ((align & CANVAS_ALIGN_RIGHT) != 0) {
            options |= DT_RIGHT;
        }
        if ((align & CANVAS_ALIGN_VCENTER) != 0) {
            options |= DT_VCENTER;
            y = (rect->height - measure_rect.bottom) / 2;
        }
        if ((align & CANVAS_ALIGN_BOTTOM) != 0) {
            options |= DT_BOTTOM;
            y = rect->height - measure_rect.bottom;
        }

        if ((color >> 24) == 0xff) {
            SetTextColor(canvas->data.gdi.buffer_hdc, color & 0x00ffffff);
            RECT real_rect = { rect->x, rect->y + y, rect->x + rect->width, rect->y + rect->height };
            DrawTextW(canvas->data.gdi.buffer_hdc, text, length, &real_rect, options);
        } else {
            BitBlt(canvas->data.gdi.alpha_hdc, 0, 0, rect->width, rect->height, canvas->data.gdi.buffer_hdc, rect->x, rect->y, SRCCOPY);
            SelectObject(canvas->data.gdi.alpha_hdc, hfont);
            SetTextColor(canvas->data.gdi.alpha_hdc, color & 0x00ffffff);
            RECT real_rect = { 0, y, rect->width, rect->height };
            DrawTextW(canvas->data.gdi.alpha_hdc, text, length, &real_rect, options);
            BLENDFUNCTION blend = { AC_SRC_OVER, 0, color >> 24, 0 };
            GdiAlphaBlend(canvas->data.gdi.buffer_hdc, rect->x, rect->y, rect->width, rect->height, canvas->data.gdi.alpha_hdc, 0, 0, rect->width, rect->height, blend);
        }
        DeleteObject(hfont);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        D2D1_COLOR_F color_float = { (float)(color & 0xff) / 255, (float)((color >> 8) & 0xff) / 255,
            (float)((color >> 16) & 0xff) / 255, (float)((color >> 24) & 0xff) / 255 };
        ID2D1Brush *brush;
        ID2D1RenderTarget_CreateSolidColorBrush(canvas->data.d2d.render_target, &color_float, NULL, &brush);

        IDWriteTextFormat *text_format;
        int32_t weight = DWRITE_FONT_WEIGHT_NORMAL;
        if (font->weight == CANVAS_FONT_WEIGHT_BOLD) weight = DWRITE_FONT_WEIGHT_BOLD;
        IDWriteFactory_CreateTextFormat(canvas->data.d2d.dwrite_factory, font->name, NULL, weight,
            font->italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            (font->size / 72) * 96, L"", &text_format);
        if ((align & CANVAS_ALIGN_CENTER) != 0) IDWriteTextFormat_SetTextAlignment(text_format, DWRITE_TEXT_ALIGNMENT_CENTER);
        if ((align & CANVAS_ALIGN_RIGHT) != 0) IDWriteTextFormat_SetTextAlignment(text_format, DWRITE_TEXT_ALIGNMENT_TRAILING);
        if ((align & CANVAS_ALIGN_VCENTER) != 0) IDWriteTextFormat_SetParagraphAlignment(text_format, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        if ((align & CANVAS_ALIGN_BOTTOM) != 0) IDWriteTextFormat_SetParagraphAlignment(text_format, DWRITE_PARAGRAPH_ALIGNMENT_FAR);

        IDWriteTextLayout *text_layout;
        IDWriteFactory_CreateTextLayout(canvas->data.d2d.dwrite_factory, text, length, text_format, rect->width, rect->height == 0 ? canvas->height * 2 : rect->height, &text_layout);
        if (font->underline) IDWriteTextLayout_SetUnderline(text_layout, true, ((DWRITE_TEXT_RANGE){ 0, length}));
        if (font->line_through) IDWriteTextLayout_SetStrikethrough(text_layout, true, ((DWRITE_TEXT_RANGE){ 0, length}));
        if (rect->height == 0) {
            DWRITE_TEXT_METRICS metrics;
            IDWriteTextLayout_GetMetrics(text_layout, &metrics);
            rect->height = metrics.height;
        }

        ID2D1RenderTarget_DrawTextLayout(canvas->data.d2d.render_target, ((D2D1_POINT_2F){ rect->x, rect->y }), text_layout, brush,
            D2D1_DRAW_TEXT_OPTIONS_CLIP | D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);

        IUnknown_Release(text_format);
        IUnknown_Release(text_layout);
        IUnknown_Release(brush);
    }
}

float Canvas_ParsePathFloat(char **string) {
    float number = 0;
    bool negative = false;
    int32_t precision = 0;
    char *c = *string;
    while (*c != '\0' && ((*c == '-' && !negative) || *c == '.' || (*c >= '0' && *c <= '9'))) {
        if (*c == '-') {
            negative = true;
            c++;
        } else if (*c == '.') {
            precision = 10;
            c++;
        } else {
            if (precision > 0) {
                number += (float)(*c++ - '0') / precision;
                precision *= 10;
            } else {
                number = number * 10 + (*c++ - '0');
            }
        }
    }
    *string = c;
    return negative ? -number : number;
}

void Canvas_FillPath(Canvas *canvas, CanvasRect *rect, int32_t viewport_width, int32_t viewport_height, char *path, CanvasColor color) {
    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        int32_t smooth_scale = 2; // Experimental smoothing feature gives dark border artifacts
        StretchBlt(canvas->data.gdi.alpha_hdc, 0, 0, rect->width * smooth_scale, rect->height * smooth_scale, canvas->data.gdi.buffer_hdc, rect->x, rect->y, rect->width, rect->height, SRCCOPY);
        BeginPath(canvas->data.gdi.alpha_hdc);
        float x = 0;
        float y = 0;
        float scale_x = (float)(rect->width * smooth_scale) / viewport_width;
        float scale_y = (float)(rect->height * smooth_scale) / viewport_height;
        char *c = path;
        while (*c != '\0') {
            if (*c == 'M') {
                c++;
                x = Canvas_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y = Canvas_ParsePathFloat(&c);
                MoveToEx(canvas->data.gdi.alpha_hdc, x * scale_x, y * scale_y, NULL);
            } else if (*c == 'm') {
                c++;
                x += Canvas_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y += Canvas_ParsePathFloat(&c);
                MoveToEx(canvas->data.gdi.alpha_hdc, x * scale_x, y * scale_y, NULL);
            } else if (*c == 'L') {
                c++;
                x = Canvas_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y = Canvas_ParsePathFloat(&c);
                LineTo(canvas->data.gdi.alpha_hdc, x * scale_x, y * scale_y);
            } else if (*c == 'l') {
                c++;
                x += Canvas_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y += Canvas_ParsePathFloat(&c);
                LineTo(canvas->data.gdi.alpha_hdc, x * scale_x, y * scale_y);
            } else if (*c == 'H') {
                c++;
                x = Canvas_ParsePathFloat(&c);
                LineTo(canvas->data.gdi.alpha_hdc, x * scale_x, y * scale_y);
            } else if (*c == 'h') {
                c++;
                x += Canvas_ParsePathFloat(&c);
                LineTo(canvas->data.gdi.alpha_hdc, x * scale_x, y * scale_y);
            } else if (*c == 'V') {
                c++;
                y = Canvas_ParsePathFloat(&c);
                LineTo(canvas->data.gdi.alpha_hdc, x * scale_x, y * scale_y);
            } else if (*c == 'v') {
                c++;
                y += Canvas_ParsePathFloat(&c);
                LineTo(canvas->data.gdi.alpha_hdc, x * scale_x, y * scale_y);
            } else if (*c == 'Z' || *c == 'z') {
                c++;
            }
        }
        EndPath(canvas->data.gdi.alpha_hdc);
        HBRUSH brush = CreateSolidBrush(color & 0x00ffffff);
        SelectObject(canvas->data.gdi.alpha_hdc, brush);
        FillPath(canvas->data.gdi.alpha_hdc);
        DeleteObject(brush);
        if ((color >> 24) == 0xff) {
            StretchBlt(canvas->data.gdi.buffer_hdc, rect->x, rect->y, rect->width, rect->height, canvas->data.gdi.alpha_hdc, 0, 0, rect->width * smooth_scale, rect->height * smooth_scale, SRCCOPY);
        } else {
            BLENDFUNCTION blend = { AC_SRC_OVER, 0, color >> 24, 0 };
            GdiAlphaBlend(canvas->data.gdi.buffer_hdc, rect->x, rect->y, rect->width, rect->height, canvas->data.gdi.alpha_hdc, 0, 0, rect->width * smooth_scale, rect->height * smooth_scale, blend);
        }
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1PathGeometry *path_geometry;
        ID2D1Factory_CreatePathGeometry(canvas->data.d2d.d2d_factory, &path_geometry);

        ID2D1GeometrySink *sink;
        ID2D1PathGeometry_Open(path_geometry, &sink);
        ID2D1SimplifiedGeometrySink_SetFillMode(sink, D2D1_FILL_MODE_WINDING);

        float x = 0;
        float y = 0;
        float scale_x = (float)rect->width / viewport_width;
        float scale_y = (float)rect->height / viewport_height;
        bool figure_open = false;
        char *c = path;
        while (*c != '\0') {
            if (*c == 'M') {
                c++;
                x = Canvas_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y = Canvas_ParsePathFloat(&c);
                if (figure_open) {
                    ID2D1SimplifiedGeometrySink_EndFigure(sink, D2D1_FIGURE_END_CLOSED);
                }
                ID2D1SimplifiedGeometrySink_BeginFigure(sink, ((D2D1_POINT_2F){ rect->x + x * scale_x, rect->y + y * scale_y }), D2D1_FIGURE_BEGIN_FILLED);
                figure_open = true;
            } else if (*c == 'm') {
                c++;
                x += Canvas_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y += Canvas_ParsePathFloat(&c);
                if (figure_open) {
                    ID2D1SimplifiedGeometrySink_EndFigure(sink, D2D1_FIGURE_END_CLOSED);
                }
                ID2D1SimplifiedGeometrySink_BeginFigure(sink, ((D2D1_POINT_2F){ rect->x + x * scale_x, rect->y + y * scale_y }), D2D1_FIGURE_BEGIN_FILLED);
                figure_open = true;
            } else if (*c == 'L') {
                c++;
                x = Canvas_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y = Canvas_ParsePathFloat(&c);
                ID2D1GeometrySink_AddLine(sink, ((D2D1_POINT_2F){ rect->x + x * scale_x, rect->y + y * scale_y }));
            } else if (*c == 'l') {
                c++;
                x += Canvas_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y += Canvas_ParsePathFloat(&c);
                ID2D1GeometrySink_AddLine(sink, ((D2D1_POINT_2F){ rect->x + x * scale_x, rect->y + y * scale_y }));
            } else if (*c == 'H') {
                c++;
                x = Canvas_ParsePathFloat(&c);
                ID2D1GeometrySink_AddLine(sink, ((D2D1_POINT_2F){ rect->x + x * scale_x, rect->y + y * scale_y }));
            } else if (*c == 'h') {
                c++;
                x += Canvas_ParsePathFloat(&c);
                ID2D1GeometrySink_AddLine(sink, ((D2D1_POINT_2F){ rect->x + x * scale_x, rect->y + y * scale_y }));
            } else if (*c == 'V') {
                c++;
                y = Canvas_ParsePathFloat(&c);
                ID2D1GeometrySink_AddLine(sink, ((D2D1_POINT_2F){ rect->x + x * scale_x, rect->y + y * scale_y }));
            } else if (*c == 'v') {
                c++;
                y += Canvas_ParsePathFloat(&c);
                ID2D1GeometrySink_AddLine(sink, ((D2D1_POINT_2F){ rect->x + x * scale_x, rect->y + y * scale_y }));
            } else if (*c == 'Z' || *c == 'z') {
                c++;
                if (figure_open) {
                    ID2D1SimplifiedGeometrySink_EndFigure(sink, D2D1_FIGURE_END_CLOSED);
                    figure_open = false;
                }
            }
        }
        if (figure_open) {
            ID2D1SimplifiedGeometrySink_EndFigure(sink, D2D1_FIGURE_END_CLOSED);
        }
        ID2D1SimplifiedGeometrySink_Close(sink);
        IUnknown_Release(sink);

        D2D1_COLOR_F color_float = { (float)(color & 0xff) / 255, (float)((color >> 8) & 0xff) / 255,
            (float)((color >> 16) & 0xff) / 255, (float)((color >> 24) & 0xff) / 255 };
        ID2D1Brush *brush;
        ID2D1RenderTarget_CreateSolidColorBrush(canvas->data.d2d.render_target, &color_float, NULL, &brush);
        ID2D1RenderTarget_FillGeometry(canvas->data.d2d.render_target, (ID2D1Geometry *)path_geometry, brush, NULL);
        IUnknown_Release(brush);
        IUnknown_Release(path_geometry);
    }
}
