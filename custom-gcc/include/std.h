#ifndef STD_H
#define STD_H

#include <stdint.h>

void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);

extern uint32_t rand_seed;
void srand(uint32_t seed);
uint32_t rand(void);

size_t strlen(const char *string);
int32_t strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);
char *strdup(const char *src);

size_t wcslen(const wchar_t *string);
int32_t wcscmp(const wchar_t *s1, const wchar_t *s2);
wchar_t *wcscpy(wchar_t *dest, const wchar_t *src);
wchar_t *wcscat(wchar_t *dest, const wchar_t *src);
wchar_t *wcsdup(const wchar_t *src);

int32_t puts(const char *str);
int32_t printf(const char *format, ...);
int32_t wprintf(const wchar_t *format, ...);

#endif
