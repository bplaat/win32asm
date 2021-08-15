#define WIN32_MALLOC
#define WIN32_FREE
#define WIN32_RAND
#define WIN32_WCSLEN
#include "win32.h"

// ##########################################################################################
// ######################################### Canvas #########################################
// ##########################################################################################
// Some DPI aware functions and a simple canvas wrapper with two renderer backends:
// - A back-buffered GDI wrapper with alpha transperancy support
// - A GPU-accelerated Direct2D renderer with DirectWrite text drawing

typedef int32_t (__stdcall *_SetProcessDpiAwareness)(uint32_t value);

void SetDPIAware(void) {
    HMODULE hshcore = LoadLibraryW(L"shcore");
    if (hshcore) {
        #ifdef __GNUC__
            __extension__
        #endif
        _SetProcessDpiAwareness SetProcessDpiAwareness = GetProcAddress(hshcore, "SetProcessDpiAwareness");
        if (SetProcessDpiAwareness) {
            SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
        }
    } else {
        SetProcessDPIAware();
    }
}

typedef int32_t (__stdcall *_GetDpiForMonitor)(HMONITOR hmonitor, int dpiType, uint32_t *dpiX, uint32_t *dpiY);

int32_t GetWindowDPI(HWND hwnd) {
    HMODULE hShcore = LoadLibraryW(L"shcore");
    if (hShcore) {
        #ifdef __GNUC__
            __extension__
        #endif
        _GetDpiForMonitor GetDpiForMonitor = GetProcAddress(hShcore, "GetDpiForMonitor");
        if (GetDpiForMonitor) {
            HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
            uint32_t uiDpiX, uiDpiY;
            GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &uiDpiX, &uiDpiY);
            return uiDpiY;
        }
    }

    HDC hdc = GetDC(hwnd);
    int32_t dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    DeleteDC(hdc);
    return dpi;
}

typedef struct {
    wchar_t *name;
    float size;
} Font;

typedef struct {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
} Rect;

#undef RGB
#define RGB(r, g, b) ((r & 0xff) | ((g & 0xff) << 8) | ((b & 0xff) << 16) | (0xff << 24))
#define RGBA(r, g, b, a) ((r & 0xff) | ((g & 0xff) << 8) | ((b & 0xff) << 16) | ((a & 0xff) << 24))
#define HEX(x) (((x >> 16) & 0xff) | (((x >> 8) & 0xff) << 8) | ((x & 0xff) << 16) | (0xff << 24))
#define HEXA(x) (((x >> 24) & 0xff) | (((x >> 16) & 0xff) << 8) | (((x >> 8) & 0xff) << 16) | ((x & 0xff) << 24))

typedef enum {
    CANVAS_RENDERER_GDI,
    CANVAS_RENDERER_DIRECT2D
} CanvasRenderer;

typedef struct {
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

void Canvas_Init(Canvas *canvas, HWND hwnd, CanvasRenderer renderer);

Canvas *Canvas_New(HWND hwnd, CanvasRenderer renderer) {
    Canvas *canvas = malloc(sizeof(Canvas));
    Canvas_Init(canvas, hwnd, renderer);
    return canvas;
}

void Canvas_Init(Canvas *canvas, HWND hwnd, CanvasRenderer renderer) {
    canvas->renderer = renderer;
    canvas->width = -1;
    canvas->height = -1;

    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        canvas->hdc = GetDC(hwnd);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        GUID ID2D1Factory_guid = { 0xbb12d362, 0xdaee, 0x4b9a, { 0xaa, 0x1d, 0x14, 0xba, 0x40, 0x1c, 0xfa, 0x1f } };
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &ID2D1Factory_guid, NULL, &canvas->d2d_factory);

        GUID IDWriteFactory_guid = { 0xb859ee5a, 0xd838, 0x4b5b, { 0xa2, 0xe8, 0x1a, 0xdc, 0x7d, 0x93, 0xdb, 0x48 } };
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, &IDWriteFactory_guid, &canvas->dwrite_factory);

        D2D1_RENDER_TARGET_PROPERTIES render_props = { D2D1_RENDER_TARGET_TYPE_DEFAULT,
            { DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_UNKNOWN },
            96, 96, D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT };
        D2D1_HWND_RENDER_TARGET_PROPERTIES hwnd_render_props = { hwnd, { 0, 0 }, D2D1_PRESENT_OPTIONS_NONE };
        ID2D1Factory_CreateHwndRenderTarget(canvas->d2d_factory, &render_props, &hwnd_render_props, &canvas->render_target);
    }
}

void Canvas_Free(Canvas *canvas) {
    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        DeleteObject(canvas->alpha_bitmap);
        DeleteDC(canvas->alpha_hdc);

        DeleteObject(canvas->buffer_bitmap);
        DeleteDC(canvas->buffer_hdc);

        DeleteDC(canvas->hdc);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        IUnknown_Release(canvas->render_target);
        IUnknown_Release(canvas->dwrite_factory);
        IUnknown_Release(canvas->d2d_factory);
    }

    free(canvas);
}

void Canvas_Resize(Canvas *canvas, int32_t width, int32_t height) {
    if (canvas->renderer == CANVAS_RENDERER_GDI && canvas->width != -1 && canvas->height != -1) {
        DeleteObject(canvas->alpha_bitmap);
        DeleteDC(canvas->alpha_hdc);

        DeleteObject(canvas->buffer_bitmap);
        DeleteDC(canvas->buffer_hdc);
    }

    canvas->width = width;
    canvas->height = height;

    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        canvas->buffer_hdc = CreateCompatibleDC(canvas->hdc);
        SetBkMode(canvas->buffer_hdc, TRANSPARENT);
        SetTextAlign(canvas->buffer_hdc, TA_LEFT);
        SetStretchBltMode(canvas->buffer_hdc, STRETCH_HALFTONE);
        canvas->buffer_bitmap = CreateCompatibleBitmap(canvas->hdc, canvas->width, canvas->height);
        SelectObject(canvas->buffer_hdc, canvas->buffer_bitmap);

        canvas->alpha_hdc = CreateCompatibleDC(canvas->hdc);
        SetBkMode(canvas->alpha_hdc, TRANSPARENT);
        SetTextAlign(canvas->alpha_hdc, TA_LEFT);
        SetStretchBltMode(canvas->alpha_hdc, STRETCH_HALFTONE);
        canvas->alpha_bitmap = CreateCompatibleBitmap(canvas->hdc, canvas->width * 2, canvas->height * 2);
        SelectObject(canvas->alpha_hdc, canvas->alpha_bitmap);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        D2D1_SIZE_U size = { canvas->width, canvas->height };
        ID2D1HwndRenderTarget_Resize(canvas->render_target, &size);
    }
}

void Canvas_BeginDraw(Canvas *canvas) {
    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1RenderTarget_BeginDraw(canvas->render_target);
    }
}

void Canvas_EndDraw(Canvas *canvas) {
    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        BitBlt(canvas->hdc, 0, 0, canvas->width, canvas->height, canvas->buffer_hdc, 0, 0, SRCCOPY);
    }
    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1RenderTarget_EndDraw(canvas->render_target, NULL, NULL);
    }
}

void Canvas_FillRect(Canvas *canvas, Rect *rect, uint32_t color) {
    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        HBRUSH brush = CreateSolidBrush(color & 0x00ffffff);
        if ((color >> 24) == 0xff) {
            RECT real_rect = { rect->x, rect->y, rect->x + rect->width, rect->y + rect->height };
            FillRect(canvas->buffer_hdc, &real_rect, brush);
        } else {
            RECT real_rect = { 0, 0, rect->width, rect->height };
            FillRect(canvas->alpha_hdc, &real_rect, brush);
            BLENDFUNCTION blend = { AC_SRC_OVER, 0, color >> 24, 0 };
            GdiAlphaBlend(canvas->buffer_hdc, rect->x, rect->y, rect->width, rect->height, canvas->alpha_hdc, 0, 0, rect->width, rect->height, blend);
        }
        DeleteObject(brush);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        D2D1_COLOR_F color_float = { (float)(color & 0xff) / 255, (float)((color >> 8) & 0xff) / 255,
            (float)((color >> 16) & 0xff) / 255, (float)((color >> 24) & 0xff) / 255 };
        ID2D1Brush *brush;
        ID2D1RenderTarget_CreateSolidColorBrush(canvas->render_target, &color_float, NULL, &brush);
        D2D1_RECT_F real_rect = { rect->x, rect->y, rect->x + rect->width, rect->y + rect->height };
        ID2D1RenderTarget_FillRectangle(canvas->render_target, &real_rect, brush);
        IUnknown_Release(brush);
    }
}

void Canvas_DrawText(Canvas *canvas, wchar_t *text, int32_t length, Rect *rect, Font *font, uint32_t align, uint32_t color) {
    if (length == -1) length = wcslen(text);

    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        HFONT hfont = CreateFontW(-MulDiv(font->size, 96, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font->name);
        SelectObject(canvas->buffer_hdc, hfont);

        SIZE measure_size;
        if (
            (color >> 24) != 0xff || (align & DT_CENTER) != 0 || (align & DT_RIGHT) != 0 ||
            (align & DT_VCENTER) != 0 || (align & DT_BOTTOM) != 0
        ) {
            GetTextExtentPoint32W(canvas->buffer_hdc, text, length, &measure_size);
            if (measure_size.cx > canvas->width) measure_size.cx = canvas->width;
            if (measure_size.cy > canvas->height) measure_size.cy = canvas->height;
        }
        int32_t real_x = rect->x;
        if ((align & DT_CENTER) != 0) real_x += (rect->width - measure_size.cx) / 2;
        if ((align & DT_RIGHT) != 0) real_x += rect->width - measure_size.cx;
        int32_t real_y = rect->y;
        if ((align & DT_VCENTER) != 0) real_y += (rect->height - measure_size.cy) / 2;
        if ((align & DT_BOTTOM) != 0) real_y += rect->height - measure_size.cy;

        if ((color >> 24) == 0xff) {
            SetTextColor(canvas->buffer_hdc, color & 0x00ffffff);
            RECT real_rect = { rect->x, rect->y, rect->x + rect->width, rect->y + rect->height };
            ExtTextOutW(canvas->buffer_hdc, real_x, real_y, ETO_CLIPPED, &real_rect, text, length, NULL);
        } else {
            BitBlt(canvas->alpha_hdc, 0, 0, measure_size.cx, measure_size.cy, canvas->buffer_hdc, real_x, real_y, SRCCOPY);
            SelectObject(canvas->alpha_hdc, hfont);
            SetTextColor(canvas->alpha_hdc, color & 0x00ffffff);
            RECT real_rect = { 0, 0, rect->width, rect->height };
            ExtTextOutW(canvas->alpha_hdc, 0, 0, ETO_CLIPPED, &real_rect, text, length, NULL);
            BLENDFUNCTION blend = { AC_SRC_OVER, 0, color >> 24, 0 };
            GdiAlphaBlend(canvas->buffer_hdc, real_x, real_y, measure_size.cx, measure_size.cy, canvas->alpha_hdc, 0, 0, measure_size.cx, measure_size.cy, blend);
        }
        DeleteObject(hfont);
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        D2D1_COLOR_F color_float = { (float)(color & 0xff) / 255, (float)((color >> 8) & 0xff) / 255,
            (float)((color >> 16) & 0xff) / 255, (float)((color >> 24) & 0xff) / 255 };
        ID2D1Brush *brush;
        ID2D1RenderTarget_CreateSolidColorBrush(canvas->render_target, &color_float, NULL, &brush);

        IDWriteTextFormat *text_format;
        IDWriteFactory_CreateTextFormat(canvas->dwrite_factory, font->name, NULL,
            DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            (font->size / 72) * 96, L"", &text_format);
        if ((align & DT_CENTER) != 0) IDWriteTextFormat_SetTextAlignment(text_format, DWRITE_TEXT_ALIGNMENT_CENTER);
        if ((align & DT_RIGHT) != 0) IDWriteTextFormat_SetTextAlignment(text_format, DWRITE_TEXT_ALIGNMENT_TRAILING);
        if ((align & DT_VCENTER) != 0) IDWriteTextFormat_SetParagraphAlignment(text_format, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        if ((align & DT_BOTTOM) != 0) IDWriteTextFormat_SetParagraphAlignment(text_format, DWRITE_PARAGRAPH_ALIGNMENT_FAR);

        D2D1_RECT_F real_rect = { rect->x, rect->y, rect->x + rect->width, rect->y + rect->height };
        ID2D1RenderTarget_DrawText(canvas->render_target, text, length, text_format,
            &real_rect, brush, D2D1_DRAW_TEXT_OPTIONS_CLIP | D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT, DWRITE_MEASURING_MODE_NATURAL);

        IUnknown_Release(text_format);
        IUnknown_Release(brush);
    }
}

float Canvas_ParsePathFloat(wchar_t **string) {
    float number = 0;
    bool negative = false;
    int32_t precision = 0;
    wchar_t *c = *string;
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

void Canvas_FillPath(Canvas *canvas, Rect *rect, int32_t viewport_width, int32_t viewport_height, wchar_t *path, uint32_t color) {
    if (canvas->renderer == CANVAS_RENDERER_GDI) {
        int32_t smooth_scale = 2; // Experimental smoothing feature gives dark border artifacts
        StretchBlt(canvas->alpha_hdc, 0, 0, rect->width * smooth_scale, rect->height * smooth_scale, canvas->buffer_hdc, rect->x, rect->y, rect->width, rect->height, SRCCOPY);
        HBRUSH brush = CreateSolidBrush(color & 0x00ffffff);
        SelectObject(canvas->alpha_hdc, brush);
        BeginPath(canvas->alpha_hdc);
        float x = 0;
        float y = 0;
        float scale_x = (float)(rect->width * smooth_scale) / viewport_width;
        float scale_y = (float)(rect->height * smooth_scale) / viewport_height;
        wchar_t *c = path;
        while (*c != '\0') {
            if (*c == 'M') {
                c++;
                x = Canvas_ParsePathFloat(&c);
                if (*c == ',') c++;
                y = Canvas_ParsePathFloat(&c);
                MoveToEx(canvas->alpha_hdc, x * scale_x, y * scale_y, NULL);
            } else if (*c == 'm') {
                c++;
                x += Canvas_ParsePathFloat(&c);
                if (*c == ',') c++;
                y += Canvas_ParsePathFloat(&c);
                MoveToEx(canvas->alpha_hdc, x * scale_x, y * scale_y, NULL);
            } else if (*c == 'L') {
                c++;
                x = Canvas_ParsePathFloat(&c);
                if (*c == ',') c++;
                y = Canvas_ParsePathFloat(&c);
                LineTo(canvas->alpha_hdc, x * scale_x, y * scale_y);
            } else if (*c == 'l') {
                c++;
                x += Canvas_ParsePathFloat(&c);
                if (*c == ',') c++;
                y += Canvas_ParsePathFloat(&c);
                LineTo(canvas->alpha_hdc, x * scale_x, y * scale_y);
            } else if (*c == 'H') {
                c++;
                x = Canvas_ParsePathFloat(&c);
                LineTo(canvas->alpha_hdc, x * scale_x, y * scale_y);
            } else if (*c == 'h') {
                c++;
                x += Canvas_ParsePathFloat(&c);
                LineTo(canvas->alpha_hdc, x * scale_x, y * scale_y);
            } else if (*c == 'V') {
                c++;
                y = Canvas_ParsePathFloat(&c);
                LineTo(canvas->alpha_hdc, x * scale_x, y * scale_y);
            } else if (*c == 'v') {
                c++;
                y += Canvas_ParsePathFloat(&c);
                LineTo(canvas->alpha_hdc, x * scale_x, y * scale_y);
            } else if (*c == 'Z' || *c == 'z') {
                c++;
            }
        }
        EndPath(canvas->alpha_hdc);
        FillPath(canvas->alpha_hdc);
        DeleteObject(brush);
        if ((color >> 24) == 0xff) {
            StretchBlt(canvas->buffer_hdc, rect->x, rect->y, rect->width, rect->height, canvas->alpha_hdc, 0, 0, rect->width * smooth_scale, rect->height * smooth_scale, SRCCOPY);
        } else {
            BLENDFUNCTION blend = { AC_SRC_OVER, 0, color >> 24, 0 };
            GdiAlphaBlend(canvas->buffer_hdc, rect->x, rect->y, rect->width, rect->height, canvas->alpha_hdc, 0, 0, rect->width * smooth_scale, rect->height * smooth_scale, blend);
        }
    }

    if (canvas->renderer == CANVAS_RENDERER_DIRECT2D) {
        ID2D1PathGeometry *path_geometry;
        ID2D1Factory_CreatePathGeometry(canvas->d2d_factory, &path_geometry);

        ID2D1GeometrySink *sink;
        ID2D1PathGeometry_Open(path_geometry, &sink);
        ID2D1SimplifiedGeometrySink_SetFillMode(sink, D2D1_FILL_MODE_WINDING);

        float x = 0;
        float y = 0;
        float scale_x = (float)rect->width / viewport_width;
        float scale_y = (float)rect->height / viewport_height;
        bool open = false;
        wchar_t *c = path;
        while (*c != '\0') {
            if (*c == 'M') {
                c++;
                x = Canvas_ParsePathFloat(&c);
                if (*c == ',') c++;
                y = Canvas_ParsePathFloat(&c);
                if (open) {
                    ID2D1SimplifiedGeometrySink_EndFigure(sink, D2D1_FIGURE_END_CLOSED);
                }
                D2D1_POINT_2F point;
                point.x = rect->x + x * scale_x;
                point.y = rect->y + y * scale_y;
                ID2D1SimplifiedGeometrySink_BeginFigure(sink, point, D2D1_FIGURE_BEGIN_FILLED);
                open = true;
            } else if (*c == 'm') {
                c++;
                x += Canvas_ParsePathFloat(&c);
                if (*c == ',') c++;
                y += Canvas_ParsePathFloat(&c);
                if (open) {
                    ID2D1SimplifiedGeometrySink_EndFigure(sink, D2D1_FIGURE_END_CLOSED);
                }
                D2D1_POINT_2F point;
                point.x = rect->x + x * scale_x;
                point.y = rect->y + y * scale_y;
                ID2D1SimplifiedGeometrySink_BeginFigure(sink, point, D2D1_FIGURE_BEGIN_FILLED);
                open = true;
            } else if (*c == 'L') {
                c++;
                x = Canvas_ParsePathFloat(&c);
                if (*c == ',') c++;
                y = Canvas_ParsePathFloat(&c);
                D2D1_POINT_2F point;
                point.x = rect->x + x * scale_x;
                point.y = rect->y + y * scale_y;
                ID2D1GeometrySink_AddLine(sink, point);
            } else if (*c == 'l') {
                c++;
                x += Canvas_ParsePathFloat(&c);
                if (*c == ',') c++;
                y += Canvas_ParsePathFloat(&c);
                D2D1_POINT_2F point;
                point.x = rect->x + x * scale_x;
                point.y = rect->y + y * scale_y;
                ID2D1GeometrySink_AddLine(sink, point);
            } else if (*c == 'H') {
                c++;
                x = Canvas_ParsePathFloat(&c);
                D2D1_POINT_2F point;
                point.x = rect->x + x * scale_x;
                point.y = rect->y + y * scale_y;
                ID2D1GeometrySink_AddLine(sink, point);
            } else if (*c == 'h') {
                c++;
                x += Canvas_ParsePathFloat(&c);
                D2D1_POINT_2F point;
                point.x = rect->x + x * scale_x;
                point.y = rect->y + y * scale_y;
                ID2D1GeometrySink_AddLine(sink, point);
            } else if (*c == 'V') {
                c++;
                y = Canvas_ParsePathFloat(&c);
                D2D1_POINT_2F point;
                point.x = rect->x + x * scale_x;
                point.y = rect->y + y * scale_y;
                ID2D1GeometrySink_AddLine(sink, point);
            } else if (*c == 'v') {
                c++;
                y += Canvas_ParsePathFloat(&c);
                D2D1_POINT_2F point;
                point.x = rect->x + x * scale_x;
                point.y = rect->y + y * scale_y;
                ID2D1GeometrySink_AddLine(sink, point);
            } else if (*c == 'Z' || *c == 'z') {
                c++;
                if (open) {
                    ID2D1SimplifiedGeometrySink_EndFigure(sink, D2D1_FIGURE_END_CLOSED);
                    open = false;
                }
            }
        }
        if (open) {
            ID2D1SimplifiedGeometrySink_EndFigure(sink, D2D1_FIGURE_END_CLOSED);
        }
        ID2D1SimplifiedGeometrySink_Close(sink);
        IUnknown_Release(sink);

        D2D1_COLOR_F color_float = { (float)(color & 0xff) / 255, (float)((color >> 8) & 0xff) / 255,
            (float)((color >> 16) & 0xff) / 255, (float)((color >> 24) & 0xff) / 255 };
        ID2D1Brush *brush;
        ID2D1RenderTarget_CreateSolidColorBrush(canvas->render_target, &color_float, NULL, &brush);

        ID2D1RenderTarget_FillGeometry(canvas->render_target, (ID2D1Geometry *)path_geometry, brush, NULL);
        IUnknown_Release(brush);
        IUnknown_Release(path_geometry);
    }
}

// ##########################################################################################
// ######################################### Window #########################################
// ##########################################################################################

wchar_t *window_class_name = L"window-borderless";

#ifdef WIN64
    wchar_t *window_title = L"This is a test borderless window 🤩 (64-bit)";
#else
    wchar_t *window_title = L"This is a test borderless window 🤩 (32-bit)";
#endif

wchar_t *font_name = L"Segoe UI";

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_STYLE (WS_POPUP | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CAPTION | WS_SYSMENU)

typedef struct {
    int32_t width;
    int32_t height;
    int32_t dpi;
    Canvas *canvas;
    uint32_t background_color;
    int32_t titlebar_height;
    int32_t titlebar_button_width;
    int32_t titlebar_icon_size;
    bool active;
    bool menu_hover;
    bool minimize_hover;
    bool maximize_hover;
    bool close_hover;
    bool renderer;
} WindowData;

int32_t __stdcall WndProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam) {
    WindowData *window = GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    if (msg == WM_CREATE) {
        // Create window data
        window = malloc(sizeof(WindowData));
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, window);

        // Resize window when dpi is not 96
        window->dpi = GetWindowDPI(hwnd);
        if (window->dpi != 96) {
            window->width = MulDiv(WINDOW_WIDTH, window->dpi, 96);
            window->height = MulDiv(WINDOW_HEIGHT, window->dpi, 96);
            SetWindowPos(hwnd, NULL, (GetSystemMetrics(SM_CXSCREEN) - window->width) / 2,
                (GetSystemMetrics(SM_CYSCREEN) - window->height) / 2,
                window->width, window->height, SWP_NOZORDER | SWP_NOACTIVATE);
        }

        // Create canvas
        window->renderer = true;
        window->canvas = Canvas_New(hwnd, window->renderer ? CANVAS_RENDERER_DIRECT2D : CANVAS_RENDERER_GDI);

        // Generate random seed by time
        SYSTEMTIME time;
        GetLocalTime(&time);
        srand((time.wHour * 60 + time.wMinute) * 60 + time.wSecond);

        // Generate random background color and fill other window data
        window->background_color = (rand() & 0x007f7f7f) | 0xff000000;
        window->active = true;
        window->minimize_hover = false;
        window->maximize_hover = false;
        window->close_hover = false;

        // Toggle renderer every half second
        SetTimer(hwnd, 0, 500, NULL);
        return 0;
    }

    if (msg == WM_TIMER) {
        Canvas_Free(window->canvas);
        window->renderer = !window->renderer;
        window->canvas = Canvas_New(hwnd, window->renderer ? CANVAS_RENDERER_DIRECT2D : CANVAS_RENDERER_GDI);
        Canvas_Resize(window->canvas, window->width, window->height);
        InvalidateRect(hwnd, NULL, false);
        return 0;
    }

    if (msg == WM_ACTIVATE) {
        MARGINS borderless = { 1, 1, 1, 1 };
        DwmExtendFrameIntoClientArea(hwnd, &borderless);
        return 0;
    }

    if (msg == WM_NCACTIVATE) {
        window->active = wParam;
        InvalidateRect(hwnd, NULL, false);
    }

    if (msg == WM_NCCALCSIZE) {
        if (wParam) {
            WINDOWPLACEMENT placement;
            GetWindowPlacement(hwnd, &placement);
            if (placement.showCmd == SW_MAXIMIZE) {
                NCCALCSIZE_PARAMS *params = lParam;
                HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
                if (!monitor) return 0;
                MONITORINFO monitor_info;
                monitor_info.cbSize = sizeof(MONITORINFO);
                if (!GetMonitorInfoW(monitor, &monitor_info)) {
                    return 0;
                }
                params->rgrc[0] = monitor_info.rcWork;
            }
        }
        return 0;
    }

    if (msg == WM_NCHITTEST) {
        int32_t x = GET_X_LPARAM(lParam);
        int32_t y = GET_Y_LPARAM(lParam);

        RECT window_rect;
        GetWindowRect(hwnd, &window_rect);

        int32_t border_horizontal = GetSystemMetrics(SM_CXSIZEFRAME);
        int32_t border_vertical = GetSystemMetrics(SM_CYSIZEFRAME);

        if (y >= window_rect.top && y < window_rect.bottom) {
            if (x >= window_rect.left && x < window_rect.left + border_horizontal) {
                if (y < window_rect.top + border_vertical) {
                    return HTTOPLEFT;
                }
                if (y > window_rect.bottom - border_vertical) {
                    return HTBOTTOMLEFT;
                }
                return HTLEFT;
            }
            if (x >= window_rect.right - border_horizontal && x < window_rect.right) {
                if (y < window_rect.top + border_vertical) {
                    return HTTOPRIGHT;
                }
                if (y > window_rect.bottom - border_vertical) {
                    return HTBOTTOMRIGHT;
                }
                return HTRIGHT;
            }
        }

        if (x >= window_rect.left && x < window_rect.right) {
            if (y >= window_rect.top && y < window_rect.top + border_vertical) {
                return HTTOP;
            }
            if (y >= window_rect.bottom - border_vertical && y < window_rect.bottom) {
                return HTBOTTOM;
            }
        }

        if (x >= window_rect.left && y >= window_rect.top && x < window_rect.right && y < window_rect.bottom) {
            x -= window_rect.left;
            y -= window_rect.top;
            if (y < window->titlebar_height) {
                if (x >= window->titlebar_button_width && x < window->width - window->titlebar_button_width * 3) {
                    return HTCAPTION;
                }
            }
            return HTCLIENT;
        }

        return HTNOWHERE;
    }

    if (msg == WM_DPICHANGED) {
        window->dpi = HIWORD(wParam);

        RECT *new_size = lParam;
        SetWindowPos(hwnd, NULL, new_size->left, new_size->top, new_size->right - new_size->left,
            new_size->bottom - new_size->top, SWP_NOZORDER | SWP_NOACTIVATE);
        return 0;
    }

    if (msg == WM_SIZE) {
        // Save new window size
        window->width = LOWORD(lParam);
        window->height = HIWORD(lParam);

        // Resize some controls
        window->titlebar_height = MulDiv(32, window->dpi, 96);
        window->titlebar_button_width = MulDiv(48, window->dpi, 96);
        window->titlebar_icon_size = MulDiv(24, window->dpi, 96);

        // Resize canvas
        Canvas_Resize(window->canvas, window->width, window->height);
        return 0;
    }

    if (msg == WM_GETMINMAXINFO) {
        // Set window min size
        MINMAXINFO *minMaxInfo = (MINMAXINFO *)lParam;
        minMaxInfo->ptMinTrackSize.x = 640;
        minMaxInfo->ptMinTrackSize.y = 480;
        return 0;
    }

    if (msg == WM_LBUTTONUP) {
        int32_t x = GET_X_LPARAM(lParam);
        int32_t y = GET_Y_LPARAM(lParam);

        // Window decoration buttons
        if (y >= 0 && y < window->titlebar_height) {
            if (
                x < window->titlebar_button_width
            ) {
                ShellExecuteW(hwnd, L"open", L"https://youtu.be/dQw4w9WgXcQ?t=43", NULL, NULL, SW_SHOWNORMAL);
            }

            if (
                x >= window->width - window->titlebar_button_width * 3 &&
                x < window->width - window->titlebar_button_width  * 2
            ) {
                ShowWindow(hwnd, SW_MINIMIZE);
                window->minimize_hover = false;
                ReleaseCapture();
            }

            if (
                x >= window->width - window->titlebar_button_width  * 2 &&
                x < window->width - window->titlebar_button_width  * 1
            ) {
                WINDOWPLACEMENT placement;
                GetWindowPlacement(hwnd, &placement);
                if (placement.showCmd == SW_MAXIMIZE) {
                    ShowWindow(hwnd, SW_RESTORE);
                } else {
                    ShowWindow(hwnd, SW_MAXIMIZE);
                }
                window->maximize_hover = false;
                ReleaseCapture();
            }

            if (
                x >= window->width - window->titlebar_button_width  * 1 &&
                x < window->width
            ) {
                DestroyWindow(hwnd);
                window->close_hover = false;
                ReleaseCapture();
            }
        }
        return 0;
    }

    if (msg == WM_MOUSEMOVE) {
        int32_t x = GET_X_LPARAM(lParam);
        int32_t y = GET_Y_LPARAM(lParam);

        // Window decoration buttons
        bool new_menu_hover = window->menu_hover;
        bool new_minimize_hover = window->minimize_hover;
        bool new_maximize_hover = window->minimize_hover;
        bool new_close_hover = window->close_hover;
        if (y >= 0 && y < window->titlebar_height) {
            new_menu_hover = x >= 0 && x < window->titlebar_button_width;

            new_minimize_hover = x >= window->width - window->titlebar_button_width * 3 &&
                x < window->width - window->titlebar_button_width  * 2;

            new_maximize_hover = x >= window->width - window->titlebar_button_width  * 2 &&
                x < window->width - window->titlebar_button_width  * 1;

            new_close_hover = x >= window->width - window->titlebar_button_width  * 1 &&
                    x < window->width;
        } else {
            new_menu_hover = false;
            new_minimize_hover = false;
            new_maximize_hover = false;
            new_close_hover = false;
        }

        if (
            new_menu_hover != window->menu_hover ||
            new_minimize_hover != window->minimize_hover ||
            new_maximize_hover != window->maximize_hover ||
            new_close_hover != window->close_hover
        ) {
            window->menu_hover = new_menu_hover;
            window->minimize_hover = new_minimize_hover;
            window->maximize_hover = new_maximize_hover;
            window->close_hover = new_close_hover;

            if (window->menu_hover || window->minimize_hover || window->maximize_hover || window->close_hover) {
                SetCapture(hwnd);
            } else {
                ReleaseCapture();
            }
            InvalidateRect(hwnd, NULL, false);
        }
        return 0;
    }

    if (msg == WM_ERASEBKGND) {
        // Draw no background
        return TRUE;
    }

    if (msg == WM_PAINT) {
        PAINTSTRUCT paint_struct;
        BeginPaint(hwnd, &paint_struct);
        Canvas_BeginDraw(window->canvas);

        // Draw background color
        Rect background_rect = { 0, 0, window->width, window->height };
        Canvas_FillRect(window->canvas, &background_rect, window->background_color);

        uint32_t active_text_color = RGB(255, 255, 255);
        uint32_t inactive_text_color = RGBA(255, 255, 255, 128);

        // Draw window decoration buttons

        // Menu button
        if (window->menu_hover) {
            Rect menu_button_rect = { 0, 0, window->titlebar_button_width, window->titlebar_height };
            Canvas_FillRect(window->canvas, &menu_button_rect, RGBA(255, 255, 255, 48));
        }
        Rect menu_icon_rect = { (window->titlebar_button_width - window->titlebar_icon_size) / 2,
            (window->titlebar_height - window->titlebar_icon_size) / 2, window->titlebar_icon_size, window->titlebar_icon_size };
        Canvas_FillPath(window->canvas, &menu_icon_rect, 24, 24, L"M3,6H21V8H3V6M3,11H21V13H3V11M3,16H21V18H3V16Z", window->active ? active_text_color : inactive_text_color);

        // Minimize button
        int32_t x = window->width - window->titlebar_button_width * 3;
        if (window->minimize_hover) {
            Rect minimize_button_rect = { x, 0, window->titlebar_button_width, window->titlebar_height };
            Canvas_FillRect(window->canvas, &minimize_button_rect, RGBA(255, 255, 255, 48));
        }
        Rect minimize_icon_rect = { x + (window->titlebar_button_width - window->titlebar_icon_size) / 2,
            (window->titlebar_height - window->titlebar_icon_size) / 2, window->titlebar_icon_size, window->titlebar_icon_size };
        Canvas_FillPath(window->canvas, &minimize_icon_rect, 24, 24, L"M20,14H4V10H20", window->active ? active_text_color : inactive_text_color);
        x += window->titlebar_button_width;

        // Maximize button
        if (window->maximize_hover) {
            Rect maximize_button_rect = { x, 0, window->titlebar_button_width, window->titlebar_height };
            Canvas_FillRect(window->canvas, &maximize_button_rect, RGBA(255, 255, 255, 48));
        }
        WINDOWPLACEMENT placement;
        GetWindowPlacement(hwnd, &placement);
        Rect maximize_icon_rect = { x + (window->titlebar_button_width - window->titlebar_icon_size) / 2,
            (window->titlebar_height - window->titlebar_icon_size) / 2, window->titlebar_icon_size, window->titlebar_icon_size };
        Canvas_FillPath(window->canvas, &maximize_icon_rect, 24, 24,
            placement.showCmd == SW_MAXIMIZE ? L"M4,8H8V4H20V16H16V20H4V8M16,8V14H18V6H10V8H16M6,12V18H14V12H6Z" :
            L"M4,4H20V20H4V4M6,8V18H18V8H6Z", window->active ? active_text_color : inactive_text_color);
        x += window->titlebar_button_width;

        // Close button
        if (window->close_hover) {
            Rect close_button_rect = { x, 0, window->titlebar_button_width, window->titlebar_height };
            Canvas_FillRect(window->canvas, &close_button_rect, RGBA(255, 0, 0, 128));
        }
        Rect close_icon_rect = { x + (window->titlebar_button_width - window->titlebar_icon_size) / 2,
            (window->titlebar_height - window->titlebar_icon_size) / 2, window->titlebar_icon_size, window->titlebar_icon_size };
        Canvas_FillPath(window->canvas, &close_icon_rect, 24, 24, L"M19,6.41L17.59,5L12,10.59L6.41,5L5,6.41L10.59,12L5,17.59L6.41,19L12,13.41L17.59,19L19,17.59L13.41,12L19,6.41Z",
            window->active ? active_text_color : inactive_text_color);

        // Draw centered text
        Font title_font = { font_name, (float)window->width / 32 };
        Rect title_rect = { 0, (window->height - title_font.size * 2) / 2, window->width, title_font.size * 2 };
        Canvas_DrawText(window->canvas, window_title,  -1, &title_rect, &title_font, DT_CENTER | DT_VCENTER, active_text_color);

        // Draw footer text
        Font footer_font = { font_name, (float)window->width / 42 };
        Rect footer_rect = { 0, window->height - footer_font.size * 2 - 24, window->width, footer_font.size * 2 };
        wchar_t string_buffer[64];
        wsprintfW(string_buffer, L"Window size: %dx%d at %d dpi", window->width, window->height, window->dpi);
        Canvas_DrawText(window->canvas, string_buffer, -1, &footer_rect, &footer_font, DT_CENTER | DT_BOTTOM, active_text_color);

        Canvas_EndDraw(window->canvas);
        EndPaint(hwnd, &paint_struct);
        return 0;
    }

    if (msg == WM_DESTROY) {
        // Free window data
        Canvas_Free(window->canvas);
        free(window);

        // Close process
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void _start(void) {
    SetDPIAware();

    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.hIcon = LoadIconW(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.lpszClassName = window_class_name;
    wc.hIconSm = wc.hIcon;
    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowExW(0, window_class_name, window_title, WINDOW_STYLE,
        (GetSystemMetrics(SM_CXSCREEN) - WINDOW_WIDTH) / 2,
        (GetSystemMetrics(SM_CYSCREEN) - WINDOW_HEIGHT) / 2,
        WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, wc.hInstance, NULL);
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    MSG message;
    while (GetMessageW(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    ExitProcess((uintptr_t)message.wParam);
}
