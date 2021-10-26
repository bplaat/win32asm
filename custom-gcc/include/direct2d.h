#ifndef DIRECT2D_H
#define DIRECT2D_H

#include "win32.h"
#include "directwrite.h"

#define DXGI_FORMAT_UNKNOWN 0

#define D2D1_ALPHA_MODE_UNKNOWN 0

#define D2D1_RENDER_TARGET_TYPE_DEFAULT 0

#define D2D1_RENDER_TARGET_USAGE_NONE 0

#define D2D1_FEATURE_LEVEL_DEFAULT 0

#define D2D1_PRESENT_OPTIONS_NONE 0

#define D2D1_FILL_MODE_WINDING 1

#define D2D1_FIGURE_BEGIN_FILLED 0

#define D2D1_FIGURE_END_CLOSED 1

#define D2D1_DRAW_TEXT_OPTIONS_NONE 0
#define D2D1_DRAW_TEXT_OPTIONS_CLIP 2
#define D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT 4

#define D2D1_ANTIALIAS_MODE_PER_PRIMITIVE 0

#define D2D1_FACTORY_TYPE_SINGLE_THREADED 0

#define D2D1_LAYER_OPTIONS_NONE 0

typedef struct D2D1_COLOR_F {
    float r;
    float g;
    float b;
    float a;
} D2D1_COLOR_F;

typedef struct D2D1_POINT_2F {
    float x;
    float y;
} D2D1_POINT_2F;

typedef struct D2D1_SIZE_U {
    uint32_t width;
    uint32_t height;
} D2D1_SIZE_U;

typedef struct D2D1_SIZE_F {
    float width;
    float height;
} D2D1_SIZE_F;

typedef struct D2D1_RECT_F {
    float left;
    float top;
    float right;
    float bottom;
} D2D1_RECT_F;

typedef struct D2D1_MATRIX_3X2_F {
    union {
        struct {
            float m11;
            float m12;
            float m21;
            float m22;
            float dx;
            float dy;
        };
        struct {
            float _11;
            float _12;
            float _21;
            float _22;
            float _31;
            float _32;
        };
        float m[3][2];
    };
} D2D1_MATRIX_3X2_F;

typedef struct D2D1_PIXEL_FORMAT {
  uint32_t format;
  uint32_t alphaMode;
} D2D1_PIXEL_FORMAT;

typedef struct D2D1_RENDER_TARGET_PROPERTIES {
  uint32_t type;
  D2D1_PIXEL_FORMAT pixelFormat;
  float dpiX;
  float dpiY;
  uint32_t usage;
  uint32_t minLevel;
} D2D1_RENDER_TARGET_PROPERTIES;

typedef struct D2D1_HWND_RENDER_TARGET_PROPERTIES {
  HWND hwnd;
  D2D1_SIZE_U pixelSize;
  uint32_t presentOptions;
} D2D1_HWND_RENDER_TARGET_PROPERTIES;

// ID2D1Factory (forward define)
typedef struct ID2D1Factory ID2D1Factory;

// ID2D1Resource
typedef struct ID2D1Resource ID2D1Resource;

typedef struct ID2D1ResourceVtbl {
    IUnknownVtbl base;
    void __stdcall (*GetFactory)(ID2D1Resource *this, ID2D1Factory **factory);
} ID2D1ResourceVtbl;

struct ID2D1Resource {
    const ID2D1ResourceVtbl *lpVtbl;
};

// ID2D1Image
typedef struct ID2D1Image ID2D1Image;

typedef struct ID2D1ImageVtbl {
    ID2D1ResourceVtbl base;
} ID2D1ImageVtbl;

struct ID2D1Image {
    const ID2D1ImageVtbl *lpVtbl;
};

// ID2D1Bitmap
typedef struct ID2D1Bitmap ID2D1Bitmap;

typedef struct ID2D1BitmapVtbl {
    ID2D1ImageVtbl base;
    uint8_t padding1[7 * sizeof(void *)];
} ID2D1BitmapVtbl;

struct ID2D1Bitmap {
    const ID2D1BitmapVtbl *lpVtbl;
};

// ID2D1Layer
typedef struct ID2D1Layer ID2D1Layer;

typedef struct ID2D1LayerVtbl {
    ID2D1ResourceVtbl base;
    uint8_t padding1[1 * sizeof(void *)];
} ID2D1LayerVtbl;

struct ID2D1Layer {
    const ID2D1LayerVtbl *lpVtbl;
};

// ID2D1SimplifiedGeometrySink
typedef struct ID2D1SimplifiedGeometrySink ID2D1SimplifiedGeometrySink;

typedef struct ID2D1SimplifiedGeometrySinkVtbl {
    IUnknownVtbl base;

    void __stdcall (*SetFillMode)(ID2D1SimplifiedGeometrySink *this, uint32_t fillMode);
    uint8_t padding1[1 * sizeof(void *)];

    void __stdcall (*BeginFigure)(ID2D1SimplifiedGeometrySink *this, D2D1_POINT_2F startPoint, uint32_t figureBegin);
    uint8_t padding2[2 * sizeof(void *)];

    void __stdcall (*EndFigure)(ID2D1SimplifiedGeometrySink *this, uint32_t figureEnd);
    int32_t __stdcall (*Close)(ID2D1SimplifiedGeometrySink *this);
} ID2D1SimplifiedGeometrySinkVtbl;

#define ID2D1SimplifiedGeometrySink_SetFillMode(this, fillMode) ((ID2D1SimplifiedGeometrySink *)this)->lpVtbl->SetFillMode((ID2D1SimplifiedGeometrySink *)this, fillMode)
#define ID2D1SimplifiedGeometrySink_BeginFigure(this, startPoint, figureBegin) ((ID2D1SimplifiedGeometrySink *)this)->lpVtbl->BeginFigure((ID2D1SimplifiedGeometrySink *)this, startPoint, figureBegin)
#define ID2D1SimplifiedGeometrySink_EndFigure(this, figureEnd) ((ID2D1SimplifiedGeometrySink *)this)->lpVtbl->EndFigure((ID2D1SimplifiedGeometrySink *)this, figureEnd)
#define ID2D1SimplifiedGeometrySink_Close(this) ((ID2D1SimplifiedGeometrySink *)this)->lpVtbl->Close((ID2D1SimplifiedGeometrySink *)this)

struct ID2D1SimplifiedGeometrySink {
    const ID2D1SimplifiedGeometrySinkVtbl *lpVtbl;
};

// ID2D1GeometrySink
typedef struct ID2D1GeometrySink ID2D1GeometrySink;

typedef struct ID2D1GeometrySinkVtbl {
    ID2D1SimplifiedGeometrySinkVtbl base;

    void __stdcall (*AddLine)(ID2D1GeometrySink *this, D2D1_POINT_2F point);
    uint8_t padding1[4 * sizeof(void *)];
} ID2D1GeometrySinkVtbl;

struct ID2D1GeometrySink {
    const ID2D1GeometrySinkVtbl *lpVtbl;
};

#define ID2D1GeometrySink_AddLine(this, point) ((ID2D1GeometrySink *)this)->lpVtbl->AddLine((ID2D1GeometrySink *)this, point)

// ID2D1Geometry
typedef struct ID2D1Geometry ID2D1Geometry;

typedef struct ID2D1GeometryVtbl {
    ID2D1ResourceVtbl base;
    uint8_t padding1[13 * sizeof(void *)];
} ID2D1GeometryVtbl;

struct ID2D1Geometry {
    const ID2D1GeometryVtbl *lpVtbl;
};

// ID2D1PathGeometry
typedef struct ID2D1PathGeometry ID2D1PathGeometry;

typedef struct ID2D1PathGeometryVtbl {
    ID2D1GeometryVtbl base;

    int32_t __stdcall (*Open)(ID2D1PathGeometry *this, ID2D1GeometrySink **geometrySink);
    uint8_t padding1[3 * sizeof(void *)];
} ID2D1PathGeometryVtbl;

#define ID2D1PathGeometry_Open(this, geometrySink) ((ID2D1PathGeometry *)this)->lpVtbl->Open((ID2D1PathGeometry *)this, geometrySink)

struct ID2D1PathGeometry {
    const ID2D1PathGeometryVtbl *lpVtbl;
};

// ID2D1Brush
typedef struct ID2D1Brush ID2D1Brush;

// ID2D1SolidColorBrush
typedef ID2D1Brush ID2D1SolidColorBrush;

// ID2D1BitmapRenderTarget (forward define)
typedef struct ID2D1BitmapRenderTarget ID2D1BitmapRenderTarget;

// ID2D1RenderTarget
typedef struct ID2D1RenderTarget ID2D1RenderTarget;

typedef struct D2D1_LAYER_PARAMETERS {
    D2D1_RECT_F contentBounds;
    ID2D1Geometry *geometricMask;
    uint32_t maskAntialiasMode;
    D2D1_MATRIX_3X2_F maskTransform;
    float opacity;
    ID2D1Brush *opacityBrush;
    uint32_t layerOptions;
} D2D1_LAYER_PARAMETERS;

typedef struct ID2D1RenderTargetVtbl {
    ID2D1ResourceVtbl base;
    uint8_t padding1[4 * sizeof(void *)];

    int32_t __stdcall (*CreateSolidColorBrush)(ID2D1RenderTarget *this, const D2D1_COLOR_F *color, const void *brushProperties, ID2D1SolidColorBrush **solidColorBrush);
    uint8_t padding2[2 * sizeof(void *)];

    int32_t __stdcall (*CreateCompatibleRenderTarget)(const D2D1_SIZE_F *desiredSize, const D2D1_SIZE_U *desiredPixelSize, const D2D1_PIXEL_FORMAT *desiredFormat, uint32_t options, ID2D1BitmapRenderTarget **bitmapRenderTarget);
    uint8_t padding3[1 * sizeof(void *)];

    int32_t __stdcall (*CreateLayer)(ID2D1RenderTarget *this, const D2D1_SIZE_F *size, ID2D1Layer **layer);
    uint8_t padding4[1 * sizeof(void *)];

    void __stdcall (*DrawLine)(ID2D1RenderTarget *this, D2D1_POINT_2F point0, D2D1_POINT_2F point1, ID2D1Brush *brush, float strokeWidth, void *strokeStyle);
    void __stdcall (*DrawRectangle)(ID2D1RenderTarget *this, const D2D1_RECT_F *rect, ID2D1Brush *brush, float strokeWidth, void *strokeStyle);
    void __stdcall (*FillRectangle)(ID2D1RenderTarget *this, const D2D1_RECT_F *rect, ID2D1Brush *brush);
    uint8_t padding5[5 * sizeof(void *)];

    void __stdcall (*FillGeometry)(ID2D1RenderTarget *this, ID2D1Geometry *geometry, ID2D1Brush *brush, ID2D1Brush *opacityBrush);
    uint8_t padding6[2 * sizeof(void *)];

    void __stdcall (*DrawBitmap)(ID2D1RenderTarget *this, ID2D1Bitmap *bitmap, const D2D1_RECT_F *destinationRectangle, float opacity, uint32_t interpolationMode, const D2D1_RECT_F *sourceRectangle);
    void __stdcall (*DrawText)(ID2D1RenderTarget *this, const wchar_t *string, uint32_t stringLength, IDWriteTextFormat *textFormat, const D2D1_RECT_F *layoutRect, ID2D1Brush *defaultForegroundBrush, uint32_t options, uint32_t measuringMode);
    void __stdcall (*DrawTextLayout)(ID2D1RenderTarget *this, D2D1_POINT_2F origin, IDWriteTextLayout *textLayout, ID2D1Brush *defaultForegroundBrush, uint32_t options);
    uint8_t padding7[1 * sizeof(void *)];

    void __stdcall (*SetTransform)(ID2D1RenderTarget *this, const D2D1_MATRIX_3X2_F *transform);
    uint8_t padding8[9 * sizeof(void *)];

    void __stdcall (*PushLayer)(ID2D1RenderTarget *this, const D2D1_LAYER_PARAMETERS *layerParameters, ID2D1Layer *layer);
    void __stdcall (*PopLayer)(ID2D1RenderTarget *this);
    int32_t __stdcall (*Flush)(ID2D1RenderTarget *this, void *tag1, void *tag2);
    uint8_t padding9[2 * sizeof(void *)];

    void __stdcall (*PushAxisAlignedClip)(ID2D1RenderTarget *this, const D2D1_RECT_F *clip_rect, uint32_t antialias_mode);
    void __stdcall (*PopAxisAlignedClip)(ID2D1RenderTarget *this);
    void __stdcall (*Clear)(ID2D1RenderTarget *this, const D2D1_COLOR_F *clearColor);
    void __stdcall (*BeginDraw)(ID2D1RenderTarget *this);
    int32_t __stdcall (*EndDraw)(ID2D1RenderTarget *this, void *tag1, void *tag2);
    uint8_t padding10[1 * sizeof(void *)];

    void __stdcall (*SetDpi)(ID2D1RenderTarget *this, float dpiX, float dpiY);
    uint8_t padding11[5 * sizeof(void *)];
} ID2D1RenderTargetVtbl;

#define ID2D1RenderTarget_CreateSolidColorBrush(this, color, brushProperties, solidColorBrush) \
    ((ID2D1RenderTarget *)this)->lpVtbl->CreateSolidColorBrush((ID2D1RenderTarget *)this, color, brushProperties, solidColorBrush)
#define ID2D1RenderTarget_CreateCompatibleRenderTarget(this, desiredSize, desiredPixelSize, desiredFormat, options, bitmapRenderTarget) \
    ((ID2D1RenderTarget *)this)->lpVtbl->CreateCompatibleRenderTarget((ID2D1RenderTarget *)this, desiredSize, desiredPixelSize, desiredFormat, options, bitmapRenderTarget)
#define ID2D1RenderTarget_CreateLayer(this, size, layer) ((ID2D1RenderTarget *)this)->lpVtbl->CreateLayer((ID2D1RenderTarget *)this, size, layer)
#define ID2D1RenderTarget_DrawLine(this, point0, point1, brush, strokeWidth, strokeStyle) \
    ((ID2D1RenderTarget *)this)->lpVtbl->DrawLine((ID2D1RenderTarget *)this,  point0, point1, brush, strokeWidth, strokeStyle)
#define ID2D1RenderTarget_DrawRectangle(this, rect, brush, strokeWidth, strokeStyle) \
    ((ID2D1RenderTarget *)this)->lpVtbl->DrawRectangle((ID2D1RenderTarget *)this, rect, brush, strokeWidth, strokeStyle)
#define ID2D1RenderTarget_FillRectangle(this, rect, brush) \
    ((ID2D1RenderTarget *)this)->lpVtbl->FillRectangle((ID2D1RenderTarget *)this, rect, brush)
#define ID2D1RenderTarget_FillGeometry(this, geometry, brush, opacityBrush) \
    ((ID2D1RenderTarget *)this)->lpVtbl->FillGeometry((ID2D1RenderTarget *)this, geometry, brush, opacityBrush)
#define ID2D1RenderTarget_DrawBitmap(this, bitmap, destinationRectangle, opacity, interpolationMode, sourceRectangle) \
    ((ID2D1RenderTarget *)this)->lpVtbl->DrawBitmap((ID2D1RenderTarget *)this, bitmap, destinationRectangle, opacity, interpolationMode, sourceRectangle)
#define ID2D1RenderTarget_DrawText(this, string, stringLength, textFormat, layoutRect, defaultForegroundBrush, options, measuringMode) \
    ((ID2D1RenderTarget *)this)->lpVtbl->DrawText((ID2D1RenderTarget *)this, string, stringLength, textFormat, layoutRect, defaultForegroundBrush, options, measuringMode)
#define ID2D1RenderTarget_DrawTextLayout(this, origin, textLayout, defaultForegroundBrush, options) \
    ((ID2D1RenderTarget *)this)->lpVtbl->DrawTextLayout((ID2D1RenderTarget *)this, origin, textLayout, defaultForegroundBrush, options)
#define ID2D1RenderTarget_SetTransform(this, transform) ((ID2D1RenderTarget *)this)->lpVtbl->SetTransform((ID2D1RenderTarget *)this, transform)
#define ID2D1RenderTarget_PushLayer(this, layerParameters, layer) ((ID2D1RenderTarget *)this)->lpVtbl->PushLayer((ID2D1RenderTarget *)this, layerParameters, layer)
#define ID2D1RenderTarget_PopLayer(this) ((ID2D1RenderTarget *)this)->lpVtbl->PopLayer((ID2D1RenderTarget *)this)
#define ID2D1RenderTarget_Flush(this, tag1, tag2) ((ID2D1RenderTarget *)this)->lpVtbl->Flush((ID2D1RenderTarget *)this, tag1, tag2)
#define ID2D1RenderTarget_PushAxisAlignedClip(this, clip_rect, antialias_mode) ((ID2D1RenderTarget *)this)->lpVtbl->PushAxisAlignedClip((ID2D1RenderTarget *)this, clip_rect, antialias_mode)
#define ID2D1RenderTarget_PopAxisAlignedClip(this) ((ID2D1RenderTarget *)this)->lpVtbl->PopAxisAlignedClip((ID2D1RenderTarget *)this)
#define ID2D1RenderTarget_Clear(this, clearColor) ((ID2D1RenderTarget *)this)->lpVtbl->Clear((ID2D1RenderTarget *)this, clearColor)
#define ID2D1RenderTarget_BeginDraw(this) ((ID2D1RenderTarget *)this)->lpVtbl->BeginDraw((ID2D1RenderTarget *)this)
#define ID2D1RenderTarget_EndDraw(this, tag1, tag2) ((ID2D1RenderTarget *)this)->lpVtbl->EndDraw((ID2D1RenderTarget *)this, tag1, tag2)
#define ID2D1RenderTarget_SetDpi(this, dpiX, dpiY) ((ID2D1RenderTarget *)this)->lpVtbl->SetDpi((ID2D1RenderTarget *)this, dpiX, dpiX)

struct ID2D1RenderTarget {
    const ID2D1RenderTargetVtbl *lpVtbl;
};

// ID2D1HwndRenderTarget
typedef struct ID2D1HwndRenderTarget ID2D1HwndRenderTarget;

typedef struct ID2D1HwndRenderTargetVtbl {
    ID2D1RenderTargetVtbl base;
    uint8_t padding1[1 * sizeof(void *)];

    int32_t __stdcall (*Resize)(ID2D1HwndRenderTarget *this, const D2D1_SIZE_U *pixelSize);
    uint8_t padding2[1 * sizeof(void *)];
} ID2D1HwndRenderTargetVtbl;

#define ID2D1HwndRenderTarget_Resize(this, pixelSize) ((ID2D1HwndRenderTarget *)this)->lpVtbl->Resize((ID2D1HwndRenderTarget *)this, pixelSize)

struct ID2D1HwndRenderTarget {
    const ID2D1HwndRenderTargetVtbl *lpVtbl;
};

// ID2D1BitmapRenderTarget
typedef struct ID2D1BitmapRenderTargetVtbl {
    ID2D1RenderTargetVtbl base;
    int32_t __stdcall (*GetBitmap)(ID2D1BitmapRenderTarget *this, ID2D1Bitmap **bitmap);
} ID2D1BitmapRenderTargetVtbl;

#define ID2D1BitmapRenderTarget_GetBitmap(this, bitmap) ((ID2D1BitmapRenderTarget *)this)->lpVtbl->GetBitmap((ID2D1BitmapRenderTarget *)this, bitmap)

struct ID2D1BitmapRenderTarget {
    const ID2D1BitmapRenderTargetVtbl *lpVtbl;
};

// ID2D1Factory
typedef struct ID2D1FactoryVtbl {
    IUnknownVtbl base;

    uint8_t padding1[7 * sizeof(void *)];

    int32_t __stdcall (*CreatePathGeometry)(ID2D1Factory *this, ID2D1PathGeometry **pathGeometry);
    uint8_t padding2[3 * sizeof(void *)];

    int32_t __stdcall (*CreateHwndRenderTarget)(ID2D1Factory *this, const D2D1_RENDER_TARGET_PROPERTIES *renderTargetProperties, const D2D1_HWND_RENDER_TARGET_PROPERTIES *hwndRenderTargetProperties, ID2D1HwndRenderTarget **hwndRenderTarget);
    uint8_t padding3[2 * sizeof(void *)];
} ID2D1FactoryVtbl;

#define ID2D1Factory_CreatePathGeometry(this, pathGeometry) \
    ((ID2D1Factory *)this)->lpVtbl->CreatePathGeometry((ID2D1Factory *)this, pathGeometry)
#define ID2D1Factory_CreateHwndRenderTarget(this, renderTargetProperties, hwndRenderTargetProperties, hwndRenderTarget) \
    ((ID2D1Factory *)this)->lpVtbl->CreateHwndRenderTarget((ID2D1Factory *)this, renderTargetProperties, hwndRenderTargetProperties, hwndRenderTarget)

struct ID2D1Factory {
    const ID2D1FactoryVtbl *lpVtbl;
};

// D2D1CreateFactory
extern int32_t __stdcall D2D1CreateFactory(uint32_t factoryType, const IID *riid, const void *pFactoryOptions, ID2D1Factory **ppIFactory);

#endif
