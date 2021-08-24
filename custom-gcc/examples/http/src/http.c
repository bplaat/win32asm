#include "win32.h"
#include "hans.h"

void _start(void) {
    Uri *uri = Uri_New("http://www.example.com"); // http://www.plaatsoft.nl/android/coinlist/
    printf("Uri scheme: %s\n", uri->scheme);
    printf("Uri host: %s\n", uri->host);
    printf("Uri port: %s\n", uri->port);
    printf("Uri path: %s\n\n", uri->path);

    char *http_header;
    char *response = Http_Get(uri, &http_header);
    printf("Http header size: %d\n", (int32_t)strlen(http_header));
    printf("Response size: %d\n\n", (int32_t)strlen(response));
    puts(http_header);
    puts(response);
    free(http_header);

    Uri_Free(uri);

    ExitProcess(EXIT_SUCCESS);
}
