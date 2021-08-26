#include "hello.h"

const IID IID_IHello = { 0xad866b1c, 0x5735, 0x45e7, { 0x84, 0x06, 0xcd, 0x19, 0x9e, 0x66, 0x91, 0x3d } };

const IHelloVtbl HelloVtbl = {
    {
        (void *)&Hello_QueryInterface,
        (void *)&Hello_AddRef,
        (void *)&Hello_Release,
    },
    (void *)&Hello_SayHello
};

int32_t __stdcall CreateHello(const IID *riid, IHello **ppIHello) {
    if (!memcmp(riid, &IID_IHello, sizeof(IID))) {
        Hello *hello = malloc(sizeof(Hello));
        hello->Base.lpVtbl = &HelloVtbl;
        hello->RefCount = 1;
        *ppIHello = (IHello *)hello;
        return E_OK;
    }
    *ppIHello = NULL;
    return E_NOINTERFACE;
}

int32_t __stdcall Hello_QueryInterface(Hello *This, const IID *riid, void **ppv) {
    IID IID_IUnknown = { 0x00000000, 0x0000, 0x0000, { 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };
    if (!memcmp(riid, &IID_IUnknown, sizeof(IID))) {
        *ppv = This;
    } else if (!memcmp(riid, &IID_IHello, sizeof(IID))) {
        *ppv = This;
    } else {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    IUnknown_AddRef(*ppv);
    return E_OK;
}

uint32_t __stdcall Hello_AddRef(Hello *This) {
    return ++This->RefCount;
}

uint32_t __stdcall Hello_Release(Hello *This) {
    This->RefCount--;
    if (This->RefCount == 0) {
        free(This);
    }
    return This->RefCount;
}

void __stdcall Hello_SayHello(Hello *This, wchar_t *name) {
    (void)This;
    wprintf(L"Hello %s!\n", name);
}
