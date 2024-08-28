// A simple Windows application which parses an URL via the WinRT APIs
// tcc url.c -lcombase && ./url

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

// IUriRuntimeClass
typedef struct IUriRuntimeClass IUriRuntimeClass;

typedef struct IUriRuntimeClassVtbl {
    IInspectableVtbl base;
    HRESULT (STDMETHODCALLTYPE *get_AbsoluteUri)(IUriRuntimeClass *This, HSTRING *value);
    HRESULT (STDMETHODCALLTYPE *get_DisplayUri)(IUriRuntimeClass *This, HSTRING *value);
    HRESULT (STDMETHODCALLTYPE *get_Domain)(IUriRuntimeClass *This, HSTRING *value);
    HRESULT (STDMETHODCALLTYPE *get_Extension)(IUriRuntimeClass *This, HSTRING *value);
    HRESULT (STDMETHODCALLTYPE *get_Fragment)(IUriRuntimeClass *This, HSTRING *value);
    HRESULT (STDMETHODCALLTYPE *get_Host)(IUriRuntimeClass *This, HSTRING *value);
    HRESULT (STDMETHODCALLTYPE *get_Password)(IUriRuntimeClass *This, HSTRING *value);
    HRESULT (STDMETHODCALLTYPE *get_Path)(IUriRuntimeClass *This, HSTRING *value);
    HRESULT (STDMETHODCALLTYPE *get_Query)(IUriRuntimeClass *This, HSTRING *value);
    void *padding1[1];
    HRESULT (STDMETHODCALLTYPE *get_RawUri)(IUriRuntimeClass *This, HSTRING *value);
    HRESULT (STDMETHODCALLTYPE *get_SchemeName)(IUriRuntimeClass *This, HSTRING *value);
    HRESULT (STDMETHODCALLTYPE *get_UserName)(IUriRuntimeClass *This, HSTRING *value);
    HRESULT (STDMETHODCALLTYPE *get_Port)(IUriRuntimeClass *This, INT32 *value);
    HRESULT (STDMETHODCALLTYPE *get_Suspicious)(IUriRuntimeClass *This, boolean *value);
    void *padding2[2];
} IUriRuntimeClassVtbl;

struct IUriRuntimeClass {
    const IUriRuntimeClassVtbl *lpVtbl;
};

#define IUriRuntimeClass_get_AbsoluteUri(This, value) ((IUriRuntimeClass *)This)->lpVtbl->get_AbsoluteUri((IUriRuntimeClass *)This, value)
#define IUriRuntimeClass_get_DisplayUri(This, value) ((IUriRuntimeClass *)This)->lpVtbl->get_DisplayUri((IUriRuntimeClass *)This, value)
#define IUriRuntimeClass_get_Domain(This, value) ((IUriRuntimeClass *)This)->lpVtbl->get_Domain((IUriRuntimeClass *)This, value)
#define IUriRuntimeClass_get_Extension(This, value) ((IUriRuntimeClass *)This)->lpVtbl->get_Extension((IUriRuntimeClass *)This, value)
#define IUriRuntimeClass_get_Fragment(This, value) ((IUriRuntimeClass *)This)->lpVtbl->get_Fragment((IUriRuntimeClass *)This, value)
#define IUriRuntimeClass_get_Host(This, value) ((IUriRuntimeClass *)This)->lpVtbl->get_Host((IUriRuntimeClass *)This, value)
#define IUriRuntimeClass_get_Password(This, value) ((IUriRuntimeClass *)This)->lpVtbl->get_Password((IUriRuntimeClass *)This, value)
#define IUriRuntimeClass_get_Path(This, value) ((IUriRuntimeClass *)This)->lpVtbl->get_Path((IUriRuntimeClass *)This, value)
#define IUriRuntimeClass_get_Query(This, value) ((IUriRuntimeClass *)This)->lpVtbl->get_Query((IUriRuntimeClass *)This, value)
#define IUriRuntimeClass_get_RawUri(This, value) ((IUriRuntimeClass *)This)->lpVtbl->get_RawUri((IUriRuntimeClass *)This, value)
#define IUriRuntimeClass_get_SchemeName(This, value) ((IUriRuntimeClass *)This)->lpVtbl->get_SchemeName((IUriRuntimeClass *)This, value)
#define IUriRuntimeClass_get_UserName(This, value) ((IUriRuntimeClass *)This)->lpVtbl->get_UserName((IUriRuntimeClass *)This, value)
#define IUriRuntimeClass_get_Port(This, value) ((IUriRuntimeClass *)This)->lpVtbl->get_Port((IUriRuntimeClass *)This, value)
#define IUriRuntimeClass_get_Suspicious(This, value) ((IUriRuntimeClass *)This)->lpVtbl->get_Suspicious((IUriRuntimeClass *)This, value)
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
    HSTRING uriString = hstr(L"https://bastiaan.ml/winrt/stuff/nice?x=1&y=2#dfda");
    IUriRuntimeClassFactory_CreateUri(uriFactory, uriString, &uri);
    printf("URI: %ls\n", WindowsGetStringRawBuffer(uriString, NULL));
    WindowsDeleteString(uriString);
    IUriRuntimeClassFactory_Release(uriFactory);

    HSTRING scheme;
    IUriRuntimeClass_get_SchemeName(uri, &scheme);
    printf("URI scheme: %ls\n", WindowsGetStringRawBuffer(scheme, NULL));
    WindowsDeleteString(scheme);

    HSTRING domain;
    IUriRuntimeClass_get_Domain(uri, &domain);
    printf("URI domain: %ls\n", WindowsGetStringRawBuffer(domain, NULL));
    WindowsDeleteString(domain);

    INT32 port;
    IUriRuntimeClass_get_Port(uri, &port);
    printf("URI domain: %d\n", port);

    HSTRING path;
    IUriRuntimeClass_get_Path(uri, &path);
    printf("URI path: %ls\n", WindowsGetStringRawBuffer(path, NULL));
    WindowsDeleteString(path);

    HSTRING query;
    IUriRuntimeClass_get_Query(uri, &query);
    printf("URI query: %ls\n", WindowsGetStringRawBuffer(query, NULL));
    WindowsDeleteString(query);

    HSTRING fragment;
    IUriRuntimeClass_get_Fragment(uri, &fragment);
    printf("URI fragment: %ls\n", WindowsGetStringRawBuffer(fragment, NULL));
    WindowsDeleteString(fragment);

    IUriRuntimeClass_Release(uri);

    RoUninitialize();
    return 0;
}
