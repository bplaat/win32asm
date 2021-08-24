#ifndef HANS_H
#define HANS_H

#include "win32.h"
#include "map.h"

typedef struct Uri {
    char *scheme;
    char *host;
    char *port;
    char *path;
} Uri;

Uri *Uri_New(char *uri_string);

void Uri_Free(Uri *uri);

char *Http_Get(Uri *uri, char **buffer_ptr, Map **headers_ptr);

#endif
