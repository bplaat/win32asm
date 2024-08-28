// ### FileDialog Header ###
// A simple recreation of the Windows COM File dialog headers to compile with a limited compiler like TCC
// ~ Made by Bastiaan van der Plaat (https://bastiaan.ml/)

#pragma once

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
