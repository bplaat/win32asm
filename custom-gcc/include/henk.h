#ifndef HENK_H
#define HENK_H

#include "win32.h"
#include "direct2d.h"

#define HENK_RGB(r, g, b) ((r & 0xff) | ((g & 0xff) << 8) | ((b & 0xff) << 16) | (0xff << 24))
#define HENK_RGBA(r, g, b, a) ((r & 0xff) | ((g & 0xff) << 8) | ((b & 0xff) << 16) | ((a & 0xff) << 24))
#define HENK_HEX(x) (((x >> 16) & 0xff) | (((x >> 8) & 0xff) << 8) | ((x & 0xff) << 16) | (0xff << 24))
#define HENK_HEXA(x) (((x >> 24) & 0xff) | (((x >> 16) & 0xff) << 8) | (((x >> 8) & 0xff) << 16) | ((x & 0xff) << 24))

typedef struct HenkRect {
    float x;
    float y;
    float width;
    float height;
} HenkRect;

typedef struct HenkFont {
    wchar_t *name;
    float size;
} HenkFont;

typedef enum HenkRenderer {
    HENK_RENDERER_DEFAULT,
    HENK_RENDERER_GDI,
    HENK_RENDERER_DIRECT2D
} HenkRenderer;

typedef struct Henk {
    HenkRenderer renderer;
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
} Henk;

Henk *Henk_New(HWND hwnd, HenkRenderer renderer);

typedef int32_t (__stdcall *_D2D1CreateFactory)(uint32_t factoryType, GUID *riid, const void *pFactoryOptions, ID2D1Factory **ppIFactory);

typedef int32_t (__stdcall *_DWriteCreateFactory)(uint32_t factoryType, GUID *riid, IDWriteFactory **factory);

void Henk_Init(Henk *henk, HWND hwnd, HenkRenderer renderer);

void Henk_Free(Henk *henk);

void Henk_Resize(Henk *henk, int32_t width, int32_t height);

void Henk_BeginDraw(Henk *henk);

void Henk_EndDraw(Henk *henk);

void Henk_StrokeRect(Henk *henk, HenkRect *rect, uint32_t color, float stroke_width);

void Henk_FillRect(Henk *henk, HenkRect *rect, uint32_t color);

void Henk_DrawText(Henk *henk, wchar_t *text, int32_t length, HenkRect *rect, HenkFont *font, uint32_t align, uint32_t color);

float Henk_ParsePathFloat(char **string);

void Henk_FillPath(Henk *henk, HenkRect *rect, int32_t viewport_width, int32_t viewport_height, char *path, uint32_t color);

#endif
