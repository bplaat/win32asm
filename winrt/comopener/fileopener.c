// A simple Windows application which opens a COM file picker
// tcc fileopener.c -lole32 && ./fileopener

#define UNICODE
#include <windows.h>

// combaseapi.h
#define COINIT_APARTMENTTHREADED (DWORD)0x2
#define COINIT_DISABLE_OLE1DDE (DWORD)0x4
#define CLSCTX_ALL (DWORD)0x17
HRESULT CoInitializeEx(LPVOID pvReserved, DWORD dwCoInit);
void CoUninitialize(void);
HRESULT CoCreateInstance(REFIID rclsid, LPVOID pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID *ppv);
HRESULT CoTaskMemFree(LPVOID pv);

// unknwn.h
typedef struct IUnknown IUnknown;

typedef struct IUnknownVtbl {
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IUnknown *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IUnknown *This);
    ULONG (STDMETHODCALLTYPE *Release)(IUnknown *This);
} IUnknownVtbl;

struct IUnknown {
    const IUnknownVtbl *lpVtbl;
};

// ####################################################################################
// ########################## COM Open File Dialog Header #############################
// ####################################################################################

// IShellItem
#define SIGDN_FILESYSPATH (int)0x80058000

typedef struct IShellItem IShellItem;

typedef struct IShellItemVtbl {
    IUnknownVtbl base;
    void *padding1[2];
    HRESULT (STDMETHODCALLTYPE *GetDisplayName)(IShellItem *This, int sigdnName, LPWSTR *ppszName);
    void *padding2[2];
} IShellItemVtbl;

struct IShellItem {
    const IShellItemVtbl *lpVtbl;
};

#define IShellItem_GetDisplayName(This, sigdnName, ppszName) ((IShellItem *)This)->lpVtbl->GetDisplayName((IShellItem *)This, sigdnName, ppszName)
#define IShellItem_Release(This) ((IUnknown *)This)->lpVtbl->Release((IUnknown *)This)

// IModalWindow
typedef struct IModalWindow IModalWindow;

typedef struct IModalWindowVtbl {
    IUnknownVtbl base;
    HRESULT (STDMETHODCALLTYPE *Show)(IModalWindow *This, HANDLE hwndOwner);
} IModalWindowVtbl;

struct IModalWindow {
    const IModalWindowVtbl *lpVtbl;
};

// IFileDialog
typedef struct _COMDLG_FILTERSPEC {
    LPCWSTR pszName;
    LPCWSTR pszSpec;
} COMDLG_FILTERSPEC;

typedef struct IFileDialog IFileDialog;

typedef struct IFileDialogVtbl {
    IModalWindowVtbl base;
    HRESULT (STDMETHODCALLTYPE *SetFileTypes)(IFileDialog *This, UINT cFileTypes, const COMDLG_FILTERSPEC *rgFilterSpec);
    void *padding1[12];
    HRESULT (STDMETHODCALLTYPE *SetTitle)(IFileDialog *This, LPCWSTR pszText);
    void *padding2[2];
    HRESULT (STDMETHODCALLTYPE *GetResult)(IFileDialog *This, IShellItem **ppsi);
    void *padding3[6];
} IFileDialogVtbl;

struct IFileDialog {
    const IFileDialogVtbl *lpVtbl;
};

// IFileOpenDialog
typedef struct IFileOpenDialog IFileOpenDialog;

typedef struct IFileOpenDialogVtbl {
    IFileDialogVtbl base;
    void *padding[2];
} IFileOpenDialogVtbl;

const GUID CLSID_FileOpenDialog = { 0xdc1c5a9c, 0xe88a, 0x4dde, { 0xa5, 0xa1, 0x60, 0xf8, 0x2a, 0x20, 0xae, 0xf7 } };
const GUID IID_IFileOpenDialog = { 0xd57c7288, 0xd4ad, 0x4768, { 0xbe, 0x02, 0x9d, 0x96, 0x95, 0x32, 0xd9, 0x60 } };

struct IFileOpenDialog {
    const IFileOpenDialogVtbl *lpVtbl;
};

#define IFileOpenDialog_Show(This, hwndOwner) ((IModalWindow *)This)->lpVtbl->Show((IModalWindow *)This, hwndOwner)
#define IFileOpenDialog_SetFileTypes(This, cFileTypes, rgFilterSpec) ((IFileDialog *)This)->lpVtbl->SetFileTypes((IFileDialog *)This, cFileTypes, rgFilterSpec)
#define IFileOpenDialog_SetTitle(This, pszText) ((IFileDialog *)This)->lpVtbl->SetTitle((IFileDialog *)This, pszText)
#define IFileOpenDialog_GetResult(This, ppsi) ((IFileDialog *)This)->lpVtbl->GetResult((IFileDialog *)This, ppsi)
#define IFileOpenDialog_Release(This) ((IUnknown *)This)->lpVtbl->Release((IUnknown *)This)

// ####################################################################################
// ####################################################################################
// ####################################################################################

#define DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 19
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
typedef HRESULT (STDMETHODCALLTYPE *_DwmSetWindowAttribute)(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute);

void OpenFileDialog(HWND hwnd) {
    IFileOpenDialog *fileDialog;
    if (SUCCEEDED(CoCreateInstance(&CLSID_FileOpenDialog, NULL, CLSCTX_ALL, &IID_IFileOpenDialog, (void *)&fileDialog))) {
        if (SUCCEEDED(IFileOpenDialog_Show(fileDialog, NULL))) {
            IShellItem *item;
            if (SUCCEEDED(IFileOpenDialog_GetResult(fileDialog, &item))) {
                LPWSTR filePath;
                if (SUCCEEDED(IShellItem_GetDisplayName(item, SIGDN_FILESYSPATH, &filePath))) {
                    MessageBox(hwnd, filePath, L"File Path", MB_ICONINFORMATION | MB_OK);
                    CoTaskMemFree(filePath);
                }
                IShellItem_Release(item);
            }
        }
        IFileOpenDialog_Release(fileDialog);
    }
}

#define BUTTON_ID 1

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_CREATE) {
        HWND button = CreateWindowEx(0, L"BUTTON", L"Open File",
            WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)BUTTON_ID, NULL, NULL);
        SendMessage(button, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
        return 0;
    }

    if (msg == WM_SIZE) {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);
        HWND button = GetDlgItem(hwnd, BUTTON_ID);
        SetWindowPos(button, NULL, (width - 100) / 2, (height - 32) / 2, 100, 32, SWP_NOZORDER);
        return 0;
    }

    if (msg == WM_COMMAND) {
        int id = LOWORD(wParam);
        if (id == BUTTON_ID) {
            OpenFileDialog(hwnd);
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
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"fileopener";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassEx(&wc);

    #define WINDOW_WIDTH 1024
    #define WINDOW_HEIGHT 768
    RECT windowRect;
    windowRect.left = (GetSystemMetrics(SM_CXSCREEN) - WINDOW_WIDTH) / 2;
    windowRect.top = (GetSystemMetrics(SM_CYSCREEN) - WINDOW_HEIGHT) / 2;
    windowRect.right = windowRect.left + WINDOW_WIDTH;
    windowRect.bottom = windowRect.top + WINDOW_HEIGHT;
    AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, 0);

    HWND hwnd = CreateWindowEx(0, wc.lpszClassName, L"Windows File Opener Example",
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

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    CoUninitialize();
    return msg.wParam;
}
