#include "canvas.h"

#define DP2PX(dp) MulDiv(dp, canvas->dpi, 96)
#define PX2DP(px) MulDiv(px, 96, canvas->dpi)

Canvas *Canvas_New(HWND hwnd, CanvasRenderer renderer) {
    Canvas *canvas = malloc(sizeof(Canvas));
    canvas->renderer = renderer;
    canvas->hwnd = hwnd;
    canvas->width = -1;
    canvas->height = -1;

    HMODULE hd2d1 = LoadLibraryW(L"d2d1");
    HMODULE hdwrite = LoadLibraryW(L"dwrite");
    if (canvas->renderer == CANVAS_RENDERER_DEFAULT) {
        canvas->renderer = hd2d1 && hdwrite ? CANVAS_RENDERER_DIRECT2D : CANVAS_RENDERER_GDI;
    }

    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        canvas->gdi.hdc = GetDC(hwnd);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        _D2D1CreateFactory D2D1CreateFactory = GetProcAddress(hd2d1, "D2D1CreateFactory");
        IID IID_ID2D1Factory = { 0xbb12d362, 0xdaee, 0x4b9a, { 0xaa, 0x1d, 0x14, 0xba, 0x40, 0x1c, 0xfa, 0x1f } };
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &IID_ID2D1Factory, NULL, &canvas->d2d.d2d_factory);

        _DWriteCreateFactory DWriteCreateFactory = GetProcAddress(hdwrite, "DWriteCreateFactory");
        IID IID_IDWriteFactory = { 0xb859ee5a, 0xd838, 0x4b5b, { 0xa2, 0xe8, 0x1a, 0xdc, 0x7d, 0x93, 0xdb, 0x48 } };
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, &IID_IDWriteFactory, &canvas->d2d.dwrite_factory);

        D2D1_RENDER_TARGET_PROPERTIES render_props = { D2D1_RENDER_TARGET_TYPE_DEFAULT,
            { DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_UNKNOWN },
            96, 96, D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT };
        D2D1_HWND_RENDER_TARGET_PROPERTIES hwnd_render_props = { hwnd, { 0, 0 }, D2D1_PRESENT_OPTIONS_NONE };
        ID2D1Factory_CreateHwndRenderTarget(canvas->d2d.d2d_factory, &render_props, &hwnd_render_props, &canvas->d2d.render_target);
    }

    return canvas;
}

void Canvas_Free(Canvas *canvas) {
    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        DeleteObject(canvas->gdi.alpha_bitmap);
        DeleteDC(canvas->gdi.alpha_hdc);

        DeleteObject(canvas->gdi.buffer_bitmap);
        DeleteDC(canvas->gdi.buffer_hdc);

        ReleaseDC(canvas->hwnd, canvas->gdi.hdc);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        IUnknown_Release(canvas->d2d.render_target);
        IUnknown_Release(canvas->d2d.dwrite_factory);
        IUnknown_Release(canvas->d2d.d2d_factory);
    }

    free(canvas);
}

void Canvas_Resize(Canvas *canvas, int32_t width, int32_t height, int32_t dpi) {
    if (canvas->renderer == CANVAS_RENDERER_GDI && canvas->width != -1 && canvas->height != -1) {
        DeleteObject(canvas->gdi.alpha_bitmap);
        DeleteDC(canvas->gdi.alpha_hdc);

        DeleteObject(canvas->gdi.buffer_bitmap);
        DeleteDC(canvas->gdi.buffer_hdc);
    }

    canvas->width = width;
    canvas->height = height;
    canvas->dpi = dpi;

    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        canvas->gdi.buffer_hdc = CreateCompatibleDC(canvas->gdi.hdc);
        SetGraphicsMode(canvas->gdi.buffer_hdc, GM_ADVANCED);
        SetBkMode(canvas->gdi.buffer_hdc, TRANSPARENT);
        SetStretchBltMode(canvas->gdi.buffer_hdc, STRETCH_HALFTONE);
        canvas->gdi.buffer_bitmap = CreateCompatibleBitmap(canvas->gdi.hdc, canvas->width, canvas->height);
        SelectObject(canvas->gdi.buffer_hdc, canvas->gdi.buffer_bitmap);

        canvas->gdi.alpha_hdc = CreateCompatibleDC(canvas->gdi.hdc);
        SetBkMode(canvas->gdi.alpha_hdc, TRANSPARENT);
        SetStretchBltMode(canvas->gdi.alpha_hdc, STRETCH_HALFTONE);
        canvas->gdi.alpha_bitmap = CreateCompatibleBitmap(canvas->gdi.hdc, canvas->width * 2, canvas->height * 2);
        SelectObject(canvas->gdi.alpha_hdc, canvas->gdi.alpha_bitmap);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1HwndRenderTarget_Resize(canvas->d2d.render_target, (&(D2D1_SIZE_U){ canvas->width, canvas->height }));
        ID2D1RenderTarget_SetDpi(canvas->d2d.render_target, canvas->dpi, canvas->dpi);
    }
}

void Canvas_BeginDraw(Canvas *canvas) {
    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1RenderTarget_BeginDraw(canvas->d2d.render_target);
    }
}

void Canvas_EndDraw(Canvas *canvas) {
    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        BitBlt(canvas->gdi.hdc, 0, 0, canvas->width, canvas->height, canvas->gdi.buffer_hdc, 0, 0, SRCCOPY);
    }
    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1RenderTarget_EndDraw(canvas->d2d.render_target, NULL, NULL);
    }
}

void Canvas_Transform(Canvas *canvas, CanvasTransform *transform) {
    CanvasTransform identity = { 1, 0, 0, 1, 0, 0 };
    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        if (transform != NULL) {
            SetWorldTransform(canvas->gdi.buffer_hdc, &(XFORM){
                transform->m11, transform->m12,
                transform->m21, transform->m22,
                DP2PX(transform->dx), DP2PX(transform->dy)
            });
        } else {
            SetWorldTransform(canvas->gdi.buffer_hdc, (XFORM *)&identity);
        }
    }
    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1RenderTarget_SetTransform(canvas->d2d.render_target, (D2D1_MATRIX_3X2_F *)(transform != NULL ? transform : &identity));
    }
}

void Canvas_Clip(Canvas *canvas, CanvasRect *rect) {
    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        if (rect != NULL) {
            CanvasRect real_rect = { DP2PX(rect->x), DP2PX(rect->y), DP2PX(rect->width), DP2PX(rect->height) };
            POINT points[] = { { real_rect.x, real_rect.y }, { real_rect.x + real_rect.width, real_rect.y + real_rect.height } };
            LPtoDP(canvas->gdi.buffer_hdc, points, 2);
            HRGN clip_region = CreateRectRgn(points[0].x, points[0].y, points[1].x, points[1].y);
            SelectClipRgn(canvas->gdi.buffer_hdc, clip_region);
            DeleteObject(clip_region);
        } else {
            SelectClipRgn(canvas->gdi.buffer_hdc, NULL);
        }
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        if (rect != NULL) {
            ID2D1RenderTarget_PushAxisAlignedClip(canvas->d2d.render_target,
                (&(D2D1_RECT_F){ rect->x, rect->y, rect->x + rect->width, rect->y + rect->height }),
                D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        } else {
            ID2D1RenderTarget_PopAxisAlignedClip(canvas->d2d.render_target);
        }
    }
}

void Canvas_FillRect(Canvas *canvas, CanvasRect *rect, CanvasColor color) {
    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        CanvasRect real_rect = { DP2PX(rect->x), DP2PX(rect->y), DP2PX(rect->width), DP2PX(rect->height) };
        HBRUSH brush = CreateSolidBrush(color & 0x00ffffff);
        if ((color >> 24) == 0xff) {
            FillRect(canvas->gdi.buffer_hdc, &(RECT){ real_rect.x, real_rect.y, real_rect.x + real_rect.width, real_rect.y + real_rect.height }, brush);
        } else {
            FillRect(canvas->gdi.alpha_hdc, &(RECT){ 0, 0, real_rect.width, real_rect.height }, brush);
            GdiAlphaBlend(canvas->gdi.buffer_hdc, real_rect.x, real_rect.y, real_rect.width, real_rect.height,
                canvas->gdi.alpha_hdc, 0, 0, real_rect.width, real_rect.height, (BLENDFUNCTION){ AC_SRC_OVER, 0, color >> 24, 0 });
        }
        DeleteObject(brush);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1Brush *brush;
        ID2D1RenderTarget_CreateSolidColorBrush(canvas->d2d.render_target, (&(D2D1_COLOR_F){
            (float)(color & 0xff) / 255, (float)((color >> 8) & 0xff) / 255,
            (float)((color >> 16) & 0xff) / 255, (float)((color >> 24) & 0xff) / 255
        }), NULL, &brush);
        ID2D1RenderTarget_FillRectangle(canvas->d2d.render_target,
            (&(D2D1_RECT_F){ rect->x, rect->y, rect->x + rect->width, rect->y + rect->height }), brush);
        IUnknown_Release(brush);
    }
}

void Canvas_StrokeRect(Canvas *canvas, CanvasRect *rect, CanvasColor color, float stroke_width) {
    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        CanvasRect real_rect = { DP2PX(rect->x), DP2PX(rect->y), DP2PX(rect->width), DP2PX(rect->height) };
        HPEN pen = CreatePen(PS_SOLID, stroke_width, color & 0x00ffffff);
        if ((color >> 24) == 0xff) {
            SelectObject(canvas->gdi.buffer_hdc, pen);
            SelectObject(canvas->gdi.buffer_hdc, GetStockObject(NULL_BRUSH));
            Rectangle(canvas->gdi.buffer_hdc, real_rect.x, real_rect.y, real_rect.x + real_rect.width + 1, real_rect.y + real_rect.height + 1); // TODO
        } else {
            BitBlt(canvas->gdi.alpha_hdc, 0, 0, real_rect.width + stroke_width * 2, real_rect.height + stroke_width * 2, canvas->gdi.buffer_hdc, real_rect.x - stroke_width, real_rect.y - stroke_width, SRCCOPY);
            SelectObject(canvas->gdi.alpha_hdc, pen);
            SelectObject(canvas->gdi.alpha_hdc, GetStockObject(NULL_BRUSH));
            Rectangle(canvas->gdi.alpha_hdc, stroke_width, stroke_width, stroke_width + real_rect.width + 1, stroke_width + real_rect.height + 1);
            GdiAlphaBlend(canvas->gdi.buffer_hdc, real_rect.x - stroke_width, real_rect.y - stroke_width, real_rect.width + stroke_width * 2, real_rect.height + stroke_width * 2,
                canvas->gdi.alpha_hdc, 0, 0, real_rect.width + stroke_width * 2, real_rect.height + stroke_width * 2, (BLENDFUNCTION){ AC_SRC_OVER, 0, color >> 24, 0 });
        }
        DeleteObject(pen);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        D2D1_COLOR_F color_float = { (float)(color & 0xff) / 255, (float)((color >> 8) & 0xff) / 255,
            (float)((color >> 16) & 0xff) / 255, (float)((color >> 24) & 0xff) / 255 };
        ID2D1Brush *brush;
        ID2D1RenderTarget_CreateSolidColorBrush(canvas->d2d.render_target, &color_float, NULL, &brush);
        D2D1_RECT_F stroke_rect = { rect->x, rect->y, rect->x + rect->width, rect->y + rect->height };
        ID2D1RenderTarget_DrawRectangle(canvas->d2d.render_target, &stroke_rect, brush, stroke_width, NULL);
        IUnknown_Release(brush);
    }
}

void Canvas_MeasureText(Canvas *canvas, wchar_t *text, int32_t length, CanvasRect *rect, CanvasFont *font, CanvasTextFormat format) {
    if (length == -1) length = wcslen(text);

    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        int32_t weight = FW_NORMAL;
        if (font->weight == CANVAS_FONT_WEIGHT_BOLD) weight = FW_BOLD;
        HFONT hfont = CreateFontW(-MulDiv(font->size, canvas->dpi, 72), 0, 0, 0, weight, font->italic, font->underline, font->line_through,
            ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font->name);
        SelectObject(canvas->gdi.buffer_hdc, hfont);

        if ((format & CANVAS_TEXT_FORMAT_WRAP) != 0) {
            RECT measure_rect = { 0, 0, DP2PX(rect->width), 0 };
            DrawTextW(canvas->gdi.buffer_hdc, text, length, &measure_rect, DT_CALCRECT | DT_WORDBREAK);
            rect->width = PX2DP(measure_rect.right);
            rect->height = PX2DP(measure_rect.bottom);
        } else {
            SIZE measure_rect = { 0 };
            GetTextExtentPoint32W(canvas->gdi.buffer_hdc, text, length, &measure_rect);
            rect->width = PX2DP(measure_rect.cx);
            rect->height = PX2DP(measure_rect.cy);
        }

        DeleteObject(hfont);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        IDWriteTextFormat *text_format;
        int32_t weight = DWRITE_FONT_WEIGHT_NORMAL;
        if (font->weight == CANVAS_FONT_WEIGHT_BOLD) weight = DWRITE_FONT_WEIGHT_BOLD;
        IDWriteFactory_CreateTextFormat(canvas->d2d.dwrite_factory, font->name, NULL, weight,
            font->italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            (font->size / 72) * 96, L"", &text_format);
        if ((format & CANVAS_TEXT_FORMAT_WRAP) == 0) IDWriteTextFormat_SetWordWrapping(text_format, DWRITE_WORD_WRAPPING_NO_WRAP);

        IDWriteTextLayout *text_layout;
        IDWriteFactory_CreateTextLayout(canvas->d2d.dwrite_factory, text, length, text_format,
            (format & CANVAS_TEXT_FORMAT_WRAP) != 0 ? rect->width : INT32_MAX, INT32_MAX, &text_layout);
        if (font->underline) IDWriteTextLayout_SetUnderline(text_layout, true, ((DWRITE_TEXT_RANGE){ 0, length}));
        if (font->line_through) IDWriteTextLayout_SetStrikethrough(text_layout, true, ((DWRITE_TEXT_RANGE){ 0, length}));

        DWRITE_TEXT_METRICS metrics;
        IDWriteTextLayout_GetMetrics(text_layout, &metrics);
        rect->width = metrics.width;
        rect->height = metrics.height;

        IUnknown_Release(text_layout);
        IUnknown_Release(text_format);
    }
}

void Canvas_DrawText(Canvas *canvas, wchar_t *text, int32_t length, CanvasRect *rect, CanvasFont *font, CanvasTextFormat format, CanvasColor color) {
    if (length == -1) length = wcslen(text);

    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        CanvasRect real_rect = { DP2PX(rect->x), DP2PX(rect->y), DP2PX(rect->width), DP2PX(rect->height) };

        int32_t weight = FW_NORMAL;
        if (font->weight == CANVAS_FONT_WEIGHT_BOLD) weight = FW_BOLD;
        HFONT hfont = CreateFontW(-MulDiv(font->size, canvas->dpi, 72), 0, 0, 0, weight, font->italic, font->underline, font->line_through,
            ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font->name);
        SelectObject(canvas->gdi.buffer_hdc, hfont);

        if ((format & CANVAS_TEXT_FORMAT_WRAP) == 0) {
            SIZE measure_rect;
            if (
                rect->width == 0 ||
                rect->height == 0 ||
                (format & CANVAS_TEXT_FORMAT_VERTICAL_CENTER) != 0
            ) {
                GetTextExtentPoint32W(canvas->gdi.buffer_hdc, text, length, &measure_rect);
                if (rect->width == 0) {
                    rect->width = PX2DP(measure_rect.cx);
                    real_rect.width = measure_rect.cx;
                }
                if (rect->height == 0) {
                    rect->height = PX2DP(measure_rect.cy);
                    real_rect.height = measure_rect.cy;
                }
            }

            int32_t x = 0, y = 0;
            SetTextAlign(canvas->gdi.buffer_hdc, TA_LEFT);
            if ((format & CANVAS_TEXT_FORMAT_HORIZONTAL_CENTER) != 0) x = real_rect.width / 2;
            if ((format & CANVAS_TEXT_FORMAT_HORIZONTAL_RIGHT) != 0) x = real_rect.width;
            if ((format & CANVAS_TEXT_FORMAT_VERTICAL_CENTER) != 0) y = (real_rect.height - measure_rect.cy) / 2;
            if ((format & CANVAS_TEXT_FORMAT_VERTICAL_BOTTOM) != 0) y = real_rect.height;

            uint32_t align = TA_LEFT;
            if ((format & CANVAS_TEXT_FORMAT_HORIZONTAL_CENTER) != 0) align = TA_CENTER;
            if ((format & CANVAS_TEXT_FORMAT_HORIZONTAL_RIGHT) != 0) align = TA_RIGHT;
            if ((format & CANVAS_TEXT_FORMAT_VERTICAL_BOTTOM) != 0) align |= TA_BOTTOM;

            if ((color >> 24) == 0xff) {
                SetTextColor(canvas->gdi.buffer_hdc, color & 0x00ffffff);
                SetTextAlign(canvas->gdi.buffer_hdc, align);
                ExtTextOutW(canvas->gdi.buffer_hdc, real_rect.x + x, real_rect.y + y, ETO_CLIPPED,
                    &(RECT){ real_rect.x, real_rect.y, real_rect.x + real_rect.width, real_rect.y + real_rect.height }, text, length, NULL);
            } else {
                BitBlt(canvas->gdi.alpha_hdc, 0, 0, real_rect.width, real_rect.height, canvas->gdi.buffer_hdc, real_rect.x, real_rect.y, SRCCOPY);
                SelectObject(canvas->gdi.alpha_hdc, hfont);
                SetTextColor(canvas->gdi.alpha_hdc, color & 0x00ffffff);
                SetTextAlign(canvas->gdi.alpha_hdc, align);
                ExtTextOutW(canvas->gdi.alpha_hdc, x, y, ETO_CLIPPED, &(RECT){ 0, 0, real_rect.width, real_rect.height }, text, length, NULL);
                GdiAlphaBlend(canvas->gdi.buffer_hdc, real_rect.x, real_rect.y, real_rect.width, real_rect.height,
                    canvas->gdi.alpha_hdc, 0, 0, real_rect.width, real_rect.height, (BLENDFUNCTION){ AC_SRC_OVER, 0, color >> 24, 0 });
            }
        } else {
            int32_t options = DT_WORDBREAK;
            RECT measure_rect = { 0, 0, real_rect.width, 0 };
            if (
                rect->height == 0 ||
                (format & CANVAS_TEXT_FORMAT_VERTICAL_CENTER) != 0 ||
                (format & CANVAS_TEXT_FORMAT_VERTICAL_BOTTOM) != 0
            ) {
                DrawTextW(canvas->gdi.buffer_hdc, text, length, &measure_rect, DT_CALCRECT | options);
                if (rect->height == 0) {
                    rect->height = PX2DP(measure_rect.bottom);
                    real_rect.height = measure_rect.bottom;
                }
            }

            int32_t y = 0;
            if ((format & CANVAS_TEXT_FORMAT_HORIZONTAL_CENTER) != 0) options |= DT_CENTER;
            if ((format & CANVAS_TEXT_FORMAT_HORIZONTAL_RIGHT) != 0) options |= DT_RIGHT;
            if ((format & CANVAS_TEXT_FORMAT_VERTICAL_CENTER) != 0) {
                options |= DT_VCENTER;
                y = (real_rect.height - measure_rect.bottom) / 2;
            }
            if ((format & CANVAS_TEXT_FORMAT_VERTICAL_BOTTOM) != 0) {
                options |= DT_BOTTOM;
                y = real_rect.height - measure_rect.bottom;
            }

            if ((color >> 24) == 0xff) {
                SetTextColor(canvas->gdi.buffer_hdc, color & 0x00ffffff);
                DrawTextW(canvas->gdi.buffer_hdc, text, length,
                    &(RECT){real_rect.x, real_rect.y + y, real_rect.x + real_rect.width, real_rect.y + real_rect.height }, options);
            } else {
                BitBlt(canvas->gdi.alpha_hdc, 0, 0, real_rect.width, real_rect.height, canvas->gdi.buffer_hdc, real_rect.x, real_rect.y, SRCCOPY);
                SelectObject(canvas->gdi.alpha_hdc, hfont);
                SetTextColor(canvas->gdi.alpha_hdc, color & 0x00ffffff);
                DrawTextW(canvas->gdi.alpha_hdc, text, length, &(RECT){ 0, y, real_rect.width, real_rect.height }, options);
                GdiAlphaBlend(canvas->gdi.buffer_hdc, real_rect.x, real_rect.y, real_rect.width, real_rect.height,
                    canvas->gdi.alpha_hdc, 0, 0, real_rect.width, real_rect.height, (BLENDFUNCTION){ AC_SRC_OVER, 0, color >> 24, 0 });
            }
        }

        DeleteObject(hfont);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1Brush *brush;
        ID2D1RenderTarget_CreateSolidColorBrush(canvas->d2d.render_target, (&(D2D1_COLOR_F){
            (float)(color & 0xff) / 255, (float)((color >> 8) & 0xff) / 255,
            (float)((color >> 16) & 0xff) / 255, (float)((color >> 24) & 0xff) / 255
        }), NULL, &brush);

        IDWriteTextFormat *text_format;
        int32_t weight = DWRITE_FONT_WEIGHT_NORMAL;
        if (font->weight == CANVAS_FONT_WEIGHT_BOLD) weight = DWRITE_FONT_WEIGHT_BOLD;
        IDWriteFactory_CreateTextFormat(canvas->d2d.dwrite_factory, font->name, NULL, weight,
            font->italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            (font->size / 72) * 96, L"", &text_format);
        if ((format & CANVAS_TEXT_FORMAT_HORIZONTAL_CENTER) != 0) IDWriteTextFormat_SetTextAlignment(text_format, DWRITE_TEXT_ALIGNMENT_CENTER);
        if ((format & CANVAS_TEXT_FORMAT_HORIZONTAL_RIGHT) != 0) IDWriteTextFormat_SetTextAlignment(text_format, DWRITE_TEXT_ALIGNMENT_TRAILING);
        if ((format & CANVAS_TEXT_FORMAT_VERTICAL_CENTER) != 0) IDWriteTextFormat_SetParagraphAlignment(text_format, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        if ((format & CANVAS_TEXT_FORMAT_VERTICAL_BOTTOM) != 0) IDWriteTextFormat_SetParagraphAlignment(text_format, DWRITE_PARAGRAPH_ALIGNMENT_FAR);
        if ((format & CANVAS_TEXT_FORMAT_WRAP) == 0) IDWriteTextFormat_SetWordWrapping(text_format, DWRITE_WORD_WRAPPING_NO_WRAP);

        IDWriteTextLayout *text_layout;
        IDWriteFactory_CreateTextLayout(canvas->d2d.dwrite_factory, text, length, text_format, rect->width == 0 ? INT32_MAX : rect->width,
            rect->height == 0 ? INT32_MAX : rect->height, &text_layout);
        if (font->underline) IDWriteTextLayout_SetUnderline(text_layout, true, ((DWRITE_TEXT_RANGE){ 0, length}));
        if (font->line_through) IDWriteTextLayout_SetStrikethrough(text_layout, true, ((DWRITE_TEXT_RANGE){ 0, length}));
        if (rect->width == 0 || rect->height == 0) {
            DWRITE_TEXT_METRICS metrics;
            IDWriteTextLayout_GetMetrics(text_layout, &metrics);
            if (rect->width == 0) rect->width = metrics.width;
            if (rect->height == 0) rect->height = metrics.height;
        }

        ID2D1RenderTarget_DrawTextLayout(canvas->d2d.render_target, ((D2D1_POINT_2F){ rect->x, rect->y }),
            text_layout, brush, D2D1_DRAW_TEXT_OPTIONS_CLIP | D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);

        IUnknown_Release(text_layout);
        IUnknown_Release(text_format);
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
        CanvasRect real_rect = { DP2PX(rect->x), DP2PX(rect->y), DP2PX(rect->width), DP2PX(rect->height) };

        int32_t smooth_scale = 2; // Experimental smoothing feature gives dark border artifacts
        StretchBlt(canvas->gdi.alpha_hdc, 0, 0, real_rect.width * smooth_scale, real_rect.height * smooth_scale,
            canvas->gdi.buffer_hdc, real_rect.x, real_rect.y, real_rect.width, real_rect.height, SRCCOPY);
        BeginPath(canvas->gdi.alpha_hdc);
        float x = 0;
        float y = 0;
        float scale_x = (float)(real_rect.width * smooth_scale) / viewport_width;
        float scale_y = (float)(real_rect.height * smooth_scale) / viewport_height;
        char *c = path;
        while (*c != '\0') {
            if (*c == 'M') {
                c++;
                x = Canvas_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y = Canvas_ParsePathFloat(&c);
                MoveToEx(canvas->gdi.alpha_hdc, x * scale_x, y * scale_y, NULL);
            } else if (*c == 'm') {
                c++;
                x += Canvas_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y += Canvas_ParsePathFloat(&c);
                MoveToEx(canvas->gdi.alpha_hdc, x * scale_x, y * scale_y, NULL);
            } else if (*c == 'L') {
                c++;
                x = Canvas_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y = Canvas_ParsePathFloat(&c);
                LineTo(canvas->gdi.alpha_hdc, x * scale_x, y * scale_y);
            } else if (*c == 'l') {
                c++;
                x += Canvas_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y += Canvas_ParsePathFloat(&c);
                LineTo(canvas->gdi.alpha_hdc, x * scale_x, y * scale_y);
            } else if (*c == 'H') {
                c++;
                x = Canvas_ParsePathFloat(&c);
                LineTo(canvas->gdi.alpha_hdc, x * scale_x, y * scale_y);
            } else if (*c == 'h') {
                c++;
                x += Canvas_ParsePathFloat(&c);
                LineTo(canvas->gdi.alpha_hdc, x * scale_x, y * scale_y);
            } else if (*c == 'V') {
                c++;
                y = Canvas_ParsePathFloat(&c);
                LineTo(canvas->gdi.alpha_hdc, x * scale_x, y * scale_y);
            } else if (*c == 'v') {
                c++;
                y += Canvas_ParsePathFloat(&c);
                LineTo(canvas->gdi.alpha_hdc, x * scale_x, y * scale_y);
            } else if (*c == 'Z' || *c == 'z') {
                c++;
            }
        }
        EndPath(canvas->gdi.alpha_hdc);
        HBRUSH brush = CreateSolidBrush(color & 0x00ffffff);
        SelectObject(canvas->gdi.alpha_hdc, brush);
        FillPath(canvas->gdi.alpha_hdc);
        DeleteObject(brush);
        if ((color >> 24) == 0xff) {
            StretchBlt(canvas->gdi.buffer_hdc, real_rect.x, real_rect.y, real_rect.width, real_rect.height,
                canvas->gdi.alpha_hdc, 0, 0, real_rect.width * smooth_scale, real_rect.height * smooth_scale, SRCCOPY);
        } else {
            GdiAlphaBlend(canvas->gdi.buffer_hdc, real_rect.x, real_rect.y, real_rect.width, real_rect.height,
                canvas->gdi.alpha_hdc, 0, 0, real_rect.width * smooth_scale, real_rect.height * smooth_scale, (BLENDFUNCTION){ AC_SRC_OVER, 0, color >> 24, 0 });
        }
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1PathGeometry *path_geometry;
        ID2D1Factory_CreatePathGeometry(canvas->d2d.d2d_factory, &path_geometry);

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

        ID2D1Brush *brush;
        ID2D1RenderTarget_CreateSolidColorBrush(canvas->d2d.render_target, (&(D2D1_COLOR_F){
            (float)(color & 0xff) / 255, (float)((color >> 8) & 0xff) / 255,
            (float)((color >> 16) & 0xff) / 255, (float)((color >> 24) & 0xff) / 255
        }), NULL, &brush);
        ID2D1RenderTarget_FillGeometry(canvas->d2d.render_target, (ID2D1Geometry *)path_geometry, brush, NULL);
        IUnknown_Release(brush);
        IUnknown_Release(path_geometry);
    }
}
