#include "win32.h"
#include "hello.h"

void _start(void) {
    IHello *hello;
    CreateHello(&IID_IHello, &hello);

    IHello_SayHello(hello, L"Bastiaan");
    IHello_SayHello(hello, L"Leonard");
    IHello_SayHello(hello, L"Sander");
    IHello_SayHello(hello, L"Jiska");

    IUnknown_Release(hello);

    ExitProcess(EXIT_SUCCESS);
}
