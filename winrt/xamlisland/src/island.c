#define UNICODE
#include <windows.h>
#include <stdio.h>
#include "../res/resource.h"

// ####################################################################################
// ######################## Windows Runtime UWP XAML Header ###########################
// ####################################################################################

// roapi.h
typedef HANDLE HSTRING;
typedef enum RO_INIT_TYPE { RO_INIT_SINGLETHREADED, RO_INIT_MULTITHREADED } RO_INIT_TYPE;
extern HRESULT RoInitialize(RO_INIT_TYPE initType);
extern void RoUninitialize(void);
extern HRESULT RoGetActivationFactory(HSTRING activatableClassId, REFIID iid, void **factory);
extern HRESULT WindowsCreateString(LPCWSTR sourceString, UINT32 length, HSTRING *string);
extern LPCWSTR WindowsGetStringRawBuffer(HSTRING string, UINT32 *length);
extern HRESULT WindowsDeleteString(HSTRING string);

// Hstring helper
HSTRING hstr(wchar_t *string) {
    HSTRING hstring;
    WindowsCreateString(string, wcslen(string), &hstring);
    return hstring;
}

// IInspectable
typedef struct IInspectable IInspectable;

typedef struct IInspectableVtbl {
    IUnknownVtbl base;
    void *padding[3];
} IInspectableVtbl;

struct IInspectable {
    const IInspectableVtbl *lpVtbl;
};

#define IInspectable_QueryInterface(This, riid, ppvObject) ((IUnknown *)This)->lpVtbl->QueryInterface((IUnknown *)This, riid, ppvObject)
#define IInspectable_Release(This) ((IUnknown *)This)->lpVtbl->Release((IUnknown *)This)

// // IWindowsXamlManager
// typedef struct IWindowsXamlManager IWindowsXamlManager;

// typedef struct IWindowsXamlManagerVtbl {
//     IInspectableVtbl base;
// } IWindowsXamlManagerVtbl;

// struct IWindowsXamlManager {
//     const IWindowsXamlManagerVtbl *lpVtbl;
// };

// #define IWindowsXamlManager_Release(This) ((IUnknown *)This)->lpVtbl->Release((IUnknown *)This)

// // IWindowsXamlManagerStatics
// typedef struct IWindowsXamlManagerStatics IWindowsXamlManagerStatics;

// typedef struct IWindowsXamlManagerStaticsVtbl {
//     IInspectableVtbl base;
//     HRESULT (STDMETHODCALLTYPE *InitializeForCurrentThread)(IWindowsXamlManagerStatics *This, IWindowsXamlManager **result);
// } IWindowsXamlManagerStaticsVtbl;

// struct IWindowsXamlManagerStatics {
//     const IWindowsXamlManagerStaticsVtbl *lpVtbl;
// };

// const GUID IID_IWindowsXamlManagerStatics = { 0x28258A12, 0x7D82, 0x505B, { 0xB2, 0x10, 0x71, 0x2B, 0x04, 0xA5, 0x88, 0x82 } };

// #define IWindowsXamlManagerStatics_InitializeForCurrentThread(This, result) ((IWindowsXamlManagerStatics *)This)->lpVtbl\
//     ->InitializeForCurrentThread((IWindowsXamlManagerStatics *)This, result)
// #define IWindowsXamlManagerStatics_Release(This) ((IUnknown *)This)->lpVtbl->Release((IUnknown *)This)

// IDesktopWindowXamlSource
typedef struct IDesktopWindowXamlSource IDesktopWindowXamlSource;

typedef struct IDesktopWindowXamlSourceVtbl {
    IInspectableVtbl base;
    void *padding1[1];
    HRESULT (STDMETHODCALLTYPE *put_Content)(IDesktopWindowXamlSource *This, void *value); // IUIElement;
    void *padding2[6];
} IDesktopWindowXamlSourceVtbl;

struct IDesktopWindowXamlSource {
    const IDesktopWindowXamlSourceVtbl *lpVtbl;
};

#define IDesktopWindowXamlSource_put_Content(This, value) ((IDesktopWindowXamlSource *)This)->lpVtbl->put_Content((IDesktopWindowXamlSource *)This, value)
#define IDesktopWindowXamlSource_QueryInterface(This, riid, ppvObject) ((IUnknown *)This)->lpVtbl->QueryInterface((IUnknown *)This, riid, ppvObject)
#define IDesktopWindowXamlSource_Release(This) ((IUnknown *)This)->lpVtbl->Release((IUnknown *)This)

// IDesktopWindowXamlSourceFactory
typedef struct IDesktopWindowXamlSourceFactory IDesktopWindowXamlSourceFactory;

typedef struct IDesktopWindowXamlSourceFactoryVtbl {
    IInspectableVtbl base;
    HRESULT (STDMETHODCALLTYPE *CreateInstance)(IDesktopWindowXamlSourceFactory *This, IInspectable *baseInterface, IInspectable **innerInterface, IDesktopWindowXamlSource **value);
} IDesktopWindowXamlSourceFactoryVtbl;

struct IDesktopWindowXamlSourceFactory {
    const IDesktopWindowXamlSourceFactoryVtbl *lpVtbl;
};

const GUID IID_IDesktopWindowXamlSourceFactory = { 0x5CD61DC0, 0x2561, 0x56E1, { 0x8E, 0x75, 0x6E, 0x44, 0x17, 0x38, 0x05, 0xE3 } };

#define IDesktopWindowXamlSourceFactory_CreateInstance(This, baseInterface, innerInterface, value) ((IDesktopWindowXamlSourceFactory *)This)->lpVtbl\
    ->CreateInstance((IDesktopWindowXamlSourceFactory *)This, baseInterface, innerInterface, value)
#define IDesktopWindowXamlSourceFactory_Release(This) ((IUnknown *)This)->lpVtbl->Release((IUnknown *)This)

// IDesktopWindowXamlSourceNative
typedef struct IDesktopWindowXamlSourceNative IDesktopWindowXamlSourceNative;

typedef struct IDesktopWindowXamlSourceNativeVtbl {
    IUnknownVtbl base;
    HRESULT (STDMETHODCALLTYPE *AttachToWindow)(IDesktopWindowXamlSourceNative *This, HWND parentWnd);
    HRESULT (STDMETHODCALLTYPE *get_WindowHandle)(IDesktopWindowXamlSourceNative *This, HWND *hWnd);
} IDesktopWindowXamlSourceNativeVtbl;

struct IDesktopWindowXamlSourceNative {
    const IDesktopWindowXamlSourceNativeVtbl *lpVtbl;
};

const GUID IID_IDesktopWindowXamlSourceNative = { 0x3cbcf1bf, 0x2f76, 0x4e9c, { 0x96, 0xab, 0xe8, 0x4b, 0x37, 0x97, 0x25, 0x54 } };

#define IDesktopWindowXamlSourceNative_AttachToWindow(This, parentWnd) ((IDesktopWindowXamlSourceNative *)This)->lpVtbl\
    ->AttachToWindow((IDesktopWindowXamlSourceNative *)This, parentWnd)
#define IDesktopWindowXamlSourceNative_get_WindowHandle(This, hWnd) ((IDesktopWindowXamlSourceNative *)This)->lpVtbl\
    ->get_WindowHandle((IDesktopWindowXamlSourceNative *)This, hWnd)
#define IDesktopWindowXamlSourceNative_Release(This) ((IUnknown *)This)->lpVtbl->Release((IUnknown *)This)

// IXamlReaderStatics
typedef struct IXamlReaderStatics IXamlReaderStatics;

typedef struct IXamlReaderStaticsVtbl {
    IInspectableVtbl base;
    HRESULT (STDMETHODCALLTYPE *Load)(IXamlReaderStatics *This, HSTRING xaml, IInspectable **result);
    void *padding[1];
} IXamlReaderStaticsVtbl;

struct IXamlReaderStatics {
    const IXamlReaderStaticsVtbl *lpVtbl;
};

const GUID IID_IXamlReaderStatics = { 0x9891C6BD, 0x534F, 0x4955, { 0xb8, 0x5a, 0x8a, 0x8d, 0xc0, 0xdc, 0xa6, 0x02 } };

#define IXamlReaderStatics_Load(This, xaml, result) ((IXamlReaderStatics *)This)->lpVtbl->Load((IXamlReaderStatics *)This, xaml, result)
#define IXamlReaderStatics_Release(This) ((IUnknown *)This)->lpVtbl->Release((IUnknown *)This)

// IUIElement
typedef struct IUIElement IUIElement;

typedef struct IUIElementVtbl {
    IInspectableVtbl base;
} IUIElementVtbl;

struct IUIElement {
    const IUIElementVtbl *lpVtbl;
};

const GUID IID_IUIElement = { 0x676D0BE9,0xB65C,0x41C6,{ 0xba,0x40,0x58,0xcf,0x87,0xf2,0x01,0xc1 } };

#define IUIElement_Release(This) ((IUnknown *)This)->lpVtbl->Release((IUnknown *)This)

// ####################################################################################
// ####################################################################################
// ####################################################################################

#ifndef WS_EX_NOREDIRECTIONBITMAP
    #define WS_EX_NOREDIRECTIONBITMAP 0x00200000L
#endif

#define DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 19
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
typedef HRESULT (STDMETHODCALLTYPE *_DwmSetWindowAttribute)(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute);

HWND xamlHwnd = NULL;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_SIZE) {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);
        if (xamlHwnd != NULL) {
            SetWindowPos(xamlHwnd, NULL, 0, 0, width, height, SWP_NOZORDER);
        }
        return 0;
    }

    if (msg == WM_GETMINMAXINFO) {
        RECT windowRect = { 0, 0, 320, 240 };
        AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, 0);
        MINMAXINFO *minMaxInfo = (MINMAXINFO *)lParam;
        minMaxInfo->ptMinTrackSize.x = windowRect.right - windowRect.left;
        minMaxInfo->ptMinTrackSize.y = windowRect.bottom - windowRect.top;
        return 0;
    }

    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    RoInitialize(RO_INIT_SINGLETHREADED);

    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(ID_ICON_APP), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_SHARED);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"xaml-island";
    wc.hIconSm = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(ID_ICON_APP), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR | LR_SHARED);
    RegisterClassEx(&wc);

    #define WINDOW_WIDTH 1024
    #define WINDOW_HEIGHT 768
    RECT windowRect;
    windowRect.left = (GetSystemMetrics(SM_CXSCREEN) - WINDOW_WIDTH) / 2;
    windowRect.top = (GetSystemMetrics(SM_CYSCREEN) - WINDOW_HEIGHT) / 2;
    windowRect.right = windowRect.left + WINDOW_WIDTH;
    windowRect.bottom = windowRect.top + WINDOW_HEIGHT;
    AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, 0);

    HWND hwnd = CreateWindowEx(WS_EX_NOREDIRECTIONBITMAP, wc.lpszClassName, L"Windows WinRT XAML Island Example",
        WS_OVERLAPPEDWINDOW, windowRect.left, windowRect.top,
        windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
        HWND_DESKTOP, NULL, hInstance, NULL);

    HMODULE hdwmapi = LoadLibrary(L"dwmapi.dll");
    _DwmSetWindowAttribute DwmSetWindowAttribute = (_DwmSetWindowAttribute)GetProcAddress(hdwmapi, "DwmSetWindowAttribute");
    if (DwmSetWindowAttribute != NULL) {
        BOOL enabled = TRUE;
        if (FAILED(DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &enabled, sizeof(BOOL)))) {
            DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1, &enabled, sizeof(BOOL));
        }
    }

    // IWindowsXamlManagerStatics *windowsXamlManagerStatics;
    // HSTRING windowsXamlManagerClassId = hstr(L"Windows.UI.Xaml.Hosting.WindowsXamlManager");
    // RoGetActivationFactory(windowsXamlManagerClassId, &IID_IWindowsXamlManagerStatics, &windowsXamlManagerStatics);
    // WindowsDeleteString(windowsXamlManagerClassId);

    // IWindowsXamlManager *windowsXamlManager;
    // IWindowsXamlManagerStatics_InitializeForCurrentThread(windowsXamlManagerStatics, &windowsXamlManager);
    // IWindowsXamlManagerStatics_Release(windowsXamlManagerStatics);

    IDesktopWindowXamlSourceFactory *desktopWindowXamlSourceFactory;
    HSTRING desktopWindowXamlSourceClassId = hstr(L"Windows.UI.Xaml.Hosting.DesktopWindowXamlSource");
    RoGetActivationFactory(desktopWindowXamlSourceClassId, &IID_IDesktopWindowXamlSourceFactory, &desktopWindowXamlSourceFactory);
    WindowsDeleteString(desktopWindowXamlSourceClassId);

    IDesktopWindowXamlSource *desktopSource;
    IInspectable *innerInterface = NULL;
    IDesktopWindowXamlSourceFactory_CreateInstance(desktopWindowXamlSourceFactory, NULL, &innerInterface, &desktopSource);
    IDesktopWindowXamlSourceFactory_Release(desktopWindowXamlSourceFactory);

    IDesktopWindowXamlSourceNative *interop;
    IDesktopWindowXamlSource_QueryInterface(desktopSource, &IID_IDesktopWindowXamlSourceNative, &interop);

    IDesktopWindowXamlSourceNative_AttachToWindow(interop, hwnd);
    IDesktopWindowXamlSourceNative_get_WindowHandle(interop, &xamlHwnd);
    SetWindowPos(xamlHwnd, NULL, 0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_NOZORDER | SWP_SHOWWINDOW);

    IXamlReaderStatics *xamlReaderStatics;
    HSTRING xamlReaderClassId = hstr(L"Windows.UI.Xaml.Markup.XamlReader");
    RoGetActivationFactory(xamlReaderClassId, &IID_IXamlReaderStatics, &xamlReaderStatics);
    WindowsDeleteString(xamlReaderClassId);

    HRSRC hsrc = FindResource(NULL, (wchar_t *)ID_XAML_WINDOW, L"XAML");
    char *xamlStringAscii = LockResource(LoadResource(NULL, hsrc));
    DWORD xamlStringAsciiSize = SizeofResource(NULL, hsrc);
    wchar_t *xamlStringWide = malloc((xamlStringAsciiSize + 1) * sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, 0, xamlStringAscii, xamlStringAsciiSize, xamlStringWide, xamlStringAsciiSize + 1);

    IInspectable *rootInstance;
    HSTRING xamlHstring = hstr(xamlStringWide);
    free(xamlStringWide);
    if (FAILED(IXamlReaderStatics_Load(xamlReaderStatics, xamlHstring, &rootInstance))) {
        MessageBox(hwnd, L"There is an error in your XAML layout!", L"XAML Error!", MB_ICONERROR | MB_OK);
        return 1;
    }
    WindowsDeleteString(xamlHstring);
    IXamlReaderStatics_Release(xamlReaderStatics);

    IUIElement *rootElement = NULL;
    IInspectable_QueryInterface(rootInstance, &IID_IUIElement, &rootElement);
    IDesktopWindowXamlSource_put_Content(desktopSource, rootElement);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    IUIElement_Release(rootElement);
    IInspectable_Release(rootInstance);
    IDesktopWindowXamlSourceNative_Release(interop);
    IDesktopWindowXamlSource_Release(desktopSource);
    RoUninitialize();

    return msg.wParam;
}
