    ; custom.asm - An pure 32-bit and 64-bit win32 assembly custom UI program
    ; Made by Bastiaan van der Plaat (https://bastiaan.ml/)
    ; 32-bit: nasm -f bin custom.asm -o custom-x86.exe && ./custom-x86
    ; 64-bit: nasm -DWIN64 -f bin custom.asm -o custom-x64.exe && ./custom-x64

    ; This programma shows that it is possible to create object oriented widgets
    ; library in assembly with custom drawing. If it is a super plessend experience
    ; is another question. But he it works 🎉

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
        loop
            mov al, [_si]
            test al, al
            je %$end_loop

            inc _si
        end_loop
        sub _si, [string]
        return _si

    ; ### Widget object ###
    struct Widget, \
        rect, Rect_size, \
        background_color, DWORD_size, \
        draw_function, POINTER_size, \
        free_function, POINTER_size

    function widget_new, rect, background_color
        local widget, POINTER_size

        fcall malloc, Widget_size
        mov [widget], _ax

        fcall widget_init, _ax, [rect], [background_color]

        end_local
        return [widget]
        %undef rect
        %undef background_color

        %undef widget

    function widget_init, widget, rect, background_color
        mov _di, [widget]

        mov _si, [rect]
        mov eax, [_si + Rect.x]
        mov [_di + Widget.rect + Rect.x], eax

        mov eax, [_si + Rect.y]
        mov [_di + Widget.rect + Rect.y], eax

        mov eax, [_si + Rect.width]
        mov [_di + Widget.rect + Rect.width], eax

        mov eax, [_si + Rect.height]
        mov [_di + Widget.rect + Rect.height], eax

        mov eax, [background_color]
        mov [_di + Widget.background_color], eax

        mov pointer [_di + Widget.draw_function], widget_draw

        mov pointer [_di + Widget.free_function], widget_free

        return
        %undef widget
        %undef rect
        %undef background_color

    function widget_set_x, widget, x
        mov _di, [widget]
        mov eax, [x]
        mov [_di + Widget.rect + Rect.x], eax
        return
        %undef widget
        %undef x

    function widget_set_y, widget, y
        mov _di, [widget]
        mov eax, [y]
        mov [_di + Widget.rect + Rect.y], eax
        return
        %undef widget
        %undef y

    function widget_set_width, widget, width
        mov _di, [widget]
        mov eax, [width]
        mov [_di + Widget.rect + Rect.width], eax
        return
        %undef widget
        %undef width

    function widget_set_height, widget, height
        mov _di, [widget]
        mov eax, [height]
        mov [_di + Widget.rect + Rect.height], eax
        return
        %undef widget
        %undef height

    function widget_draw, widget, hdc
        local brush, POINTER_size, \
            rect, RECT_size

        ; Create background color solid brush
        mov _si, [widget]
        invoke CreateSolidBrush, [_si + Widget.background_color]
        mov [brush], _ax

        ; Draw widget background color
        mov _si, [widget]
        mov eax, [_si + Widget.rect + Rect.x]
        mov [rect + RECT.left], eax

        mov eax, [_si + Widget.rect + Rect.y]
        mov [rect + RECT.top], eax

        mov eax, [_si + Widget.rect + Rect.x]
        add eax, [_si + Widget.rect + Rect.width]
        mov [rect + RECT.right], eax

        mov eax, [_si + Widget.rect + Rect.y]
        add eax, [_si + Widget.rect + Rect.height]
        mov [rect + RECT.bottom], eax

        invoke FillRect, [hdc], addr rect, [brush]

        ; Delete brush object
        invoke DeleteObject, [brush]

        end_local
        return
        %undef widget
        %undef hdc

        %undef brush
        %undef rect

    function widget_free, widget
        fcall free, [widget]
        return
        %undef widget

    ; ### Font object ###
    %define FONT_STYLE_NORMAL 0
    %define FONT_STYLE_ITALIC 1

    struct Font, \
        font_name, POINTER_size, \
        font_weight, DWORD_size, \
        font_style, DWORD_size

    function font_new, font_name, font_weight, font_style
        local font, POINTER_size

        fcall malloc, Font_size
        mov [font], _ax

        fcall font_init, _ax, [font_name], [font_weight], [font_style]

        end_local
        return [font]
        %undef font_name
        %undef font_weight
        %undef font_style

        %undef font

    function font_init, font, font_name, font_weight, font_style
        mov _di, [font]

        mov _ax, [font_name]
        mov [_di + Font.font_name], _ax

        mov eax, [font_weight]
        mov [_di + Font.font_weight], eax

        mov eax, [font_style]
        mov [_di + Font.font_style], eax

        return
        %undef font
        %undef font_name
        %undef font_weight
        %undef font_style

    function font_free, font
        fcall free, [font]
        return
        %undef font

    ; Label object
    struct Label, \
        widget, Widget_size, \
        text, POINTER_size, \
        font, POINTER_size, \
        text_size, DWORD_size, \
        text_color, DWORD_size

    function label_new, rect, background_color, text, font, text_size, text_color
        local label, POINTER_size

        fcall malloc, Label_size
        mov [label], _ax

        fcall label_init, _ax, [rect], [background_color], [text], [font], [text_size], [text_color]

        end_local
        return [label]
        %undef rect
        %undef background_color
        %undef text
        %undef font
        %undef text_size
        %undef text_color

        %undef label

    function label_init, label, rect, background_color, text, font, text_size, text_color
        fcall widget_init, [label], [rect], [background_color]

        mov _di, [label]
        mov pointer [_di + Widget.draw_function], label_draw

        mov pointer [_di + Widget.free_function], label_free

        mov _ax, [text]
        mov [_di + Label.text], _ax

        mov _ax, [font]
        mov [_di + Label.font], _ax

        mov eax, [text_size]
        mov [_di + Label.text_size], eax

        mov eax, [text_color]
        mov [_di + Label.text_color], eax

        return
        %undef label
        %undef rect
        %undef background_color
        %undef text
        %undef font
        %undef text_size
        %undef text_color

    function label_draw, label, hdc
        local font, POINTER_size, \
            rect, Rect_size

        ; Draw parent widget object
        fcall widget_draw, [label], [hdc]

        ; Create font object
        mov _si, [label]
        mov _di, [_si + Label.font]
        invoke CreateFontA, [_si + Label.text_size], 0, 0, 0, [_di + Font.font_weight], [_di + Font.font_style], FALSE, FALSE, \
            ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, [_di + Font.font_name]
        mov [font], _ax

        ; Draw label
        invoke SelectObject, [hdc], [font]
        invoke SetBkMode, [hdc], TRANSPARENT
        mov _si, [label]
        invoke SetTextColor, [hdc], [_si + Label.text_color]
        invoke SetTextAlign, [hdc], TA_CENTER

        mov _si, [label]
        fcall strlen, [_si + Label.text]

        mov _si, [label]
        mov edx, [_si + Widget.rect + Rect.width]
        shr edx, 1
        add edx, [_si + Widget.rect + Rect.x]

        mov edi, [_si + Widget.rect + Rect.height]
        sub edi, [_si + Label.text_size]
        shr edi, 1
        add edi, [_si + Widget.rect + Rect.y]

        invoke TextOutA, [hdc], _dx, _di, [_si + Label.text], _ax

        ; Free font object
        invoke DeleteObject, [font]

        end_local
        return
        %undef label
        %undef hdc

        %undef font

    function label_free, label
        fcall free, [label]
        return
        %undef label

    ; ### Window Code ###
    struct WindowData, \
        background_color, DWORD_size, \
        header_font, POINTER_size, \
        footer_font, POINTER_size, \
        widgets, 64 * POINTER_size, \
        widgets_size, DWORD_size

    ; Function to add widget to window data
    function window_add_widget, window_data, widget
        mov _di, [window_data]
        mov ecx, [_di + WindowData.widgets_size]
        mov _ax, [widget]
        mov [_di + WindowData.widgets + _cx * POINTER_size], _ax
        inc ecx
        mov [_di + WindowData.widgets_size], ecx
        return
        %undef window_data
        %undef widget

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
                window_rect, RECT_size, \
                new_window_rect, Rect_size

            ; Create window data
            fcall malloc, WindowData_size
            mov [window_data], _ax
            invoke SetWindowLongPtrA, [hwnd], GWLP_USERDATA, _ax

            ; Set background color
            mov _di, [window_data]
            mov dword [_di + WindowData.background_color], 0x00ffffff

            ; Creat fonts
            fcall font_new, header_font_name, 700, FONT_STYLE_ITALIC
            mov _di, [window_data]
            mov [_di + WindowData.header_font], _ax

            fcall font_new, footer_font_name, 400, FONT_STYLE_NORMAL
            mov _di, [window_data]
            mov [_di + WindowData.footer_font], _ax

            ;  Create widgets
            mov _di, [window_data]
            mov dword [_di + WindowData.widgets_size], 0

            ; Create header label widget
            mov dword [new_window_rect + Rect.x], 0
            mov dword [new_window_rect + Rect.y], 0
            mov dword [new_window_rect + Rect.height], 64
            mov _si, [window_data]
            fcall label_new, addr new_window_rect, 0x00222222, window_title, [_si + WindowData.header_font], 24, 0x00ffffff
            fcall window_add_widget, [window_data], _ax

            ; Create red rect
            mov dword [new_window_rect + Rect.x], 100
            mov dword [new_window_rect + Rect.y], 100
            mov dword [new_window_rect + Rect.width], 320
            mov dword [new_window_rect + Rect.height], 240
            fcall widget_new, addr new_window_rect, 0x000000ff
            fcall window_add_widget, [window_data], _ax

            ; Create green rect
            mov dword [new_window_rect + Rect.x], 200
            mov dword [new_window_rect + Rect.y], 200
            fcall widget_new, addr new_window_rect, 0x0000ff00
            fcall window_add_widget, [window_data], _ax

            ; Create blue rect
            mov dword [new_window_rect + Rect.x], 300
            mov dword [new_window_rect + Rect.y], 300
            fcall widget_new, addr new_window_rect, 0x00ff0000
            fcall window_add_widget, [window_data], _ax

            ; Create footer label widget
            mov dword [new_window_rect + Rect.x], 16
            mov dword [new_window_rect + Rect.height], 32
            mov _si, [window_data]
            fcall label_new, addr new_window_rect, 0x00eeeeee, about_label, [_si + WindowData.footer_font], 24, 0x00111111
            fcall window_add_widget, [window_data], _ax

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
            jmp .leave

            %undef window_data

    .wm_size:
            local window_data, POINTER_size

            ; Get window data
            invoke GetWindowLongPtrA, [hwnd], GWLP_USERDATA
            mov [window_data], _ax

            ; Save new window size
            movzx eax, word [lParam]
            mov [window_width], eax

            mov eax, [lParam]
            shr eax, 16
            mov [window_height], eax

            ; Change header label width
            mov _si, [window_data]
            mov _si, [_si + WindowData.widgets + 0 * POINTER_size]
            fcall widget_set_width, _si, [window_width]

            ; Change footer label y and width
            mov _si, [window_data]
            mov _si, [_si + WindowData.widgets + 4 * POINTER_size]

            mov eax, [window_height]
            sub eax, 32 + 16
            fcall widget_set_y, _si, _ax

            mov eax, [window_width]
            sub eax, 32
            fcall widget_set_width, _si, _ax

            end_local
            jmp .leave

            %undef window_data

        .wm_getminmaxinfo:
            ; Set window min size
            mov _ax, [lParam]
            mov dword [_ax + MINMAXINFO.ptMinTrackSize + POINT.x], 320
            mov dword [_ax + MINMAXINFO.ptMinTrackSize + POINT.y], 240
            jmp .leave

        .wm_erasebkgnd:
            ; Draw no background
            return TRUE

        .wm_paint:
            local window_data, POINTER_size, \
                paint_struct, PAINTSTRUCT_size, \
                hdc_buffer, POINTER_size, \
                bitmap_buffer, POINTER_size, \
                brush, POINTER_size, \
                index, DWORD_size

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
            mov _di, [window_data]
            invoke CreateSolidBrush, [_di + WindowData.background_color]
            mov [brush], _ax

            mov dword [rect + RECT.left], 0
            mov dword [rect + RECT.top], 0
            mov eax, [window_width]
            mov [rect + RECT.right], eax
            mov eax, [window_height]
            mov [rect + RECT.bottom], eax

            invoke FillRect, [hdc_buffer], addr rect, [brush]

            invoke DeleteObject, [brush]

            ; Draw widgets
            mov dword [index], 0
            mov ecx, [index]
            mov _si, [window_data]
            while ecx, "!=", [_si + WindowData.widgets_size]
                mov _si, [_si + WindowData.widgets + _cx * POINTER_size]
                fcall [_si + Widget.draw_function], _si, [hdc_buffer]

                inc dword [index]
                mov ecx, [index]
                mov _si, [window_data]
            end_while

            ; Draw and delete back buffer
            invoke BitBlt, [paint_struct + PAINTSTRUCT.hdc], 0, 0, [window_width], [window_height], [hdc_buffer], 0, 0, SRCCOPY
            invoke DeleteObject, [bitmap_buffer]
            invoke DeleteDC, [hdc_buffer]

            ; End paint
            invoke EndPaint, [hwnd], addr paint_struct

            end_local
            jmp .leave

            %undef window_data
            %undef index

        .wm_destroy:
            local window_data, POINTER_size, \
                index, DWORD_size

            ; Get window data
            invoke GetWindowLongPtrA, [hwnd], GWLP_USERDATA
            mov [window_data], _ax

            ; Free fonts
            mov _si, [window_data]
            fcall font_free, [_si + WindowData.header_font]
            mov _si, [window_data]
            fcall font_free, [_si + WindowData.footer_font]

            ; Free widgets
            mov dword [index], 0
            mov ecx, [index]
            mov _si, [window_data]
            while ecx, "!=", [_si + WindowData.widgets_size]
                mov _si, [_si + WindowData.widgets + _cx * POINTER_size]
                fcall [_si + Widget.free_function], _si

                inc dword [index]
                mov ecx, [index]
                mov _si, [window_data]
            end_while

            ; Free window data
            fcall free, [window_data]

            ; Close process
            invoke PostQuitMessage, 0

            end_local
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
    window_class_name db "custom-test", 0
    %ifdef WIN64
        window_title db "This is a custom UI window (64-bit)", 0
    %else
        window_title db "This is a custom UI window (32-bit)", 0
    %endif
    header_font_name db "Georgia", 0
    footer_font_name db "Comic Sans MS", 0
    about_label db "Made by Bastiaan van der Plaat", 0

    ; Global variables
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
            HeapAlloc, "HeapAlloc", \
            HeapFree, "HeapFree", \
            GetModuleHandleA, "GetModuleHandleA", \
            GetProcessHeap, "GetProcessHeap"

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
