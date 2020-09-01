    ; window-x86.asm - A 32-bit win32 assembly window program
    ; Made by Bastiaan van der Plaat (https://bastiaan.ml/)
    ; nasm -f bin window-x86.asm -o window-x86.exe && ./window-x86

%include "libwindows-x86.inc"

code_section
    WindowProc:
        push ebp
        mov ebp, esp
        %define hwnd dword [ebp + 2 * 4]
        %define uMsg dword [ebp + 3 * 4]
        %define wParam dword [ebp + 4 * 4]
        %define lParam dword [ebp + 5 * 4]

        mov eax, uMsg
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
            invoke GetClientRect, hwnd, window_rect

            mov eax, [window_width]
            shl eax, 1
            sub eax, [window_rect + RECT.right]
            mov [new_window_width], eax

            mov eax, [window_height]
            shl eax, 1
            sub eax, [window_rect + RECT.bottom]
            mov [new_window_height], eax

            push_reverse [new_window_width], [new_window_height], SWP_NOZORDER

            invoke GetSystemMetrics, SM_CYSCREEN
            sub eax, [new_window_height]
            shr eax, 1
            push eax

            invoke GetSystemMetrics, SM_CXSCREEN
            sub eax, [new_window_width]
            shr eax, 1
            push eax

            invoke SetWindowPos, [ebp + 8], 0

            jmp .leave

        .wm_getminmaxinfo:
            mov eax, lParam
            mov dword [eax + MINMAXINFO.ptMinTrackSize + POINT.x], 320
            mov dword [eax + MINMAXINFO.ptMinTrackSize + POINT.y], 240

            jmp .leave

        .wm_paint:
            invoke BeginPaint, hwnd, paint_struct

            invoke GetClientRect, hwnd, window_rect

            push_reverse 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, font_name
            mov eax, [window_rect + RECT.right]
            shr eax, 4
            invoke CreateFontA, eax
            mov [hfont], eax

            invoke SelectObject, [paint_struct + PAINTSTRUCT.hdc], [hfont]
            invoke SetBkMode, [paint_struct + PAINTSTRUCT.hdc], TRANSPARENT
            invoke SetTextColor, [paint_struct + PAINTSTRUCT.hdc], 0x00ffffff
            invoke DrawTextA, [paint_struct + PAINTSTRUCT.hdc], window_title, -1, window_rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER
            invoke DeleteObject, [hfont]

            invoke EndPaint, hwnd, paint_struct

            jmp .leave

        .wm_destroy:
            invoke PostQuitMessage, 0
        .leave:
            xor eax, eax
            leave
            ret 16

        .default:
            invoke DefWindowProcA, hwnd, uMsg, wParam, lParam
            leave
            ret 16

        %undef hwnd

    rand:
        push ebp
        mov ebp, esp

        mov eax, [seed]
        imul eax, 1103515245

        add eax, 12345

        xor edx, edx
        mov ebx, 1 << 31
        idiv ebx

        mov [seed], edx
        mov eax, edx

        leave
        ret

    _start:
        invoke GetLocalTime, time
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

        call rand
        shr eax, 8
        invoke CreateSolidBrush, eax
        mov [window_class + WNDCLASSEX.hbrBackground], eax

        invoke RegisterClassExA, window_class

        invoke CreateWindowExA, 0, window_class_name, window_title, WS_OVERLAPPEDWINDOW, \
            CW_USEDEFAULT, CW_USEDEFAULT, [window_width], [window_height], HWND_DESKTOP, 0, [window_class + WNDCLASSEX.hInstance], 0
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

    hwnd:
    hfont:
    new_window_width dd 0
    new_window_height dd 0
    time:
    message times MSG_size db 0
    window_rect times RECT_size db 0
    paint_struct times PAINTSTRUCT_size db 0

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
