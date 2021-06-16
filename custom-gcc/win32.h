#ifndef WIN32_H
#define WIN32_H

#include <stdint.h>
#include <stdbool.h>

// Types & Macros
#define HMODULE void *
#define HINSTANCE void *
#define HWND void *
#define HICON void *
#define HCURSOR void *
#define HBRUSH void *
#define HMENU void *
#define HDC void *
#define HBITMAP void *
#define HGDIOBJ void *
#define WPARAM void *
#define LPARAM void *

#define LOWORD(a) ((uint32_t)(a) & 0xffff)
#define HIWORD(a) ((uint32_t)(a) >> 16)

#define TRUE 1
#define FALSE 0

// Kernel32
#define EXIT_SUCCESS 0
extern void __stdcall __attribute__((noreturn)) ExitProcess(uint32_t uExitCode);

extern HMODULE __stdcall GetModuleHandleA(char *lpModuleName);

// User32
#define HWND_DESKTOP 0

#define MB_OK 0

#define CS_VREDRAW 0x0001
#define CS_HREDRAW 0x0002

#define COLOR_WINDOW 5

#define IDI_APPLICATION (HICON)32512

#define IDC_ARROW (HCURSOR)32512

#define CW_USEDEFAULT 0x80000000

#define WS_OVERLAPPEDWINDOW 0x00CF0000

#define SW_SHOWDEFAULT 10

#define WM_CREATE 0x0001
#define WM_DESTROY 0x0002
#define WM_SIZE 0x0005
#define WM_PAINT 0x000F
#define WM_ERASEBKGND 0x0014
#define WM_GETMINMAXINFO 0x0024

#define SM_CXSCREEN 0
#define SM_CYSCREEN 1

#define SWP_NOZORDER 0x0004

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
    uint32_t   message;
    WPARAM wParam;
    LPARAM lParam;
    uint32_t  time;
    POINT pt;
    uint32_t  lPrivate;
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
extern void __stdcall RegisterClassExA(const WNDCLASSEXA *unnamedParam1);
extern HWND __stdcall CreateWindowExA(uint32_t dwExStyle, char *lpClassName, char *lpWindowName, uint32_t dwStyle,
    int32_t X, int32_t Y, int32_t nWidth, int32_t nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPARAM lpParam);
extern bool __stdcall ShowWindow(HWND hWnd, int32_t nCmdShow);
extern bool __stdcall UpdateWindow(HWND hWnd);
extern bool __stdcall GetMessageA(MSG *lpMsg, HWND hWnd, uint32_t wMsgFilterMin, uint32_t wMsgFilterMax);
extern bool __stdcall TranslateMessage(const MSG *lpMsg);
extern int32_t __stdcall DispatchMessageA(const MSG *lpMsg);
extern bool __stdcall GetClientRect(HWND hWnd, RECT *lpRect);
extern int32_t __stdcall GetSystemMetrics(int32_t nIndex);
extern bool __stdcall SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int32_t X, int32_t Y, int32_t cx, int32_t cy, uint32_t uFlags);

extern HDC __stdcall BeginPaint(HWND hWnd, PAINTSTRUCT *lpPaint);
extern bool __stdcall EndPaint(HWND hWnd, PAINTSTRUCT *lpPaint);
extern int32_t __stdcall FillRect(HDC hDC, const RECT *lprc, HBRUSH hbr);

// Gdi32
#define SRCCOPY 0x00CC0020

extern HDC __stdcall CreateCompatibleDC(HDC hdc);
extern HBITMAP __stdcall CreateCompatibleBitmap(HDC hdc, int32_t cx, int32_t cy);
extern HBRUSH __stdcall CreateSolidBrush(uint32_t color);
extern HGDIOBJ __stdcall SelectObject(HDC hdc, HGDIOBJ h);
extern bool __stdcall DeleteObject(HGDIOBJ ho);
extern bool __stdcall DeleteDC(HDC hdc);
extern bool __stdcall BitBlt(HDC hdc, int32_t x, int32_t y, int32_t cx, int32_t cy,
    HDC hdcSrc, int32_t x1, int32_t y1, uint32_t rop);

#endif
