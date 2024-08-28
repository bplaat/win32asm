// ### Canvas Renderer v0.2.1-dev ###
// A simple dpi aware / alpha blending 2D Win32 Canvas library with a pure GDI and Direct2D backend
// ~ Made by Bastiaan van der Plaat (https://bastiaan.ml/)

// Define options:
// CANVAS_IMPLEMENTATION: Use this once in your project for the c code
// CANVAS_USE_CUSTOM_HEADERS: Use custom Direct2D and DirectWrite header files
// CANVAS_ENABLE_BITMAP: Enable bitmap loading and rendering code
// CANVAS_ENABLE_STBI_IMAGE: Enable code that uses the STBI image library (needs CANVAS_ENABLE_BITMAP)
// CANVAS_ENABLE_TEXT: Enable text measuring and rendering code
// CANVAS_ENABLE_PATH: Enable basic SVG path string rendering

#pragma once

#include <windows.h>
#include <stdint.h>
#include <stdbool.h>

// stringapiset.h
#define CP_ACP 0
int WideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWCH lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCCH lpDefaultChar, LPBOOL lpUsedDefaultChar);

// wingdi.h
BOOL GdiAlphaBlend(HDC hdcDest, int xoriginDest, int yoriginDest, int wDest, int hDest, HDC hdcSrc, int xoriginSrc, int yoriginSrc, int wSrc, int hSrc, BLENDFUNCTION ftn);

#ifdef CANVAS_USE_CUSTOM_HEADERS
#include "direct2d.h"
#include "directwrite.h"
#else
#define D2D_USE_C_DEFINITIONS
#include <d2d1.h>
#define COBJMACROS
#include <dwrite.h>
#endif

#ifdef CANVAS_ENABLE_STBI_IMAGE
#include "stb_image.h"
#endif

#define CANVAS_RGB(r, g, b) ((r & 0xff) | ((g & 0xff) << 8) | ((b & 0xff) << 16) | (0xff << 24))
#define CANVAS_RGBA(r, g, b, a) ((r & 0xff) | ((g & 0xff) << 8) | ((b & 0xff) << 16) | ((a & 0xff) << 24))
#define CANVAS_HEX(x) (((x >> 16) & 0xff) | (((x >> 8) & 0xff) << 8) | ((x & 0xff) << 16) | (0xff << 24))
#define CANVAS_HEXA(x) (((x >> 24) & 0xff) | (((x >> 16) & 0xff) << 8) | (((x >> 8) & 0xff) << 16) | ((x & 0xff) << 24))

typedef struct CanvasPoint {
    float x;
    float y;
} CanvasPoint;

typedef struct CanvasRect {
    float x;
    float y;
    float width;
    float height;
} CanvasRect;

#define CANVAS_POINT_IN_RECT(point, rect) (point.x >= rect.x && point.y >= rect.y \
    && point.x < rect.x + rect.width && point.y < rect.y + rect.height)

typedef uint32_t CanvasColor;

typedef enum CanvasFontWeight {
    CANVAS_FONT_WEIGHT_NORMAL,
    CANVAS_FONT_WEIGHT_BOLD
} CanvasFontWeight;

typedef struct CanvasFont {
    wchar_t *name;
    float size;
    CanvasFontWeight weight;
    bool italic;
    bool underline;
    bool lineThrough;
} CanvasFont;

typedef enum CanvasTextFormat {
    CANVAS_TEXT_FORMAT_DEFAULT = 0,
    CANVAS_TEXT_FORMAT_HORIZONTAL_LEFT = 0,
    CANVAS_TEXT_FORMAT_HORIZONTAL_CENTER = 1,
    CANVAS_TEXT_FORMAT_HORIZONTAL_RIGHT = 2,
    CANVAS_TEXT_FORMAT_VERTICAL_TOP = 0,
    CANVAS_TEXT_FORMAT_VERTICAL_CENTER = 4,
    CANVAS_TEXT_FORMAT_VERTICAL_BOTTOM = 8,
    CANVAS_TEXT_FORMAT_NO_WRAP = 0,
    CANVAS_TEXT_FORMAT_WRAP = 16
} CanvasTextFormat;

typedef struct CanvasTransform {
    float m11;
    float m12;
    float m21;
    float m22;
    float dx;
    float dy;
} CanvasTransform;

typedef enum CanvasRenderer {
    CANVAS_RENDERER_DEFAULT,
    CANVAS_RENDERER_GDI,
    CANVAS_RENDERER_DIRECT2D
} CanvasRenderer;

typedef struct Canvas {
    CanvasRenderer renderer;
    HWND hwnd;
    int32_t width;
    int32_t height;
    int32_t dpi;
    PAINTSTRUCT ps;

    union {
        struct {
            HDC bufferHdc;
            HBITMAP bufferBitmap;
            HDC alphaHdc;
            HBITMAP alphaBitmap;
        } gdi;

        struct {
            ID2D1Factory *d2dFactory;
            IDWriteFactory *dwriteFactory;
            ID2D1HwndRenderTarget *renderTarget;
        } d2d;
    };
} Canvas;

typedef struct CanvasBitmap {
    Canvas *canvas;
    int32_t width;
    int32_t height;

    union {
        struct {
            uint8_t *data;
            HBITMAP bitmap;
        } gdi;

        struct {
            ID2D1Bitmap *bitmap;
        } d2d;
    };
} CanvasBitmap;

extern CanvasBitmap *CanvasBitmap_NewFromMemory(Canvas *canvas, int32_t width, int32_t height, uint8_t *data);

extern CanvasBitmap *CanvasBitmap_NewFromFile(Canvas *canvas, wchar_t *path);

extern CanvasBitmap *CanvasBitmap_NewFromResource(Canvas *canvas, wchar_t *type, wchar_t *name);

extern void CanvasBitmap_Free(CanvasBitmap *bitmap);

extern Canvas *Canvas_New(HWND hwnd, CanvasRenderer renderer);

extern void Canvas_Free(Canvas *canvas);

extern void Canvas_Resize(Canvas *canvas, int32_t width, int32_t height, int32_t dpi);

extern void Canvas_BeginDraw(Canvas *canvas);

extern void Canvas_EndDraw(Canvas *canvas);

extern void Canvas_Transform(Canvas *canvas, CanvasTransform *transform);

extern void Canvas_Clip(Canvas *canvas, CanvasRect *rect);

extern void Canvas_FillRect(Canvas *canvas, CanvasRect *rect, CanvasColor color);

extern void Canvas_StrokeRect(Canvas *canvas, CanvasRect *rect, CanvasColor color, float strokeWidth);

extern void Canvas_DrawBitmap(Canvas *canvas, CanvasBitmap *bitmap, CanvasRect *destinationRect, CanvasRect *sourceRect);

extern void Canvas_MeasureText(Canvas *canvas, wchar_t *text, int32_t length, CanvasRect *rect, CanvasFont *font, CanvasTextFormat format);

extern void Canvas_DrawText(Canvas *canvas, wchar_t *text, int32_t length, CanvasRect *rect, CanvasFont *font, CanvasTextFormat format, CanvasColor color);

extern void Canvas_FillPath(Canvas *canvas, char *path, int32_t viewportWidth, int32_t viewportHeight, CanvasRect *rect, CanvasColor color);

// ##########################################################################################################
// ##########################################################################################################
// ##########################################################################################################

#ifdef CANVAS_IMPLEMENTATION

#define DP2PX(dp) MulDiv(dp, canvas->dpi, 96)
#define PX2DP(px) MulDiv(px, 96, canvas->dpi)

#ifdef CANVAS_ENABLE_BITMAP

CanvasBitmap *CanvasBitmap_NewFromMemory(Canvas *canvas, int32_t width, int32_t height, uint8_t *data) {
    CanvasBitmap *bitmap = malloc(sizeof(CanvasBitmap));
    bitmap->canvas = canvas;
    bitmap->width = width;
    bitmap->height = height;

    // TODO: support transparent images for real not alpha blending with black
    uint8_t *pixels = malloc(width * height * 4);
    int32_t in = 0, out = 0;
    for (int32_t y = 0; y < height; y++) {
        for (int32_t x = 0; x < width; x++) {
            uint8_t alpha = data[in + 3];
            pixels[out + 0] = data[in + 2] * alpha >> 8;
            pixels[out + 1] = data[in + 1] * alpha >> 8;
            pixels[out + 2] = data[in + 0] * alpha >> 8;
            pixels[out + 3] = 0;
            in += 4;
            out += 4;
        }
    }

    if (bitmap->canvas->renderer == CANVAS_RENDERER_GDI) {
        bitmap->gdi.data = pixels;
        bitmap->gdi.bitmap = CreateBitmap(width, height, 1, 32, pixels);
    }

    if (bitmap->canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1HwndRenderTarget_CreateBitmap(canvas->d2d.renderTarget, ((D2D1_SIZE_U){ width, height }), pixels, width * 4,
            (&(D2D1_BITMAP_PROPERTIES){ { DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE }, 96, 96 }), &bitmap->d2d.bitmap);
        free(pixels);
    }

    return bitmap;
}

#ifdef CANVAS_ENABLE_STBI_IMAGE

CanvasBitmap *CanvasBitmap_NewFromFile(Canvas *canvas, wchar_t *path) {
    char pathAscii[MAX_PATH];
    WideCharToMultiByte(CP_ACP, 0, path, -1, pathAscii, MAX_PATH, NULL, FALSE);

    int32_t width, height, channels;
    uint8_t *pixels = stbi_load(pathAscii, &width, &height, &channels, 4);
    if (pixels == NULL) return NULL;
    CanvasBitmap *bitmap = CanvasBitmap_NewFromMemory(canvas, width, height, pixels);
    free(pixels);
    return bitmap;
}

CanvasBitmap *CanvasBitmap_NewFromResource(Canvas *canvas, wchar_t *type, wchar_t *name) {
    HRSRC hsrc = FindResourceW(NULL, name, type);
    if (hsrc == NULL) return NULL;
    uint8_t *data = (uint8_t *)LockResource(LoadResource(NULL, hsrc));
    DWORD size = SizeofResource(NULL, hsrc);

    int32_t width, height, channels;
    uint8_t *pixels = stbi_load_from_memory(data, size, &width, &height, &channels, 4);
    if (pixels == NULL) return NULL;
    CanvasBitmap *bitmap = CanvasBitmap_NewFromMemory(canvas, width, height, pixels);
    free(pixels);
    return bitmap;
}

#endif

void CanvasBitmap_Free(CanvasBitmap *bitmap) {
    if (bitmap->canvas->renderer == CANVAS_RENDERER_GDI) {
        DeleteObject(bitmap->gdi.bitmap);
        free(bitmap->gdi.data);
    }
    if (bitmap->canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1Bitmap_Release(bitmap->d2d.bitmap);
    }
    free(bitmap);
}

#endif

typedef int32_t (STDMETHODCALLTYPE *_D2D1CreateFactory)(uint32_t factoryType, REFIID riid, const void *pFactoryOptions, ID2D1Factory **ppIFactory);

typedef int32_t (STDMETHODCALLTYPE *_DWriteCreateFactory)(uint32_t factoryType, REFIID riid, IDWriteFactory **factory);

Canvas *Canvas_New(HWND hwnd, CanvasRenderer renderer) {
    Canvas *canvas = malloc(sizeof(Canvas));
    canvas->renderer = renderer;
    canvas->hwnd = hwnd;

    HMODULE hd2d1 = LoadLibraryW(L"d2d1");
    HMODULE hdwrite = LoadLibraryW(L"dwrite");
    if (canvas->renderer == CANVAS_RENDERER_DEFAULT) {
        canvas->renderer = hd2d1 && hdwrite ? CANVAS_RENDERER_DIRECT2D : CANVAS_RENDERER_GDI;
    }

    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        canvas->gdi.bufferHdc = NULL;
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        _D2D1CreateFactory D2D1CreateFactory = (_D2D1CreateFactory)GetProcAddress(hd2d1, "D2D1CreateFactory");
        IID IID_ID2D1Factory = { 0xbb12d362, 0xdaee, 0x4b9a, { 0xaa, 0x1d, 0x14, 0xba, 0x40, 0x1c, 0xfa, 0x1f } };
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &IID_ID2D1Factory, NULL, &canvas->d2d.d2dFactory))) {
            canvas->renderer = CANVAS_RENDERER_GDI;
            canvas->gdi.bufferHdc = NULL;
            return canvas;
        }

        _DWriteCreateFactory DWriteCreateFactory = (_DWriteCreateFactory)GetProcAddress(hdwrite, "DWriteCreateFactory");
        IID IID_IDWriteFactory = { 0xb859ee5a, 0xd838, 0x4b5b, { 0xa2, 0xe8, 0x1a, 0xdc, 0x7d, 0x93, 0xdb, 0x48 } };
        if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, &IID_IDWriteFactory, &canvas->d2d.dwriteFactory))) {
            ID2D1Factory_Release(canvas->d2d.d2dFactory);
            canvas->renderer = CANVAS_RENDERER_GDI;
            canvas->gdi.bufferHdc = NULL;
            return canvas;
        }

        if (FAILED(
            ID2D1Factory_CreateHwndRenderTarget(canvas->d2d.d2dFactory, (&(D2D1_RENDER_TARGET_PROPERTIES){ D2D1_RENDER_TARGET_TYPE_DEFAULT,
            { DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_UNKNOWN }, 96, 96, D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT }),
            (&(D2D1_HWND_RENDER_TARGET_PROPERTIES){ hwnd, { 0, 0 }, D2D1_PRESENT_OPTIONS_NONE }), &canvas->d2d.renderTarget)
        )) {
            IDWriteFactory_Release(canvas->d2d.dwriteFactory);
            ID2D1Factory_Release(canvas->d2d.d2dFactory);
            canvas->renderer = CANVAS_RENDERER_GDI;
            canvas->gdi.bufferHdc = NULL;
            return canvas;
        }
    }
    return canvas;
}

void Canvas_Free(Canvas *canvas) {
    if (canvas->renderer == CANVAS_RENDERER_GDI && canvas->gdi.bufferHdc != NULL) {
        DeleteObject(canvas->gdi.alphaBitmap);
        DeleteDC(canvas->gdi.alphaHdc);
        DeleteObject(canvas->gdi.bufferBitmap);
        DeleteDC(canvas->gdi.bufferHdc);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1HwndRenderTarget_Release(canvas->d2d.renderTarget);
        IDWriteFactory_Release(canvas->d2d.dwriteFactory);
        ID2D1Factory_Release(canvas->d2d.d2dFactory);
    }

    free(canvas);
}

void Canvas_Resize(Canvas *canvas, int32_t width, int32_t height, int32_t dpi) {
    canvas->width = width;
    canvas->height = height;
    canvas->dpi = dpi;

    if (canvas->renderer == CANVAS_RENDERER_GDI && canvas->gdi.bufferHdc != NULL) {
        DeleteObject(canvas->gdi.alphaBitmap);
        DeleteDC(canvas->gdi.alphaHdc);
        DeleteObject(canvas->gdi.bufferBitmap);
        DeleteDC(canvas->gdi.bufferHdc);
        canvas->gdi.bufferHdc = NULL;
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1HwndRenderTarget_Resize(canvas->d2d.renderTarget, (&(D2D1_SIZE_U){ canvas->width, canvas->height }));
        ID2D1HwndRenderTarget_SetDpi(canvas->d2d.renderTarget, canvas->dpi, canvas->dpi);
    }
}

void Canvas_BeginDraw(Canvas *canvas) {
    BeginPaint(canvas->hwnd, &canvas->ps);

    if (canvas->renderer == CANVAS_RENDERER_GDI && canvas->gdi.bufferHdc == NULL) {
        canvas->gdi.bufferHdc = CreateCompatibleDC(canvas->ps.hdc);
        SetGraphicsMode(canvas->gdi.bufferHdc, GM_ADVANCED);
        SetBkMode(canvas->gdi.bufferHdc, TRANSPARENT);
        SetStretchBltMode(canvas->gdi.bufferHdc, STRETCH_HALFTONE);
        canvas->gdi.bufferBitmap = CreateCompatibleBitmap(canvas->ps.hdc, canvas->width, canvas->height);
        SelectObject(canvas->gdi.bufferHdc, canvas->gdi.bufferBitmap);

        canvas->gdi.alphaHdc = CreateCompatibleDC(canvas->ps.hdc);
        SetBkMode(canvas->gdi.alphaHdc, TRANSPARENT);
        SetStretchBltMode(canvas->gdi.alphaHdc, STRETCH_HALFTONE);
        canvas->gdi.alphaBitmap = CreateCompatibleBitmap(canvas->ps.hdc, canvas->width * 2, canvas->height * 2);
        SelectObject(canvas->gdi.alphaHdc, canvas->gdi.alphaBitmap);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1HwndRenderTarget_BeginDraw(canvas->d2d.renderTarget);
    }
}

void Canvas_EndDraw(Canvas *canvas) {
    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        BitBlt(canvas->ps.hdc, 0, 0, canvas->width, canvas->height, canvas->gdi.bufferHdc, 0, 0, SRCCOPY);
    }
    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1HwndRenderTarget_EndDraw(canvas->d2d.renderTarget, NULL, NULL);
    }
    EndPaint(canvas->hwnd, &canvas->ps);
}

void Canvas_Transform(Canvas *canvas, CanvasTransform *transform) {
    CanvasTransform identity = { 1, 0, 0, 1, 0, 0 };
    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        if (transform != NULL) {
            SetWorldTransform(canvas->gdi.bufferHdc, &(XFORM){
                transform->m11, transform->m12,
                transform->m21, transform->m22,
                DP2PX(transform->dx), DP2PX(transform->dy)
            });
        } else {
            SetWorldTransform(canvas->gdi.bufferHdc, (XFORM *)&identity);
        }
    }
    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1HwndRenderTarget_SetTransform(canvas->d2d.renderTarget, (D2D1_MATRIX_3X2_F *)(transform != NULL ? transform : &identity));
    }
}

void Canvas_Clip(Canvas *canvas, CanvasRect *rect) {
    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        if (rect != NULL) {
            CanvasRect realRect = { DP2PX(rect->x), DP2PX(rect->y), DP2PX(rect->width), DP2PX(rect->height) };
            POINT points[] = { { realRect.x, realRect.y }, { realRect.x + realRect.width, realRect.y + realRect.height } };
            LPtoDP(canvas->gdi.bufferHdc, points, 2);
            HRGN clipRegion = CreateRectRgn(points[0].x, points[0].y, points[1].x, points[1].y);
            SelectClipRgn(canvas->gdi.bufferHdc, clipRegion);
            DeleteObject(clipRegion);
        } else {
            SelectClipRgn(canvas->gdi.bufferHdc, NULL);
        }
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        if (rect != NULL) {
            ID2D1HwndRenderTarget_PushAxisAlignedClip(canvas->d2d.renderTarget,
                (&(D2D1_RECT_F){ rect->x, rect->y, rect->x + rect->width, rect->y + rect->height }),
                D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        } else {
            ID2D1HwndRenderTarget_PopAxisAlignedClip(canvas->d2d.renderTarget);
        }
    }
}

void Canvas_FillRect(Canvas *canvas, CanvasRect *rect, CanvasColor color) {
    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        CanvasRect realRect = { DP2PX(rect->x), DP2PX(rect->y), DP2PX(rect->width), DP2PX(rect->height) };
        HBRUSH brush = CreateSolidBrush(color & 0x00ffffff);
        if ((color >> 24) == 0xff) {
            FillRect(canvas->gdi.bufferHdc, &(RECT){ realRect.x, realRect.y, realRect.x + realRect.width, realRect.y + realRect.height }, brush);
        } else {
            FillRect(canvas->gdi.alphaHdc, &(RECT){ 0, 0, realRect.width, realRect.height }, brush);
            GdiAlphaBlend(canvas->gdi.bufferHdc, realRect.x, realRect.y, realRect.width, realRect.height,
                canvas->gdi.alphaHdc, 0, 0, realRect.width, realRect.height, (BLENDFUNCTION){ AC_SRC_OVER, 0, color >> 24, 0 });
        }
        DeleteObject(brush);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1SolidColorBrush *brush;
        ID2D1HwndRenderTarget_CreateSolidColorBrush(canvas->d2d.renderTarget, (&(D2D1_COLOR_F){
            (float)(color & 0xff) / 255, (float)((color >> 8) & 0xff) / 255,
            (float)((color >> 16) & 0xff) / 255, (float)((color >> 24) & 0xff) / 255
        }), NULL, &brush);
        ID2D1HwndRenderTarget_FillRectangle(canvas->d2d.renderTarget,
            (&(D2D1_RECT_F){ rect->x, rect->y, rect->x + rect->width, rect->y + rect->height }), (ID2D1Brush *)brush);
        ID2D1SolidColorBrush_Release(brush);
    }
}

void Canvas_StrokeRect(Canvas *canvas, CanvasRect *rect, CanvasColor color, float strokeWidth) {
    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        CanvasRect realRect = { DP2PX(rect->x), DP2PX(rect->y), DP2PX(rect->width), DP2PX(rect->height) };
        HPEN pen = CreatePen(PS_SOLID, strokeWidth, color & 0x00ffffff);
        if ((color >> 24) == 0xff) {
            SelectObject(canvas->gdi.bufferHdc, pen);
            SelectObject(canvas->gdi.bufferHdc, GetStockObject(NULL_BRUSH));
            Rectangle(canvas->gdi.bufferHdc, realRect.x, realRect.y, realRect.x + realRect.width + 1, realRect.y + realRect.height + 1);
        } else {
            BitBlt(canvas->gdi.alphaHdc, 0, 0, realRect.width + strokeWidth * 2, realRect.height + strokeWidth * 2, canvas->gdi.bufferHdc, realRect.x - strokeWidth, realRect.y - strokeWidth, SRCCOPY);
            SelectObject(canvas->gdi.alphaHdc, pen);
            SelectObject(canvas->gdi.alphaHdc, GetStockObject(NULL_BRUSH));
            Rectangle(canvas->gdi.alphaHdc, strokeWidth, strokeWidth, strokeWidth + realRect.width + 1, strokeWidth + realRect.height + 1);
            GdiAlphaBlend(canvas->gdi.bufferHdc, realRect.x - strokeWidth, realRect.y - strokeWidth, realRect.width + strokeWidth * 2, realRect.height + strokeWidth * 2,
                canvas->gdi.alphaHdc, 0, 0, realRect.width + strokeWidth * 2, realRect.height + strokeWidth * 2, (BLENDFUNCTION){ AC_SRC_OVER, 0, color >> 24, 0 });
        }
        DeleteObject(pen);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1SolidColorBrush *brush;
        ID2D1HwndRenderTarget_CreateSolidColorBrush(canvas->d2d.renderTarget,
            (&(D2D1_COLOR_F){ (float)(color & 0xff) / 255, (float)((color >> 8) & 0xff) / 255,
            (float)((color >> 16) & 0xff) / 255, (float)((color >> 24) & 0xff) / 255 }), NULL, &brush);
        ID2D1HwndRenderTarget_DrawRectangle(canvas->d2d.renderTarget, (&(D2D1_RECT_F){ rect->x, rect->y,
            rect->x + rect->width, rect->y + rect->height }), (ID2D1Brush *)brush, strokeWidth, NULL);
        ID2D1SolidColorBrush_Release(brush);
    }
}

#ifdef CANVAS_ENABLE_BITMAP

void Canvas_DrawBitmap(Canvas *canvas, CanvasBitmap *bitmap, CanvasRect *destinationRect, CanvasRect *sourceRect) {
    // TODO: Support real transparent bitmaps and more alpha blending stuff
    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        CanvasRect realRect = { DP2PX(destinationRect->x), DP2PX(destinationRect->y), DP2PX(destinationRect->width), DP2PX(destinationRect->height) };
        HBITMAP oldBitmap = SelectObject(canvas->gdi.alphaHdc, bitmap->gdi.bitmap);
        StretchBlt(canvas->gdi.bufferHdc, realRect.x, realRect.y, realRect.width, realRect.height,
            canvas->gdi.alphaHdc, sourceRect != NULL ? sourceRect->x : 0, sourceRect != NULL ? sourceRect->y : 0,
            sourceRect != NULL ? sourceRect->width : bitmap->width, sourceRect != NULL ? sourceRect->height : bitmap->height, SRCCOPY);
        SelectObject(canvas->gdi.alphaHdc, oldBitmap);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1HwndRenderTarget_DrawBitmap(canvas->d2d.renderTarget, bitmap->d2d.bitmap, (&(D2D1_RECT_F){ destinationRect->x, destinationRect->y,
            destinationRect->x + destinationRect->width, destinationRect->y + destinationRect->height }), 1, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
            sourceRect != NULL ? (&(D2D1_RECT_F){ sourceRect->x, sourceRect->y, sourceRect->x + sourceRect->width, sourceRect->y + sourceRect->height }): NULL);
    }
}

#endif

#ifdef CANVAS_ENABLE_TEXT

void Canvas_MeasureText(Canvas *canvas, wchar_t *text, int32_t length, CanvasRect *rect, CanvasFont *font, CanvasTextFormat format) {
    if (length == -1) length = wcslen(text);

    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        HDC hdc = canvas->gdi.bufferHdc != NULL ? canvas->gdi.bufferHdc : GetDC(canvas->hwnd);
        int32_t weight = FW_NORMAL;
        if (font->weight == CANVAS_FONT_WEIGHT_BOLD) weight = FW_BOLD;
        HFONT hfont = CreateFontW(-MulDiv(font->size, canvas->dpi, 72), 0, 0, 0, weight, font->italic, font->underline, font->lineThrough,
            ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font->name);
        SelectObject(hdc, hfont);

        if ((format & CANVAS_TEXT_FORMAT_WRAP) != 0) {
            RECT measureRect = { 0, 0, DP2PX(rect->width), 0 };
            DrawTextW(hdc, text, length, &measureRect, DT_CALCRECT | DT_WORDBREAK);
            rect->width = PX2DP(measureRect.right);
            rect->height = PX2DP(measureRect.bottom);
        } else {
            SIZE measureRect;
            GetTextExtentPoint32W(hdc, text, length, &measureRect);
            rect->width = PX2DP(measureRect.cx);
            rect->height = PX2DP(measureRect.cy);
        }

        DeleteObject(hfont);
        if (canvas->gdi.bufferHdc == NULL) ReleaseDC(canvas->hwnd, hdc);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        IDWriteTextFormat *textFormat;
        int32_t weight = DWRITE_FONT_WEIGHT_NORMAL;
        if (font->weight == CANVAS_FONT_WEIGHT_BOLD) weight = DWRITE_FONT_WEIGHT_BOLD;
        IDWriteFactory_CreateTextFormat(canvas->d2d.dwriteFactory, font->name, NULL, weight,
            font->italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            (font->size / 72) * 96, L"", &textFormat);
        if ((format & CANVAS_TEXT_FORMAT_WRAP) == 0) IDWriteTextFormat_SetWordWrapping(textFormat, DWRITE_WORD_WRAPPING_NO_WRAP);

        IDWriteTextLayout *textLayout;
        IDWriteFactory_CreateTextLayout(canvas->d2d.dwriteFactory, text, length, textFormat,
            (format & CANVAS_TEXT_FORMAT_WRAP) != 0 ? rect->width : INT32_MAX, INT32_MAX, &textLayout);
        if (font->underline) IDWriteTextLayout_SetUnderline(textLayout, true, ((DWRITE_TEXT_RANGE){ 0, length}));
        if (font->lineThrough) IDWriteTextLayout_SetStrikethrough(textLayout, true, ((DWRITE_TEXT_RANGE){ 0, length}));

        DWRITE_TEXT_METRICS metrics;
        IDWriteTextLayout_GetMetrics(textLayout, &metrics);
        rect->width = metrics.width;
        rect->height = metrics.height;

        IDWriteTextLayout_Release(textLayout);
        IDWriteTextFormat_Release(textFormat);
    }
}

void Canvas_DrawText(Canvas *canvas, wchar_t *text, int32_t length, CanvasRect *rect, CanvasFont *font, CanvasTextFormat format, CanvasColor color) {
    if (length == -1) length = wcslen(text);

    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        CanvasRect realRect = { DP2PX(rect->x), DP2PX(rect->y), DP2PX(rect->width), DP2PX(rect->height) };

        int32_t weight = FW_NORMAL;
        if (font->weight == CANVAS_FONT_WEIGHT_BOLD) weight = FW_BOLD;
        HFONT hfont = CreateFontW(-MulDiv(font->size, canvas->dpi, 72), 0, 0, 0, weight, font->italic, font->underline, font->lineThrough,
            ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font->name);
        SelectObject(canvas->gdi.bufferHdc, hfont);

        if ((format & CANVAS_TEXT_FORMAT_WRAP) == 0) {
            SIZE measureRect;
            if (
                rect->width == 0 ||
                rect->height == 0 ||
                (format & CANVAS_TEXT_FORMAT_VERTICAL_CENTER) != 0
            ) {
                GetTextExtentPoint32W(canvas->gdi.bufferHdc, text, length, &measureRect);
                if (rect->width == 0) {
                    rect->width = PX2DP(measureRect.cx);
                    realRect.width = measureRect.cx;
                }
                if (rect->height == 0) {
                    rect->height = PX2DP(measureRect.cy);
                    realRect.height = measureRect.cy;
                }
            }

            int32_t x = 0, y = 0;
            SetTextAlign(canvas->gdi.bufferHdc, TA_LEFT);
            if ((format & CANVAS_TEXT_FORMAT_HORIZONTAL_CENTER) != 0) x = realRect.width / 2;
            if ((format & CANVAS_TEXT_FORMAT_HORIZONTAL_RIGHT) != 0) x = realRect.width;
            if ((format & CANVAS_TEXT_FORMAT_VERTICAL_CENTER) != 0) y = (realRect.height - measureRect.cy) / 2;
            if ((format & CANVAS_TEXT_FORMAT_VERTICAL_BOTTOM) != 0) y = realRect.height;

            uint32_t align = TA_LEFT;
            if ((format & CANVAS_TEXT_FORMAT_HORIZONTAL_CENTER) != 0) align = TA_CENTER;
            if ((format & CANVAS_TEXT_FORMAT_HORIZONTAL_RIGHT) != 0) align = TA_RIGHT;
            if ((format & CANVAS_TEXT_FORMAT_VERTICAL_BOTTOM) != 0) align |= TA_BOTTOM;

            if ((color >> 24) == 0xff) {
                SetTextColor(canvas->gdi.bufferHdc, color & 0x00ffffff);
                SetTextAlign(canvas->gdi.bufferHdc, align);
                ExtTextOutW(canvas->gdi.bufferHdc, realRect.x + x, realRect.y + y, ETO_CLIPPED,
                    &(RECT){ realRect.x, realRect.y, realRect.x + realRect.width, realRect.y + realRect.height }, text, length, NULL);
            } else {
                BitBlt(canvas->gdi.alphaHdc, 0, 0, realRect.width, realRect.height, canvas->gdi.bufferHdc, realRect.x, realRect.y, SRCCOPY);
                SelectObject(canvas->gdi.alphaHdc, hfont);
                SetTextColor(canvas->gdi.alphaHdc, color & 0x00ffffff);
                SetTextAlign(canvas->gdi.alphaHdc, align);
                ExtTextOutW(canvas->gdi.alphaHdc, x, y, ETO_CLIPPED, &(RECT){ 0, 0, realRect.width, realRect.height }, text, length, NULL);
                GdiAlphaBlend(canvas->gdi.bufferHdc, realRect.x, realRect.y, realRect.width, realRect.height,
                    canvas->gdi.alphaHdc, 0, 0, realRect.width, realRect.height, (BLENDFUNCTION){ AC_SRC_OVER, 0, color >> 24, 0 });
            }
        } else {
            int32_t options = DT_WORDBREAK;
            RECT measureRect = { 0, 0, realRect.width, 0 };
            if (
                rect->height == 0 ||
                (format & CANVAS_TEXT_FORMAT_VERTICAL_CENTER) != 0 ||
                (format & CANVAS_TEXT_FORMAT_VERTICAL_BOTTOM) != 0
            ) {
                DrawTextW(canvas->gdi.bufferHdc, text, length, &measureRect, DT_CALCRECT | options);
                if (rect->height == 0) {
                    rect->height = PX2DP(measureRect.bottom);
                    realRect.height = measureRect.bottom;
                }
            }

            int32_t y = 0;
            if ((format & CANVAS_TEXT_FORMAT_HORIZONTAL_CENTER) != 0) options |= DT_CENTER;
            if ((format & CANVAS_TEXT_FORMAT_HORIZONTAL_RIGHT) != 0) options |= DT_RIGHT;
            if ((format & CANVAS_TEXT_FORMAT_VERTICAL_CENTER) != 0) {
                options |= DT_VCENTER;
                y = (realRect.height - measureRect.bottom) / 2;
            }
            if ((format & CANVAS_TEXT_FORMAT_VERTICAL_BOTTOM) != 0) {
                options |= DT_BOTTOM;
                y = realRect.height - measureRect.bottom;
            }

            if ((color >> 24) == 0xff) {
                SetTextColor(canvas->gdi.bufferHdc, color & 0x00ffffff);
                DrawTextW(canvas->gdi.bufferHdc, text, length,
                    &(RECT){realRect.x, realRect.y + y, realRect.x + realRect.width, realRect.y + realRect.height }, options);
            } else {
                BitBlt(canvas->gdi.alphaHdc, 0, 0, realRect.width, realRect.height, canvas->gdi.bufferHdc, realRect.x, realRect.y, SRCCOPY);
                SelectObject(canvas->gdi.alphaHdc, hfont);
                SetTextColor(canvas->gdi.alphaHdc, color & 0x00ffffff);
                DrawTextW(canvas->gdi.alphaHdc, text, length, &(RECT){ 0, y, realRect.width, realRect.height }, options);
                GdiAlphaBlend(canvas->gdi.bufferHdc, realRect.x, realRect.y, realRect.width, realRect.height,
                    canvas->gdi.alphaHdc, 0, 0, realRect.width, realRect.height, (BLENDFUNCTION){ AC_SRC_OVER, 0, color >> 24, 0 });
            }
        }

        DeleteObject(hfont);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1SolidColorBrush *brush;
        ID2D1HwndRenderTarget_CreateSolidColorBrush(canvas->d2d.renderTarget, (&(D2D1_COLOR_F){
            (float)(color & 0xff) / 255, (float)((color >> 8) & 0xff) / 255,
            (float)((color >> 16) & 0xff) / 255, (float)((color >> 24) & 0xff) / 255
        }), NULL, &brush);

        IDWriteTextFormat *textFormat;
        int32_t weight = DWRITE_FONT_WEIGHT_NORMAL;
        if (font->weight == CANVAS_FONT_WEIGHT_BOLD) weight = DWRITE_FONT_WEIGHT_BOLD;
        IDWriteFactory_CreateTextFormat(canvas->d2d.dwriteFactory, font->name, NULL, weight,
            font->italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            (font->size / 72) * 96, L"", &textFormat);
        if ((format & CANVAS_TEXT_FORMAT_HORIZONTAL_CENTER) != 0) IDWriteTextFormat_SetTextAlignment(textFormat, DWRITE_TEXT_ALIGNMENT_CENTER);
        if ((format & CANVAS_TEXT_FORMAT_HORIZONTAL_RIGHT) != 0) IDWriteTextFormat_SetTextAlignment(textFormat, DWRITE_TEXT_ALIGNMENT_TRAILING);
        if ((format & CANVAS_TEXT_FORMAT_VERTICAL_CENTER) != 0) IDWriteTextFormat_SetParagraphAlignment(textFormat, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        if ((format & CANVAS_TEXT_FORMAT_VERTICAL_BOTTOM) != 0) IDWriteTextFormat_SetParagraphAlignment(textFormat, DWRITE_PARAGRAPH_ALIGNMENT_FAR);
        if ((format & CANVAS_TEXT_FORMAT_WRAP) == 0) IDWriteTextFormat_SetWordWrapping(textFormat, DWRITE_WORD_WRAPPING_NO_WRAP);

        IDWriteTextLayout *textLayout;
        IDWriteFactory_CreateTextLayout(canvas->d2d.dwriteFactory, text, length, textFormat, rect->width == 0 ? INT32_MAX : rect->width,
            rect->height == 0 ? INT32_MAX : rect->height, &textLayout);
        if (font->underline) IDWriteTextLayout_SetUnderline(textLayout, true, ((DWRITE_TEXT_RANGE){ 0, length}));
        if (font->lineThrough) IDWriteTextLayout_SetStrikethrough(textLayout, true, ((DWRITE_TEXT_RANGE){ 0, length}));
        if (rect->width == 0 || rect->height == 0) {
            DWRITE_TEXT_METRICS metrics;
            IDWriteTextLayout_GetMetrics(textLayout, &metrics);
            if (rect->width == 0) rect->width = metrics.width;
            if (rect->height == 0) rect->height = metrics.height;
        }

        ID2D1HwndRenderTarget_DrawTextLayout(canvas->d2d.renderTarget, ((D2D1_POINT_2F){ rect->x, rect->y }),
            textLayout, (ID2D1Brush *)brush, D2D1_DRAW_TEXT_OPTIONS_CLIP | D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);

        IDWriteTextLayout_Release(textLayout);
        IDWriteTextFormat_Release(textFormat);
        ID2D1SolidColorBrush_Release(brush);
    }
}

#endif

#ifdef CANVAS_ENABLE_PATH

float _Canvas_ParsePathFloat(char **string) {
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

void Canvas_FillPath(Canvas *canvas, char *path, int32_t viewportWidth, int32_t viewportHeight, CanvasRect *rect, CanvasColor color) {
    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        CanvasRect realRect = { DP2PX(rect->x), DP2PX(rect->y), DP2PX(rect->width), DP2PX(rect->height) };

        int32_t smoothScale = 2; // Experimental smoothing feature gives dark border artifacts
        StretchBlt(canvas->gdi.alphaHdc, 0, 0, realRect.width * smoothScale, realRect.height * smoothScale,
            canvas->gdi.bufferHdc, realRect.x, realRect.y, realRect.width, realRect.height, SRCCOPY);
        BeginPath(canvas->gdi.alphaHdc);
        float x = 0, y = 0;
        float scaleX = (float)(realRect.width * smoothScale) / viewportWidth;
        float scaleY = (float)(realRect.height * smoothScale) / viewportHeight;
        char *c = path;
        while (*c != '\0') {
            if (*c == 'M') {
                c++;
                x = _Canvas_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y = _Canvas_ParsePathFloat(&c);
                MoveToEx(canvas->gdi.alphaHdc, x * scaleX, y * scaleY, NULL);
            } else if (*c == 'm') {
                c++;
                x += _Canvas_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y += _Canvas_ParsePathFloat(&c);
                MoveToEx(canvas->gdi.alphaHdc, x * scaleX, y * scaleY, NULL);
            } else if (*c == 'L') {
                c++;
                x = _Canvas_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y = _Canvas_ParsePathFloat(&c);
                LineTo(canvas->gdi.alphaHdc, x * scaleX, y * scaleY);
            } else if (*c == 'l') {
                c++;
                x += _Canvas_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y += _Canvas_ParsePathFloat(&c);
                LineTo(canvas->gdi.alphaHdc, x * scaleX, y * scaleY);
            } else if (*c == 'H') {
                c++;
                x = _Canvas_ParsePathFloat(&c);
                LineTo(canvas->gdi.alphaHdc, x * scaleX, y * scaleY);
            } else if (*c == 'h') {
                c++;
                x += _Canvas_ParsePathFloat(&c);
                LineTo(canvas->gdi.alphaHdc, x * scaleX, y * scaleY);
            } else if (*c == 'V') {
                c++;
                y = _Canvas_ParsePathFloat(&c);
                LineTo(canvas->gdi.alphaHdc, x * scaleX, y * scaleY);
            } else if (*c == 'v') {
                c++;
                y += _Canvas_ParsePathFloat(&c);
                LineTo(canvas->gdi.alphaHdc, x * scaleX, y * scaleY);
            } else if (*c == 'Z' || *c == 'z') {
                c++;
            }
        }
        EndPath(canvas->gdi.alphaHdc);
        HBRUSH brush = CreateSolidBrush(color & 0x00ffffff);
        SelectObject(canvas->gdi.alphaHdc, brush);
        FillPath(canvas->gdi.alphaHdc);
        DeleteObject(brush);
        if ((color >> 24) == 0xff) {
            StretchBlt(canvas->gdi.bufferHdc, realRect.x, realRect.y, realRect.width, realRect.height,
                canvas->gdi.alphaHdc, 0, 0, realRect.width * smoothScale, realRect.height * smoothScale, SRCCOPY);
        } else {
            GdiAlphaBlend(canvas->gdi.bufferHdc, realRect.x, realRect.y, realRect.width, realRect.height,
                canvas->gdi.alphaHdc, 0, 0, realRect.width * smoothScale, realRect.height * smoothScale, (BLENDFUNCTION){ AC_SRC_OVER, 0, color >> 24, 0 });
        }
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1PathGeometry *pathGeometry;
        ID2D1Factory_CreatePathGeometry(canvas->d2d.d2dFactory, &pathGeometry);

        ID2D1GeometrySink *sink;
        ID2D1PathGeometry_Open(pathGeometry, &sink);
        ID2D1GeometrySink_SetFillMode(sink, D2D1_FILL_MODE_WINDING);

        float x = 0, y = 0;
        float scaleX = (float)rect->width / viewportWidth;
        float scaleY = (float)rect->height / viewportHeight;
        bool figureOpen = false;
        char *c = path;
        while (*c != '\0') {
            if (*c == 'M') {
                c++;
                x = _Canvas_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y = _Canvas_ParsePathFloat(&c);
                if (figureOpen) {
                    ID2D1GeometrySink_EndFigure(sink, D2D1_FIGURE_END_CLOSED);
                }
                ID2D1GeometrySink_BeginFigure(sink, ((D2D1_POINT_2F){ rect->x + x * scaleX, rect->y + y * scaleY }), D2D1_FIGURE_BEGIN_FILLED);
                figureOpen = true;
            } else if (*c == 'm') {
                c++;
                x += _Canvas_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y += _Canvas_ParsePathFloat(&c);
                if (figureOpen) {
                    ID2D1GeometrySink_EndFigure(sink, D2D1_FIGURE_END_CLOSED);
                }
                ID2D1GeometrySink_BeginFigure(sink, ((D2D1_POINT_2F){ rect->x + x * scaleX, rect->y + y * scaleY }), D2D1_FIGURE_BEGIN_FILLED);
                figureOpen = true;
            } else if (*c == 'L') {
                c++;
                x = _Canvas_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y = _Canvas_ParsePathFloat(&c);
                ID2D1GeometrySink_AddLine(sink, ((D2D1_POINT_2F){ rect->x + x * scaleX, rect->y + y * scaleY }));
            } else if (*c == 'l') {
                c++;
                x += _Canvas_ParsePathFloat(&c);
                if (*c == ',' || *c == ' ') c++;
                y += _Canvas_ParsePathFloat(&c);
                ID2D1GeometrySink_AddLine(sink, ((D2D1_POINT_2F){ rect->x + x * scaleX, rect->y + y * scaleY }));
            } else if (*c == 'H') {
                c++;
                x = _Canvas_ParsePathFloat(&c);
                ID2D1GeometrySink_AddLine(sink, ((D2D1_POINT_2F){ rect->x + x * scaleX, rect->y + y * scaleY }));
            } else if (*c == 'h') {
                c++;
                x += _Canvas_ParsePathFloat(&c);
                ID2D1GeometrySink_AddLine(sink, ((D2D1_POINT_2F){ rect->x + x * scaleX, rect->y + y * scaleY }));
            } else if (*c == 'V') {
                c++;
                y = _Canvas_ParsePathFloat(&c);
                ID2D1GeometrySink_AddLine(sink, ((D2D1_POINT_2F){ rect->x + x * scaleX, rect->y + y * scaleY }));
            } else if (*c == 'v') {
                c++;
                y += _Canvas_ParsePathFloat(&c);
                ID2D1GeometrySink_AddLine(sink, ((D2D1_POINT_2F){ rect->x + x * scaleX, rect->y + y * scaleY }));
            } else if (*c == 'Z' || *c == 'z') {
                c++;
                if (figureOpen) {
                    ID2D1GeometrySink_EndFigure(sink, D2D1_FIGURE_END_CLOSED);
                    figureOpen = false;
                }
            }
        }
        if (figureOpen) {
            ID2D1GeometrySink_EndFigure(sink, D2D1_FIGURE_END_CLOSED);
        }
        ID2D1GeometrySink_Close(sink);
        ID2D1GeometrySink_Release(sink);

        ID2D1SolidColorBrush *brush;
        ID2D1HwndRenderTarget_CreateSolidColorBrush(canvas->d2d.renderTarget, (&(D2D1_COLOR_F){
            (float)(color & 0xff) / 255, (float)((color >> 8) & 0xff) / 255,
            (float)((color >> 16) & 0xff) / 255, (float)((color >> 24) & 0xff) / 255
        }), NULL, &brush);
        ID2D1HwndRenderTarget_FillGeometry(canvas->d2d.renderTarget, (ID2D1Geometry *)pathGeometry, (ID2D1Brush *)brush, NULL);
        ID2D1SolidColorBrush_Release(brush);
        ID2D1PathGeometry_Release(pathGeometry);
    }
}

#endif

#undef DP2PX
#undef PX2DP

#endif
