    ; socket.asm - An pure 32-bit and 64-bit win32 windows socket program
    ; Made by Bastiaan van der Plaat (https://bastiaan.ml/)
    ; 32-bit: nasm -f bin socket.asm -o socket-x86.exe && ./socket-x86
    ; 64-bit: nasm -DWIN64 -f bin socket.asm -o socket-x64.exe && ./socket-x64

%include "libwindows.inc"

header HEADER_CONSOLE

code_section
    ; ### Some stdlib like Win32 wrappers ###
    function memset, address, value, size
        mov al, [value]
        mov _di, [address]
        mov _si, _di
        add _si, [size]
    .repeat:
        cmp _di, _si
        je .done
        mov [_di], al
        inc _di
        jmp .repeat
    .done:
        return

    ; ### Socket code ###
    entrypoint
        %define data_buffer_size 1024

        local wsaData, WSADATA_size, \
            console_out, POINTER_size, \
            address_hints, addrinfo_size, \
            client_address, POINTER_size, \
            client_socket, POINTER_size, \
            request_buffer, 128, \
            data_buffer, data_buffer_size

        ; Init WinSock2
        invoke WSAStartup, 0x0202, addr wsaData

        ; Get console out handle
        invoke GetStdHandle, STD_OUTPUT_HANDLE
        mov [console_out], _ax

        ; Fill address hints
        fcall memset, addr address_hints, 0, addrinfo_size
        mov dword [address_hints + addrinfo.ai_family], AF_UNSPEC
        mov dword [address_hints + addrinfo.ai_socktype], SOCK_STREAM
        mov dword [address_hints + addrinfo.ai_protocol], IPPROTO_TCP

        ; Get address
        invoke getaddrinfo, address_host, address_port, addr address_hints, addr client_address

        ; Create socket
        mov _si, [client_address]
        invoke socket, [_si + addrinfo.ai_family], [_si + addrinfo.ai_socktype], [_si + addrinfo.ai_protocol]
        mov [client_socket], _ax

        ; Connect socket
        mov _si, [client_address]
        invoke connect, [client_socket], [_si + addrinfo.ai_addr], [_si + addrinfo.ai_addrlen]

        ; Free address
        invoke freeaddrinfo, [client_address]

        ; Send http request
        cinvoke wsprintfA, addr request_buffer, http_request, address_host
        invoke send, [client_socket], addr request_buffer, _ax, 0

        ; Stop sending data
        invoke shutdown, [client_socket], SD_SEND

        ; Read response to buffer and print to console out
    read_socket_write_stout_loop:
        invoke recv, [client_socket], addr data_buffer, data_buffer_size, 0
        test _ax, _ax
        je .done

        invoke WriteConsoleA, [console_out], addr data_buffer, _ax, NULL, 0
        jmp read_socket_write_stout_loop
    .done:
        ; Close socket and WinSock2
        invoke closesocket, [client_socket]
        invoke WSACleanup

        ; Exit successfully
        invoke ExitProcess, EXIT_SUCCESS

    end_local
end_code_section

data_section
    ; String Constants
    address_host db "example.net", 0
    address_port db "80", 0
    http_request db "GET / HTTP/1.1", 13, 10, \
            "Host: %s", 13, 10, \
            "Connection: close", 13, 10, 13, 10, 0

    ; Import table
    import_table
        library kernel_table, "KERNEL32.DLL", \
            user_table, "USER32.DLL", \
            winsock2_table, "WS2_32.DLL"

        import kernel_table, \
            ExitProcess, "ExitProcess", \
            GetStdHandle, "GetStdHandle", \
            WriteConsoleA, "WriteConsoleA"

        import user_table, \
            wsprintfA, "wsprintfA"

        import winsock2_table, \
            closesocket, "closesocket", \
            connect, "connect", \
            freeaddrinfo, "freeaddrinfo", \
            getaddrinfo, "getaddrinfo", \
            recv, "recv", \
            send, "send", \
            shutdown, "shutdown", \
            socket, "socket", \
            WSACleanup, "WSACleanup", \
            WSAStartup, "WSAStartup"
    end_import_table
end_data_section
