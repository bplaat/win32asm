#ifndef HELLO_H
#define HELLO_H

#include "win32.h"

// IHello
extern const IID IID_IHello;

typedef struct IHello IHello;

typedef struct IHelloVtbl {
    IUnknownVtbl base;
    void __stdcall (*SayHello)(IHello *this, wchar_t *name);
} IHelloVtbl;

#define IHello_SayHello(this, name) ((IHello *)this)->lpVtbl->SayHello((IHello *)this, name)

struct IHello {
    const IHelloVtbl *lpVtbl;
};

int32_t __stdcall CreateHello(const IID *riid, IHello **ppIHello);

// Hello
typedef struct Hello {
    IHello base;
    uint32_t refs;
} Hello;

extern const IHelloVtbl HelloVtbl;

int32_t __stdcall Hello_QueryInterface(Hello *this, const IID *riid, void **ppvObject);

uint32_t __stdcall Hello_AddRef(Hello *this);

uint32_t __stdcall Hello_Release(Hello *this);

Hello * __stdcall Hello_New(void);

void __stdcall Hello_SayHello(Hello *this, wchar_t *name);

void __stdcall Hello_Free(Hello *this);

#endif
