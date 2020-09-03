    ; window-x64.asm - A 64-bit win32 assembly window program
    ; Made by Bastiaan van der Plaat (https://bastiaan.ml/)
    ; nasm -f bin window-x64.asm -o window-x64.exe && ./window-x64

%include "libwindows-x64.inc"

    ; It works but segmentation faults
    ; With the stack alignment 😭

code_section
    function WindowProc, hwnd, uMsg, wParam, lParam
        mov rax, [uMsg]
        cmp rax, WM_CREATE
        je .wm_create
        cmp rax, WM_GETMINMAXINFO
        je .wm_getminmaxinfo
        cmp rax, WM_PAINT
        je .wm_paint
        cmp rax, WM_DESTROY
        je .wm_destroy
        jmp .default

        .wm_create:
            local window_rect, RECT_size, \
                new_window_width, DWORD_size, \
                new_window_height, DWORD_size, \
                new_window_x, DWORD_size, \
                new_window_y, DWORD_size

            lea rax, [window_rect]
            invoke GetClientRect, [hwnd], rax

            mov rax, [window_width]
            shl rax, 1
            sub rax, [window_rect + RECT.right]
            mov [new_window_width], rax

            mov rax, [window_height]
            shl rax, 1
            sub rax, [window_rect + RECT.bottom]
            mov [new_window_height], rax

            invoke GetSystemMetrics, SM_CXSCREEN
            sub rax, [new_window_width]
            shr rax, 1
            mov [new_window_x], rax

            invoke GetSystemMetrics, SM_CYSCREEN
            sub rax, [new_window_height]
            shr rax, 1
            mov [new_window_y], rax

            invoke SetWindowPos, [hwnd], HWND_TOP, [new_window_x], [new_window_y], [new_window_width], [new_window_height], SWP_NOZORDER

            jmp .leave

            %undef window_rect

        .wm_getminmaxinfo:
            mov rax, [lParam]
            mov dword [rax + MINMAXINFO.ptMinTrackSize + POINT.x], 320
            mov dword [rax + MINMAXINFO.ptMinTrackSize + POINT.y], 240
            jmp .leave

        .wm_paint:
            local paint_struct, PAINTSTRUCT_size, \
                window_rect, RECT_size, \
                hfont, DWORD_size

            lea rcx, [paint_struct]
            invoke BeginPaint, [hwnd], rcx

            lea rcx, [window_rect]
            invoke GetClientRect, [hwnd], rcx

            mov rcx, [window_rect + RECT.right]
            shr rcx, 4
            invoke CreateFontA, rcx, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, font_name
            mov [hfont], rax

            invoke SelectObject, [paint_struct + PAINTSTRUCT.hdc], [hfont]
            invoke SetBkMode, [paint_struct + PAINTSTRUCT.hdc], TRANSPARENT
            invoke SetTextColor, [paint_struct + PAINTSTRUCT.hdc], 0x00ffffff

            lea rcx, [window_rect]
            invoke DrawTextA, [paint_struct + PAINTSTRUCT.hdc], window_title, -1, rcx, DT_SINGLELINE | DT_CENTER | DT_VCENTER

            invoke DeleteObject, [hfont]

            lea rcx, [paint_struct]
            invoke EndPaint, [hwnd], rcx

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
        imul rax, [seed], 1103515245
        add rax, 12345

        mov rdx, 0
        mov rcx, 1 << 31
        idiv rcx

        mov [seed], rdx
        return rdx

    _start:
        sub rsp, 8

        local time, SYSTEMTIME_size, \
            hwnd, 8, \
            message, MSG_size

        lea rax, [time]
        invoke GetLocalTime, rax

        mov rax, [time + SYSTEMTIME.wHour]
        and rax, 0x000000000000ffff
        imul rax, 60

        mov rcx, [time + SYSTEMTIME.wMinute]
        and rcx, 0x000000000000ffff
        add rax, rcx
        imul rax, 60

        mov rcx, [time + SYSTEMTIME.wSecond]
        and rcx, 0x000000000000ffff
        add rax, rcx

        mov [seed], rax

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
            lea rcx, [message]
            invoke GetMessageA, rcx, 0, 0, 0

            cmp rax, 0
            jle .done

            lea rcx, [message]
            invoke TranslateMessage, rcx

            lea rcx, [message]
            invoke DispatchMessageA, rcx

            jmp .message_loop

        .done:
            invoke ExitProcess, [message + MSG.wParam]
end_code_section

data_section
    window_class_name db "test", 0
    window_title db "This is a test window (64-bit)", 0
    font_name db "Tahoma", 0

    seed dq 0
    window_width dq 800
    window_height dq 600

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
