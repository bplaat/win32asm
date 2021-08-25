#include "win32.h"
#include "canvas.h"

typedef enum JanOrientation {
    JAN_ORIENTATION_HORIZONTAL,
    JAN_ORIENTATION_VERTICAL
} JanOrientation;

typedef struct Jan {
    Canvas *canvas;
    JanOrientation orientation;
    CanvasRect rect;
} Jan;

void Jan_Label(Jan *jan, wchar_t *text, CanvasFont *font, CanvasColor color) {
    CanvasRect rect = { jan->rect.x, jan->rect.y, jan->rect.width, 0 };
    Canvas_DrawText(jan->canvas, text, -1, &rect, font, CANVAS_ALIGN_DEFAULT, color);
    if (jan->orientation == JAN_ORIENTATION_VERTICAL) {
        jan->rect.y += rect.height;
    }
}

void Jan_Gap(Jan *jan, float size) {
    if (jan->orientation == JAN_ORIENTATION_HORIZONTAL) {
        jan->rect.x += size;
    }
    if (jan->orientation == JAN_ORIENTATION_VERTICAL) {
        jan->rect.y += size;
    }
}

void Jan_BeginPadding(Jan *jan, float top, float right, float bottom, float left) {
    jan->rect.x += left;
    jan->rect.y += top;
    jan->rect.width -= left + right;
    jan->rect.height -= top + bottom;
}

void Jan_EndPadding(Jan *jan, float top, float right, float bottom, float left) {
    if (jan->orientation == JAN_ORIENTATION_HORIZONTAL) {
        jan->rect.x += left;
        jan->rect.y -= top;
    }
    if (jan->orientation == JAN_ORIENTATION_VERTICAL) {
        jan->rect.x -= left;
        jan->rect.y += top;
    }
    jan->rect.width += left + right;
    jan->rect.height += top + bottom;
}

// ###########################################################

wchar_t *window_class_name = L"canvas-test";

#ifdef WIN64
    wchar_t *window_title = L"This is a test canvas window 😍 (64-bit)";
#else
    wchar_t *window_title = L"This is a test canvas window 😍 (32-bit)";
#endif

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_STYLE WS_OVERLAPPEDWINDOW

typedef struct WindowData {
    uint32_t width;
    uint32_t height;
    Jan *jan;
} WindowData;

int32_t __stdcall WndProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam) {
    WindowData *window = GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    if (msg == WM_CREATE) {
        // Create window data
        window = malloc(sizeof(WindowData));
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, window);
        window->width = WINDOW_WIDTH;
        window->height = WINDOW_HEIGHT;

        window->jan = malloc(sizeof(Jan));
        window->jan->canvas = Canvas_New(hwnd, CANVAS_RENDERER_DEFAULT);
        return 0;
    }

    if (msg == WM_SIZE) {
        // Save new window size
        window->width = LOWORD(lParam);
        window->height = HIWORD(lParam);
        Canvas_Resize(window->jan->canvas, window->width, window->height);
        return 0;
    }

    if (msg == WM_GETMINMAXINFO) {
        // Set window min size
        MINMAXINFO *minMaxInfo = (MINMAXINFO *)lParam;
        RECT window_rect = { 0, 0, 640, 480 };
        AdjustWindowRectEx(&window_rect, WINDOW_STYLE, false, 0);
        minMaxInfo->ptMinTrackSize.x = window_rect.right - window_rect.left;
        minMaxInfo->ptMinTrackSize.y = window_rect.bottom - window_rect.top;
        return 0;
    }

    if (msg == WM_ERASEBKGND) {
        return true;
    }

    if (msg == WM_PAINT) {
        PAINTSTRUCT paint_struct;
        BeginPaint(hwnd, &paint_struct);
        Canvas_BeginDraw(window->jan->canvas);

        window->jan->orientation = JAN_ORIENTATION_VERTICAL;
        window->jan->rect = (CanvasRect){ 0, 0, window->width, window->height };
        Canvas_FillRect(window->jan->canvas, &window->jan->rect, CANVAS_HEX(0x222222));

        Jan_BeginPadding(window->jan, 16, 16, 16, 16);
        Jan_Label(window->jan, L"Lorem Ipsum", &(CanvasFont){ .name = L"Segoe UI", .size = 20, .weight = CANVAS_FONT_WEIGHT_BOLD }, CANVAS_RGB(255, 255, 255));
        Jan_Gap(window->jan, 20);

        CanvasFont font = { .name = L"Georgia", .size = 16 };
        CanvasFont italic_font = { .name = L"Georgia", .size = 16, .italic = true };
        for (int i = 0; i < 2; i++) {
            Jan_Label(window->jan, L"Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Donec odio. Quisque volutpat mattis eros. Nullam malesuada erat ut turpis. Suspendisse urna nibh, viverra non, semper suscipit, posuere a, pede.", &font, CANVAS_RGB(255, 255, 255));
            Jan_Gap(window->jan, 16);
            Jan_Label(window->jan, L"Donec nec justo eget felis facilisis fermentum. Aliquam porttitor mauris sit amet orci. Aenean dignissim pellentesque felis.", &font, CANVAS_RGB(255, 255, 255));
            Jan_BeginPadding(window->jan, 16, 16, 16, 16);
            Jan_Label(window->jan, L"Morbi in sem quis dui placerat ornare. Pellentesque odio nisi, euismod in, pharetra a, ultricies in, diam. Sed arcu. Cras consequat.", &italic_font, CANVAS_RGB(255, 255, 255));
            Jan_Gap(window->jan, 16);
            Jan_Label(window->jan, L"Praesent dapibus, neque id cursus faucibus, tortor neque egestas auguae, eu vulputate magna eros eu erat. Aliquam erat volutpat. Nam dui mi, tincidunt quis, accumsan porttitor, facilisis luctus, metus.", &italic_font, CANVAS_RGB(255, 255, 255));
            Jan_EndPadding(window->jan, 16, 16, 16, 16);
            Jan_Label(window->jan, L"Phasellus ultrices nulla quis nibh. Quisque a lectus. Donec consectetuer ligula vulputate sem tristique cursus. Nam nulla quam, gravida non, commodo a, sodales sit amet, nisi.", &font, CANVAS_RGB(255, 255, 255));
            Jan_Gap(window->jan, 16);
        }
        Jan_Label(window->jan, L"Written by Bastiaan van der Plaat", &italic_font, CANVAS_HEX(0x888888));

        Canvas_EndDraw(window->jan->canvas);
        EndPaint(hwnd, &paint_struct);
        return 0;
    }

    if (msg == WM_DESTROY) {
        // Free window data
        Canvas_Free(window->jan->canvas);
        free(window->jan);
        free(window);

        // Close process
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void _start(void) {
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

    RECT window_rect;
    window_rect.left = (GetSystemMetrics(SM_CXSCREEN) - WINDOW_WIDTH) / 2;
    window_rect.top = (GetSystemMetrics(SM_CYSCREEN) - WINDOW_HEIGHT) / 2;
    window_rect.right = window_rect.left + WINDOW_WIDTH;
    window_rect.bottom = window_rect.top + WINDOW_HEIGHT;
    AdjustWindowRectEx(&window_rect, WINDOW_STYLE, false, 0);

    HWND hwnd = CreateWindowExW(0, window_class_name, window_title,
        WINDOW_STYLE, window_rect.left, window_rect.top,
        window_rect.right - window_rect.left, window_rect.bottom - window_rect.top,
        NULL, NULL, wc.hInstance, NULL);
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    MSG message;
    while (GetMessageW(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    ExitProcess((int32_t)(uintptr_t)message.wParam);
}
