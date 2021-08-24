#include "std.h"
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

size_t strlen(const char *string) {
    const char *start = string;
    while (*string != '\0') string++;
    return string - start;
}

int32_t strcmp(const char *s1, const char *s2) {
    while (*s1 != '\0' && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

char *strcpy(char *dest, const char *src) {
    char *start = dest;
    while ((*dest++ = *src++) != '\0');
    return start;
}

char *strcat(char *dest, const char *src) {
    char *start = dest;
    while (*dest != '\0') dest++;
    strcpy(dest, src);
    return start;
}

char *strdup(const char *src) {
    char *dest = malloc(strlen(src) + 1);
    if (dest == NULL) return NULL;
    strcpy(dest, src);
    return dest;
}

size_t wcslen(const wchar_t *string) {
    const wchar_t *start = string;
    while (*string != '\0') string++;
    return string - start;
}

int32_t wcscmp(const wchar_t *s1, const wchar_t *s2) {
    while (*s1 != '\0' && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

wchar_t *wcscpy(wchar_t *dest, const wchar_t *src) {
    wchar_t *start = dest;
    while ((*dest++ = *src++) != '\0');
    return start;
}

wchar_t *wcscat(wchar_t *dest, const wchar_t *src) {
    wchar_t *start = dest;
    while (*dest != '\0') dest++;
    wcscpy(dest, src);
    return start;
}

wchar_t *wcsdup(const wchar_t *src) {
    wchar_t *dest = malloc((wcslen(src) + 1) * sizeof(wchar_t));
    if (dest == NULL) return NULL;
    wcscpy(dest, src);
    return dest;
}

int32_t puts(const char *str) {
    HANDLE stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleA(stdout, str, strlen(str), NULL, NULL);
    WriteConsoleA(stdout, "\n", 1, NULL, NULL);
    return 0;
}

int32_t printf(const char *format, ...) {
    char string_buffer[1024];
    va_list args;
    va_start(args, format);
    wvsprintfA(string_buffer, format, args);
    va_end(args);
    int32_t string_length = strlen(string_buffer);
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), string_buffer, string_length, NULL, NULL);
    return string_length;
}

int32_t wprintf(const wchar_t *format, ...) {
    wchar_t string_buffer[1024];
    va_list args;
    va_start(args, format);
    wvsprintfW(string_buffer, format, args);
    va_end(args);
    int32_t string_length = wcslen(string_buffer);
    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), string_buffer, string_length, NULL, NULL);
    return string_length;
}
