#ifndef HANS_H
#define HANS_H

typedef struct Uri {
    char *scheme;
    char *host;
    char *port;
    char *path;
} Uri;

Uri *Uri_New(char *uri_string);

void Uri_Free(Uri *uri);

char *Http_Get(Uri *uri, char **http_header);

#endif
