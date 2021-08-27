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
        *ppIHello = (IHello *)Hello_New();
        return E_OK;
    }
    *ppIHello = NULL;
    return E_NOINTERFACE;
}

int32_t __stdcall Hello_QueryInterface(Hello *this, const IID *riid, void **ppvObject) {
    const IID IID_IUnknown = { 0x00000000, 0x0000, 0x0000, { 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };
    if (!memcmp(riid, &IID_IUnknown, sizeof(IID)) || !memcmp(riid, &IID_IHello, sizeof(IID))) {
        *ppvObject = this;
        IUnknown_AddRef(*ppvObject);
        return E_OK;
    }
    *ppvObject = NULL;
    return E_NOINTERFACE;
}

uint32_t __stdcall Hello_AddRef(Hello *this) {
    return ++this->refs;
}

uint32_t __stdcall Hello_Release(Hello *this) {
    if (--this->refs == 0) Hello_Free(this);
    return this->refs;
}

Hello * __stdcall Hello_New(void) {
    Hello *hello = malloc(sizeof(Hello));
    hello->base.lpVtbl = &HelloVtbl;
    hello->refs = 1;
    return hello;
}

void __stdcall Hello_SayHello(Hello *this, wchar_t *name) {
    (void)this;
    wprintf(L"Hello %s!\n", name);
}

void __stdcall Hello_Free(Hello *this) {
    free(this);
}
