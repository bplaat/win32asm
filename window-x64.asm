    ; window-x64.asm - A 64-bit win32 assembly window program
    ; Made by Bastiaan van der Plaat (https://bastiaan.ml/)
    ; nasm -f bin window-x64.asm -o window-x64.exe && ./window-x64

%include "libwindows-x64.inc"

    ; It is working but local variables are bugged because of stack alignment 😡

code_section
    function WindowProc, hwnd, uMsg, wParam, lParam
        mov eax, [uMsg]
        cmp eax, WM_CREATE
        je .wm_create
        cmp eax, WM_GETMINMAXINFO
        je .wm_getminmaxinfo
        cmp eax, WM_PAINT
        je .wm_paint
        cmp eax, WM_DESTROY
        je .wm_destroy
        jmp .default

        .wm_create:
            lea r10, [window_rect]
            invoke GetClientRect, [hwnd], r10

            mov eax, [window_width]
            shl eax, 1
            sub eax, [window_rect + RECT.right]
            mov [new_window_width], eax

            mov eax, [window_height]
            shl eax, 1
            sub eax, [window_rect + RECT.bottom]
            mov [new_window_height], eax

            invoke GetSystemMetrics, SM_CXSCREEN
            sub eax, [new_window_width]
            shr eax, 1
            mov [new_window_x], eax

            invoke GetSystemMetrics, SM_CYSCREEN
            sub eax, [new_window_height]
            shr eax, 1
            mov [new_window_y], eax

            invoke SetWindowPos, [hwnd], HWND_TOP, [new_window_x], [new_window_y], [new_window_width], [new_window_height], SWP_NOZORDER

            jmp .leave

        .wm_getminmaxinfo:
            mov rax, [lParam]
            mov dword [rax + MINMAXINFO.ptMinTrackSize + POINT.x], 320
            mov dword [rax + MINMAXINFO.ptMinTrackSize + POINT.y], 240
            jmp .leave

        .wm_paint:
            lea r10, [paint_struct]
            invoke BeginPaint, [hwnd], r10

            lea r10, [window_rect]
            invoke GetClientRect, [hwnd], r10

            mov ecx, [window_rect + RECT.right]
            mov r10, rcx
            shr r10, 4
            invoke CreateFontA, r10, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, font_name
            mov [hfont], rax

            invoke SelectObject, [paint_struct + PAINTSTRUCT.hdc], [hfont]
            invoke SetBkMode, [paint_struct + PAINTSTRUCT.hdc], TRANSPARENT
            invoke SetTextColor, [paint_struct + PAINTSTRUCT.hdc], 0x00ffffff

            lea r10, [window_rect]
            invoke DrawTextA, [paint_struct + PAINTSTRUCT.hdc], window_title, -1, r10, DT_SINGLELINE | DT_CENTER | DT_VCENTER

            invoke DeleteObject, [hfont]

            lea r10, [paint_struct]
            invoke EndPaint, [hwnd], r10

            jmp .leave

        .wm_destroy:
            invoke PostQuitMessage, 0
        .leave:
            return 0

        .default:
            invoke DefWindowProcA, [hwnd], [uMsg], [wParam], [lParam]
            return

        %undef hwnd

    function rand
        imul eax, [seed], 1103515245
        add eax, 12345

        mov edx, 0
        mov ecx, 1 << 31
        idiv ecx

        mov [seed], edx
        mov eax, edx
        return

    _start:
        sub rsp, 8

        lea r10, [time]
        invoke GetLocalTime, r10

        movzx eax, word [time + SYSTEMTIME.wHour]
        imul eax, 60

        movzx ecx, word [time + SYSTEMTIME.wMinute]
        add eax, ecx
        imul eax, 60

        movzx ecx, word [time + SYSTEMTIME.wSecond]
        add eax, ecx

        mov [seed], eax

        invoke GetModuleHandleA, 0
        mov [window_class + WNDCLASSEX.hInstance], rax

        invoke LoadCursorA, 0, IDC_ARROW
        mov [window_class + WNDCLASSEX.hCursor], rax

        fastcall rand
        and rax, 0x00ffffff
        invoke CreateSolidBrush, rax
        mov [window_class + WNDCLASSEX.hbrBackground], rax

        invoke RegisterClassExA, window_class

        invoke CreateWindowExA, 0, window_class_name, window_title, WS_OVERLAPPEDWINDOW, \
            CW_USEDEFAULT, CW_USEDEFAULT, [window_width], [window_height], \
            HWND_DESKTOP, 0, [window_class + WNDCLASSEX.hInstance], 0
        mov [hwnd], rax
        invoke ShowWindow, [hwnd], SW_SHOWDEFAULT
        invoke UpdateWindow, [hwnd]

        .message_loop:
            lea r10, [message]
            invoke GetMessageA, r10, 0, 0, 0

            cmp rax, 0
            jle .done

            lea r10, [message]
            invoke TranslateMessage, r10

            lea r10, [message]
            invoke DispatchMessageA, r10

            jmp .message_loop

        .done:
            invoke ExitProcess, [message + MSG.wParam]
end_code_section

data_section
    window_class_name db "test", 0
    window_title db "This is a test window (64-bit)", 0
    font_name db "Tahoma", 0

    seed dd 0
    window_width dd 800
    window_height dd 600

    window_class:
        dd WNDCLASSEX_size         ; cbSize
        dd CS_HREDRAW | CS_VREDRAW ; style
        dq WindowProc              ; lpfnWndProc
        dd 0                       ; cbClsExtra
        dd 0                       ; cbWndExtra
        dq 0                       ; hInstance
        dq 0                       ; hIcon
        dq 0                       ; hCursor
        dq 0                       ; hbrBackground
        dq 0                       ; lpszMenuName
        dq window_class_name       ; lpszClassName
        dq 0                       ; hIconSm

    window_rect times RECT_size db 0
    new_window_width dd 0
    new_window_height dd 0
    new_window_x dd 0
    new_window_y dd 0

    paint_struct times PAINTSTRUCT_size db 0
    hfont dq 0

    time times SYSTEMTIME_size db 0
    hwnd dq 0
    message times MSG_size db 0

    import_table
        library gdi_table, "GDI32.DLL", \
            kernel_table, "KERNEL32.DLL", \
            user_table, "USER32.DLL"

        import gdi_table, \
            CreateFontA, "CreateFontA", \
            CreateSolidBrush, "CreateSolidBrush", \
            DeleteObject, "DeleteObject", \
            SelectObject, "SelectObject", \
            SetBkMode, "SetBkMode", \
            SetTextColor, "SetTextColor"

        import kernel_table, \
            ExitProcess, "ExitProcess", \
            GetModuleHandleA, "GetModuleHandleA", \
            GetLocalTime, "GetLocalTime"

        import user_table, \
            BeginPaint, "BeginPaint", \
            CreateWindowExA, "CreateWindowExA", \
            DefWindowProcA, "DefWindowProcA", \
            DispatchMessageA, "DispatchMessageA", \
            DrawTextA, "DrawTextA", \
            EndPaint, "EndPaint", \
            GetClientRect, "GetClientRect", \
            GetMessageA, "GetMessageA", \
            GetSystemMetrics, "GetSystemMetrics", \
            LoadCursorA, "LoadCursorA", \
            PostQuitMessage, "PostQuitMessage", \
            RegisterClassExA, "RegisterClassExA", \
            ShowWindow, "ShowWindow", \
            SetWindowPos, "SetWindowPos", \
            TranslateMessage, "TranslateMessage", \
            UpdateWindow, "UpdateWindow"
    end_import_table
end_data_section
