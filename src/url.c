// A simple Windows application which parses an URL via the WinRT APIs
// tcc url.c -lcombase -o url.exe && ./url.exe

#define UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <unknwn.h>
#include <stdio.h>

// ####################################################################################
// ########################### Windows Runtime URI Header #############################
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

// IInspectable
typedef struct IInspectable IInspectable;

typedef struct IInspectableVtbl {
    IUnknownVtbl base;
    void *padding[3];
} IInspectableVtbl;

struct IInspectable {
    const IInspectableVtbl *lpVtbl;
};

// IUriRuntimeClass
typedef struct IUriRuntimeClass IUriRuntimeClass;

typedef struct IUriRuntimeClassVtbl {
    IInspectableVtbl base;
    HRESULT (STDMETHODCALLTYPE *get_AbsoluteUri)(IUriRuntimeClass *This, HSTRING *value);
    HRESULT (STDMETHODCALLTYPE *get_DisplayUri)(IUriRuntimeClass *This, HSTRING *value);
    HRESULT (STDMETHODCALLTYPE *get_Domain)(IUriRuntimeClass *This, HSTRING *value);
    void *padding1[4];
    HRESULT (STDMETHODCALLTYPE *get_Path)(IUriRuntimeClass *This, HSTRING *value);
    void *padding2[9];
} IUriRuntimeClassVtbl;

struct IUriRuntimeClass {
    const IUriRuntimeClassVtbl *lpVtbl;
};

#define IUriRuntimeClass_get_AbsoluteUri(This, value) ((IUriRuntimeClass *)This)->lpVtbl->get_AbsoluteUri((IUriRuntimeClass *)This, value)
#define IUriRuntimeClass_get_DisplayUri(This, value) ((IUriRuntimeClass *)This)->lpVtbl->get_DisplayUri((IUriRuntimeClass *)This, value)
#define IUriRuntimeClass_get_Domain(This, value) ((IUriRuntimeClass *)This)->lpVtbl->get_Domain((IUriRuntimeClass *)This, value)
#define IUriRuntimeClass_get_Path(This, value) ((IUriRuntimeClass *)This)->lpVtbl->get_Path((IUriRuntimeClass *)This, value)
#define IUriRuntimeClass_Release(This) ((IUnknown *)This)->lpVtbl->Release((IUnknown *)This)

// IUriRuntimeClassFactory
typedef struct IUriRuntimeClassFactory IUriRuntimeClassFactory;

typedef struct IUriRuntimeClassFactoryVtbl {
    IInspectableVtbl base;
    HRESULT (STDMETHODCALLTYPE *CreateUri)(IUriRuntimeClassFactory *This, HSTRING uri, IUriRuntimeClass **instance);
    void *padding[1];
} IUriRuntimeClassFactoryVtbl;

struct IUriRuntimeClassFactory {
    const IUriRuntimeClassFactoryVtbl *lpVtbl;
};

const GUID IID_IUriRuntimeClassFactory = { 0x44A9796F, 0x723E, 0x4FDF, { 0xA2, 0x18, 0x03, 0x3E, 0x75, 0xB0, 0xC0, 0x84 } };

#define IUriRuntimeClassFactory_CreateUri(This, uri, instance) ((IUriRuntimeClassFactory *)This)->lpVtbl->CreateUri((IUriRuntimeClassFactory *)This, uri, instance)
#define IUriRuntimeClassFactory_Release(This) ((IUnknown *)This)->lpVtbl->Release((IUnknown *)This)

// Hstring helper
HSTRING hstr(wchar_t *string) {
    HSTRING hstring;
    WindowsCreateString(string, wcslen(string), &hstring);
    return hstring;
}

// ####################################################################################
// ####################################################################################
// ####################################################################################

int main(void) {
    RoInitialize(RO_INIT_SINGLETHREADED);

    IUriRuntimeClassFactory *uriFactory;
    HSTRING uriClassId = hstr(L"Windows.Foundation.Uri");
    RoGetActivationFactory(uriClassId, &IID_IUriRuntimeClassFactory, &uriFactory);
    WindowsDeleteString(uriClassId);

    IUriRuntimeClass *uri;
    HSTRING uriString = hstr(L"https://bastiaan.ml/winrt/stuff/nice");
    IUriRuntimeClassFactory_CreateUri(uriFactory, uriString, &uri);
    printf("URI: %ls\n", WindowsGetStringRawBuffer(uriString, NULL));
    WindowsDeleteString(uriString);

    HSTRING domain;
    IUriRuntimeClass_get_Domain(uri, &domain);
    printf("URI domain: %ls\n", WindowsGetStringRawBuffer(domain, NULL));
    WindowsDeleteString(domain);

    HSTRING path;
    IUriRuntimeClass_get_Path(uri, &path);
    printf("URI path: %ls\n", WindowsGetStringRawBuffer(path, NULL));
    WindowsDeleteString(path);

    IUriRuntimeClass_Release(uri);

    IUriRuntimeClassFactory_Release(uriFactory);

    RoUninitialize();
    return 0;
}
