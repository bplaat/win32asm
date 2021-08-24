#ifndef CANVAS_H
#define CANVAS_H

#include "win32.h"
#include "direct2d.h"

#define CANVAS_RGB(r, g, b) ((r & 0xff) | ((g & 0xff) << 8) | ((b & 0xff) << 16) | (0xff << 24))
#define CANVAS_RGBA(r, g, b, a) ((r & 0xff) | ((g & 0xff) << 8) | ((b & 0xff) << 16) | ((a & 0xff) << 24))
#define CANVAS_HEX(x) (((x >> 16) & 0xff) | (((x >> 8) & 0xff) << 8) | ((x & 0xff) << 16) | (0xff << 24))
#define CANVAS_HEXA(x) (((x >> 24) & 0xff) | (((x >> 16) & 0xff) << 8) | (((x >> 8) & 0xff) << 16) | ((x & 0xff) << 24))

typedef struct CanvasRect {
    float x;
    float y;
    float width;
    float height;
} CanvasRect;

typedef struct CanvasFont {
    wchar_t *name;
    float size;
} CanvasFont;

typedef enum CanvasRenderer {
    CANVAS_RENDERER_DEFAULT,
    CANVAS_RENDERER_GDI,
    CANVAS_RENDERER_DIRECT2D
} CanvasRenderer;

typedef enum CanvasAlign {
    CANVAS_ALIGN_TOP = 0,
    CANVAS_ALIGN_LEFT = 0,
    CANVAS_ALIGN_CENTER = 1,
    CANVAS_ALIGN_RIGHT = 2,
    CANVAS_ALIGN_VCENTER = 4,
    CANVAS_ALIGN_BOTTOM = 8
} CanvasAlign;

typedef struct Canvas {
    CanvasRenderer renderer;
    int32_t width;
    int32_t height;

    HDC hdc;
    HDC buffer_hdc;
    HBITMAP buffer_bitmap;
    HDC alpha_hdc;
    HBITMAP alpha_bitmap;

    ID2D1Factory *d2d_factory;
    IDWriteFactory *dwrite_factory;
    ID2D1HwndRenderTarget *render_target;
} Canvas;

typedef int32_t (__stdcall *_D2D1CreateFactory)(uint32_t factoryType, GUID *riid, const void *pFactoryOptions, ID2D1Factory **ppIFactory);

typedef int32_t (__stdcall *_DWriteCreateFactory)(uint32_t factoryType, GUID *riid, IDWriteFactory **factory);

Canvas *Canvas_New(HWND hwnd, CanvasRenderer renderer);

void Canvas_Free(Canvas *canvas);

void Canvas_Resize(Canvas *canvas, int32_t width, int32_t height);

void Canvas_BeginDraw(Canvas *canvas);

void Canvas_EndDraw(Canvas *canvas);

void Canvas_StrokeRect(Canvas *canvas, CanvasRect *rect, uint32_t color, float stroke_width);

void Canvas_FillRect(Canvas *canvas, CanvasRect *rect, uint32_t color);

void Canvas_DrawText(Canvas *canvas, wchar_t *text, int32_t length, CanvasRect *rect, CanvasFont *font, CanvasAlign align, uint32_t color);

float Canvas_ParsePathFloat(char **string);

void Canvas_FillPath(Canvas *canvas, CanvasRect *rect, int32_t viewport_width, int32_t viewport_height, char *path, uint32_t color);

#endif
