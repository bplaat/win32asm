    ; window-x86.asm - A 32-bit win32 assembly window program
    ; Made by Bastiaan van der Plaat (https://bastiaan.ml/)
    ; nasm -f bin window-x86.asm -o window-x86.exe && ./window-x86

%include "libwindows-x86.inc"

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
            local window_rect, RECT_size, \
                new_window_width, 4, \
                new_window_height, 4, \
                new_window_x, 4, \
                new_window_y, 4

            lea eax, [window_rect]
            invoke GetClientRect, [hwnd], eax

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

            %undef window_rect

        .wm_getminmaxinfo:
            mov eax, [lParam]
            mov dword [eax + MINMAXINFO.ptMinTrackSize + POINT.x], 320
            mov dword [eax + MINMAXINFO.ptMinTrackSize + POINT.y], 240
            jmp .leave

        .wm_paint:
            local paint_struct, PAINTSTRUCT_size, \
                window_rect, RECT_size, \
                hfont, 4

            lea eax, [paint_struct]
            invoke BeginPaint, [hwnd], eax

            lea eax, [window_rect]
            invoke GetClientRect, [hwnd], eax

            mov eax, [window_rect + RECT.right]
            shr eax, 4
            invoke CreateFontA, eax, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, font_name
            mov [hfont], eax

            invoke SelectObject, [paint_struct + PAINTSTRUCT.hdc], [hfont]
            invoke SetBkMode, [paint_struct + PAINTSTRUCT.hdc], TRANSPARENT
            invoke SetTextColor, [paint_struct + PAINTSTRUCT.hdc], 0x00ffffff

            lea eax, [window_rect]
            invoke DrawTextA, [paint_struct + PAINTSTRUCT.hdc], window_title, -1, eax, DT_SINGLELINE | DT_CENTER | DT_VCENTER

            invoke DeleteObject, [hfont]

            lea eax, [paint_struct]
            invoke EndPaint, [hwnd], eax

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
        return edx

    _start:
        local time, SYSTEMTIME_size, \
            hwnd, 4, \
            message, MSG_size

        lea eax, [time]
        invoke GetLocalTime, eax
        mov eax, [time + SYSTEMTIME.wHour]
        imul eax, 60
        add eax, [time + SYSTEMTIME.wMinute]
        imul eax, 60
        add eax, [time + SYSTEMTIME.wSecond]
        mov [seed], eax

        invoke GetModuleHandleA, 0
        mov [window_class + WNDCLASSEX.hInstance], eax

        invoke LoadCursorA, 0, IDC_ARROW
        mov [window_class + WNDCLASSEX.hCursor], eax

        stdcall rand
        and eax, 0x00ffffff
        invoke CreateSolidBrush, eax
        mov [window_class + WNDCLASSEX.hbrBackground], eax

        invoke RegisterClassExA, window_class

        invoke CreateWindowExA, 0, window_class_name, window_title, WS_OVERLAPPEDWINDOW, \
            CW_USEDEFAULT, CW_USEDEFAULT, [window_width], [window_height], \
            HWND_DESKTOP, 0, [window_class + WNDCLASSEX.hInstance], 0
        mov [hwnd], eax
        invoke ShowWindow, [hwnd], SW_SHOWDEFAULT
        invoke UpdateWindow, [hwnd]

        .message_loop:
            lea eax, [message]
            invoke GetMessageA, eax, 0, 0, 0

            cmp eax, 0
            jle .done

            lea eax, [message]
            invoke TranslateMessage, eax

            lea eax, [message]
            invoke DispatchMessageA, eax

            jmp .message_loop

        .done:
            invoke ExitProcess, [message + MSG.wParam]
end_code_section

data_section
    window_class_name db "test", 0
    window_title db "This is a test window (32-bit)", 0
    font_name db "Tahoma", 0

    seed dd 0
    window_width dd 800
    window_height dd 600

    window_class:
        dd WNDCLASSEX_size         ; cbSize
        dd CS_HREDRAW | CS_VREDRAW ; style
        dd WindowProc              ; lpfnWndProc
        dd 0                       ; cbClsExtra
        dd 0                       ; cbWndExtra
        dd 0                       ; hInstance
        dd 0                       ; hIcon
        dd 0                       ; hCursor
        dd 0                       ; hbrBackground
        dd 0                       ; lpszMenuName
        dd window_class_name       ; lpszClassName
        dd 0                       ; hIconSm

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
