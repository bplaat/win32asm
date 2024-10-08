    ; window.asm - An pure 32-bit and 64-bit win32 assembly window program
    ; Made by Bastiaan van der Plaat (https://bplaat.nl/)
    ; 32-bit: nasm -f bin window.asm -o window-x86.exe && ./window-x86
    ; 64-bit: nasm -DWIN64 -f bin window.asm -o window-x64.exe && ./window-x64

%include "libwindows.inc"

header

code_section
    ; ### Some stdlib like Win32 wrappers ###
    function malloc, size
        invoke GetProcessHeap
        invoke HeapAlloc, _ax, 0, [size]
        return

    function free, ptr
        invoke GetProcessHeap
        invoke HeapFree, _ax, 0, [ptr]
        return

    function strlen, string
        mov _si, [string]
        mov al, [_si]
        while al, "!=", 0
            inc _si
            mov al, [_si]
        end_while
        sub _si, [string]
        return _si

    function srand, seed
        mov eax, [seed]
        mov [rand_seed], eax
        return

    function rand
        imul eax, [rand_seed], 1103515245
        add eax, 12345

        xor edx, edx
        mov ecx, 1 << 31
        idiv ecx

        mov [rand_seed], edx
        return _dx

    ; ### Window code ###
    struct WindowData, \
        background_color, DWORD_size

    ; Window procedure function
    function WindowProc, hwnd, uMsg, wParam, lParam
        mov eax, [uMsg]
        cmp eax, WM_CREATE
        je .wm_create
        cmp eax, WM_SIZE
        je .wm_size
        cmp eax, WM_GETMINMAXINFO
        je .wm_getminmaxinfo
        cmp eax, WM_ERASEBKGND
        je .wm_erasebkgnd
        cmp eax, WM_PAINT
        je .wm_paint
        cmp eax, WM_DESTROY
        je .wm_destroy
        jmp .default

        .wm_create:
            local window_data, POINTER_size, \
                time, SYSTEMTIME_size, \
                window_rect, RECT_size, \
                new_window_rect, Rect_size

            ; Create window data
            fcall malloc, WindowData_size
            mov [window_data], _ax
            invoke SetWindowLongPtrA, [hwnd], GWLP_USERDATA, _ax

            ; Generate random seed by time
            invoke GetLocalTime, addr time

            movzx eax, word [time + SYSTEMTIME.wHour]
            imul eax, 60

            movzx ecx, word [time + SYSTEMTIME.wMinute]
            add eax, ecx
            imul eax, 60

            movzx ecx, word [time + SYSTEMTIME.wSecond]
            add eax, ecx

            fcall srand, _ax

            ; Generate random background color
            fcall rand
            and eax, 0x007f7f7f
            mov _di, [window_data]
            mov [_di + WindowData.background_color], eax

            ; Center window
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

            end_local
            return 0

            %undef window_data

        .wm_size:
            ; Save new window size
            movzx eax, word [lParam]
            mov [window_width], eax

            mov eax, [lParam]
            shr eax, 16
            mov [window_height], eax

            return 0

        .wm_getminmaxinfo:
            ; Set window min size
            mov _di, [lParam]
            mov dword [_di + MINMAXINFO.ptMinTrackSize + POINT.x], 320
            mov dword [_di + MINMAXINFO.ptMinTrackSize + POINT.y], 240

            return 0

        .wm_erasebkgnd:
            ; Draw no background
            return TRUE

        .wm_paint:
            local window_data, POINTER_size, \
                paint_struct, PAINTSTRUCT_size, \
                hdc_buffer, POINTER_size, \
                bitmap_buffer, POINTER_size, \
                brush, POINTER_size, \
                rect, RECT_size, \
                font_size, DWORD_size, \
                font, POINTER_size

            ; Get window data
            invoke GetWindowLongPtrA, [hwnd], GWLP_USERDATA
            mov [window_data], _ax

            ; Begin paint
            invoke BeginPaint, [hwnd], addr paint_struct

            ; Create back buffer
            invoke CreateCompatibleDC, [paint_struct + PAINTSTRUCT.hdc]
            mov [hdc_buffer], _ax
            invoke CreateCompatibleBitmap, [paint_struct + PAINTSTRUCT.hdc], [window_width], [window_height]
            mov [bitmap_buffer], _ax
            invoke SelectObject, [hdc_buffer], [bitmap_buffer]

            ; Draw background color
            mov _si, [window_data]
            invoke CreateSolidBrush, [_si + WindowData.background_color]
            mov [brush], _ax

            mov dword [rect + RECT.left], 0
            mov dword [rect + RECT.top], 0
            mov eax, [window_width]
            mov [rect + RECT.right], eax
            mov eax, [window_height]
            mov [rect + RECT.bottom], eax

            invoke FillRect, [hdc_buffer], addr rect, [brush]

            invoke DeleteObject, [brush]

            ; Draw centered text
            mov eax, [window_width]
            shr eax, 4
            mov [font_size], eax

            invoke CreateFontA, [font_size], 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, \
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name
            mov [font], _ax

            invoke SelectObject, [hdc_buffer], [font]
            invoke SetBkMode, [hdc_buffer], TRANSPARENT
            invoke SetTextColor, [hdc_buffer], 0x00ffffff
            invoke SetTextAlign, [hdc_buffer], TA_CENTER

            fcall strlen, window_title
            mov esi, [window_width]
            shr esi, 1
            mov edi, [window_height]
            sub edi, [font_size]
            shr edi, 1
            invoke TextOutA, [hdc_buffer], _si, _di, window_title, _ax

            invoke DeleteObject, [font]

            ; Draw and delete back buffer
            invoke BitBlt, [paint_struct + PAINTSTRUCT.hdc], 0, 0, [window_width], [window_height], [hdc_buffer], 0, 0, SRCCOPY
            invoke DeleteObject, [bitmap_buffer]
            invoke DeleteDC, [hdc_buffer]

            ; End paint
            invoke EndPaint, [hwnd], addr paint_struct

            end_local
            return 0

        .wm_destroy:
            ; Free window data
            invoke GetWindowLongPtrA, [hwnd], GWLP_USERDATA
            fcall free, _ax

            ; Close process
            invoke PostQuitMessage, 0

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

        mov pointer [window_class + WNDCLASSEX.hbrBackground], NULL

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
        loop
            invoke GetMessageA, addr message, NULL, 0, 0
            test _ax, _ax
            jle %$end_loop

            invoke TranslateMessage, addr message
            invoke DispatchMessageA, addr message
        end_loop

        invoke ExitProcess, [message + MSG.wParam]

        end_local
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
    rand_seed dd 0
    window_width dd 800
    window_height dd 600

    ; Import table
    import_table
        library gdi_table, "GDI32.DLL", \
            kernel_table, "KERNEL32.DLL", \
            user_table, "USER32.DLL"

        import gdi_table, \
            BitBlt, "BitBlt", \
            CreateCompatibleBitmap, "CreateCompatibleBitmap", \
            CreateCompatibleDC, "CreateCompatibleDC", \
            CreateFontA, "CreateFontA", \
            CreateSolidBrush, "CreateSolidBrush", \
            DeleteDC, "DeleteDC", \
            DeleteObject, "DeleteObject", \
            SelectObject, "SelectObject", \
            SetBkMode, "SetBkMode", \
            SetTextAlign, "SetTextAlign", \
            SetTextColor, "SetTextColor", \
            TextOutA, "TextOutA"

        import kernel_table, \
            ExitProcess, "ExitProcess", \
            GetLocalTime, "GetLocalTime", \
            GetModuleHandleA, "GetModuleHandleA", \
            GetProcessHeap, "GetProcessHeap", \
            HeapAlloc, "HeapAlloc", \
            HeapFree, "HeapFree"

        import user_table, \
            BeginPaint, "BeginPaint", \
            CreateWindowExA, "CreateWindowExA", \
            DefWindowProcA, "DefWindowProcA", \
            DispatchMessageA, "DispatchMessageA", \
            EndPaint, "EndPaint", \
            FillRect, "FillRect", \
            GetClientRect, "GetClientRect", \
            GetMessageA, "GetMessageA", \
            GetSystemMetrics, "GetSystemMetrics", \
            GetWindowLongPtrA, GetWindowLongPtrAString, \
            LoadCursorA, "LoadCursorA", \
            LoadIconA, "LoadIconA", \
            PostQuitMessage, "PostQuitMessage", \
            RegisterClassExA, "RegisterClassExA", \
            SetWindowLongPtrA, SetWindowLongPtrAString, \
            SetWindowPos, "SetWindowPos", \
            ShowWindow, "ShowWindow", \
            TranslateMessage, "TranslateMessage", \
            UpdateWindow, "UpdateWindow"
    end_import_table
end_data_section
