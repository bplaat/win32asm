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
#define HRGN HANDLE
#define HRSRC HANDLE
#define HGLOBAL HANDLE
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

#define CREATE_NEW 1
#define CREATE_ALWAYS 2
#define OPEN_EXISTING 3
#define OPEN_ALWAYS 4
#define TRUNCATE_EXISTING 5

#define FILE_ATTRIBUTE_NORMAL 0x80

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

extern void __stdcall __attribute__((noreturn)) ExitProcess(uint32_t uExitCode);
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
#define WS_BORDER 0x000800000
#define WS_OVERLAPPEDWINDOW 0x00CF0000
#define WS_THICKFRAME 0x000040000
#define WS_MAXIMIZEBOX 0x000010000
#define WS_CLIPCHILDREN 0x02000000

#define WS_EX_CLIENTEDGE 0x00000200

#define SW_HIDE 0
#define SW_SHOWNORMAL 1
#define SW_SHOW 5
#define SW_SHOWDEFAULT 10

#define WM_CREATE 0x0001
#define WM_DESTROY 0x0002
#define WM_SIZE 0x0005
#define WM_SETTEXT 0x000C
#define WM_GETTEXT 0x000D
#define WM_PAINT 0x000F
#define WM_QUIT 0x0012
#define WM_ERASEBKGND 0x0014
#define WM_GETMINMAXINFO 0x0024
#define WM_SETFONT 0x0030
#define WM_COMMAND 0x0111
#define WM_TIMER 0x0113
#define WM_MOUSEMOVE 0x0200
#define WM_LBUTTONDOWN 0x0201
#define WM_LBUTTONUP 0x0202

#define EM_SETLIMITTEXT 0x00C5

#define PM_REMOVE 0x0001

#define SM_CXSCREEN 0
#define SM_CYSCREEN 1
#define SM_CXSMICON 49
#define SM_CYSMICON 50

#define SWP_NOSIZE 0x0001
#define SWP_NOMOVE 0x0002
#define SWP_NOZORDER 0x0004

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
    wchar_t *lpszMenuName;
    wchar_t *lpszClassName;
    HICON hIconSm;
} WNDCLASSEXW;

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
extern HDC __stdcall BeginPaint(HWND hWnd, PAINTSTRUCT *lpPaint);
extern bool __stdcall EndPaint(HWND hWnd, PAINTSTRUCT *lpPaint);
extern int32_t __stdcall FillRect(HDC hDC, const RECT *lprc, HBRUSH hbr);
extern int32_t __stdcall FrameRect(HDC hDC, const RECT *lprc, HBRUSH hbr);
extern bool __stdcall InvalidateRect(HWND hWnd, const RECT *lpRect, bool bErase);
extern bool __stdcall IsIconic(HWND hWnd);
extern HWND __stdcall FindWindowW(const wchar_t *lpClassName, const wchar_t *lpWindowName);
extern bool __stdcall SetForegroundWindow(HWND hWnd);
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

#define RGB(r, g, b) ((r & 0xff) | ((g & 0xff) << 8) | ((b & 0xff) << 16))

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
extern HFONT __stdcall CreateFontW(int32_t cHeight, int32_t cWidth, int32_t cEscapement, int32_t cOrientation,
    int32_t cWeight, uint32_t bItalic, uint32_t bUnderline, uint32_t bStrikeOut, uint32_t iCharSet,
    uint32_t iOutPrecision, uint32_t iClipPrecision, uint32_t iQuality, uint32_t iPitchAndFamily, wchar_t *pszFaceName);
extern int32_t __stdcall SetBkMode(HDC hdc, int32_t mode);
extern uint32_t __stdcall SetTextColor(HDC hdc, uint32_t color);
extern uint32_t __stdcall SetTextAlign(HDC hdc, uint32_t align);
extern bool __stdcall TextOutW(HDC hdc, int32_t x, int32_t y, wchar_t *lpString, int32_t c);

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

#define SmoothingModeAntiAlias 5

extern uint32_t __stdcall GdiplusStartup(uint32_t *token, const GdiplusStartupInput *input, void *output);
extern void __stdcall GdiplusShutdown(uint32_t *token);
extern uint32_t __stdcall GdipCreateFromHDC(HDC hdc, GpGraphics **graphics);
extern uint32_t __stdcall GdipSetSmoothingMode(GpGraphics *graphics, int32_t smoothingMode);
extern uint32_t __stdcall GdipGraphicsClear(GpGraphics *graphics, uint32_t color);
extern uint32_t __stdcall GdipDeleteGraphics(GpGraphics *graphics);
extern uint32_t __stdcall GdipFillRectangle(GpGraphics *graphics, GpBrush *brush, float x, float y, float width, float height);
extern uint32_t __stdcall GdipFillRectangleI(GpGraphics *graphics, GpBrush *brush, int32_t x, int32_t y, int32_t width, int32_t height);
extern uint32_t __stdcall GdipCreateSolidFill(uint32_t color, GpSolidFill **brush);
extern uint32_t __stdcall GdipDeleteBrush(GpBrush *brush);

// Shell32
#define CSIDL_COMMON_APPDATA 0x0023
#define CSIDL_LOCAL_APPDATA 0x001C

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

// Winmm
#define SND_ASYNC 0x00000001
#define SND_RESOURCE 0x00040004

extern bool __stdcall PlaySoundW(wchar_t *pszSound, HMODULE hmod, uint32_t fdwSound);

// Advapi
extern bool __stdcall GetUserNameW(wchar_t *lpBuffer, uint32_t *pcbBuffer);

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

    inline void srand(uint32_t seed) {
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
        return string - start;
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

#endif
