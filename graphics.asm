    ; graphics.asm - An pure 32-bit and 64-bit win32 assembly GDI+ program
    ; Made by Bastiaan van der Plaat (https://bastiaan.ml/)
    ; 32-bit: nasm -f bin graphics.asm -o graphics-x86.exe && ./graphics-x86
    ; 64-bit: nasm -DWIN64 -f bin graphics.asm -o graphics-x64.exe && ./graphics-x64

    ; Because the flat GDI+ API gives antialiased drawing methods
    ; This program uses SSE floating point extentions: this is mandatory for x86-64
    ; but can crash an 32-bit program on a realy old processor

%ifdef WIN64
    %include "libwindows-x64.inc"
%else
    %include "libwindows-x86.inc"
%endif

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
            local window_rect, RECT_size, \
                new_window_rect, Rect_size

            frame

            ; Generate random background color
            fcall rand_rand
            and eax, 0x00ffffff
            or eax, 0xff000000
            mov [background_color], eax

            ; Center new created window
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

        .wm_size:
            ; Save new window size
            movzx eax, word [lParam]
            mov [window_width], eax

            mov eax, dword [lParam]
            shr eax, 16
            mov [window_height], eax

            jmp .leave

        .wm_erasebkgnd:
            ; This window draws it's own background
            return 1

        .wm_getminmaxinfo:
            ; Set window min size
            mov _ax, [lParam]
            mov dword [_ax + MINMAXINFO.ptMinTrackSize + POINT.x], 320
            mov dword [_ax + MINMAXINFO.ptMinTrackSize + POINT.y], 240
            jmp .leave

        .wm_paint:
            ; Draw stuff in the window via GDI+
            local paint_struct, PAINTSTRUCT_size, \
                window_rect, RECT_size, \
                graphics, POINTER_size, \
                line_width, DWORD_size, \
                pen, POINTER_size, \
                items_rect, Rect_size, \
                font_family, POINTER_size, \
                font_size, DWORD_size, \
                font, POINTER_size, \
                text_brush, POINTER_size, \
                string_format, POINTER_size, \
                text_rect, Rect_size, \
                text_buffer, 64 * BYTE_size

            frame

            invoke BeginPaint, [hwnd], addr paint_struct

            invoke GetClientRect, [hwnd], addr window_rect

            ; Create graphics object
            invoke GdipCreateFromHDC, [paint_struct + PAINTSTRUCT.hdc], addr graphics

            ; Enable anti aliasing
            invoke GdipSetSmoothingMode, [graphics], SmoothingModeAntiAlias
            invoke GdipSetTextRenderingHint, [graphics], TextRenderingHintClearTypeGridFit

            ; Clear screen with window background color
            invoke GdipGraphicsClear, [graphics], [background_color]

            ; Create pen object
            mov eax, 3
            cvtsi2ss xmm0, eax
            movss [line_width], xmm0
            invoke GdipCreatePen1, 0xffffffff, float [line_width], UnitPixel, addr pen

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

            ; Calculate font size by window width
            mov eax, [window_width]
            cvtsi2ss xmm0, eax
            mov eax, 30
            cvtsi2ss xmm1, eax
            divss xmm0, xmm1
            movss [font_size], xmm0

            ; Create font object
            invoke GdipCreateFontFamilyFromName, font_name, 0, addr font_family
            invoke GdipCreateFont, [font_family], float [font_size], FontStyleRegular, UnitPixel, addr font

            ; Create text solid fill brush object
            invoke GdipCreateSolidFill, 0xffffffff, addr text_brush

            ; Create centered string format object
            invoke GdipStringFormatGetGenericDefault, addr string_format
            invoke GdipSetStringFormatAlign, [string_format], StringAlignmentCenter

            ; Calculate text_rect
            mov eax, [window_rect + RECT.right]
            cvtsi2ss xmm0, eax
            movss [text_rect + Rect.width], xmm0

            movss xmm1, [font_size]
            mov eax, 2
            cvtsi2ss xmm2, eax
            mulss xmm1, xmm2
            movss [text_rect + Rect.height], xmm1

            mov eax, 0
            cvtsi2ss xmm0, eax
            movss [text_rect + Rect.x], xmm0

            mov eax, [window_rect + RECT.bottom]
            cvtsi2ss xmm0, eax
            subss xmm0, xmm1
            movss [text_rect + Rect.y], xmm0

            ; Generate window size string
            cinvoke wsprintfW, addr text_buffer, window_size_format, [window_width], [window_height]

            ; Draw window size text
            invoke GdipDrawString, [graphics], addr text_buffer, -1, [font], addr text_rect, [string_format], [text_brush]

            ; Delete the GDI+ objects
            invoke GdipDeleteStringFormat, [string_format]
            invoke GdipDeleteBrush, [text_brush]
            invoke GdipDeleteFont, [font]
            invoke GdipDeleteFontFamily, [font_family]
            invoke GdipDeletePen, [pen]
            invoke GdipDeleteGraphics, [graphics]

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
        local gdiplusToken, DWORD_size, \
            gdiplusStartupInput, GdiplusStartupInput_size, \
            window_class, WNDCLASSEX_size, \
            hwnd, DWORD_size, \
            message, MSG_size

        frame

        ; Generate rand seed
        fcall rand_generate_seed

        ; Startup GDI+
        mov dword [gdiplusStartupInput + GdiplusStartupInput.GdiplusVersion], 1
        mov pointer [gdiplusStartupInput + GdiplusStartupInput.DebugEventCallback], 0
        mov dword [gdiplusStartupInput + GdiplusStartupInput.SuppressBackgroundThread], 0
        mov dword [gdiplusStartupInput + GdiplusStartupInput.SuppressExternalCodecs], 0
        invoke GdiplusStartup, addr gdiplusToken, addr gdiplusStartupInput, 0

        ; Register the window class
        mov dword [window_class + WNDCLASSEX.cbSize], WNDCLASSEX_size

        mov dword [window_class + WNDCLASSEX.style], CS_HREDRAW | CS_VREDRAW

        mov pointer [window_class + WNDCLASSEX.lpfnWndProc], WindowProc

        mov dword [window_class + WNDCLASSEX.cbClsExtra], 0

        mov dword [window_class + WNDCLASSEX.cbWndExtra], 0

        invoke GetModuleHandleA, 0
        mov [window_class + WNDCLASSEX.hInstance], _ax

        invoke LoadIconA, 0, IDI_APPLICATION
        mov [window_class + WNDCLASSEX.hIcon], _ax
        mov [window_class + WNDCLASSEX.hIconSm], _ax

        invoke LoadCursorA, 0, IDC_ARROW
        mov [window_class + WNDCLASSEX.hCursor], _ax

        mov pointer [window_class + WNDCLASSEX.hbrBackground], COLOR_WINDOW + 1

        mov pointer [window_class + WNDCLASSEX.lpszMenuName], 0

        mov pointer [window_class + WNDCLASSEX.lpszClassName], window_class_name

        invoke RegisterClassExA, addr window_class

        ; Create the window
        invoke CreateWindowExA, 0, window_class_name, window_title, WS_OVERLAPPEDWINDOW, \
            CW_USEDEFAULT, CW_USEDEFAULT, [window_width], [window_height], \
            HWND_DESKTOP, 0, [window_class + WNDCLASSEX.hInstance], 0
        mov [hwnd], _ax
        invoke ShowWindow, [hwnd], SW_SHOWDEFAULT
        invoke UpdateWindow, [hwnd]

        ; Message loop
        .message_loop:
            invoke GetMessageA, addr message, 0, 0, 0
            cmp _ax, 0
            jle .done

            invoke TranslateMessage, addr message
            invoke DispatchMessageA, addr message
            jmp .message_loop
        .done:
            ; Shutdown GDI+
            invoke GdiplusShutdown, [gdiplusToken]

            invoke ExitProcess, [message + MSG.wParam]
            end_frame
end_code_section

data_section
    ; String constants
    window_class_name db "graphics-test", 0
    %ifdef WIN64
        window_title db "This is a test GDI+ window (64-bit)", 0
    %else
        window_title db "This is a test GDI+ window (32-bit)", 0
    %endif
    font_name dw "T","a","h","o","m","a", 0
    window_size_format dw "(","%","d","x","%","d",")", 0

    ; Global variables
    seed dd 0
    window_width dd 1280
    window_height dd 720
    background_color dd 0

    ; Import table
    import_table
        library gdiplus_table, "gdiplus.dll", \
            kernel_table, "KERNEL32.DLL", \
            user_table, "USER32.DLL"

        import gdiplus_table, \
            GdipCreateFont, "GdipCreateFont", \
            GdipCreateFontFamilyFromName, "GdipCreateFontFamilyFromName", \
            GdipCreateFromHDC, "GdipCreateFromHDC", \
            GdipCreatePen1, "GdipCreatePen1", \
            GdipCreateSolidFill, "GdipCreateSolidFill", \
            GdipDeleteBrush, "GdipDeleteBrush", \
            GdipDeleteFont, "GdipDeleteFont", \
            GdipDeleteFontFamily, "GdipDeleteFontFamily", \
            GdipDeleteGraphics, "GdipDeleteGraphics", \
            GdipDeletePen, "GdipDeletePen", \
            GdipDeleteStringFormat, "GdipDeleteStringFormat", \
            GdipDrawEllipseI, "GdipDrawEllipseI", \
            GdipDrawLineI, "GdipDrawLineI", \
            GdipDrawRectangleI, "GdipDrawRectangleI", \
            GdipDrawString, "GdipDrawString", \
            GdipGraphicsClear, "GdipGraphicsClear", \
            GdipSetSmoothingMode, "GdipSetSmoothingMode", \
            GdipSetStringFormatAlign, "GdipSetStringFormatAlign", \
            GdipStringFormatGetGenericDefault, "GdipStringFormatGetGenericDefault", \
            GdipSetTextRenderingHint, "GdipSetTextRenderingHint", \
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
            UpdateWindow, "UpdateWindow", \
            wsprintfW, "wsprintfW"
    end_import_table
end_data_section
