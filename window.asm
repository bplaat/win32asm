    ; window.asm - An pure 32-bit and 64-bit win32 assembly window program
    ; Made by Bastiaan van der Plaat (https://bastiaan.ml/)
    ; 32-bit: nasm -f bin window.asm -o window-x86.exe && ./window-x86
    ; 64-bit: nasm -DWIN64 -f bin window.asm -o window-x64.exe && ./window-x64

%ifdef WIN64
    %include "libwindows-x64.inc"
%else
    %include "libwindows-x86.inc"
%endif

code_section
    ; Window procedure function
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
            ; Center new created window
            local window_rect, RECT_size, \
                new_window_width, DWORD_size, \
                new_window_height, DWORD_size, \
                new_window_x, DWORD_size, \
                new_window_y, DWORD_size

            lea _ax, [window_rect]
            invoke GetClientRect, [hwnd], _ax

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
            ; Set window min size
            mov _ax, [lParam]
            mov DWORD_size_word [_ax + MINMAXINFO.ptMinTrackSize + POINT.x], 320
            mov DWORD_size_word [_ax + MINMAXINFO.ptMinTrackSize + POINT.y], 240
            jmp .leave

        .wm_paint:
            ; Draw a centered text in the window
            local paint_struct, PAINTSTRUCT_size, \
                window_rect, RECT_size, \
                hfont, DWORD_size

            lea _ax, [paint_struct]
            invoke BeginPaint, [hwnd], _ax

            lea _ax, [window_rect]
            invoke GetClientRect, [hwnd], _ax

            mov eax, [window_rect + RECT.right]
            shr eax, 4
            invoke CreateFontA, _ax, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, font_name
            mov [hfont], _ax

            invoke SelectObject, [paint_struct + PAINTSTRUCT.hdc], [hfont]
            invoke SetBkMode, [paint_struct + PAINTSTRUCT.hdc], TRANSPARENT
            invoke SetTextColor, [paint_struct + PAINTSTRUCT.hdc], 0x00ffffff

            lea _ax, [window_rect]
            invoke DrawTextA, [paint_struct + PAINTSTRUCT.hdc], window_title, -1, _ax, DT_SINGLELINE | DT_CENTER | DT_VCENTER

            invoke DeleteObject, [hfont]

            lea _ax, [paint_struct]
            invoke EndPaint, [hwnd], _ax

            jmp .leave

        .wm_destroy:
            invoke PostQuitMessage, 0
        .leave:
            return 0

        .default:
            invoke DefWindowProcA, [hwnd], [uMsg], [wParam], [lParam]
            return

        %undef hwnd

    ; Simple random number generator function
    function rand
        imul eax, [seed], 1103515245
        add eax, 12345

        mov edx, 0
        mov ecx, 1 << 31
        idiv ecx

        mov [seed], edx
        return _dx

    ; Main entry point
    entrypoint
        local time, SYSTEMTIME_size, \
            window_class, WNDCLASSEX_size, \
            hwnd, DWORD_size, \
            message, MSG_size

        ; Fill the seed with current time in seconds
        lea _ax, [time]
        invoke GetLocalTime, _ax

        movzx eax, WORD_size_word [time + SYSTEMTIME.wHour]
        imul eax, 60

        movzx ecx, WORD_size_word [time + SYSTEMTIME.wMinute]
        add eax, ecx
        imul eax, 60

        movzx ecx, WORD_size_word [time + SYSTEMTIME.wSecond]
        add eax, ecx

        mov [seed], eax

        ; Register the window class
        mov DWORD_size_word [window_class + WNDCLASSEX.cbSize], WNDCLASSEX_size

        mov DWORD_size_word [window_class + WNDCLASSEX.style], CS_HREDRAW | CS_VREDRAW

        mov HANDLE_size_word [window_class + WNDCLASSEX.lpfnWndProc], WindowProc

        mov DWORD_size_word [window_class + WNDCLASSEX.cbClsExtra], 0

        mov DWORD_size_word [window_class + WNDCLASSEX.cbWndExtra], 0

        invoke GetModuleHandleA, 0
        mov [window_class + WNDCLASSEX.hInstance], _ax

        mov HANDLE_size_word [window_class + WNDCLASSEX.hIcon], 0

        invoke LoadCursorA, 0, IDC_ARROW
        mov [window_class + WNDCLASSEX.hCursor], _ax

        fcall rand
        and eax, 0x00ffffff
        invoke CreateSolidBrush, _ax
        mov [window_class + WNDCLASSEX.hbrBackground], _ax

        mov HANDLE_size_word [window_class + WNDCLASSEX.lpszMenuName], 0

        mov HANDLE_size_word [window_class + WNDCLASSEX.lpszClassName], window_class_name

        mov HANDLE_size_word [window_class + WNDCLASSEX.hIconSm], 0

        lea _ax, [window_class]
        invoke RegisterClassExA, _ax

        ; Create the window
        invoke CreateWindowExA, 0, window_class_name, window_title, WS_OVERLAPPEDWINDOW, \
            CW_USEDEFAULT, CW_USEDEFAULT, [window_width], [window_height], \
            HWND_DESKTOP, 0, [window_class + WNDCLASSEX.hInstance], 0
        mov [hwnd], _ax
        invoke ShowWindow, [hwnd], SW_SHOWDEFAULT
        invoke UpdateWindow, [hwnd]

        ; Message loop
        .message_loop:
            lea _ax, [message]
            invoke GetMessageA, _ax, 0, 0, 0

            cmp _ax, 0
            jle .done

            lea _ax, [message]
            invoke TranslateMessage, _ax

            lea _ax, [message]
            invoke DispatchMessageA, _ax

            jmp .message_loop
        .done:
            invoke ExitProcess, [message + MSG.wParam]
end_code_section

data_section
    ; String constants
    window_class_name db "test", 0
    %ifdef WIN64
        window_title db "This is a test window (64-bit)", 0
    %else
        window_title db "This is a test window (32-bit)", 0
    %endif
    font_name db "Tahoma", 0

    ; Global variables
    seed dd 0
    window_width dd 800
    window_height dd 600

    ; Import table
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
