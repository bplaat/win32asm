; libwindows-shared.inc - A win32 assembly helper library shared stuff
; Made by Bastiaan van der Plaat (https://bastiaan.ml/)

; Kernel32
struct SYSTEMTIME, \
    wYear, WORD_size, \
    wMonth, WORD_size, \
    wDayOfWeek, WORD_size, \
    wDay, WORD_size, \
    wHour, WORD_size, \
    wMinute, WORD_size, \
    wSecond, WORD_size, \
    wMilliseconds, WORD_size

; User32
HWND_DESKTOP equ 0
HWND_TOP equ 0

MB_OK equ 0

CS_VREDRAW equ 0x0001
CS_HREDRAW equ 0x0002

COLOR_WINDOW equ 5

IDI_APPLICATION equ 32512

IDC_ARROW equ 32512

CW_USEDEFAULT equ 0x80000000

WS_OVERLAPPEDWINDOW equ 0x00CF0000

SW_SHOWDEFAULT equ 10

WM_CREATE equ 0x0001
WM_GETMINMAXINFO equ 0x0024
WM_DESTROY equ 0x0002
WM_PAINT equ 0x000F

SM_CXSCREEN equ 0
SM_CYSCREEN equ 1

SWP_NOZORDER equ 0x0004

DT_CENTER equ 0x00000001
DT_VCENTER equ 0x00000004
DT_SINGLELINE equ 0x00000020

struct POINT, \
    x, DWORD_size, \
    y, DWORD_size

struct RECT, \
    left, DWORD_size, \
    top, DWORD_size, \
    right, DWORD_size, \
    bottom, DWORD_size

struct WNDCLASSEX, \
    cbSize, DWORD_size, \
    style, DWORD_size, \
    lpfnWndProc, POINTER_size, \
    cbClsExtra, DWORD_size, \
    cbWndExtra, DWORD_size, \
    hInstance, POINTER_size, \
    hIcon, POINTER_size, \
    hCursor, POINTER_size, \
    hbrBackground, POINTER_size, \
    lpszMenuName, POINTER_size, \
    lpszClassName, POINTER_size, \
    hIconSm, POINTER_size

struct MSG, \
    hwnd, POINTER_size, \
    message, DWORD_size, \
    wParam, POINTER_size, \
    lParam, POINTER_size, \
    time, DWORD_size, \
    pt, POINT_size, \
    lPrivate, DWORD_size

struct MINMAXINFO, \
    ptReserved, POINT_size, \
    ptMaxSize, POINT_size, \
    ptMaxPosition, POINT_size, \
    ptMinTrackSize, POINT_size, \
    ptMaxTrackSize, POINT_size

struct PAINTSTRUCT, \
    hdc, POINTER_size, \
    fErase, DWORD_size, \
    rcPaint, RECT_size, \
    fRestore, DWORD_size, \
    fIncUpdate, DWORD_size, \
    rgbReserved, 32 * BYTE_size

; Gdi32
CLEARTYPE_QUALITY equ 5

TRANSPARENT equ 1

; Gdiplus
SmoothingModeAntiAlias equ 5

UnitPixel equ 2

struct GdiplusStartupInput, \
    GdiplusVersion, DWORD_size, \
    DebugEventCallback, POINTER_size, \
    SuppressBackgroundThread, DWORD_size, \
    SuppressExternalCodecs, DWORD_size

; Custom
struct Rect, \
    x, DWORD_size, \
    y, DWORD_size, \
    width, DWORD_size, \
    height, DWORD_size
