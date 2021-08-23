#include "win32.h"

#ifdef WIN64
    wchar_t *message = L"Hello World! 😎 (64-bit)";
#else
    wchar_t *message = L"Hello World! 😎 (32-bit)";
#endif

void _start(void) {
    MessageBoxW(HWND_DESKTOP, message, message, MB_OK);
    ExitProcess(EXIT_SUCCESS);
}
