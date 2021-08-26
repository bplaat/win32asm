#ifndef HELLO_H
#define HELLO_H

#include "win32.h"

// IHello
extern const IID IID_IHello;

typedef struct IHello IHello;

typedef struct IHelloVtbl {
    IUnknownVtbl Base;
    int32_t (__stdcall *SayHello)(IHello *This, wchar_t *name);
} IHelloVtbl;

#define IHello_SayHello(This, name) ((IHello *)This)->lpVtbl->SayHello((IHello *)This, name)

struct IHello {
    const IHelloVtbl *lpVtbl;
};

// Hello
typedef struct Hello {
    IHello interface;
    uint32_t references;
} Hello;

int32_t __stdcall CreateHello(const IID *riid, IHello **ihello);

int32_t __stdcall Hello_QueryInterface(Hello *hello, const IID *riid, void **ppv);

uint32_t __stdcall Hello_AddRef(Hello *hello);

uint32_t __stdcall Hello_Release(Hello *hello);

int32_t __stdcall Hello_SayHello(Hello *hello, wchar_t *name);

#endif
