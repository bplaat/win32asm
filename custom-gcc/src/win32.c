#include "win32.h"

void *malloc(size_t size) {
    return HeapAlloc(GetProcessHeap(), 0, size);
}

void *realloc(void *ptr, size_t size) {
    return HeapReAlloc(GetProcessHeap(), 0, ptr, size);
}

void free(void *ptr) {
    HeapFree(GetProcessHeap(), 0, ptr);
}

uint32_t rand_seed;

void srand(uint32_t seed) {
    rand_seed = seed;
}

uint32_t rand(void) {
    return rand_seed = rand_seed * 1103515245 + 12345;
}

size_t wcslen(wchar_t *string) {
    wchar_t *start = string;
    while (*string++ != '\0');
    return string - start - 1;
}

wchar_t *wcscpy(wchar_t *dest, wchar_t *src) {
    wchar_t *start = dest;
    while ((*dest++ = *src++) != '\0');
    return start;
}

wchar_t *wcscat(wchar_t *dest, wchar_t *src) {
    wchar_t *start = dest;
    while (*dest++ != '\0');
    dest--;
    while ((*dest++ = *src++) != '\0');
    *dest = '\0';
    return start;
}

wchar_t *wcsdup(wchar_t *src) {
    wchar_t *dst = malloc((wcslen(src) + 1) * sizeof(wchar_t));
    if (dst == NULL) return NULL;
    wcscpy(dst, src);
    return dst;
}

void wprintf(wchar_t *format, ...) {
    wchar_t string_buffer[256];
    va_list args;
    va_start(args, format);
    wvsprintfW(string_buffer, format, args);
    va_end(args);
    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), string_buffer, wcslen(string_buffer), NULL, NULL);
}
