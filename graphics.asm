    ; graphics.asm - An pure 32-bit and 64-bit win32 assembly GDI+ program
    ; Because the flat GDI+ API gives antialiased drawing methods
    ; Made by Bastiaan van der Plaat (https://bastiaan.ml/)
    ; 32-bit: nasm -f bin graphics.asm -o graphics-x86.exe && ./graphics-x86
    ; 64-bit: nasm -DWIN64 -f bin graphics.asm -o graphics-x64.exe && ./graphics-x64

%ifdef WIN64
    %include "libwindows-x64.inc"
%else
    %include "libwindows-x86.inc"
%endif

; A simple rect struct
struct Rect, \
    x, DWORD_size, \
    y, DWORD_size, \
    width, DWORD_size, \
    height, DWORD_size

code_section
    ; ### Simple random number generator code ###

    ; Generate rand seed by time
    function rand_generate_seed
        local time, SYSTEMTIME_size

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

        return

    ; Simple random number generator function
    function rand_rand
        imul eax, [seed], 1103515245
        add eax, 12345

        mov edx, 0
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
            ; Generate random background color
            fcall rand_rand
            and eax, 0x00ffffff
            or eax, 0xff000000
            mov [window_background_color], eax

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
            ; Draw stuff in the window via GDI+
            local paint_struct, PAINTSTRUCT_size, \
                window_rect, RECT_size, \
                graphics, POINTER_size, \
                pen, POINTER_size, \
                items_rect, Rect_size

            lea _ax, [paint_struct]
            invoke BeginPaint, [hwnd], _ax

            lea _ax, [window_rect]
            invoke GetClientRect, [hwnd], _ax

            ; Create GDI+ Graphics object
            lea _ax, [graphics]
            invoke GdipCreateFromHDC, [paint_struct + PAINTSTRUCT.hdc], _ax

            ; Enable anti aliasing
            invoke GdipSetSmoothingMode, [graphics], SmoothingModeAntiAlias

            ; Clear screen with window background color
            invoke GdipGraphicsClear, [graphics], [window_background_color]

            ; Create GDI+ pen object
            lea _ax, [pen]
            invoke GdipCreatePen1, 0xffffffff, [line_width], UnitPixel, _ax

            ; Draw a cross with lines
            invoke GdipDrawLineI, [graphics], [pen], 0, 0, [window_rect + RECT.right], [window_rect + RECT.bottom]
            invoke GdipDrawLineI, [graphics], [pen], [window_rect + RECT.right], 0, 0, [window_rect + RECT.bottom]

            ; Calculate items rect
            mov eax, [window_rect + RECT.right]
            shr eax, 2
            mov [items_rect + Rect.x], eax

            mov eax, [window_rect + RECT.bottom]
            shr eax, 2
            mov [items_rect + Rect.y], eax

            mov eax, [window_rect + RECT.right]
            shr eax, 1
            mov [items_rect + Rect.width], eax

            mov eax, [window_rect + RECT.bottom]
            shr eax, 1
            mov [items_rect + Rect.height], eax

            ; Draw a circle and a rect in the middle with items rect
            invoke GdipDrawEllipseI, [graphics], [pen], [items_rect + Rect.x], [items_rect + Rect.y], [items_rect + Rect.width], [items_rect + Rect.height]
            invoke GdipDrawRectangleI, [graphics], [pen], [items_rect + Rect.x], [items_rect + Rect.y], [items_rect + Rect.width], [items_rect + Rect.height]

            ; Delete the GDI+ objects
            invoke GdipDeletePen, [pen]
            invoke GdipDeleteGraphics, [graphics]

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

    ; Main entry point
    entrypoint
        local gdiplusToken, DWORD_size, \
            gdiplusStartupInput, GdiplusStartupInput_size, \
            window_class, WNDCLASSEX_size, \
            hwnd, DWORD_size, \
            message, MSG_size

        ; Generate rand seed
        fcall rand_generate_seed

        ; Startup GDI+
        mov DWORD_size_word [gdiplusStartupInput + GdiplusStartupInput.GdiplusVersion], 1
        mov POINTER_size_word [gdiplusStartupInput + GdiplusStartupInput.DebugEventCallback], 0
        mov DWORD_size_word [gdiplusStartupInput + GdiplusStartupInput.SuppressBackgroundThread], 0
        mov DWORD_size_word [gdiplusStartupInput + GdiplusStartupInput.SuppressExternalCodecs], 0

        lea _bx, [gdiplusStartupInput]
        lea _ax, [gdiplusToken]
        invoke GdiplusStartup, _ax, _bx, 0

        ; Register the window class
        mov DWORD_size_word [window_class + WNDCLASSEX.cbSize], WNDCLASSEX_size

        mov DWORD_size_word [window_class + WNDCLASSEX.style], CS_HREDRAW | CS_VREDRAW

        mov POINTER_size_word [window_class + WNDCLASSEX.lpfnWndProc], WindowProc

        mov DWORD_size_word [window_class + WNDCLASSEX.cbClsExtra], 0

        mov DWORD_size_word [window_class + WNDCLASSEX.cbWndExtra], 0

        invoke GetModuleHandleA, 0
        mov [window_class + WNDCLASSEX.hInstance], _ax

        invoke LoadIconA, 0, IDI_APPLICATION
        mov [window_class + WNDCLASSEX.hIcon], _ax
        mov [window_class + WNDCLASSEX.hIconSm], _ax

        invoke LoadCursorA, 0, IDC_ARROW
        mov [window_class + WNDCLASSEX.hCursor], _ax

        mov POINTER_size_word [window_class + WNDCLASSEX.hbrBackground], COLOR_WINDOW + 1

        mov POINTER_size_word [window_class + WNDCLASSEX.lpszMenuName], 0

        mov POINTER_size_word [window_class + WNDCLASSEX.lpszClassName], window_class_name

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
            ; Shutdown GDI+
            invoke GdiplusShutdown, [gdiplusToken]

            invoke ExitProcess, [message + MSG.wParam]
end_code_section

data_section
    ; String constants
    window_class_name db "graphics-test", 0
    %ifdef WIN64
        window_title db "This is a test GDI+ window (64-bit)", 0
    %else
        window_title db "This is a test GDI+ window (32-bit)", 0
    %endif
    font_name db "Tahoma", 0

    ; Global variables
    seed dd 0
    window_width dd 1280
    window_height dd 720
    window_background_color dd 0

    ; Float variables
    line_width dd 1.0

    ; Import table
    import_table
        library gdiplus_table, "gdiplus.dll", \
            kernel_table, "KERNEL32.DLL", \
            user_table, "USER32.DLL"

        import gdiplus_table, \
            GdipCreateFromHDC, "GdipCreateFromHDC", \
            GdipCreatePen1, "GdipCreatePen1", \
            GdipDeleteGraphics, "GdipDeleteGraphics", \
            GdipDeletePen, "GdipDeletePen", \
            GdipDrawEllipseI, "GdipDrawEllipseI", \
            GdipDrawLineI, "GdipDrawLineI", \
            GdipDrawRectangleI, "GdipDrawRectangleI", \
            GdipGraphicsClear, "GdipGraphicsClear", \
            GdipSetSmoothingMode, "GdipSetSmoothingMode", \
            GdiplusShutdown, "GdiplusShutdown", \
            GdiplusStartup, "GdiplusStartup"

        import kernel_table, \
            ExitProcess, "ExitProcess", \
            GetModuleHandleA, "GetModuleHandleA", \
            GetLocalTime, "GetLocalTime"

        import user_table, \
            BeginPaint, "BeginPaint", \
            CreateWindowExA, "CreateWindowExA", \
            DefWindowProcA, "DefWindowProcA", \
            DispatchMessageA, "DispatchMessageA", \
            EndPaint, "EndPaint", \
            GetClientRect, "GetClientRect", \
            GetMessageA, "GetMessageA", \
            GetSystemMetrics, "GetSystemMetrics", \
            LoadCursorA, "LoadCursorA", \
            LoadIconA, "LoadIconA", \
            PostQuitMessage, "PostQuitMessage", \
            RegisterClassExA, "RegisterClassExA", \
            ShowWindow, "ShowWindow", \
            SetWindowPos, "SetWindowPos", \
            TranslateMessage, "TranslateMessage", \
            UpdateWindow, "UpdateWindow"
    end_import_table
end_data_section
