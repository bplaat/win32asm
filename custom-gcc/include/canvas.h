#ifndef CANVAS_H
#define CANVAS_H

#include "win32.h"
#include "direct2d.h"

typedef enum CanvasRenderer {
    CANVAS_RENDERER_DEFAULT,
    CANVAS_RENDERER_GDI,
    CANVAS_RENDERER_DIRECT2D
} CanvasRenderer;

#define CANVAS_RGB(r, g, b) ((r & 0xff) | ((g & 0xff) << 8) | ((b & 0xff) << 16) | (0xff << 24))
#define CANVAS_RGBA(r, g, b, a) ((r & 0xff) | ((g & 0xff) << 8) | ((b & 0xff) << 16) | ((a & 0xff) << 24))
#define CANVAS_HEX(x) (((x >> 16) & 0xff) | (((x >> 8) & 0xff) << 8) | ((x & 0xff) << 16) | (0xff << 24))
#define CANVAS_HEXA(x) (((x >> 24) & 0xff) | (((x >> 16) & 0xff) << 8) | (((x >> 8) & 0xff) << 16) | ((x & 0xff) << 24))

typedef uint32_t CanvasColor;

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

#define CANVAS_POINT_COLLIDE(a, b) (b.x >= a.x && b.y >= a.y && \
    b.x < a.x + a.width && b.y < a.y + a.height)

#define CANVAS_RECT_COLLIDE(a, b) (a.x < b.x + b.width && \
    a.x + a.width > b.x && \
    a.y < b.y + b.height && \
    a.y + a.height > b.y)

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
    bool line_through;
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

typedef struct Canvas {
    CanvasRenderer renderer;
    HWND hwnd;
    int32_t width;
    int32_t height;

    union {
        struct {
            HDC hdc;
            HDC buffer_hdc;
            HBITMAP buffer_bitmap;
            HDC alpha_hdc;
            HBITMAP alpha_bitmap;
        } gdi;

        struct {
            ID2D1Factory *d2d_factory;
            IDWriteFactory *dwrite_factory;
            ID2D1HwndRenderTarget *render_target;
            // ID2D1Layer *layer;
        } d2d;
    };
} Canvas;

typedef int32_t __stdcall (*_D2D1CreateFactory)(uint32_t factoryType, const IID *riid, const void *pFactoryOptions, ID2D1Factory **ppIFactory);

typedef int32_t __stdcall (*_DWriteCreateFactory)(uint32_t factoryType, const IID *riid, IDWriteFactory **factory);

Canvas *Canvas_New(HWND hwnd, CanvasRenderer renderer);

void Canvas_Free(Canvas *canvas);

void Canvas_Resize(Canvas *canvas, int32_t width, int32_t height);

void Canvas_BeginDraw(Canvas *canvas);

void Canvas_EndDraw(Canvas *canvas);

void Canvas_Transform(Canvas *canvas, CanvasTransform *transform);

void Canvas_Clip(Canvas *canvas, CanvasRect *rect);

void Canvas_FillRect(Canvas *canvas, CanvasRect *rect, CanvasColor color);

void Canvas_StrokeRect(Canvas *canvas, CanvasRect *rect, CanvasColor color, float stroke_width);

void Canvas_MeasureText(Canvas *canvas, wchar_t *text, int32_t length, CanvasRect *rect, CanvasFont *font, CanvasTextFormat format);

void Canvas_DrawText(Canvas *canvas, wchar_t *text, int32_t length, CanvasRect *rect, CanvasFont *font, CanvasTextFormat format, CanvasColor color);

float Canvas_ParsePathFloat(char **string);

void Canvas_FillPath(Canvas *canvas, CanvasRect *rect, int32_t viewport_width, int32_t viewport_height, char *path, CanvasColor color);

#endif
