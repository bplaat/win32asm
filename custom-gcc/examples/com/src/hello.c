#include "hello.h"

const IID IID_IHello = { 0xad866b1c, 0x5735, 0x45e7, { 0x84, 0x06, 0xcd, 0x19, 0x9e, 0x66, 0x91, 0x3d } };

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
const IHelloVtbl HelloVtbl = {
    {
        &Hello_QueryInterface,
        &Hello_AddRef,
        &Hello_Release,
    },
    &Hello_SayHello
};
#pragma GCC diagnostic pop

int32_t __stdcall CreateHello(const IID *riid, IHello **ihello) {
    if (!memcmp(riid, &IID_IHello, sizeof(IID))) {
        Hello *hello = malloc(sizeof(Hello));
        hello->interface.lpVtbl = &HelloVtbl;
        hello->references = 0;
        IUnknown_AddRef(hello);
        *ihello = (IHello *)hello;
        return E_OK;
    }
    *ihello = NULL;
    return E_NOINTERFACE;
}

int32_t __stdcall Hello_QueryInterface(Hello *hello, const IID *riid, void **ppv) {
    IID IID_IUnknown = { 0x00000000, 0x0000, 0x0000, { 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };
    if (!memcmp(riid, &IID_IUnknown, sizeof(IID))) {
        *ppv = hello;
    } else if (!memcmp(riid, &IID_IHello, sizeof(IID))) {
        *ppv = hello;
    } else {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    IUnknown_AddRef(*ppv);
    return E_OK;
}

uint32_t __stdcall Hello_AddRef(Hello *hello) {
    return ++hello->references;
}

uint32_t __stdcall Hello_Release(Hello *hello) {
    hello->references--;
    if (hello->references == 0) {
        free(hello);
    }
    return hello->references;
}

int32_t __stdcall Hello_SayHello(Hello *hello, wchar_t *name) {
    (void)hello;
    wprintf(L"Hello %s!\n", name);
    return E_OK;
}
