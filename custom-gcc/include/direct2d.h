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

typedef struct D2D1_RECT_F {
    float left;
    float top;
    float right;
    float bottom;
} D2D1_RECT_F;

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

#define ID2D1SimplifiedGeometrySink_SetFillMode(ptr, a) ((ID2D1SimplifiedGeometrySink *)ptr)->lpVtbl->SetFillMode((ID2D1SimplifiedGeometrySink *)ptr, a)
#define ID2D1SimplifiedGeometrySink_BeginFigure(ptr, a, b) ((ID2D1SimplifiedGeometrySink *)ptr)->lpVtbl->BeginFigure((ID2D1SimplifiedGeometrySink *)ptr, a, b)
#define ID2D1SimplifiedGeometrySink_EndFigure(ptr, a) ((ID2D1SimplifiedGeometrySink *)ptr)->lpVtbl->EndFigure((ID2D1SimplifiedGeometrySink *)ptr, a)
#define ID2D1SimplifiedGeometrySink_Close(ptr) ((ID2D1SimplifiedGeometrySink *)ptr)->lpVtbl->Close((ID2D1SimplifiedGeometrySink *)ptr)

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

#define ID2D1GeometrySink_AddLine(ptr, a) ((ID2D1GeometrySink *)ptr)->lpVtbl->AddLine((ID2D1GeometrySink *)ptr, a)

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

#define ID2D1PathGeometry_Open(ptr, a) ((ID2D1PathGeometry *)ptr)->lpVtbl->Open((ID2D1PathGeometry *)ptr, a)

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

typedef struct ID2D1RenderTargetVtbl {
    ID2D1ResourceVtbl Base;
    uint8_t padding1[4 * sizeof(void *)];

    int32_t (__stdcall *CreateSolidColorBrush)(ID2D1RenderTarget *This, const D2D1_COLOR_F *color, const void *brushProperties, ID2D1SolidColorBrush **solidColorBrush);
    uint8_t padding2[6 * sizeof(void *)];

    void (__stdcall *DrawLine)(ID2D1RenderTarget *This, D2D1_POINT_2F point0, D2D1_POINT_2F point1, ID2D1Brush *brush, float strokeWidth, void *strokeStyle);
    void (__stdcall *DrawRectangle)(ID2D1RenderTarget *This, const D2D1_RECT_F *rect, ID2D1Brush *brush, float strokeWidth, void *strokeStyle);
    void (__stdcall *FillRectangle)(ID2D1RenderTarget *This, const D2D1_RECT_F *rect, ID2D1Brush *brush);
    uint8_t padding3[5 * sizeof(void *)];

    void (__stdcall *FillGeometry)(ID2D1RenderTarget *This, ID2D1Geometry *geometry, ID2D1Brush *brush, ID2D1Brush *opacityBrush);
    uint8_t padding4[3 * sizeof(void *)];

    void (__stdcall *DrawText)(ID2D1RenderTarget *This, const wchar_t *string, uint32_t stringLength, IDWriteTextFormat *textFormat, const D2D1_RECT_F *layoutRect, ID2D1Brush *defaultForegroundBrush, uint32_t options, uint32_t measuringMode);
    uint8_t padding5[19 * sizeof(void *)];

    void (__stdcall *Clear)(ID2D1RenderTarget *This, const D2D1_COLOR_F *clearColor);
    void (__stdcall *BeginDraw)(ID2D1RenderTarget *This);
    int32_t (__stdcall *EndDraw)(ID2D1RenderTarget *This, void *tag1, void *tag2);
    uint8_t padding6[7 * sizeof(void *)];
} ID2D1RenderTargetVtbl;

#define ID2D1RenderTarget_CreateSolidColorBrush(ptr, a, b, c) ((ID2D1RenderTarget *)ptr)->lpVtbl->CreateSolidColorBrush((ID2D1RenderTarget *)ptr, a, b, c)
#define ID2D1RenderTarget_DrawLine(ptr, a, b, c, d, e) ((ID2D1RenderTarget *)ptr)->lpVtbl->DrawLine((ID2D1RenderTarget *)ptr, a, b, c, d, e)
#define ID2D1RenderTarget_DrawRectangle(ptr, a, b, c, d) ((ID2D1RenderTarget *)ptr)->lpVtbl->DrawRectangle((ID2D1RenderTarget *)ptr, a, b, c, d)
#define ID2D1RenderTarget_FillRectangle(ptr, a, b) ((ID2D1RenderTarget *)ptr)->lpVtbl->FillRectangle((ID2D1RenderTarget *)ptr, a, b)
#define ID2D1RenderTarget_FillGeometry(ptr, a, b, c) ((ID2D1RenderTarget *)ptr)->lpVtbl->FillGeometry((ID2D1RenderTarget *)ptr, a, b, c)
#define ID2D1RenderTarget_DrawText(ptr, a, b, c, d, e, f, g) ((ID2D1RenderTarget *)ptr)->lpVtbl->DrawText((ID2D1RenderTarget *)ptr, a, b, c, d, e, f, g)
#define ID2D1RenderTarget_Clear(ptr, a) ((ID2D1RenderTarget *)ptr)->lpVtbl->Clear((ID2D1RenderTarget *)ptr, a)
#define ID2D1RenderTarget_BeginDraw(ptr) ((ID2D1RenderTarget *)ptr)->lpVtbl->BeginDraw((ID2D1RenderTarget *)ptr)
#define ID2D1RenderTarget_EndDraw(ptr, a, b) ((ID2D1RenderTarget *)ptr)->lpVtbl->EndDraw((ID2D1RenderTarget *)ptr, a, b)

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

#define ID2D1HwndRenderTarget_Resize(ptr, a) ((ID2D1HwndRenderTarget *)ptr)->lpVtbl->Resize((ID2D1HwndRenderTarget *)ptr, a)

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

#define ID2D1Factory_CreatePathGeometry(ptr, a) ((ID2D1Factory *)ptr)->lpVtbl->CreatePathGeometry((ID2D1Factory *)ptr, a)
#define ID2D1Factory_CreateHwndRenderTarget(ptr, a, b, c) ((ID2D1Factory *)ptr)->lpVtbl->CreateHwndRenderTarget((ID2D1Factory *)ptr, a, b, c)

struct ID2D1Factory {
    const ID2D1FactoryVtbl *lpVtbl;
};

#define D2D1_FACTORY_TYPE_SINGLE_THREADED 0

extern int32_t __stdcall D2D1CreateFactory(uint32_t factoryType, GUID *riid, const void *pFactoryOptions, ID2D1Factory **ppIFactory);

#endif
