#ifndef DPI_H
#define DPI_H

#include "win32.h"

typedef int32_t (__stdcall *_SetProcessDpiAwareness)(uint32_t value);

typedef bool (__stdcall *_SetProcessDPIAware)(void);

void SetDPIAware(void);

typedef int32_t (__stdcall *_GetDpiForMonitor)(HMONITOR hmonitor, int dpiType, uint32_t *dpiX, uint32_t *dpiY);

int32_t GetWindowDPI(HWND hwnd);

#endif
