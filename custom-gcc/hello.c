#include "win32.h"

char *message = "Hello World! (32-bit)";

void _start(void) {
    MessageBoxA(HWND_DESKTOP, message, message, MB_OK);
    ExitProcess(EXIT_SUCCESS);
}
