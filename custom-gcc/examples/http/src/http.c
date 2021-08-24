#include "win32.h"
#include "hans.h"

void _start(void) {
    Uri *uri = Uri_New("http://www.plaatsoft.nl/android/coinlist/"); // http://www.example.com
    printf("Uri scheme: %s\n", uri->scheme);
    printf("Uri host: %s\n", uri->host);
    printf("Uri port: %s\n", uri->port);
    printf("Uri path: %s\n", uri->path);

    char *buffer;
    Map *headers;
    char *response = Http_Get(uri, &buffer, &headers);
    Uri_Free(uri);

    printf("\n# Http headers:\n");
    Map_Foreach(headers, char *key, char *value, {
        printf("%s: %s\n", key, value);
    })
    Map_Free(headers, free);

    printf("\n# Response (%d bytes):\n", (int32_t)strlen(response));
    // puts(response);

    free(buffer);

    ExitProcess(EXIT_SUCCESS);
}
