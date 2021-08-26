#ifndef HELLO_H
#define HELLO_H

#include "win32.h"

// IHello
typedef struct IHello IHello;

typedef struct IHelloVtbl {
    IUnknownVtbl Base;
    void __stdcall (*SayHello)(IHello *This, wchar_t *name);
} IHelloVtbl;

#define IHello_SayHello(This, name) ((IHello *)This)->lpVtbl->SayHello((IHello *)This, name)

struct IHello {
    const IHelloVtbl *lpVtbl;
};

extern const IID IID_IHello;

int32_t __stdcall CreateHello(const IID *riid, IHello **ppIHello);

// Hello
typedef struct Hello {
    IHello Base;
    uint32_t RefCount;
} Hello;

extern const IHelloVtbl HelloVtbl;

int32_t __stdcall Hello_QueryInterface(Hello *This, const IID *riid, void **ppv);

uint32_t __stdcall Hello_AddRef(Hello *This);

uint32_t __stdcall Hello_Release(Hello *This);

void __stdcall Hello_SayHello(Hello *This, wchar_t *name);

#endif
