#ifndef DPI_H
#define DPI_H

#include "win32.h"

typedef int32_t __stdcall (*_SetProcessDpiAwarenessContext)(void *value);

typedef int32_t __stdcall (*_SetProcessDpiAwareness)(uint32_t value);

typedef bool __stdcall (*_SetProcessDPIAware)(void);

void SetDpiAware(void);

int32_t GetDesktopDpi(void);

typedef bool __stdcall (*_AdjustWindowRectExForDpi)(RECT *lpRect, uint32_t dwStyle, bool bMenu, uint32_t dwExStyle, uint32_t dpi);

bool AdjustWindowRectExForDpi(RECT *lpRect, uint32_t dwStyle, bool bMenu, uint32_t dwExStyle, uint32_t dpi);

#endif
