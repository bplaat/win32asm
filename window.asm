    ; window.asm - An pure 32-bit and 64-bit win32 assembly window program
    ; Made by Bastiaan van der Plaat (https://bastiaan.ml/)
    ; 32-bit: nasm -f bin window.asm -o window-x86.exe && ./window-x86
    ; 64-bit: nasm -DWIN64 -f bin window.asm -o window-x64.exe && ./window-x64

%include "libwindows.inc"

header

code_section
    ; ### Simple random number generator code ###

    ; Generate rand seed by time
    function rand_generate_seed
        local time, SYSTEMTIME_size

        invoke GetLocalTime, addr time

        movzx eax, word [time + SYSTEMTIME.wHour]
        imul eax, 60

        movzx ecx, word [time + SYSTEMTIME.wMinute]
        add eax, ecx
        imul eax, 60

        movzx ecx, word [time + SYSTEMTIME.wSecond]
        add eax, ecx

        mov [seed], eax

        return

    ; Simple random number generator function
    function rand_rand
        imul eax, [seed], 1103515245
        add eax, 12345

        xor edx, edx
        mov ecx, 1 << 31
        idiv ecx

        mov [seed], edx
        return _dx

    ; ### Window code ###

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
                new_window_rect, Rect_size

            frame

            invoke GetClientRect, [hwnd], addr window_rect

            mov eax, [window_width]
            shl eax, 1
            sub eax, [window_rect + RECT.right]
            mov [new_window_rect + Rect.width], eax

            mov eax, [window_height]
            shl eax, 1
            sub eax, [window_rect + RECT.bottom]
            mov [new_window_rect + Rect.height], eax

            invoke GetSystemMetrics, SM_CXSCREEN
            sub eax, [new_window_rect + Rect.width]
            shr eax, 1
            mov [new_window_rect + Rect.x], eax

            invoke GetSystemMetrics, SM_CYSCREEN
            sub eax, [new_window_rect + Rect.height]
            shr eax, 1
            mov [new_window_rect + Rect.y], eax

            invoke SetWindowPos, [hwnd], HWND_TOP, [new_window_rect + Rect.x], [new_window_rect + Rect.y], [new_window_rect + Rect.width], [new_window_rect + Rect.height], SWP_NOZORDER

            end_frame

            jmp .leave

            %undef window_rect

        .wm_getminmaxinfo:
            ; Set window min size
            mov _ax, [lParam]
            mov dword [_ax + MINMAXINFO.ptMinTrackSize + POINT.x], 320
            mov dword [_ax + MINMAXINFO.ptMinTrackSize + POINT.y], 240
            jmp .leave

        .wm_paint:
            ; Draw a centered text in the window
            local paint_struct, PAINTSTRUCT_size, \
                window_rect, RECT_size, \
                font, DWORD_size

            frame

            invoke BeginPaint, [hwnd], addr paint_struct

            invoke GetClientRect, [hwnd], addr window_rect

            mov eax, [window_rect + RECT.right]
            shr eax, 4
            invoke CreateFontA, _ax, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, font_name
            mov [font], _ax

            invoke SelectObject, [paint_struct + PAINTSTRUCT.hdc], [font]
            invoke SetBkMode, [paint_struct + PAINTSTRUCT.hdc], TRANSPARENT
            invoke SetTextColor, [paint_struct + PAINTSTRUCT.hdc], 0x00ffffff

            invoke DrawTextA, [paint_struct + PAINTSTRUCT.hdc], window_title, -1, addr window_rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER

            invoke DeleteObject, [font]

            invoke EndPaint, [hwnd], addr paint_struct

            end_frame

            jmp .leave

        .wm_destroy:
            invoke PostQuitMessage, 0
        .leave:
            return 0

        .default:
            invoke DefWindowProcA, [hwnd], [uMsg], [wParam], [lParam]
            return

        %undef hwnd

    ; Main entry point
    entrypoint
        local window_class, WNDCLASSEX_size, \
            hwnd, POINTER_size, \
            message, MSG_size

        frame

        ; Generate rand seed
        fcall rand_generate_seed

        ; Register the window class
        mov dword [window_class + WNDCLASSEX.cbSize], WNDCLASSEX_size

        mov dword [window_class + WNDCLASSEX.style], CS_HREDRAW | CS_VREDRAW

        mov pointer [window_class + WNDCLASSEX.lpfnWndProc], WindowProc

        mov dword [window_class + WNDCLASSEX.cbClsExtra], 0

        mov dword [window_class + WNDCLASSEX.cbWndExtra], 0

        invoke GetModuleHandleA, NULL
        mov [window_class + WNDCLASSEX.hInstance], _ax

        invoke LoadIconA, NULL, IDI_APPLICATION
        mov [window_class + WNDCLASSEX.hIcon], _ax
        mov [window_class + WNDCLASSEX.hIconSm], _ax

        invoke LoadCursorA, NULL, IDC_ARROW
        mov [window_class + WNDCLASSEX.hCursor], _ax

        fcall rand_rand
        and eax, 0x00ffffff
        invoke CreateSolidBrush, _ax
        mov [window_class + WNDCLASSEX.hbrBackground], _ax

        mov pointer [window_class + WNDCLASSEX.lpszMenuName], NULL

        mov pointer [window_class + WNDCLASSEX.lpszClassName], window_class_name

        invoke RegisterClassExA, addr window_class

        ; Create the window
        invoke CreateWindowExA, 0, window_class_name, window_title, WS_OVERLAPPEDWINDOW, \
            CW_USEDEFAULT, CW_USEDEFAULT, [window_width], [window_height], \
            HWND_DESKTOP, NULL, [window_class + WNDCLASSEX.hInstance], NULL
        mov [hwnd], _ax
        invoke ShowWindow, [hwnd], SW_SHOWDEFAULT
        invoke UpdateWindow, [hwnd]

        ; Message loop
        .message_loop:
            invoke GetMessageA, addr message, NULL, 0, 0
            cmp _ax, 0
            jle .done

            invoke TranslateMessage, addr message
            invoke DispatchMessageA, addr message
            jmp .message_loop
        .done:
            invoke ExitProcess, [message + MSG.wParam]
            end_frame
end_code_section

data_section
    ; String constants
    window_class_name db "window-test", 0
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
            GetLocalTime, "GetLocalTime", \
            GetModuleHandleA, "GetModuleHandleA"

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
            LoadIconA, "LoadIconA", \
            PostQuitMessage, "PostQuitMessage", \
            RegisterClassExA, "RegisterClassExA", \
            SetWindowPos, "SetWindowPos", \
            ShowWindow, "ShowWindow", \
            TranslateMessage, "TranslateMessage", \
            UpdateWindow, "UpdateWindow"
    end_import_table
end_data_section
