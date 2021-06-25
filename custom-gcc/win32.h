#ifndef WIN32_H
#define WIN32_H

#include <stdint.h>
#include <stdbool.h>

// Types & Macros
#define HANDLE void *
#define HMODULE HANDLE
#define HINSTANCE HANDLE
#define HWND HANDLE
#define HICON HANDLE
#define HCURSOR HANDLE
#define HMENU HANDLE
#define HDC HANDLE
#define HBRUSH HANDLE
#define HBITMAP HANDLE
#define HFONT HANDLE
#define HGDIOBJ HANDLE
#define WPARAM void *
#define LPARAM void *

#define LOWORD(a) ((uint32_t)(uintptr_t)(a) & 0xffff)
#define HIWORD(a) ((uint32_t)(uintptr_t)(a) >> 16)

#define TRUE 1
#define FALSE 0

// Kernel32
#define EXIT_SUCCESS 0

typedef struct {
    uint16_t wYear;
    uint16_t wMonth;
    uint16_t wDayOfWeek;
    uint16_t wDay;
    uint16_t wHour;
    uint16_t wMinute;
    uint16_t wSecond;
    uint16_t wMilliseconds;
} SYSTEMTIME;

extern void __stdcall __attribute__((noreturn)) ExitProcess(uint32_t uExitCode);
extern HMODULE __stdcall GetModuleHandleA(char *lpModuleName);
extern HANDLE __stdcall GetProcessHeap(void);
extern void * __stdcall HeapAlloc(HANDLE hHeap, uint32_t dwFlags, size_t dwBytes);
extern void * __stdcall HeapReAlloc(HANDLE hHeap, uint32_t dwFlags, void *lpMem, size_t dwBytes);
extern bool __stdcall HeapFree(HANDLE hHeap, uint32_t dwFlags, void *lpMem);
extern void __stdcall GetLocalTime(SYSTEMTIME *lpSystemTime);
extern void __stdcall Sleep(uint32_t dwMilliseconds);

// User32
#define HWND_DESKTOP 0

#define MB_OK 0

#define CS_VREDRAW 0x0001
#define CS_HREDRAW 0x0002

#define COLOR_WINDOW 5

#define IDI_APPLICATION (HICON)32512

#define IDC_ARROW (HCURSOR)32512

#define CW_USEDEFAULT 0x80000000

#define WS_CHILD 0x040000000
#define WS_VISIBLE 0x010000000
#define WS_BORDER 000800000h
#define WS_OVERLAPPEDWINDOW 0x00CF0000
#define WS_THICKFRAME 0x000040000
#define WS_MAXIMIZEBOX 0x000010000
#define WS_CLIPCHILDREN 0x02000000

#define SW_HIDE 0
#define SW_SHOW 5
#define SW_SHOWDEFAULT 10

#define WM_CREATE 0x0001
#define WM_DESTROY 0x0002
#define WM_SIZE 0x0005
#define WM_PAINT 0x000F
#define WM_QUIT 0x0012
#define WM_ERASEBKGND 0x0014
#define WM_GETMINMAXINFO 0x0024
#define WM_SETFONT 0x0030
#define WM_COMMAND 0x0111
#define WM_TIMER 0x0113

#define PM_REMOVE 0x0001

#define SM_CXSCREEN 0
#define SM_CYSCREEN 1

#define SWP_NOZORDER 0x0004

#define GWLP_USERDATA -21

#define IMAGE_BITMAP 0
#define IMAGE_ICON 1

#define LR_DEFAULTCOLOR 0x00000000
#define LR_LOADFROMFILE 0x00000010

typedef struct {
    uint32_t cbSize;
    uint32_t style;
    void *lpfnWndProc;
    int cbClsExtra;
    int cbWndExtra;
    HINSTANCE hInstance;
    HICON hIcon;
    HCURSOR hCursor;
    HBRUSH hbrBackground;
    char *lpszMenuName;
    char *lpszClassName;
    HICON hIconSm;
} WNDCLASSEXA;

typedef struct {
    uint32_t left;
    uint32_t top;
    uint32_t right;
    uint32_t bottom;
} RECT;

typedef struct {
  uint32_t x;
  uint32_t y;
} POINT;

typedef struct {
    HWND hWnd;
    uint32_t message;
    WPARAM wParam;
    LPARAM lParam;
    uint32_t time;
    POINT pt;
    uint32_t lPrivate;
} MSG;

typedef struct {
  POINT ptReserved;
  POINT ptMaxSize;
  POINT ptMaxPosition;
  POINT ptMinTrackSize;
  POINT ptMaxTrackSize;
} MINMAXINFO;

typedef struct {
  HDC  hdc;
  bool fErase;
  RECT rcPaint;
  bool fRestore;
  bool fIncUpdate;
  uint8_t rgbReserved[32];
} PAINTSTRUCT;

extern int32_t __stdcall MessageBoxA(HWND hWnd, char *lpText, char *lpCaption, uint32_t uType);
extern void __stdcall PostQuitMessage(int32_t nExitCode);
extern int32_t __stdcall DefWindowProcA(HWND hWnd, uint32_t Msg, WPARAM wParam, LPARAM lParam);
extern HICON __stdcall LoadIconA(HINSTANCE hInstance, char *lpIconName);
extern HCURSOR __stdcall LoadCursorA(HINSTANCE hInstance, char *lpCursorName);
extern HBITMAP __stdcall LoadBitmapA(HINSTANCE hInstance, char *lpIconName);
extern HANDLE __stdcall LoadImageA(HINSTANCE hInst, char *name, uint32_t type, int32_t cx, int32_t cy, uint32_t fuLoad);
extern void __stdcall RegisterClassExA(const WNDCLASSEXA *unnamedParam1);
extern HWND __stdcall CreateWindowExA(uint32_t dwExStyle, char *lpClassName, char *lpWindowName, uint32_t dwStyle,
    int32_t X, int32_t Y, int32_t nWidth, int32_t nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPARAM lpParam);
extern bool __stdcall ShowWindow(HWND hWnd, int32_t nCmdShow);
extern bool __stdcall UpdateWindow(HWND hWnd);
extern bool __stdcall GetMessageA(MSG *lpMsg, HWND hWnd, uint32_t wMsgFilterMin, uint32_t wMsgFilterMax);
extern bool __stdcall PeekMessageA(MSG *lpMsg, HWND hWnd, uint32_t wMsgFilterMin, uint32_t wMsgFilterMax, uint32_t wRemoveMsg);
extern bool __stdcall TranslateMessage(const MSG *lpMsg);
extern int32_t __stdcall DispatchMessageA(const MSG *lpMsg);
extern bool __stdcall GetClientRect(HWND hWnd, RECT *lpRect);
extern int32_t __stdcall GetSystemMetrics(int32_t nIndex);
extern bool __stdcall SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int32_t X, int32_t Y, int32_t cx, int32_t cy, uint32_t uFlags);
extern int32_t __stdcall SendMessageA(HWND hWnd, uint32_t Msg, WPARAM wParam, LPARAM lParam);
extern bool __stdcall EnumChildWindows(HWND hWndParent, void *lpEnumFunc, LPARAM lParam);
extern bool __stdcall DestroyWindow(HWND hWnd);
extern uint32_t * __stdcall SetTimer(HWND hWnd, uint32_t nIDEvent, uint32_t uElapse,void *lpTimerFunc);
extern bool __stdcall KillTimer(HWND hWnd, uint32_t uIDEvent);

extern HDC __stdcall BeginPaint(HWND hWnd, PAINTSTRUCT *lpPaint);
extern bool __stdcall EndPaint(HWND hWnd, PAINTSTRUCT *lpPaint);
extern int32_t __stdcall FillRect(HDC hDC, const RECT *lprc, HBRUSH hbr);
extern bool __stdcall InvalidateRect(HWND hWnd, const RECT *lpRect, bool bErase);

#ifdef WIN64
    extern void * __stdcall SetWindowLongPtrA(HWND hWnd, int32_t nIndex, void *dwNewLong);
    extern void * __stdcall GetWindowLongPtrA(HWND hWnd, int32_t nIndex);
#else
    extern void * __stdcall SetWindowLongA(HWND hWnd, int32_t nIndex, void *dwNewLong);
    #define SetWindowLongPtrA(hWnd, nIndex, dwNewLong) (SetWindowLongA((hWnd), (nIndex), (dwNewLong)))
    extern void * __stdcall GetWindowLongA(HWND hWnd, int32_t nIndex);
    #define GetWindowLongPtrA(hWnd, nIndex) (GetWindowLongA((hWnd), (nIndex)))
#endif

// Gdi32
#define DEFAULT_GUI_FONT 17

#define FW_NORMAL 400
#define FW_BOLD 700

#define ANSI_CHARSET 0

#define OUT_DEFAULT_PRECIS 0

#define CLIP_DEFAULT_PRECIS 0

#define CLEARTYPE_QUALITY 5

#define DEFAULT_PITCH 0

#define FF_DONTCARE 0

#define TRANSPARENT 1

#define TA_LEFT 0
#define TA_RIGHT 2
#define TA_CENTER 6

#define SRCCOPY 0x00CC0020

#define RGB(r, g, b) ((r & 0xff) | ((g & 0xff) << 8) | ((b & 0xff) << 16))

extern HGDIOBJ __stdcall GetStockObject(int32_t i);
extern HDC __stdcall CreateCompatibleDC(HDC hdc);
extern HBITMAP __stdcall CreateCompatibleBitmap(HDC hdc, int32_t cx, int32_t cy);
extern HBRUSH __stdcall CreateSolidBrush(uint32_t color);
extern HGDIOBJ __stdcall SelectObject(HDC hdc, HGDIOBJ h);
extern bool __stdcall DeleteObject(HGDIOBJ ho);
extern bool __stdcall DeleteDC(HDC hdc);
extern bool __stdcall BitBlt(HDC hdc, int32_t x, int32_t y, int32_t cx, int32_t cy,
    HDC hdcSrc, int32_t x1, int32_t y1, uint32_t rop);
extern HFONT __stdcall CreateFontA(int32_t cHeight, int32_t cWidth, int32_t cEscapement, int32_t cOrientation, int32_t cWeight, uint32_t bItalic, uint32_t bUnderline, uint32_t bStrikeOut, uint32_t iCharSet, uint32_t iOutPrecision, uint32_t iClipPrecision, uint32_t iQuality, uint32_t iPitchAndFamily, char *pszFaceName);
extern int32_t __stdcall SetBkMode(HDC hdc, int32_t mode);
extern uint32_t __stdcall SetTextColor(HDC hdc, uint32_t color);
extern uint32_t __stdcall SetTextAlign(HDC hdc, uint32_t align);
extern bool __stdcall TextOutA(HDC hdc, int32_t x, int32_t y, char *lpString, int32_t c);

// Gdiplus
typedef struct GdiplusStartupInput {
    uint32_t GdiplusVersion;
    void *DebugEventCallback;
    bool SuppressBackgroundThread;
    bool SuppressExternalCodecs;
    void (*GdiplusStartupInput)(void *debugEventCallback, bool suppressBackgroundThread, bool suppressExternalCodecs);
} GdiplusStartupInput;

typedef struct GpGraphics {} GpGraphics;
typedef struct GpSolidFill {} GpSolidFill;
typedef struct GpBrush {} GpBrush;

typedef enum SmoothingMode {
  SmoothingModeInvalid,
  SmoothingModeDefault,
  SmoothingModeHighSpeed,
  SmoothingModeHighQuality,
  SmoothingModeNone,
  SmoothingModeAntiAlias,
  SmoothingModeAntiAlias8x4,
  SmoothingModeAntiAlias8x8
} SmoothingMode;

extern uint32_t __stdcall GdiplusStartup(uint32_t *token, const GdiplusStartupInput *input, void *output);
extern void __stdcall GdiplusShutdown(uint32_t *token);
extern uint32_t __stdcall GdipCreateFromHDC(HDC hdc, GpGraphics **graphics);
extern uint32_t __stdcall GdipSetSmoothingMode(GpGraphics *graphics, SmoothingMode smoothingMode);
extern uint32_t __stdcall GdipGraphicsClear(GpGraphics *graphics, uint32_t color);
extern uint32_t __stdcall GdipDeleteGraphics(GpGraphics *graphics);
extern uint32_t __stdcall GdipFillRectangle(GpGraphics *graphics, GpBrush *brush, float x, float y, float width, float height);
extern uint32_t __stdcall GdipFillRectangleI(GpGraphics *graphics, GpBrush *brush, int32_t x, int32_t y, int32_t width, int32_t height);
extern uint32_t __stdcall GdipCreateSolidFill(uint32_t color, GpSolidFill **brush);
extern uint32_t __stdcall GdipDeleteBrush(GpBrush *brush);

// Helpers
#ifdef WIN32_USE_STDLIB_HELPERS
    void *malloc(size_t size) {
        return HeapAlloc(GetProcessHeap(), 0, size);
    }

    void *realloc(void *ptr, size_t size) {
        return HeapReAlloc(GetProcessHeap(), 0, ptr, size);
    }

    void free(void *ptr) {
        HeapFree(GetProcessHeap(), 0, ptr);
    }

    uint32_t rand_seed;

    void srand(uint32_t seed) {
        rand_seed = seed;
    }

    uint32_t rand(void) {
        return rand_seed = rand_seed * 1103515245 + 12345;
    }
#endif

#ifdef WIN32_USE_STRING_HELPERS
    size_t strlen(char *string) {
        char *begin = string;
        while (*string != '\0') string++;
        return string - begin;
    }

    char *strdup(char *string) {
        char *new_string_begin = malloc(strlen(string) + 1);
        char *new_string = new_string_begin;
        while (*string != '\0') {
            *new_string = *string;
            string++;
            new_string++;
        }
        return new_string_begin;
    }
#endif

#endif
