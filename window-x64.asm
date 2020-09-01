    ; window-x64.asm - A 64-bit win32 assembly window program
    ; Made by Bastiaan van der Plaat (https://bastiaan.ml/)
    ; nasm -f bin window-x64.asm -o window-x64.exe && ./window-x64

    ; Not working yet... 😪
    ; WindowProc is not beeing called bug 😡
    ; I will try to fix it by spending countless hours in a debugger 😂

%include "libwindows-x64.inc"

code_section
    WindowProc:
        push rbp
        mov rbp, rsp
        sub rsp, 40
        %define hwnd qword [rbp + 2 * 8]
        %define uMsg qword [rbp + 3 * 8]
        %define wParam qword [rbp + 4 * 8]
        %define lParam qword [rbp + 5 * 8]
        mov hwnd, rcx
        mov uMsg, rdx
        mov wParam, r8
        mov lParam, r9

        mov rax, uMsg
        cmp rax, WM_DESTROY
        je .wm_destroy
        jmp .default

        .wm_destroy:
            invoke PostQuitMessage, 0
            xor rax, rax
            leave
            ret

        .default:
            invoke DefWindowProcA, hwnd, uMsg, wParam, lParam
            leave
            ret

        %undef hwnd

    _start:
        invoke GetModuleHandleA, 0
        mov [window_class + WNDCLASSEX.hInstance], rax

        invoke LoadCursorA, 0, IDC_ARROW
        mov [window_class + WNDCLASSEX.hCursor], rax

        invoke RegisterClassExA, window_class

        invoke CreateWindowExA, 0, window_class_name, window_title, WS_OVERLAPPEDWINDOW, \
            CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, HWND_DESKTOP, 0, [window_class + WNDCLASSEX.hInstance], 0
        mov [hwnd], eax
        invoke ShowWindow, [hwnd], SW_SHOWDEFAULT
        invoke UpdateWindow, [hwnd]

        .message_loop:
            invoke GetMessageA, message, 0, 0, 0
            cmp eax, 0
            jle .done
            invoke TranslateMessage, message
            invoke DispatchMessageA, message
            jmp .message_loop

        .done:
            invoke ExitProcess, [message + MSG.wParam]
end_code_section

data_section
    window_class_name db "test", 0
    window_title db "This is a test window (64-bit)", 0

    window_class:
        dd WNDCLASSEX_size         ; cbSize
        dd CS_HREDRAW | CS_VREDRAW ; style
        dq WindowProc              ; lpfnWndProc
        dd 0                       ; cbClsExtra
        dd 0                       ; cbWndExtra
        dq 0                       ; hInstance
        dq 0                       ; hIcon
        dq 0                       ; hCursor
        dq 6                       ; hbrBackground
        dq 0                       ; lpszMenuName
        dq window_class_name       ; lpszClassName
        dq 0                       ; hIconSm

    hwnd dd 0
    message times MSG_size db 0

    import_table
        library kernel_table, "KERNEL32.DLL", \
            user_table, "USER32.DLL"

        import kernel_table, \
            ExitProcess, "ExitProcess", \
            GetModuleHandleA, "GetModuleHandleA"

        import user_table, \
            CreateWindowExA, "CreateWindowExA", \
            DefWindowProcA, "DefWindowProcA", \
            DispatchMessageA, "DispatchMessageA", \
            GetMessageA, "GetMessageA", \
            LoadCursorA, "LoadCursorA", \
            PostQuitMessage, "PostQuitMessage", \
            RegisterClassExA, "RegisterClassExA", \
            ShowWindow, "ShowWindow", \
            TranslateMessage, "TranslateMessage", \
            UpdateWindow, "UpdateWindow"
    end_import_table
end_data_section
