#include "henk.h"

Henk *Henk_New(HWND hwnd, HenkRenderer renderer) {
    Henk *henk = malloc(sizeof(Henk));
    Henk_Init(henk, hwnd, renderer);
    return henk;
}

void Henk_Init(Henk *henk, HWND hwnd, HenkRenderer renderer) {
    henk->renderer = renderer;
    henk->width = -1;
    henk->height = -1;

    HMODULE hd2d1 = LoadLibraryW(L"d2d1");
    HMODULE hdwrite = LoadLibraryW(L"dwrite");
    if (henk->renderer == HENK_RENDERER_DEFAULT) {
        henk->renderer = hd2d1 && hdwrite ? HENK_RENDERER_DIRECT2D : HENK_RENDERER_GDI;
    }

    if (henk->renderer == HENK_RENDERER_GDI) {
        henk->hdc = GetDC(hwnd);
    }

    if (henk->renderer == HENK_RENDERER_DIRECT2D) {
        #ifdef __GNUC__
            __extension__
        #endif
        _D2D1CreateFactory D2D1CreateFactory = GetProcAddress(hd2d1, "D2D1CreateFactory");
        GUID ID2D1Factory_guid = { 0xbb12d362, 0xdaee, 0x4b9a, { 0xaa, 0x1d, 0x14, 0xba, 0x40, 0x1c, 0xfa, 0x1f } };
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &ID2D1Factory_guid, NULL, &henk->d2d_factory);

        #ifdef __GNUC__
            __extension__
        #endif
        _DWriteCreateFactory DWriteCreateFactory = GetProcAddress(hdwrite, "DWriteCreateFactory");
        GUID IDWriteFactory_guid = { 0xb859ee5a, 0xd838, 0x4b5b, { 0xa2, 0xe8, 0x1a, 0xdc, 0x7d, 0x93, 0xdb, 0x48 } };
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, &IDWriteFactory_guid, &henk->dwrite_factory);

        D2D1_RENDER_TARGET_PROPERTIES render_props = { D2D1_RENDER_TARGET_TYPE_DEFAULT,
            { DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_UNKNOWN },
            96, 96, D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT };
        D2D1_HWND_RENDER_TARGET_PROPERTIES hwnd_render_props = { hwnd, { 0, 0 }, D2D1_PRESENT_OPTIONS_NONE };
        ID2D1Factory_CreateHwndRenderTarget(henk->d2d_factory, &render_props, &hwnd_render_props, &henk->render_target);
    }
}

void Henk_Free(Henk *henk) {
    if (henk->renderer == HENK_RENDERER_GDI) {
        DeleteObject(henk->alpha_bitmap);
        DeleteDC(henk->alpha_hdc);

        DeleteObject(henk->buffer_bitmap);
        DeleteDC(henk->buffer_hdc);

        DeleteDC(henk->hdc);
    }

    if (henk->renderer == HENK_RENDERER_DIRECT2D) {
        IUnknown_Release(henk->render_target);
        IUnknown_Release(henk->dwrite_factory);
        IUnknown_Release(henk->d2d_factory);
    }

    free(henk);
}

void Henk_Resize(Henk *henk, int32_t width, int32_t height) {
    if (henk->renderer == HENK_RENDERER_GDI && henk->width != -1 && henk->height != -1) {
        DeleteObject(henk->alpha_bitmap);
        DeleteDC(henk->alpha_hdc);

        DeleteObject(henk->buffer_bitmap);
        DeleteDC(henk->buffer_hdc);
    }

    henk->width = width;
    henk->height = height;

    if (henk->renderer == HENK_RENDERER_GDI) {
        henk->buffer_hdc = CreateCompatibleDC(henk->hdc);
        SetBkMode(henk->buffer_hdc, TRANSPARENT);
        SetTextAlign(henk->buffer_hdc, TA_LEFT);
        SetStretchBltMode(henk->buffer_hdc, STRETCH_HALFTONE);
        henk->buffer_bitmap = CreateCompatibleBitmap(henk->hdc, henk->width, henk->height);
        SelectObject(henk->buffer_hdc, henk->buffer_bitmap);

        henk->alpha_hdc = CreateCompatibleDC(henk->hdc);
        SetBkMode(henk->alpha_hdc, TRANSPARENT);
        SetTextAlign(henk->alpha_hdc, TA_LEFT);
        SetStretchBltMode(henk->alpha_hdc, STRETCH_HALFTONE);
        henk->alpha_bitmap = CreateCompatibleBitmap(henk->hdc, henk->width * 2, henk->height * 2);
        SelectObject(henk->alpha_hdc, henk->alpha_bitmap);
    }

    if (henk->renderer == HENK_RENDERER_DIRECT2D) {
        D2D1_SIZE_U size = { henk->width, henk->height };
        ID2D1HwndRenderTarget_Resize(henk->render_target, &size);
    }
}

void Henk_BeginDraw(Henk *henk) {
    if (henk->renderer == HENK_RENDERER_DIRECT2D) {
        ID2D1RenderTarget_BeginDraw(henk->render_target);
    }
}

void Henk_EndDraw(Henk *henk) {
    if (henk->renderer == HENK_RENDERER_GDI) {
        BitBlt(henk->hdc, 0, 0, henk->width, henk->height, henk->buffer_hdc, 0, 0, SRCCOPY);
    }
    if (henk->renderer == HENK_RENDERER_DIRECT2D) {
        ID2D1RenderTarget_EndDraw(henk->render_target, NULL, NULL);
    }
}

void Henk_StrokeRect(Henk *henk, HenkRect *rect, uint32_t color, float stroke_width) {
    if (henk->renderer == HENK_RENDERER_GDI) {
        HPEN pen = CreatePen(PS_SOLID, stroke_width, color & 0x00ffffff);
        if ((color >> 24) == 0xff) {
            SelectObject(henk->buffer_hdc, pen);
            SelectObject(henk->buffer_hdc, GetStockObject(NULL_BRUSH));
            Rectangle(henk->buffer_hdc, rect->x, rect->y, rect->x + rect->width + 1, rect->y + rect->height + 1);
        } else {
            BitBlt(henk->alpha_hdc, 0, 0, rect->width + stroke_width * 2, rect->height + stroke_width * 2, henk->buffer_hdc, rect->x - stroke_width, rect->y - stroke_width, SRCCOPY);
            SelectObject(henk->alpha_hdc, pen);
            SelectObject(henk->alpha_hdc, GetStockObject(NULL_BRUSH));
            Rectangle(henk->alpha_hdc, stroke_width, stroke_width, stroke_width + rect->width + 1, stroke_width + rect->height + 1);
            BLENDFUNCTION blend = { AC_SRC_OVER, 0, color >> 24, 0 };
            GdiAlphaBlend(henk->buffer_hdc, rect->x - stroke_width, rect->y - stroke_width, rect->width + stroke_width * 2, rect->height + stroke_width * 2,
                henk->alpha_hdc, 0, 0, rect->width + stroke_width * 2, rect->height + stroke_width * 2, blend);
        }
        DeleteObject(pen);
    }

    if (henk->renderer == HENK_RENDERER_DIRECT2D) {
        D2D1_COLOR_F color_float = { (float)(color & 0xff) / 255, (float)((color >> 8) & 0xff) / 255,
            (float)((color >> 16) & 0xff) / 255, (float)((color >> 24) & 0xff) / 255 };
        ID2D1Brush *brush;
        ID2D1RenderTarget_CreateSolidColorBrush(henk->render_target, &color_float, NULL, &brush);
        D2D1_RECT_F real_rect = { rect->x, rect->y, rect->x + rect->width, rect->y + rect->height };
        ID2D1RenderTarget_DrawRectangle(henk->render_target, &real_rect, brush, stroke_width, NULL);
        IUnknown_Release(brush);
    }
}

void Henk_FillRect(Henk *henk, HenkRect *rect, uint32_t color) {
    if (henk->renderer == HENK_RENDERER_GDI) {
        HBRUSH brush = CreateSolidBrush(color & 0x00ffffff);
        if ((color >> 24) == 0xff) {
            RECT real_rect = { rect->x, rect->y, rect->x + rect->width, rect->y + rect->height };
            FillRect(henk->buffer_hdc, &real_rect, brush);
        } else {
            RECT real_rect = { 0, 0, rect->width, rect->height };
            FillRect(henk->alpha_hdc, &real_rect, brush);
            BLENDFUNCTION blend = { AC_SRC_OVER, 0, color >> 24, 0 };
            GdiAlphaBlend(henk->buffer_hdc, rect->x, rect->y, rect->width, rect->height, henk->alpha_hdc, 0, 0, rect->width, rect->height, blend);
        }
        DeleteObject(brush);
    }

    if (henk->renderer == HENK_RENDERER_DIRECT2D) {
        D2D1_COLOR_F color_float = { (float)(color & 0xff) / 255, (float)((color >> 8) & 0xff) / 255,
            (float)((color >> 16) & 0xff) / 255, (float)((color >> 24) & 0xff) / 255 };
        ID2D1Brush *brush;
        ID2D1RenderTarget_CreateSolidColorBrush(henk->render_target, &color_float, NULL, &brush);
        D2D1_RECT_F real_rect = { rect->x, rect->y, rect->x + rect->width, rect->y + rect->height };
        ID2D1RenderTarget_FillRectangle(henk->render_target, &real_rect, brush);
        IUnknown_Release(brush);
    }
}

void Henk_DrawText(Henk *henk, wchar_t *text, int32_t length, HenkRect *rect, HenkFont *font, uint32_t align, uint32_t color) {
    if (length == -1) length = wcslen(text);
    Henk_StrokeRect(henk, rect, HENK_RGBA(0, 0, 0, 128), 2);

    if (henk->renderer == HENK_RENDERER_GDI) {
        HFONT hfont = CreateFontW(-MulDiv(font->size, 96, 72), 0, 0, 0, FW_NORMAL, false, false, false, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font->name);
        SelectObject(henk->buffer_hdc, hfont);

        SIZE measure_size;
        if (
            (color >> 24) != 0xff || (align & DT_CENTER) != 0 || (align & DT_RIGHT) != 0 ||
            (align & DT_VCENTER) != 0 || (align & DT_BOTTOM) != 0
        ) {
            GetTextExtentPoint32W(henk->buffer_hdc, text, length, &measure_size);
            if (measure_size.cx > henk->width) measure_size.cx = henk->width;
            if (measure_size.cy > henk->height) measure_size.cy = henk->height;
        }
        int32_t real_x = rect->x;
        if ((align & DT_CENTER) != 0) real_x += (rect->width - measure_size.cx) / 2;
        if ((align & DT_RIGHT) != 0) real_x += rect->width - measure_size.cx;
        int32_t real_y = rect->y;
        if ((align & DT_VCENTER) != 0) real_y += (rect->height - measure_size.cy) / 2;
        if ((align & DT_BOTTOM) != 0) real_y += rect->height - measure_size.cy;

        if ((color >> 24) == 0xff) {
            SetTextColor(henk->buffer_hdc, color & 0x00ffffff);
            RECT real_rect = { rect->x, rect->y, rect->x + rect->width, rect->y + rect->height };
            ExtTextOutW(henk->buffer_hdc, real_x, real_y, ETO_CLIPPED, &real_rect, text, length, NULL);
        } else {
            BitBlt(henk->alpha_hdc, 0, 0, measure_size.cx, measure_size.cy, henk->buffer_hdc, real_x, real_y, SRCCOPY);
            SelectObject(henk->alpha_hdc, hfont);
            SetTextColor(henk->alpha_hdc, color & 0x00ffffff);
            RECT real_rect = { 0, 0, rect->width, rect->height };
            ExtTextOutW(henk->alpha_hdc, 0, 0, ETO_CLIPPED, &real_rect, text, length, NULL);
            BLENDFUNCTION blend = { AC_SRC_OVER, 0, color >> 24, 0 };
            GdiAlphaBlend(henk->buffer_hdc, real_x, real_y, measure_size.cx, measure_size.cy, henk->alpha_hdc, 0, 0, measure_size.cx, measure_size.cy, blend);
        }
        DeleteObject(hfont);
    }

    if (henk->renderer == HENK_RENDERER_DIRECT2D) {
        D2D1_COLOR_F color_float = { (float)(color & 0xff) / 255, (float)((color >> 8) & 0xff) / 255,
            (float)((color >> 16) & 0xff) / 255, (float)((color >> 24) & 0xff) / 255 };
        ID2D1Brush *brush;
        ID2D1RenderTarget_CreateSolidColorBrush(henk->render_target, &color_float, NULL, &brush);

        IDWriteTextFormat *text_format;
        IDWriteFactory_CreateTextFormat(henk->dwrite_factory, font->name, NULL,
            DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            (font->size / 72) * 96, L"", &text_format);
        if ((align & DT_CENTER) != 0) IDWriteTextFormat_SetTextAlignment(text_format, DWRITE_TEXT_ALIGNMENT_CENTER);
        if ((align & DT_RIGHT) != 0) IDWriteTextFormat_SetTextAlignment(text_format, DWRITE_TEXT_ALIGNMENT_TRAILING);
        if ((align & DT_VCENTER) != 0) IDWriteTextFormat_SetParagraphAlignment(text_format, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        if ((align & DT_BOTTOM) != 0) IDWriteTextFormat_SetParagraphAlignment(text_format, DWRITE_PARAGRAPH_ALIGNMENT_FAR);

        D2D1_RECT_F real_rect = { rect->x, rect->y, rect->x + rect->width, rect->y + rect->height };
        ID2D1RenderTarget_DrawText(henk->render_target, text, length, text_format,
            &real_rect, brush, D2D1_DRAW_TEXT_OPTIONS_CLIP | D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT, DWRITE_MEASURING_MODE_NATURAL);

        IUnknown_Release(text_format);
        IUnknown_Release(brush);
    }
}

float Henk_ParsePathFloat(char **string) {
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

void Henk_FillPath(Henk *henk, HenkRect *rect, int32_t viewport_width, int32_t viewport_height, char *path, uint32_t color) {
    if (henk->renderer == HENK_RENDERER_GDI) {
        int32_t smooth_scale = 2; // Experimental smoothing feature gives dark border artifacts
        StretchBlt(henk->alpha_hdc, 0, 0, rect->width * smooth_scale, rect->height * smooth_scale, henk->buffer_hdc, rect->x, rect->y, rect->width, rect->height, SRCCOPY);
        BeginPath(henk->alpha_hdc);
        float x = 0;
        float y = 0;
        float scale_x = (float)(rect->width * smooth_scale) / viewport_width;
        float scale_y = (float)(rect->height * smooth_scale) / viewport_height;
        char *c = path;
        while (*c != '\0') {
            if (*c == 'M') {
                c++;
                x = Henk_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y = Henk_ParsePathFloat(&c);
                MoveToEx(henk->alpha_hdc, x * scale_x, y * scale_y, NULL);
            } else if (*c == 'm') {
                c++;
                x += Henk_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y += Henk_ParsePathFloat(&c);
                MoveToEx(henk->alpha_hdc, x * scale_x, y * scale_y, NULL);
            } else if (*c == 'L') {
                c++;
                x = Henk_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y = Henk_ParsePathFloat(&c);
                LineTo(henk->alpha_hdc, x * scale_x, y * scale_y);
            } else if (*c == 'l') {
                c++;
                x += Henk_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y += Henk_ParsePathFloat(&c);
                LineTo(henk->alpha_hdc, x * scale_x, y * scale_y);
            } else if (*c == 'H') {
                c++;
                x = Henk_ParsePathFloat(&c);
                LineTo(henk->alpha_hdc, x * scale_x, y * scale_y);
            } else if (*c == 'h') {
                c++;
                x += Henk_ParsePathFloat(&c);
                LineTo(henk->alpha_hdc, x * scale_x, y * scale_y);
            } else if (*c == 'V') {
                c++;
                y = Henk_ParsePathFloat(&c);
                LineTo(henk->alpha_hdc, x * scale_x, y * scale_y);
            } else if (*c == 'v') {
                c++;
                y += Henk_ParsePathFloat(&c);
                LineTo(henk->alpha_hdc, x * scale_x, y * scale_y);
            } else if (*c == 'Z' || *c == 'z') {
                c++;
            }
        }
        EndPath(henk->alpha_hdc);
        HBRUSH brush = CreateSolidBrush(color & 0x00ffffff);
        SelectObject(henk->alpha_hdc, brush);
        FillPath(henk->alpha_hdc);
        DeleteObject(brush);
        if ((color >> 24) == 0xff) {
            StretchBlt(henk->buffer_hdc, rect->x, rect->y, rect->width, rect->height, henk->alpha_hdc, 0, 0, rect->width * smooth_scale, rect->height * smooth_scale, SRCCOPY);
        } else {
            BLENDFUNCTION blend = { AC_SRC_OVER, 0, color >> 24, 0 };
            GdiAlphaBlend(henk->buffer_hdc, rect->x, rect->y, rect->width, rect->height, henk->alpha_hdc, 0, 0, rect->width * smooth_scale, rect->height * smooth_scale, blend);
        }
    }

    if (henk->renderer == HENK_RENDERER_DIRECT2D) {
        ID2D1PathGeometry *path_geometry;
        ID2D1Factory_CreatePathGeometry(henk->d2d_factory, &path_geometry);

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
                x = Henk_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y = Henk_ParsePathFloat(&c);
                if (figure_open) {
                    ID2D1SimplifiedGeometrySink_EndFigure(sink, D2D1_FIGURE_END_CLOSED);
                }
                ID2D1SimplifiedGeometrySink_BeginFigure(sink, ((D2D1_POINT_2F){ rect->x + x * scale_x, rect->y + y * scale_y }), D2D1_FIGURE_BEGIN_FILLED);
                figure_open = true;
            } else if (*c == 'm') {
                c++;
                x += Henk_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y += Henk_ParsePathFloat(&c);
                if (figure_open) {
                    ID2D1SimplifiedGeometrySink_EndFigure(sink, D2D1_FIGURE_END_CLOSED);
                }
                ID2D1SimplifiedGeometrySink_BeginFigure(sink, ((D2D1_POINT_2F){ rect->x + x * scale_x, rect->y + y * scale_y }), D2D1_FIGURE_BEGIN_FILLED);
                figure_open = true;
            } else if (*c == 'L') {
                c++;
                x = Henk_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y = Henk_ParsePathFloat(&c);
                ID2D1GeometrySink_AddLine(sink, ((D2D1_POINT_2F){ rect->x + x * scale_x, rect->y + y * scale_y }));
            } else if (*c == 'l') {
                c++;
                x += Henk_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y += Henk_ParsePathFloat(&c);
                ID2D1GeometrySink_AddLine(sink, ((D2D1_POINT_2F){ rect->x + x * scale_x, rect->y + y * scale_y }));
            } else if (*c == 'H') {
                c++;
                x = Henk_ParsePathFloat(&c);
                ID2D1GeometrySink_AddLine(sink, ((D2D1_POINT_2F){ rect->x + x * scale_x, rect->y + y * scale_y }));
            } else if (*c == 'h') {
                c++;
                x += Henk_ParsePathFloat(&c);
                ID2D1GeometrySink_AddLine(sink, ((D2D1_POINT_2F){ rect->x + x * scale_x, rect->y + y * scale_y }));
            } else if (*c == 'V') {
                c++;
                y = Henk_ParsePathFloat(&c);
                ID2D1GeometrySink_AddLine(sink, ((D2D1_POINT_2F){ rect->x + x * scale_x, rect->y + y * scale_y }));
            } else if (*c == 'v') {
                c++;
                y += Henk_ParsePathFloat(&c);
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
        ID2D1RenderTarget_CreateSolidColorBrush(henk->render_target, &color_float, NULL, &brush);
        ID2D1RenderTarget_FillGeometry(henk->render_target, (ID2D1Geometry *)path_geometry, brush, NULL);
        IUnknown_Release(brush);
        IUnknown_Release(path_geometry);
    }
}
