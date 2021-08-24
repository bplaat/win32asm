#include "hans.h"
#include "win32.h"

Uri *Uri_New(char *uri_string) {
    Uri *uri = malloc(sizeof(Uri));

    char *c = uri_string;
    char part[256];
    char *s = part;
    while (*c != ':') *s++ = *c++;
    *s = '\0';
    c++;
    uri->scheme = strdup(part);

    if (*c == '/' && *(c + 1) == '/') {
        c += 2;
        s = part;
        while (*c != ':' && *c != '/' && *c != '\0') *s++ = *c++;
        *s = '\0';
        uri->host = strdup(part);

        if (*c == ':') {
            c++;
            s = part;
            while (*c >= '0' && *c <= '9') *s++ = *c++;
            *s = '\0';
            uri->port = strdup(part);
        } else {
            uri->port = NULL;
        }
    } else {
        uri->host = NULL;
        uri->port = NULL;
    }

    s = part;
    while ((*s++ = *c++) != '\0');
    if (part[0] == '\0') {
        uri->path = strdup("/");
    } else {
        uri->path = strdup(part);
    }

    return uri;
}

void Uri_Free(Uri *uri) {
    free(uri->scheme);
    if (uri->host != NULL) free(uri->host);
    if (uri->port != NULL) free(uri->port);
    free(uri->path);
    free(uri);
}

char *Http_Get(Uri *uri, char **buffer_ptr, Map **headers_ptr) {
    // Start WSA
    WSADATA wsaData;
    if (WSAStartup(0x0202, &wsaData) != 0) {
        printf("WSAStartup failed\n");
        ExitProcess(1);
    }

    // Get address info from uri
    ADDRINFOA hints = { 0 };
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    ADDRINFOA *result = NULL;
    char *port = uri->port;
    if (port == NULL && !strcmp(uri->scheme, "http")) port = "80";
    if (port == NULL && !strcmp(uri->scheme, "https")) port = "443";
    if (getaddrinfo(uri->host, port, &hints, &result) != 0) {
        printf("getaddrinfo failed\n");
        WSACleanup();
        ExitProcess(1);
    }

    // Open socket
    SOCKET sock = INVALID_SOCKET;
    for (ADDRINFOA *ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (sock == INVALID_SOCKET) {
            printf("socket failed\n");
            WSACleanup();
            ExitProcess(1);
        }

        if (connect(sock, ptr->ai_addr, ptr->ai_addrlen) == SOCKET_ERROR) {
            closesocket(sock);
            sock = INVALID_SOCKET;
            continue;
        }
        break;
    }
    freeaddrinfo(result);
    if (sock == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        ExitProcess(1);
    }

    // Send HTTP request
    char http_request[512];
    wsprintfA(http_request, "GET %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n\r\n",
        uri->path, uri->host);
    if (send(sock, http_request, strlen(http_request), 0) == SOCKET_ERROR) {
        printf("send failed\n");
        WSACleanup();
        ExitProcess(1);
    }

    if (shutdown(sock, SD_SEND) == SOCKET_ERROR) {
        printf("shutdown failed\n");
        WSACleanup();
        ExitProcess(1);
    }

    // Read response to growing buffer
    int32_t buffer_size = 1024;
    char *buffer = malloc(buffer_size);
    int32_t position = 0;
    int32_t bytes_read;
    while ((bytes_read = recv(sock, buffer + position, 1024, MSG_WAITALL)) > 0) {
        position += bytes_read;
        if (position == buffer_size) {
            buffer_size *= 2;
            buffer = realloc(buffer, buffer_size);
        }
    }
    if (bytes_read < 0) {
        printf("recv failed\n");
        WSACleanup();
        ExitProcess(1);
    }
    buffer[position] = '\0';

    // Parse HTTP header
    Map *headers = Map_New(8);
    char *c = buffer;
    while (*c != '\r') c++;
    c += 2;
    while (*c != '\r') {
        char key[256];
        char *s = key;
        while (*c != ':') *s++ = *c++;
        *s = '\0';
        c++;
        if (*c == ' ') c++;

        char value[256];
        s = value;
        while (*c != '\r') *s++ = *c++;
        *s = '\0';
        c += 2;

        Map_Set(headers, key, strdup(value));
    }
    c += 2;

    // Remove weird HTTP chunks
    Map_Foreach(headers, char *key, char *value, {
        if (!strcmp(key, "Transfer-Encoding") && !strcmp(value, "chunked")) {
            // TODO
            break;
        }
    });

    // Close socket
    closesocket(sock);
    WSACleanup();

    // Return buffer and response
    *buffer_ptr = buffer;
    *headers_ptr = headers;
    return c;
}
