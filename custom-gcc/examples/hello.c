#include "win32.h"

#ifdef WIN64
    char *message = "Hello World! (64-bit)";
#else
    char *message = "Hello World! (32-bit)";
#endif

void _start(void) {
    MessageBoxA(HWND_DESKTOP, message, message, MB_OK);
    ExitProcess(EXIT_SUCCESS);
}
