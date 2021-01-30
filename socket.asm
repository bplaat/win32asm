    ; socket.asm - An pure 32-bit and 64-bit win32 windows socket program
    ; Made by Bastiaan van der Plaat (https://bastiaan.ml/)
    ; 32-bit: nasm -f bin socket.asm -o socket-x86.exe && ./socket-x86
    ; 64-bit: nasm -DWIN64 -f bin socket.asm -o socket-x64.exe && ./socket-x64

    ; Not yet finished

%include "libwindows.inc"

header HEADER_CONSOLE

code_section
    ; A function that zero's out some memory
    function ZeroMemory, address, size
        mov _di, [address]
        xor _cx, _cx
    .repeat:
        cmp _cx, [size]
        je .done
        mov byte [_di + _cx], 0
        inc _cx
        jmp .repeat
    .done:
        return

    entrypoint
        data_buffer_size equ 1024 * BYTE_size

        local wsaData, WSADATA_size, \
            console_out, POINTER_size, \
            address_hints, addrinfo_size, \
            client_address, addrinfo_size, \
            client_socket, POINTER_size, \
            request_buffer, 128 * BYTE_size, \
            data_buffer, data_buffer_size

        ; Init WinSock2
        invoke WSAStartup, 0x0202, addr wsaData

        cmp _ax, 0
        je .skip_error1
        invoke ExitProcess, 1 ; WSAStartup failed
    .skip_error1:

        ; Get console out handle
        invoke GetStdHandle, STD_OUTPUT_HANDLE
        mov [console_out], _ax

        ; Fill address hints
        fcall ZeroMemory, addr address_hints, addrinfo_size
        mov dword [address_hints + addrinfo.ai_family], AF_UNSPEC
        mov dword [address_hints + addrinfo.ai_socktype], SOCK_STREAM
        mov dword [address_hints + addrinfo.ai_protocol], IPPROTO_TCP

        ; Get address
        invoke getaddrinfo, address_host, address_port, addr address_hints, addr client_address

        cmp _ax, 0
        je .skip_error2
        invoke WSACleanup
        invoke ExitProcess, 2 ; getaddrinfo failed
    .skip_error2:

        ; Create socket
        invoke socket, [client_address + addrinfo.ai_family], [client_address + addrinfo.ai_socktype], [client_address + addrinfo.ai_protocol]
        mov [client_socket], _ax

        cmp _ax, ~0
        jne .skip_error3
        invoke ExitProcess, 3 ; socket failed
    .skip_error3:

        ; Connect socket
        invoke connect, [client_socket], [client_address + addrinfo.ai_addr], [client_address + addrinfo.ai_addrlen]

        cmp _ax, -1
        jne .skip_error4
        invoke ExitProcess, 4 ; connect failed
    .skip_error4:

        ; Free address
        invoke freeaddrinfo, addr client_address

        ; Send http request
        cinvoke wsprintfA, addr request_buffer, http_request, address_host
        invoke send, [client_socket], addr request_buffer, _ax, 0

        ; Stop sending data
        invoke shutdown, [client_socket], SD_SEND

        ; Read response and print to console out
    read_socket_write_stout_loop:
        invoke recv, [client_socket], addr data_buffer, data_buffer_size, 0
        cmp _ax, 0
        je .done

        invoke WriteConsoleA, [console_out], addr data_buffer, _ax, NULL, 0
        jmp read_socket_write_stout_loop
    .done:
        ; Close socket and WinSock2
        invoke closesocket, [client_socket]
        invoke WSACleanup

        invoke ExitProcess, EXIT_SUCCESS

    end_local
end_code_section

data_section
    ; String Constants
    address_host db "example.net", 0
    address_port db "80", 0
    http_request db "GET / HTTP/1.1", 13, 10, \
            "Host: %s", 13, 10, \
            "Connection: closed", 13, 10, 13, 10, 0

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
            WSAGetLastError, "WSAGetLastError", \
            WSAStartup, "WSAStartup"
    end_import_table
end_data_section
