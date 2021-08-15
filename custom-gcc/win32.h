#ifndef WIN32_H
#define WIN32_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

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
#define HRGN HANDLE
#define HRSRC HANDLE
#define HGLOBAL HANDLE
#define HACCEL HANDLE
#define HDROP HANDLE
#define HMONITOR HANDLE
#define HPEN HANDLE
#define WPARAM void *
#define LPARAM void *

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define LOWORD(a) ((uint32_t)(uintptr_t)(a) & 0xffff)
#define HIWORD(a) ((uint32_t)(uintptr_t)(a) >> 16)

#define MAKELANGID(p, s) ((((uint16_t)(s)) << 10) | (uint16_t)(p))

#define TRUE 1
#define FALSE 0

// Kernel32
#define EXIT_SUCCESS 0

#define GENERIC_WRITE 0x40000000
#define GENERIC_READ 0x80000000

#define FILE_SHARE_READ 0x00000001

#define CREATE_NEW 1
#define CREATE_ALWAYS 2
#define OPEN_EXISTING 3
#define OPEN_ALWAYS 4
#define TRUNCATE_EXISTING 5

#define FILE_ATTRIBUTE_NORMAL 0x80

#define INVALID_HANDLE_VALUE ((HANDLE)(void *)-1)

#define MAX_PATH 260

#define FILE_BEGIN 0
#define FILE_CURRENT 1
#define FILE_END 2

#define LANG_ENGLISH 0x09
#define SUBLANG_ENGLISH_US 0x01
#define LANG_DUTCH 0x13
#define SUBLANG_DUTCH 0x01

#define RT_RCDATA 10

#define ERROR_ALREADY_EXISTS 183

#define STD_INPUT_HANDLE -10
#define STD_OUTPUT_HANDLE -11
#define STD_ERROR_HANDLE -12

#define CP_ACP 0
#define CP_UTF8 65001

#define FILE_ATTRIBUTE_DIRECTORY 0x10

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

typedef struct {
    uint32_t dwOSVersionInfoSize;
    uint32_t dwMajorVersion;
    uint32_t dwMinorVersion;
    uint32_t dwBuildNumber;
    uint32_t dwPlatformId;
    wchar_t szCSDVersion[128];
} OSVERSIONINFOW;

typedef struct {
    uint32_t dwLowDateTime;
    uint32_t dwHighDateTime;
} FILETIME;

typedef struct {
  uint32_t dwFileAttributes;
  FILETIME ftCreationTime;
  FILETIME ftLastAccessTime;
  FILETIME ftLastWriteTime;
  uint32_t nFileSizeHigh;
  uint32_t nFileSizeLow;
  uint32_t dwReserved0;
  uint32_t dwReserved1;
  wchar_t cFileName[MAX_PATH];
  wchar_t cAlternateFileName[14];
  uint32_t dwFileType;
  uint32_t dwCreatorType;
  uint16_t wFinderFlags;
} WIN32_FIND_DATAW;

#ifdef __GNUC__
    extern void __stdcall __attribute__((noreturn)) ExitProcess(uint32_t uExitCode);
#else
    extern void __stdcall ExitProcess(uint32_t uExitCode);
#endif
extern HMODULE __stdcall GetModuleHandleW(wchar_t *lpModuleName);
extern HANDLE __stdcall GetProcessHeap(void);
extern bool __stdcall SetThreadLocale(uint32_t Locale);
extern uint32_t __stdcall SetThreadUILanguage(uint32_t LangId);
extern void * __stdcall HeapAlloc(HANDLE hHeap, uint32_t dwFlags, size_t dwBytes);
extern void * __stdcall HeapReAlloc(HANDLE hHeap, uint32_t dwFlags, void *lpMem, size_t dwBytes);
extern bool __stdcall HeapFree(HANDLE hHeap, uint32_t dwFlags, void *lpMem);
extern void __stdcall GetLocalTime(SYSTEMTIME *lpSystemTime);
extern void __stdcall Sleep(uint32_t dwMilliseconds);
extern uint32_t __stdcall GetLastError(void);
extern HANDLE __stdcall CreateFileW(wchar_t *lpFileName, uint32_t dwDesiredAccess, uint32_t dwShareMode, void *lpSecurityAttributes, uint32_t dwCreationDisposition, uint32_t dwFlagsAndAttributes, HANDLE hTemplateFile);
extern bool __stdcall ReadFile(HANDLE hFile, void *lpBuffer, uint32_t nNumberOfBytesToRead, uint32_t *lpNumberOfBytesRead, void *lpOverlapped);
extern bool __stdcall WriteFile(HANDLE hFile, const void *lpBuffer, uint32_t nNumberOfBytesToWrite, uint32_t *lpNumberOfBytesWritten, void *lpOverlapped);
extern uint32_t __stdcall SetFilePointer(HANDLE hFile, uint32_t lDistanceToMove, uint32_t *lpDistanceToMoveHigh, uint32_t dwMoveMethod);
extern bool __stdcall CloseHandle(HANDLE hObject);
extern HANDLE __stdcall CreateMutexW(void *lpMutexAttributes, bool bInitialOwner, const wchar_t *lpName);
extern bool __stdcall ReleaseMutex(HANDLE hMutex);
extern bool __stdcall GetVersionExW(OSVERSIONINFOW *lpVersionInformation);
extern HMODULE __stdcall LoadLibraryW(wchar_t *lpLibFileName);
extern void * __stdcall GetProcAddress(HMODULE hModule, char *lpProcName);
extern HRSRC __stdcall FindResourceW(HMODULE hModule, wchar_t *lpName, wchar_t *lpType);
extern uint32_t __stdcall SizeofResource(HMODULE hModule, HRSRC hResInfo);
extern HGLOBAL __stdcall LoadResource(HMODULE hModule, HRSRC hResInfo);
extern void * __stdcall LockResource(HGLOBAL hResData);
extern HANDLE __stdcall GetStdHandle(uint32_t nStdHandle);
extern bool __stdcall WriteConsoleW(HANDLE hConsoleOutput, const void *lpBuffer, uint32_t nNumberOfCharsToWrite, uint32_t *lpNumberOfCharsWritten, void *lpReserved);
extern uint32_t __stdcall GetFullPathNameW(const wchar_t *lpFileName, uint32_t nBufferLength, wchar_t *lpBuffer, wchar_t **lpFilePart);
extern int32_t __stdcall MultiByteToWideChar(uint32_t CodePage, uint32_t dwFlags, const char *lpMultiByteStr,
    int32_t cbMultiByte, wchar_t *lpWideCharStr, int32_t cchWideChar);
extern int32_t __stdcall WideCharToMultiByte(uint32_t CodePage, uint32_t dwFlags, const wchar_t *lpWideCharStr,
    int32_t cchWideChar, char *lpMultiByteStr, int32_t cbMultiByte, const char *lpDefaultChar, bool *lpUsedDefaultChar);
extern uint32_t __stdcall GetFileSize(HANDLE hFile, uint32_t *lpFileSizeHigh);
extern HANDLE __stdcall FindFirstFileW(const wchar_t *lpFileName, WIN32_FIND_DATAW *lpFindFileData);
extern bool __stdcall FindNextFileW(HANDLE hFindFile, WIN32_FIND_DATAW *lpFindFileData);
extern bool __stdcall FindClose(HANDLE hFindFile);
extern int32_t __stdcall MulDiv(int32_t nNumber, int32_t nNumerator, int32_t nDenominator);

// User32
#define GET_X_LPARAM(lParam) ((int32_t)(int16_t)LOWORD(lParam))

#define GET_Y_LPARAM(lParam) ((int32_t)(int16_t)HIWORD(lParam))

#define GET_WHEEL_DELTA_WPARAM(wParam) ((int32_t)(int16_t)HIWORD(wParam))

#define HWND_DESKTOP 0

#define MB_OK 0x00000000
#define MB_ICONINFORMATION 0x00000040

#define CS_VREDRAW 0x0001
#define CS_HREDRAW 0x0002

#define COLOR_WINDOW 5

#define IDI_APPLICATION (HICON)32512

#define IDC_ARROW (HCURSOR)32512

#define CW_USEDEFAULT 0x80000000

#define WS_CHILD 0x040000000
#define WS_VISIBLE 0x010000000
#define WS_BORDER 0x000800000
#define WS_OVERLAPPEDWINDOW 0x00CF0000
#define WS_CAPTION 0x00C00000
#define WS_THICKFRAME 0x000040000
#define WS_MAXIMIZEBOX 0x00010000
#define WS_MINIMIZEBOX 0x00020000
#define WS_CLIPCHILDREN 0x02000000
#define WS_SYSMENU 0x00080000
#define WS_POPUP 0x80000000

#define WS_EX_ACCEPTFILES 0x00000010
#define WS_EX_CLIENTEDGE 0x00000200

#define SW_HIDE 0
#define SW_SHOWNORMAL 1
#define SW_MAXIMIZE 3
#define SW_MINIMIZE 6
#define SW_SHOW 5
#define SW_RESTORE 9
#define SW_SHOWDEFAULT 10

#define WM_CREATE 0x0001
#define WM_DESTROY 0x0002
#define WM_SIZE 0x0005
#define WM_ACTIVATE 0x0006
#define WM_SETTEXT 0x000C
#define WM_GETTEXT 0x000D
#define WM_GETTEXTLENGTH 0x000E
#define WM_PAINT 0x000F
#define WM_QUIT 0x0012
#define WM_ERASEBKGND 0x0014
#define WM_GETMINMAXINFO 0x0024
#define WM_SETFONT 0x0030
#define WM_NCCALCSIZE 0x0083
#define WM_NCHITTEST 0x0084
#define WM_NCPAINT 0x0085
#define WM_NCACTIVATE 0x0086
#define WM_KEYDOWN 0x100
#define WM_CHAR 0x0102
#define WM_COMMAND 0x0111
#define WM_SYSCOMMAND 0x0112
#define WM_TIMER 0x0113
#define WM_MOUSEMOVE 0x0200
#define WM_LBUTTONDOWN 0x0201
#define WM_LBUTTONUP 0x0202
#define WM_MOUSEWHEEL 0x020A
#define WM_MOUSEHWHEEL 0x020E
#define WM_DROPFILES 0x0233
#define WM_MOUSELEAVE 0x02A3
#define WM_DPICHANGED 0x02E0
#define WM_USER 0x0400

#define VK_BACK 0x08
#define VK_TAB 0x09
#define VK_RETURN 0x0D
#define VK_LEFT 0x25
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28
#define VK_DELETE 0x2E

#define EM_SETLIMITTEXT 0x00C5

#define PM_REMOVE 0x0001

#define SM_CXSCREEN 0
#define SM_CYSCREEN 1
#define SM_CXSIZEFRAME 32
#define SM_CYSIZEFRAME 33
#define SM_CXSMICON 49
#define SM_CYSMICON 50

#define SWP_NOSIZE 0x0001
#define SWP_NOMOVE 0x0002
#define SWP_NOZORDER 0x0004
#define SWP_NOACTIVATE  0x0010

#define GWL_HINSTANCE -6
#define GWLP_USERDATA -21

#define IMAGE_BITMAP 0
#define IMAGE_ICON 1

#define DT_TOP 0x00000000
#define DT_LEFT 0x00000000
#define DT_CENTER 0x00000001
#define DT_RIGHT 0x00000002
#define DT_VCENTER 0x00000004
#define DT_BOTTOM 0x00000008
#define DT_WORDBREAK 0x00000010
#define DT_SINGLELINE 0x00000020
#define DT_CALCRECT 0x00000400

#define LR_DEFAULTCOLOR 0x00000000
#define LR_LOADFROMFILE 0x00000010
#define LR_DEFAULTSIZE 0x00000040
#define LR_CREATEDIBSECTION 0x00002000
#define LR_SHARED 0x00008000

#define ES_LEFT 0x0000
#define ES_CENTER 0x0001
#define ES_RIGHT 0x0002
#define ES_MULTILINE 0x0004
#define ES_AUTOHSCROLL 0x0080

#define MF_BYPOSITION 0x00000400
#define MF_SEPARATOR 0x00000800

#define HTTRANSPARENT -1
#define HTNOWHERE 0
#define HTCLIENT 1
#define HTCAPTION 2
#define HTSYSMENU 3
#define HTLEFT 10
#define HTRIGHT 11
#define HTTOP 12
#define HTTOPLEFT 13
#define HTTOPRIGHT 14
#define HTBOTTOM 15
#define HTBOTTOMLEFT 16
#define HTBOTTOMRIGHT 17

#define MONITOR_DEFAULTTONULL 0x00000000

#define ETO_CLIPPED 0x0004

typedef struct {
    uint32_t cbSize;
    uint32_t style;
    int32_t __stdcall (*lpfnWndProc)(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam);
    int cbClsExtra;
    int cbWndExtra;
    HINSTANCE hInstance;
    HICON hIcon;
    HCURSOR hCursor;
    HBRUSH hbrBackground;
    wchar_t *lpszMenuName;
    wchar_t *lpszClassName;
    HICON hIconSm;
} WNDCLASSEXW;

typedef struct {
    int32_t cx;
    int32_t cy;
} SIZE;

typedef struct {
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
} RECT;

typedef struct {
  int32_t x;
  int32_t y;
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

typedef struct {
    HWND hwnd;
    HWND hwndInsertAfter;
    int32_t x;
    int32_t y;
    int32_t cx;
    int32_t cy;
    uint32_t flags;
} WINDOWPOS;

typedef struct {
  RECT rgrc[3];
  WINDOWPOS *lppos;
} NCCALCSIZE_PARAMS;

typedef struct {
    uint32_t cbSize;
    RECT rcMonitor;
    RECT rcWork;
    uint32_t dwFlags;
} MONITORINFO;

typedef struct {
    uint32_t length;
    uint32_t flags;
    uint32_t showCmd;
    POINT ptMinPosition;
    POINT ptMaxPosition;
    RECT rcNormalPosition;
    RECT rcDevice;
} WINDOWPLACEMENT;

extern int32_t __stdcall MessageBoxW(HWND hWnd, wchar_t *lpText, wchar_t *lpCaption, uint32_t uType);
extern void __stdcall PostQuitMessage(int32_t nExitCode);
extern int32_t __stdcall DefWindowProcW(HWND hWnd, uint32_t Msg, WPARAM wParam, LPARAM lParam);
extern HICON __stdcall LoadIconW(HINSTANCE hInstance, wchar_t *lpIconName);
extern HCURSOR __stdcall LoadCursorW(HINSTANCE hInstance, wchar_t *lpCursorName);
extern HBITMAP __stdcall LoadBitmapW(HINSTANCE hInstance, wchar_t *lpIconName);
extern HANDLE __stdcall LoadImageW(HINSTANCE hInst, wchar_t *name, uint32_t type, int32_t cx, int32_t cy, uint32_t fuLoad);
extern void __stdcall RegisterClassExW(const WNDCLASSEXW *unnamedParam1);
extern HWND __stdcall CreateWindowExW(uint32_t dwExStyle, wchar_t *lpClassName, wchar_t *lpWindowName, uint32_t dwStyle,
    int32_t X, int32_t Y, int32_t nWidth, int32_t nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPARAM lpParam);
extern bool __stdcall ShowWindow(HWND hWnd, int32_t nCmdShow);
extern bool __stdcall UpdateWindow(HWND hWnd);
extern bool __stdcall GetMessageW(MSG *lpMsg, HWND hWnd, uint32_t wMsgFilterMin, uint32_t wMsgFilterMax);
extern bool __stdcall PeekMessageW(MSG *lpMsg, HWND hWnd, uint32_t wMsgFilterMin, uint32_t wMsgFilterMax, uint32_t wRemoveMsg);
extern bool __stdcall TranslateMessage(const MSG *lpMsg);
extern int32_t __stdcall DispatchMessageW(const MSG *lpMsg);
extern bool __stdcall GetWindowRect(HWND hWnd, RECT *lpRect);
extern bool __stdcall GetClientRect(HWND hWnd, RECT *lpRect);
extern int32_t __stdcall GetSystemMetrics(int32_t nIndex);
extern bool __stdcall AdjustWindowRectEx(RECT *lpRect, uint32_t dwStyle, bool bMenu, uint32_t dwExStyle);
extern bool __stdcall SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int32_t X, int32_t Y, int32_t cx, int32_t cy, uint32_t uFlags);
extern int32_t __stdcall SendMessageW(HWND hWnd, uint32_t Msg, WPARAM wParam, LPARAM lParam);
extern bool __stdcall EnumChildWindows(HWND hWndParent, void *lpEnumFunc, LPARAM lParam);
extern bool __stdcall SetMenu(HWND hWnd, HMENU hMenu);
extern bool __stdcall DestroyWindow(HWND hWnd);
extern uint32_t * __stdcall SetTimer(HWND hWnd, uint32_t nIDEvent, uint32_t uElapse,void *lpTimerFunc);
extern bool __stdcall KillTimer(HWND hWnd, uint32_t uIDEvent);
extern bool __stdcall MessageBeep(uint32_t uType);
extern int32_t __stdcall LoadStringW(HINSTANCE hInstance, uint32_t uID, wchar_t *lpBuffer, int32_t cchBufferMax);
extern HWND __stdcall GetDlgItem(HWND hDlg, int32_t nIDDlgItem);
extern HDC __stdcall GetDC(HWND hWnd);
extern int32_t __stdcall DrawTextW(HDC hdc, const wchar_t *lpchText, int32_t cchText, RECT *lprc, uint32_t format);
extern int32_t __cdecl wsprintfW(wchar_t *, wchar_t *, ...);
extern int32_t __cdecl wvsprintfW(wchar_t *, wchar_t *, va_list arglist);
extern HDC __stdcall BeginPaint(HWND hWnd, PAINTSTRUCT *lpPaint);
extern bool __stdcall EndPaint(HWND hWnd, PAINTSTRUCT *lpPaint);
extern int32_t __stdcall FillRect(HDC hDC, const RECT *lprc, HBRUSH hbr);
extern int32_t __stdcall FrameRect(HDC hDC, const RECT *lprc, HBRUSH hbr);
extern bool __stdcall InvalidateRect(HWND hWnd, const RECT *lpRect, bool bErase);
extern bool __stdcall IsIconic(HWND hWnd);
extern HWND __stdcall FindWindowW(const wchar_t *lpClassName, const wchar_t *lpWindowName);
extern bool __stdcall SetForegroundWindow(HWND hWnd);
extern HMENU __stdcall GetSystemMenu(HWND hWnd, bool bRevert);
extern bool __stdcall InsertMenuW(HMENU hMenu, uint32_t uPosition, uint32_t uFlags, uint32_t *uIDNewItem, wchar_t *lpNewItem);
extern HACCEL __stdcall LoadAcceleratorsW(HINSTANCE hInstance, wchar_t *lpTableName);
extern int32_t __stdcall TranslateAcceleratorW(HWND hWnd, HACCEL hAccTable, MSG *lpMsg);
extern HWND __stdcall SetCapture(HWND hWnd);
extern bool __stdcall ReleaseCapture(void);
extern HMONITOR __stdcall MonitorFromWindow(HWND hwnd, uint32_t dwFlags);
extern bool __stdcall GetMonitorInfoW(HMONITOR hMonitor, MONITORINFO *lpmi);
extern bool __stdcall GetWindowPlacement(HWND hWnd, WINDOWPLACEMENT *lpwndpl);
extern bool __stdcall SetProcessDPIAware();
#ifdef WIN64
    extern void * __stdcall SetWindowLongPtrW(HWND hWnd, int32_t nIndex, void *dwNewLong);
    extern void * __stdcall GetWindowLongPtrW(HWND hWnd, int32_t nIndex);
#else
    extern void * __stdcall SetWindowLongW(HWND hWnd, int32_t nIndex, void *dwNewLong);
    #define SetWindowLongPtrW(hWnd, nIndex, dwNewLong) (SetWindowLongW((hWnd), (nIndex), (dwNewLong)))
    extern void * __stdcall GetWindowLongW(HWND hWnd, int32_t nIndex);
    #define GetWindowLongPtrW(hWnd, nIndex) (GetWindowLongW((hWnd), (nIndex)))
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

#define AC_SRC_OVER 0x00

#define LOGPIXELSY 90

#define PS_SOLID 0

#define STRETCH_HALFTONE 4

#define RGB(r, g, b) ((r & 0xff) | ((g & 0xff) << 8) | ((b & 0xff) << 16))

typedef struct {
    uint8_t BlendOp;
    uint8_t BlendFlags;
    uint8_t SourceConstantAlpha;
    uint8_t AlphaFormat;
} BLENDFUNCTION;

extern HGDIOBJ __stdcall GetStockObject(int32_t i);
extern HDC __stdcall CreateCompatibleDC(HDC hdc);
extern HBITMAP __stdcall CreateCompatibleBitmap(HDC hdc, int32_t cx, int32_t cy);
extern HBRUSH __stdcall CreateSolidBrush(uint32_t color);
extern HGDIOBJ __stdcall SelectObject(HDC hdc, HGDIOBJ h);
extern HRGN __stdcall CreateRectRgn(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
extern int32_t __stdcall SelectClipRgn(HDC hdc, HRGN hrgn);
extern bool __stdcall DeleteObject(HGDIOBJ ho);
extern bool __stdcall DeleteDC(HDC hdc);
extern bool __stdcall BitBlt(HDC hdc, int32_t x, int32_t y, int32_t cx, int32_t cy,
    HDC hdcSrc, int32_t x1, int32_t y1, uint32_t rop);
extern int32_t __stdcall SetStretchBltMode(HDC hdc, int32_t mode);
extern bool __stdcall StretchBlt(HDC hdcDest, int32_t xDest, int32_t yDest, int32_t wDest, int32_t hDest,
    HDC hdcSrc, int32_t xSrc, int32_t ySrc, int32_t wSrc, int32_t hSrc, uint32_t rop);
extern HFONT __stdcall CreateFontW(int32_t cHeight, int32_t cWidth, int32_t cEscapement, int32_t cOrientation,
    int32_t cWeight, uint32_t bItalic, uint32_t bUnderline, uint32_t bStrikeOut, uint32_t iCharSet,
    uint32_t iOutPrecision, uint32_t iClipPrecision, uint32_t iQuality, uint32_t iPitchAndFamily, wchar_t *pszFaceName);
extern int32_t __stdcall SetBkMode(HDC hdc, int32_t mode);
extern uint32_t __stdcall SetTextColor(HDC hdc, uint32_t color);
extern uint32_t __stdcall SetTextAlign(HDC hdc, uint32_t align);
extern bool __stdcall TextOutW(HDC hdc, int32_t x, int32_t y, wchar_t *lpString, int32_t c);
extern bool __stdcall ExtTextOutW(HDC hdc, int32_t x, int32_t y, uint32_t options,
    const RECT *lprect, wchar_t *lpString, uint32_t c, const int32_t *lpDx);
extern bool __stdcall GetTextExtentPoint32W(HDC hdc, wchar_t *lpString, int32_t c, SIZE *psizl);
extern bool __stdcall GdiAlphaBlend(HDC hdcDest, int32_t xoriginDest, int32_t yoriginDest, int32_t wDest,
    int32_t hDest, HDC hdcSrc, int32_t xoriginSrc, int32_t yoriginSrc, int32_t wSrc, int32_t hSrc, BLENDFUNCTION ftn);
extern int32_t __stdcall GetDeviceCaps(HDC hdc, int index);
extern HPEN __stdcall CreatePen( int32_t iStyle, int32_t cWidth, uint32_t color);
extern bool __stdcall BeginPath(HDC hdc);
extern bool __stdcall MoveToEx(HDC hdc, int32_t x, int32_t y, POINT *lppt);
extern bool __stdcall LineTo(HDC hdc, int32_t x, int32_t y);
extern bool __stdcall FillPath(HDC hdc);
extern bool __stdcall EndPath(HDC hdc);

// Gdiplus
typedef struct {
    uint32_t GdiplusVersion;
    void *DebugEventCallback;
    bool SuppressBackgroundThread;
    bool SuppressExternalCodecs;
    void (*GdiplusStartupInput)(void *debugEventCallback, bool suppressBackgroundThread, bool suppressExternalCodecs);
} GdiplusStartupInput;

typedef struct GpGraphics GpGraphics;
typedef struct GpSolidFill GpSolidFill;
typedef struct GpBrush GpBrush;
typedef struct GpPen GpPen;

#define UnitPixel 2

#define SmoothingModeAntiAlias 5

extern uint32_t __stdcall GdiplusStartup(uint32_t *token, const GdiplusStartupInput *input, void *output);
extern void __stdcall GdiplusShutdown(uint32_t *token);
extern uint32_t __stdcall GdipCreateFromHDC(HDC hdc, GpGraphics **graphics);
extern uint32_t __stdcall GdipSetSmoothingMode(GpGraphics *graphics, int32_t smoothingMode);
extern uint32_t __stdcall GdipGraphicsClear(GpGraphics *graphics, uint32_t color);
extern uint32_t __stdcall GdipDeleteGraphics(GpGraphics *graphics);
extern uint32_t __stdcall GdipDrawRectangle(GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height);
extern uint32_t __stdcall GdipDrawRectangleI(GpGraphics *graphics, GpPen *pen, int32_t x, int32_t y, int32_t width, int32_t height);
extern uint32_t __stdcall GdipFillRectangle(GpGraphics *graphics, GpBrush *brush, float x, float y, float width, float height);
extern uint32_t __stdcall GdipFillRectangleI(GpGraphics *graphics, GpBrush *brush, int32_t x, int32_t y, int32_t width, int32_t height);
extern uint32_t __stdcall GdipCreateSolidFill(uint32_t color, GpSolidFill **brush);
extern uint32_t __stdcall GdipDeleteBrush(GpBrush *brush);
extern uint32_t __stdcall GdipCreatePen1(uint32_t color, float width, int32_t unit, GpPen **pen);
extern uint32_t __stdcall GdipDeletePen(GpPen *pen);

// Shell32
#define CSIDL_COMMON_APPDATA 0x0023
#define CSIDL_LOCAL_APPDATA 0x001C

extern void __stdcall DragFinish(HDROP hDrop);
extern uint32_t __stdcall DragQueryFileW(HDROP hDrop, uint32_t iFile, wchar_t *lpszFile, uint32_t cch);
extern HINSTANCE __stdcall ShellExecuteW(HWND hwnd, wchar_t *lpOperation, wchar_t *lpFile, wchar_t *lpParameters, wchar_t *lpDirectory, int32_t nShowCmd);
extern int32_t __stdcall SHGetFolderPathW(HWND hwnd, int csidl, HANDLE hToken, uint32_t dwFlags, wchar_t *pszPath);

// Comctl
#define LVM_FIRST 0x1000
#define LVM_INSERTITEMW LVM_FIRST + 77
#define LVM_DELETEALLITEMS LVM_FIRST + 9

#define LVS_LIST 0x0003
#define LVIF_TEXT 0x0001

#define CB_ADDSTRING 0x0143
#define CB_GETCURSEL 0x0147
#define CB_RESETCONTENT 0x014B
#define CB_SETCURSEL 0x014E

#define CBS_DROPDOWNLIST 0x0003
#define CBS_HASSTRINGS 0x0200

#define CBN_SELCHANGE 0x0001

typedef struct {
    uint32_t dwSize;
    uint32_t dwICC;
} INITCOMMONCONTROLSEX;

typedef struct {
    uint32_t mask;
    int32_t iItem;
    int32_t SubItem;
    uint32_t state;
    uint32_t stateMask;
    wchar_t *pszText;
    int32_t cchTextMax;
    int32_t iImage;
    LPARAM lParam;
    int32_t iIndent;
    int32_t iGroupId;
    uint32_t cColumns;
    uint32_t *puColumns;
    int32_t *piColFmt;
    int32_t iGroup;
} LVITEMW;

#define ICC_WIN95_CLASSES 0x000000FF;

extern bool __stdcall InitCommonControlsEx(const INITCOMMONCONTROLSEX *picce);

// Comdlg32
#define OFN_HIDEREADONLY 0x00000004
#define OFN_FILEMUSTEXIST 0x00001000
#define OFN_EXPLORER 0x00080000

typedef struct {
    uint32_t lStructSize;
    HWND hwndOwner;
    HINSTANCE hInstance;
    const wchar_t *lpstrFilter;
    wchar_t *lpstrCustomFilter;
    uint32_t nMaxCustFilter;
    uint32_t nFilterIndex;
    wchar_t *lpstrFile;
    uint32_t nMaxFile;
    wchar_t *lpstrFileTitle;
    uint32_t nMaxFileTitle;
    const wchar_t *lpstrInitialDir;
    const wchar_t *lpstrTitle;
    uint32_t Flags;
    uint16_t nFileOffset;
    uint16_t nFileExtension;
    const wchar_t *lpstrDefExt;
    LPARAM lCustData;
    void *lpfnHook;
    const wchar_t *lpTemplateName;
    void *lpEditInfo;
    const wchar_t *lpstrPrompt;
    void *pvReserved;
    uint32_t dwReserved;
    uint32_t FlagsEx;
} OPENFILENAMEW;

extern bool __stdcall GetOpenFileNameW(OPENFILENAMEW *unnamedParam1);
extern bool __stdcall GetSaveFileNameW(OPENFILENAMEW *unnamedParam1);

// Winmm
#define SND_ASYNC 0x00000001
#define SND_RESOURCE 0x00040004

extern bool __stdcall PlaySoundW(wchar_t *pszSound, HMODULE hmod, uint32_t fdwSound);

// Advapi
extern bool __stdcall GetUserNameW(wchar_t *lpBuffer, uint32_t *pcbBuffer);

// Dwmapi
typedef struct {
    int32_t cxLeftWidth;
    int32_t cxRightWidth;
    int32_t cyTopHeight;
    int32_t cyBottomHeight;
} MARGINS;

extern int32_t __stdcall DwmExtendFrameIntoClientArea(HWND hWnd, const MARGINS *pMarInset);

// Com
#define SUCCEEDED(hr) ((int32_t)(hr) >= 0)
#define FAILED(hr) ((int32_t)(hr) < 0)
#define S_OK ((int32_t)0)

typedef struct {
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t Data4[8];
} GUID;

typedef struct IUnknown IUnknown;

typedef struct {
    int32_t (__stdcall *QueryInterface)(IUnknown *This, GUID *riid, void **ppvObject);
    uint32_t (__stdcall *AddRef)(IUnknown *This);
    uint32_t (__stdcall *Release)(IUnknown *This);
} IUnknownVtbl;

struct IUnknown {
    const IUnknownVtbl *lpVtbl;
};

#define IUnknown_Release(ptr) ((IUnknown *)ptr)->lpVtbl->Release((IUnknown *)ptr);

// DirectWrite

// IDWriteTextFormat
typedef struct IDWriteTextFormat IDWriteTextFormat;

#define DWRITE_TEXT_ALIGNMENT_LEADING 0
#define DWRITE_TEXT_ALIGNMENT_TRAILING 1
#define DWRITE_TEXT_ALIGNMENT_CENTER 2

#define DWRITE_PARAGRAPH_ALIGNMENT_NEAR 0
#define DWRITE_PARAGRAPH_ALIGNMENT_FAR 1
#define DWRITE_PARAGRAPH_ALIGNMENT_CENTER 2

typedef struct {
    IUnknownVtbl Base;

    int32_t (__stdcall *SetTextAlignment)(IDWriteTextFormat *This, uint32_t textAlignment);
    int32_t (__stdcall *SetParagraphAlignment)(IDWriteTextFormat *This, uint32_t paragraphAlignment);
    uint8_t padding1[5 * sizeof(void *)];

    int32_t (__stdcall *SetLineSpacing)(IDWriteTextFormat *This, uint32_t spacing, float line_spacing, float baseline);
    uint8_t padding2[17 * sizeof(void *)];
} IDWriteTextFormatVtbl;

#define IDWriteTextFormat_SetTextAlignment(ptr, a) ((IDWriteTextFormat *)ptr)->lpVtbl->SetTextAlignment((IDWriteTextFormat *)ptr, a)
#define IDWriteTextFormat_SetParagraphAlignment(ptr, a) ((IDWriteTextFormat *)ptr)->lpVtbl->SetParagraphAlignment((IDWriteTextFormat *)ptr, a)
#define IDWriteTextFormat_SetLineSpacing(ptr, a, b, c) ((IDWriteTextFormat *)ptr)->lpVtbl->SetLineSpacing((IDWriteTextFormat *)ptr, a, b, c)

struct IDWriteTextFormat {
    const IDWriteTextFormatVtbl *lpVtbl;
};

// IDWriteFactory
typedef struct IDWriteFactory IDWriteFactory;

#define DWRITE_FONT_WEIGHT_NORMAL 4
#define DWRITE_FONT_STYLE_NORMAL 0
#define DWRITE_FONT_STRETCH_NORMAL 5

typedef struct {
    IUnknownVtbl Base;
    uint8_t padding1[12 * sizeof(void *)];

    int32_t (__stdcall *CreateTextFormat)(IDWriteFactory *This, const wchar_t *family_name, void *collection, uint32_t weight, uint32_t style, uint32_t stretch, float size, const wchar_t *locale, IDWriteTextFormat **format);
    uint8_t padding2[8 * sizeof(void *)];
} IDWriteFactoryVtbl;

#define IDWriteFactory_CreateTextFormat(ptr, a, b, c, d, e, f, g, h) ((IDWriteFactory *)ptr)->lpVtbl->CreateTextFormat((IDWriteFactory *)ptr, a, b, c, d, e, f, g, h)

struct IDWriteFactory {
    const IDWriteFactoryVtbl *lpVtbl;
};

#define DWRITE_FACTORY_TYPE_SHARED 0

extern int32_t __stdcall DWriteCreateFactory(uint32_t factoryType, GUID *riid, IDWriteFactory **factory);

// Direct2D
typedef struct {
    float r;
    float g;
    float b;
    float a;
} D2D1_COLOR_F;

typedef struct {
    float x;
    float y;
} D2D1_POINT_2F;

typedef struct {
    uint32_t width;
    uint32_t height;
} D2D1_SIZE_U;

typedef struct {
    float left;
    float top;
    float right;
    float bottom;
} D2D1_RECT_F;

#define DXGI_FORMAT_UNKNOWN 0
#define D2D1_ALPHA_MODE_UNKNOWN 0

typedef struct {
  uint32_t format;
  uint32_t alphaMode;
} D2D1_PIXEL_FORMAT;

#define D2D1_RENDER_TARGET_TYPE_DEFAULT 0

#define D2D1_RENDER_TARGET_USAGE_NONE 0

#define D2D1_FEATURE_LEVEL_DEFAULT 0

typedef struct {
  uint32_t type;
  D2D1_PIXEL_FORMAT pixelFormat;
  float dpiX;
  float dpiY;
  uint32_t usage;
  uint32_t minLevel;
} D2D1_RENDER_TARGET_PROPERTIES;

#define D2D1_PRESENT_OPTIONS_NONE 0

typedef struct {
  HWND hwnd;
  D2D1_SIZE_U pixelSize;
  uint32_t presentOptions;
} D2D1_HWND_RENDER_TARGET_PROPERTIES;

// ID2D1Factory (forward define)
typedef struct ID2D1Factory ID2D1Factory;

// ID2D1Resource
typedef struct ID2D1Resource ID2D1Resource;

typedef struct {
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

typedef struct {
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

typedef struct {
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

typedef struct {
    ID2D1ResourceVtbl Base;
    uint8_t padding1[13 * sizeof(void *)];
} ID2D1GeometryVtbl;

struct ID2D1Geometry {
    const ID2D1GeometryVtbl *lpVtbl;
};

// ID2D1PathGeometry
typedef struct ID2D1PathGeometry ID2D1PathGeometry;

typedef struct {
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

typedef struct {
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

typedef struct {
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
typedef struct {
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

// Multimon, Windows 8.1 DPI shit...
#define MONITOR_DEFAULTTONEAREST 0x00000002

#define MDT_EFFECTIVE_DPI 0

#define PROCESS_PER_MONITOR_DPI_AWARE 2

// Stdlib functions
#if defined(WIN32_MALLOC) || defined(WIN32_WCSDUP)
    void *malloc(size_t size) {
        return HeapAlloc(GetProcessHeap(), 0, size);
    }
#endif

#ifdef WIN32_REALLOC
    void *realloc(void *ptr, size_t size) {
        return HeapReAlloc(GetProcessHeap(), 0, ptr, size);
    }
#endif

#ifdef WIN32_FREE
    void free(void *ptr) {
        HeapFree(GetProcessHeap(), 0, ptr);
    }
#endif

#ifdef WIN32_RAND
    uint32_t rand_seed;

    void srand(uint32_t seed) {
        rand_seed = seed;
    }

    uint32_t rand(void) {
        return rand_seed = rand_seed * 1103515245 + 12345;
    }
#endif

#if defined(WIN32_WCSLEN) || defined(WIN32_WCSDUP)
    size_t wcslen(wchar_t *string) {
        wchar_t *start = string;
        while (*string++ != '\0');
        return string - start - 1;
    }
#endif

#if defined(WIN32_WCSCPY) || defined(WIN32_WCSDUP)
    wchar_t *wcscpy(wchar_t *dest, wchar_t *src) {
        wchar_t *start = dest;
        while ((*dest++ = *src++) != '\0');
        return start;
    }
#endif

#ifdef WIN32_WCSCAT
    wchar_t *wcscat(wchar_t *dest, wchar_t *src) {
        wchar_t *start = dest;
        while (*dest++ != '\0');
        dest--;
        while ((*dest++ = *src++) != '\0');
        *dest = '\0';
        return start;
    }
#endif

#ifdef WIN32_WCSDUP
    wchar_t *wcsdup(wchar_t *src) {
        wchar_t *dst = malloc((wcslen(src) + 1) * sizeof(wchar_t));
        if (dst == NULL) return NULL;
        wcscpy(dst, src);
        return dst;
    }
#endif

#ifdef WIN32_WPRINTF
    void wprintf(wchar_t *format, ...) {
        wchar_t string_buffer[256];
        va_list args;
        va_start(args, format);
        wvsprintfW(string_buffer, format, args);
        va_end(args);
        WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), string_buffer, wcslen(string_buffer), NULL, NULL);
    }
#endif

#endif
