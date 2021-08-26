#ifndef DIRECT2D_H
#define DIRECT2D_H

#include "win32.h"
#include "directwrite.h"

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
    float m11;
    float m12;
    float m21;
    float m22;
    float dx;
    float dy;
} D2D1_MATRIX_3X2_F;

#define DXGI_FORMAT_UNKNOWN 0
#define D2D1_ALPHA_MODE_UNKNOWN 0

typedef struct D2D1_PIXEL_FORMAT {
  uint32_t format;
  uint32_t alphaMode;
} D2D1_PIXEL_FORMAT;

#define D2D1_RENDER_TARGET_TYPE_DEFAULT 0

#define D2D1_RENDER_TARGET_USAGE_NONE 0

#define D2D1_FEATURE_LEVEL_DEFAULT 0

typedef struct D2D1_RENDER_TARGET_PROPERTIES {
  uint32_t type;
  D2D1_PIXEL_FORMAT pixelFormat;
  float dpiX;
  float dpiY;
  uint32_t usage;
  uint32_t minLevel;
} D2D1_RENDER_TARGET_PROPERTIES;

#define D2D1_PRESENT_OPTIONS_NONE 0

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
    IUnknownVtbl Base;
    void (__stdcall *GetFactory)(ID2D1Resource *This, ID2D1Factory **factory);
} ID2D1ResourceVtbl;

struct ID2D1Resource {
    const ID2D1ResourceVtbl *lpVtbl;
};

// ID2D1Layer
typedef struct ID2D1Layer ID2D1Layer;

typedef struct ID2D1LayerVtbl {
    ID2D1ResourceVtbl Base;
    uint8_t padding1[1 * sizeof(void *)];
} ID2D1LayerVtbl;

struct ID2D1Layer {
    const ID2D1LayerVtbl *lpVtbl;
};

// ID2D1SimplifiedGeometrySink
#define D2D1_FILL_MODE_WINDING 1

#define D2D1_FIGURE_BEGIN_FILLED 0

#define D2D1_FIGURE_END_CLOSED 1

typedef struct ID2D1SimplifiedGeometrySink ID2D1SimplifiedGeometrySink;

typedef struct ID2D1SimplifiedGeometrySinkVtbl {
    IUnknownVtbl Base;

    void (__stdcall *SetFillMode)(ID2D1SimplifiedGeometrySink *This, uint32_t fillMode);

    uint8_t padding1[1 * sizeof(void *)];
    void (__stdcall *BeginFigure)(ID2D1SimplifiedGeometrySink *This, D2D1_POINT_2F startPoint, uint32_t figureBegin);

    uint8_t padding2[2 * sizeof(void *)];
    void (__stdcall *EndFigure)(ID2D1SimplifiedGeometrySink *This, uint32_t figureEnd);
    int32_t (__stdcall *Close)(ID2D1SimplifiedGeometrySink *This);
} ID2D1SimplifiedGeometrySinkVtbl;

#define ID2D1SimplifiedGeometrySink_SetFillMode(This, fillMode) ((ID2D1SimplifiedGeometrySink *)This)->lpVtbl->SetFillMode((ID2D1SimplifiedGeometrySink *)This, fillMode)
#define ID2D1SimplifiedGeometrySink_BeginFigure(This, startPoint, figureBegin) ((ID2D1SimplifiedGeometrySink *)This)->lpVtbl->BeginFigure((ID2D1SimplifiedGeometrySink *)This, startPoint, figureBegin)
#define ID2D1SimplifiedGeometrySink_EndFigure(This, figureEnd) ((ID2D1SimplifiedGeometrySink *)This)->lpVtbl->EndFigure((ID2D1SimplifiedGeometrySink *)This, figureEnd)
#define ID2D1SimplifiedGeometrySink_Close(This) ((ID2D1SimplifiedGeometrySink *)This)->lpVtbl->Close((ID2D1SimplifiedGeometrySink *)This)

struct ID2D1SimplifiedGeometrySink {
    const ID2D1SimplifiedGeometrySinkVtbl *lpVtbl;
};

// ID2D1GeometrySink
typedef struct ID2D1GeometrySink ID2D1GeometrySink;

typedef struct ID2D1GeometrySinkVtbl {
    ID2D1SimplifiedGeometrySinkVtbl Base;

    void (__stdcall *AddLine)(ID2D1GeometrySink *This, D2D1_POINT_2F point);
    uint8_t padding1[4 * sizeof(void *)];
} ID2D1GeometrySinkVtbl;

struct ID2D1GeometrySink {
    const ID2D1GeometrySinkVtbl *lpVtbl;
};

#define ID2D1GeometrySink_AddLine(This, point) ((ID2D1GeometrySink *)This)->lpVtbl->AddLine((ID2D1GeometrySink *)This, point)

// ID2D1Geometry
typedef struct ID2D1Geometry ID2D1Geometry;

typedef struct ID2D1GeometryVtbl {
    ID2D1ResourceVtbl Base;
    uint8_t padding1[13 * sizeof(void *)];
} ID2D1GeometryVtbl;

struct ID2D1Geometry {
    const ID2D1GeometryVtbl *lpVtbl;
};

// ID2D1PathGeometry
typedef struct ID2D1PathGeometry ID2D1PathGeometry;

typedef struct ID2D1PathGeometryVtbl {
    ID2D1GeometryVtbl Base;

    int32_t (__stdcall *Open)(ID2D1PathGeometry *This, ID2D1GeometrySink **geometrySink);
    uint8_t padding1[3 * sizeof(void *)];
} ID2D1PathGeometryVtbl;

#define ID2D1PathGeometry_Open(This, geometrySink) ((ID2D1PathGeometry *)This)->lpVtbl->Open((ID2D1PathGeometry *)This, geometrySink)

struct ID2D1PathGeometry {
    const ID2D1PathGeometryVtbl *lpVtbl;
};

// ID2D1RenderTarget
typedef struct ID2D1Brush ID2D1Brush;
typedef ID2D1Brush ID2D1SolidColorBrush;

typedef struct ID2D1RenderTarget ID2D1RenderTarget;

#define D2D1_DRAW_TEXT_OPTIONS_NONE 0
#define D2D1_DRAW_TEXT_OPTIONS_CLIP 2
#define D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT 4

#define DWRITE_MEASURING_MODE_NATURAL 0

#define D2D1_ANTIALIAS_MODE_PER_PRIMITIVE 0

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
    ID2D1ResourceVtbl Base;
    uint8_t padding1[4 * sizeof(void *)];

    int32_t (__stdcall *CreateSolidColorBrush)(ID2D1RenderTarget *This, const D2D1_COLOR_F *color, const void *brushProperties, ID2D1SolidColorBrush **solidColorBrush);
    uint8_t padding2[4 * sizeof(void *)];

    int32_t (__stdcall *CreateLayer)(ID2D1RenderTarget *This, const D2D1_SIZE_F *size, ID2D1Layer **layer);
    uint8_t padding3[1 * sizeof(void *)];

    void (__stdcall *DrawLine)(ID2D1RenderTarget *This, D2D1_POINT_2F point0, D2D1_POINT_2F point1, ID2D1Brush *brush, float strokeWidth, void *strokeStyle);
    void (__stdcall *DrawRectangle)(ID2D1RenderTarget *This, const D2D1_RECT_F *rect, ID2D1Brush *brush, float strokeWidth, void *strokeStyle);
    void (__stdcall *FillRectangle)(ID2D1RenderTarget *This, const D2D1_RECT_F *rect, ID2D1Brush *brush);
    uint8_t padding4[5 * sizeof(void *)];

    void (__stdcall *FillGeometry)(ID2D1RenderTarget *This, ID2D1Geometry *geometry, ID2D1Brush *brush, ID2D1Brush *opacityBrush);
    uint8_t padding5[3 * sizeof(void *)];

    void (__stdcall *DrawText)(ID2D1RenderTarget *This, const wchar_t *string, uint32_t stringLength, IDWriteTextFormat *textFormat, const D2D1_RECT_F *layoutRect, ID2D1Brush *defaultForegroundBrush, uint32_t options, uint32_t measuringMode);
    void (__stdcall *DrawTextLayout)(ID2D1RenderTarget *This, D2D1_POINT_2F origin, IDWriteTextLayout *textLayout, ID2D1Brush *defaultForegroundBrush, uint32_t options);
    uint8_t padding6[1 * sizeof(void *)];

    void (__stdcall *SetTransform)(ID2D1RenderTarget *This, const D2D1_MATRIX_3X2_F *transform);
    uint8_t padding7[9 * sizeof(void *)];

    void (__stdcall *PushLayer)(ID2D1RenderTarget *This, const D2D1_LAYER_PARAMETERS *layerParameters, ID2D1Layer *layer);
    void (__stdcall *PopLayer)(ID2D1RenderTarget *This);
    int32_t (__stdcall *Flush)(ID2D1RenderTarget *This, void *tag1, void *tag2);
    uint8_t padding8[2 * sizeof(void *)];

    void (__stdcall *PushAxisAlignedClip)(ID2D1RenderTarget *This, const D2D1_RECT_F *clip_rect, uint32_t antialias_mode);
    void (__stdcall *PopAxisAlignedClip)(ID2D1RenderTarget *This);
    void (__stdcall *Clear)(ID2D1RenderTarget *This, const D2D1_COLOR_F *clearColor);
    void (__stdcall *BeginDraw)(ID2D1RenderTarget *This);
    int32_t (__stdcall *EndDraw)(ID2D1RenderTarget *This, void *tag1, void *tag2);
    uint8_t padding9[7 * sizeof(void *)];
} ID2D1RenderTargetVtbl;

#define ID2D1RenderTarget_CreateSolidColorBrush(This, color, brushProperties, solidColorBrush) \
    ((ID2D1RenderTarget *)This)->lpVtbl->CreateSolidColorBrush((ID2D1RenderTarget *)This, color, brushProperties, solidColorBrush)
#define ID2D1RenderTarget_CreateLayer(This, size, layer) ((ID2D1RenderTarget *)This)->lpVtbl->CreateLayer((ID2D1RenderTarget *)This, size, layer)
#define ID2D1RenderTarget_DrawLine(This, point0, point1, brush, strokeWidth, strokeStyle) \
    ((ID2D1RenderTarget *)This)->lpVtbl->DrawLine((ID2D1RenderTarget *)This,  point0, point1, brush, strokeWidth, strokeStyle)
#define ID2D1RenderTarget_DrawRectangle(This, rect, brush, strokeWidth, strokeStyle) \
    ((ID2D1RenderTarget *)This)->lpVtbl->DrawRectangle((ID2D1RenderTarget *)This, rect, brush, strokeWidth, strokeStyle)
#define ID2D1RenderTarget_FillRectangle(This, rect, brush) \
    ((ID2D1RenderTarget *)This)->lpVtbl->FillRectangle((ID2D1RenderTarget *)This, rect, brush)
#define ID2D1RenderTarget_FillGeometry(This, geometry, brush, opacityBrush) \
    ((ID2D1RenderTarget *)This)->lpVtbl->FillGeometry((ID2D1RenderTarget *)This, geometry, brush, opacityBrush)
#define ID2D1RenderTarget_DrawText(This, string, stringLength, textFormat, layoutRect, defaultForegroundBrush, options, measuringMode) \
    ((ID2D1RenderTarget *)This)->lpVtbl->DrawText((ID2D1RenderTarget *)This, string, stringLength, textFormat, layoutRect, defaultForegroundBrush, options, measuringMode)
#define ID2D1RenderTarget_DrawTextLayout(This, origin, textLayout, defaultForegroundBrush, options) \
    ((ID2D1RenderTarget *)This)->lpVtbl->DrawTextLayout((ID2D1RenderTarget *)This, origin, textLayout, defaultForegroundBrush, options)
#define ID2D1RenderTarget_SetTransform(This, transform) ((ID2D1RenderTarget *)This)->lpVtbl->SetTransform((ID2D1RenderTarget *)This, transform)
#define ID2D1RenderTarget_PushLayer(This, layerParameters, layer) ((ID2D1RenderTarget *)This)->lpVtbl->PushLayer((ID2D1RenderTarget *)This, layerParameters, layer)
#define ID2D1RenderTarget_PopLayer(This) ((ID2D1RenderTarget *)This)->lpVtbl->PopLayer((ID2D1RenderTarget *)This)
#define ID2D1RenderTarget_Flush(This, tag1, tag2) ((ID2D1RenderTarget *)This)->lpVtbl->Flush((ID2D1RenderTarget *)This, tag1, tag2)
#define ID2D1RenderTarget_PushAxisAlignedClip(This, clip_rect, antialias_mode) ((ID2D1RenderTarget *)This)->lpVtbl->PushAxisAlignedClip((ID2D1RenderTarget *)This, clip_rect, antialias_mode)
#define ID2D1RenderTarget_PopAxisAlignedClip(This) ((ID2D1RenderTarget *)This)->lpVtbl->PopAxisAlignedClip((ID2D1RenderTarget *)This)
#define ID2D1RenderTarget_Clear(This, clearColor) ((ID2D1RenderTarget *)This)->lpVtbl->Clear((ID2D1RenderTarget *)This, clearColor)
#define ID2D1RenderTarget_BeginDraw(This) ((ID2D1RenderTarget *)This)->lpVtbl->BeginDraw((ID2D1RenderTarget *)This)
#define ID2D1RenderTarget_EndDraw(This, tag1, tag2) ((ID2D1RenderTarget *)This)->lpVtbl->EndDraw((ID2D1RenderTarget *)This, tag1, tag2)

struct ID2D1RenderTarget {
    const ID2D1RenderTargetVtbl *lpVtbl;
};

// ID2D1HwndRenderTarget
typedef struct ID2D1HwndRenderTarget ID2D1HwndRenderTarget;

typedef struct ID2D1HwndRenderTargetVtbl {
    ID2D1RenderTargetVtbl Base;
    uint8_t padding1[1 * sizeof(void *)];

    int32_t (__stdcall *Resize)(ID2D1HwndRenderTarget *This, const D2D1_SIZE_U *pixelSize);
    uint8_t padding2[1 * sizeof(void *)];
} ID2D1HwndRenderTargetVtbl;

#define ID2D1HwndRenderTarget_Resize(This, pixelSize) ((ID2D1HwndRenderTarget *)This)->lpVtbl->Resize((ID2D1HwndRenderTarget *)This, pixelSize)

struct ID2D1HwndRenderTarget {
    const ID2D1HwndRenderTargetVtbl *lpVtbl;
};

// ID2D1Factory
typedef struct ID2D1FactoryVtbl {
    IUnknownVtbl Base;

    uint8_t padding1[7 * sizeof(void *)];

    int32_t (__stdcall *CreatePathGeometry)(ID2D1Factory *This, ID2D1PathGeometry **pathGeometry);
    uint8_t padding2[3 * sizeof(void *)];

    int32_t (__stdcall *CreateHwndRenderTarget)(ID2D1Factory *This, const D2D1_RENDER_TARGET_PROPERTIES *renderTargetProperties, const D2D1_HWND_RENDER_TARGET_PROPERTIES *hwndRenderTargetProperties, ID2D1HwndRenderTarget **hwndRenderTarget);
    uint8_t padding3[2 * sizeof(void *)];
} ID2D1FactoryVtbl;

#define ID2D1Factory_CreatePathGeometry(This, pathGeometry) \
    ((ID2D1Factory *)This)->lpVtbl->CreatePathGeometry((ID2D1Factory *)This, pathGeometry)
#define ID2D1Factory_CreateHwndRenderTarget(This, renderTargetProperties, hwndRenderTargetProperties, hwndRenderTarget) \
    ((ID2D1Factory *)This)->lpVtbl->CreateHwndRenderTarget((ID2D1Factory *)This, renderTargetProperties, hwndRenderTargetProperties, hwndRenderTarget)

struct ID2D1Factory {
    const ID2D1FactoryVtbl *lpVtbl;
};

#define D2D1_FACTORY_TYPE_SINGLE_THREADED 0

extern int32_t __stdcall D2D1CreateFactory(uint32_t factoryType, GUID *riid, const void *pFactoryOptions, ID2D1Factory **ppIFactory);

#endif
