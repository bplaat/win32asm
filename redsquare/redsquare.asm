    ; redsquare.asm - An pure 32-bit and 64-bit win32 assembly RedSquare game
    ; Made by Bastiaan van der Plaat (https://bplaat.nl/)
    ; 32-bit: nasm -f bin redsquare.asm -o redsquare-x86.exe && ./redsquare-x86
    ; 64-bit: nasm -DWIN64 -f bin redsquare.asm -o redsquare-x64.exe && ./redsquare-x64
    ; Use the build script to embed the resources

%include "../libwindows.inc"

header HEADER_GUI ;, HEADER_HAS_RESOURCES

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
    %define ICON_ID 1
    %define BASSIEBAS_BITMAP_ID 1

    %define FPS 50
    %define FRAME_TIMER_ID 1
    %define BLUE_SQUARES_COUNT 4

    struct Square, \
        rect, Rect_size, \
        vx, DWORD_size, \
        vy, DWORD_size

    struct WindowData, \
        bassiebas_bitmap, POINTER_size, \
        background_color, DWORD_size, \
        time, DWORD_size, \
        score, DWORD_size, \
        level, DWORD_size, \
        is_dragging, DWORD_size, \
        red_square, Square_size, \
        blue_squares, BLUE_SQUARES_COUNT * Square_size

    ; Start game function
    function game_start, hwnd
        local window_data, POINTER_size

        ; Get window data
        invoke GetWindowLongPtrA, [hwnd], GWLP_USERDATA
        mov [window_data], _ax

        ; Generate random background color
        fcall rand
        and eax, 0x007f7f7f
        or eax, 0xff000000
        mov _di, [window_data]
        mov [_di + WindowData.background_color], eax

        ; Set window data
        mov dword [_di + WindowData.time], 0
        mov dword [_di + WindowData.score], 0
        mov dword [_di + WindowData.level], 1
        mov dword [_di + WindowData.is_dragging], FALSE

        ; Set red square
        mov dword [_di + WindowData.red_square + Square.rect + Rect.width], 64
        mov dword [_di + WindowData.red_square + Square.rect + Rect.height], 64

        mov eax, [window_width]
        sub eax, [_di + WindowData.red_square + Square.rect + Rect.width]
        shr eax, 1
        mov [_di + WindowData.red_square + Square.rect + Rect.x], eax

        mov eax, [window_height]
        sub eax, [_di + WindowData.red_square + Square.rect + Rect.height]
        shr eax, 1
        mov [_di + WindowData.red_square + Square.rect + Rect.y], eax

        ; Set blue squares

        ; Set top left blue square
        mov dword [_di + WindowData.blue_squares + 0 * Square_size + Square.rect + Rect.width], 128
        mov dword [_di + WindowData.blue_squares + 0 * Square_size + Square.rect + Rect.height], 64

        mov dword [_di + WindowData.blue_squares + 0 * Square_size + Square.rect + Rect.x], 16

        mov dword [_di + WindowData.blue_squares + 0 * Square_size + Square.rect + Rect.y], 16 + 20 + 16 + 20 + 16

        mov dword [_di + WindowData.blue_squares + 0 * Square_size + Square.vx], 1
        mov dword [_di + WindowData.blue_squares + 0 * Square_size + Square.vy], 1

        ; Set top right blue square
        mov dword [_di + WindowData.blue_squares + 1 * Square_size + Square.rect + Rect.width], 96
        mov dword [_di + WindowData.blue_squares + 1 * Square_size + Square.rect + Rect.height], 96

        mov eax, [window_width]
        sub eax, [_di + WindowData.blue_squares + 1 * Square_size + Square.rect + Rect.width]
        sub eax, 16
        mov dword [_di + WindowData.blue_squares + 1 * Square_size + Square.rect + Rect.x], eax

        mov dword [_di + WindowData.blue_squares + 1 * Square_size + Square.rect + Rect.y], 16 + 20 + 16 + 20 + 16

        mov dword [_di + WindowData.blue_squares + 1 * Square_size + Square.vx], -1
        mov dword [_di + WindowData.blue_squares + 1 * Square_size + Square.vy], 1

        ; Set bottom left blue square
        mov dword [_di + WindowData.blue_squares + 2 * Square_size + Square.rect + Rect.width], 64
        mov dword [_di + WindowData.blue_squares + 2 * Square_size + Square.rect + Rect.height], 128

        mov dword [_di + WindowData.blue_squares + 2 * Square_size + Square.rect + Rect.x], 16

        mov eax, [window_height]
        sub eax, [_di + WindowData.blue_squares + 2 * Square_size + Square.rect + Rect.height]
        sub eax, 16 + 20 + 16
        mov dword [_di + WindowData.blue_squares + 2 * Square_size + Square.rect + Rect.y], eax

        mov dword [_di + WindowData.blue_squares + 2 * Square_size + Square.vx], 1
        mov dword [_di + WindowData.blue_squares + 2 * Square_size + Square.vy], -1

        ; Set bottom right blue square
        mov dword [_di + WindowData.blue_squares + 3 * Square_size + Square.rect + Rect.width], 128
        mov dword [_di + WindowData.blue_squares + 3 * Square_size + Square.rect + Rect.height], 96

        mov eax, [window_width]
        sub eax, [_di + WindowData.blue_squares + 3 * Square_size + Square.rect + Rect.width]
        sub eax, 16
        mov dword [_di + WindowData.blue_squares + 3 * Square_size + Square.rect + Rect.x], eax

        mov eax, [window_height]
        sub eax, [_di + WindowData.blue_squares + 3 * Square_size + Square.rect + Rect.height]
        sub eax, 16 + 20 + 16
        mov dword [_di + WindowData.blue_squares + 3 * Square_size + Square.rect + Rect.y], eax

        mov dword [_di + WindowData.blue_squares + 3 * Square_size + Square.vx], -1
        mov dword [_di + WindowData.blue_squares + 3 * Square_size + Square.vy], -1

        ; Create frame timer
        invoke SetTimer, [hwnd], FRAME_TIMER_ID, 1000 / FPS, NULL

        end_local
        return

        %undef hwnd
        %undef window_data

    ; Window procedure function
    function WindowProc, hwnd, uMsg, wParam, lParam
        mov eax, [uMsg]
        cmp eax, WM_CREATE
        je .wm_create
        cmp eax, WM_TIMER
        je .wm_timer
        cmp eax, WM_SIZE
        je .wm_size
        cmp eax, WM_LBUTTONDOWN
        je .wm_lbuttondown
        cmp eax, WM_MOUSEMOVE
        je .wm_mousemove
        cmp eax, WM_LBUTTONUP
        je .wm_lbuttonup
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

            ; Load bitmap
            invoke GetModuleHandleA, NULL
            invoke LoadBitmapA, _ax, BASSIEBAS_BITMAP_ID

            ; cmp _ax, NULL
            ; je .wm_destroy

            mov _di, [window_data]
            mov [_di + WindowData.bassiebas_bitmap], _ax

            ; Center  window
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

            ; Start game
            fcall game_start, [hwnd]

            end_local
            return 0

            %undef window_data

        .wm_timer:
            mov eax, [wParam]
            cmp eax, FRAME_TIMER_ID
            je .wm_timer.frame_timer
            return 0

        .wm_timer.frame_timer:
            local window_data, POINTER_size, \
                is_leveled, DWORD_size, \
                index, DWORD_size

            ; Get window data
            invoke GetWindowLongPtrA, [hwnd], GWLP_USERDATA
            mov [window_data], _ax

            ; Increase score
            mov _si, [window_data]
            mov eax, [_si + WindowData.level]
            add [_si + WindowData.score], eax

            ; Increase time
            inc dword [_si + WindowData.time]

            ; Check level increase
            mov dword [is_leveled], FALSE
            mov eax, [_si + WindowData.time]
            xor edx, edx
            mov ecx, FPS * 10
            idiv ecx
            if edx, "==", 0
                inc dword [_si + WindowData.level]
                mov dword [is_leveled], TRUE
            end_if

            ; Check border collision
            cmp dword [_si + WindowData.red_square + Square.rect + Rect.x], 16
            jl .wm_timer.frame_timer.gameover

            cmp dword [_si + WindowData.red_square + Square.rect + Rect.y], 16 + 20 + 16 + 20 + 16
            jl .wm_timer.frame_timer.gameover

            mov eax, [_si + WindowData.red_square + Square.rect + Rect.x]
            add eax, [_si + WindowData.red_square + Square.rect + Rect.width]
            mov ecx, [window_width]
            sub ecx, 16
            cmp eax, ecx
            jge .wm_timer.frame_timer.gameover

            mov eax, [_si + WindowData.red_square + Square.rect + Rect.y]
            add eax, [_si + WindowData.red_square + Square.rect + Rect.height]
            mov ecx, [window_height]
            sub ecx, 16 + 20 + 16
            cmp eax, ecx
            jge .wm_timer.frame_timer.gameover

            ; Update blue sqaures
            mov dword [index], 0
            mov ecx, [index]
            while ecx, "!=", BLUE_SQUARES_COUNT
                mov _si, [window_data]
                imul edi, ecx, Square_size
                lea _di, [_si + WindowData.blue_squares + _di]

                ; Check speed increase
                mov eax, [is_leveled]
                if eax, "==", TRUE
                    shl dword [_di + Square.vx], 1
                    shl dword [_di + Square.vy], 1
                end_if

                ; Update blue square position
                mov eax, [_di + Square.vx]
                add [_di + Square.rect + Rect.x], eax

                mov eax, [_di + Square.vy]
                add [_di + Square.rect + Rect.y], eax

                ; Blue square bounds check
                mov eax, [_di + Square.rect + Rect.x]
                if eax, "<=", 0
                    neg dword [_di + Square.vx]
                end_if

                mov eax, [_di + Square.rect + Rect.y]
                if eax, "<=", 0
                    neg dword [_di + Square.vy]
                end_if

                mov eax, [_di + Square.rect + Rect.x]
                add eax, [_di + Square.rect + Rect.width]
                mov ecx, [window_width]
                dec ecx
                if eax, ">=", ecx
                    neg dword [_di + Square.vx]
                end_if

                mov eax, [_di + Square.rect + Rect.y]
                add eax, [_di + Square.rect + Rect.height]
                mov ecx, [window_height]
                dec ecx
                if eax, ">=", ecx
                    neg dword [_di + Square.vy]
                end_if

                ; Check square collision
                mov eax, [_di + Square.rect + Rect.x]
                mov ecx, [_si + WindowData.red_square + Square.rect + Rect.x]
                add ecx, [_si + WindowData.red_square + Square.rect + Rect.width]
                if eax, "<", ecx
                    mov eax, [_di + Square.rect + Rect.x]
                    add eax, [_di + Square.rect + Rect.width]
                    if eax, ">", [_si + WindowData.red_square + Square.rect + Rect.x]
                        mov eax, [_di + Square.rect + Rect.y]
                        mov ecx, [_si + WindowData.red_square + Square.rect + Rect.y]
                        add ecx, [_si + WindowData.red_square + Square.rect + Rect.height]
                        if eax, "<", ecx
                            mov eax, [_di + Square.rect + Rect.y]
                            add eax, [_di + Square.rect + Rect.height]
                            if eax, ">", [_si + WindowData.red_square + Square.rect + Rect.y]
                                jmp .wm_timer.frame_timer.gameover
                            end_if
                        end_if
                    end_if
                end_if

                inc dword [index]
                mov ecx, [index]
            end_while

            ; Redraw window
            invoke InvalidateRect, [hwnd], NULL, TRUE

            return 0

        .wm_timer.frame_timer.gameover:
            ; Stop frame timer
            invoke KillTimer, [hwnd], FRAME_TIMER_ID

            ; Redraw window
            invoke InvalidateRect, [hwnd], NULL, TRUE

            ; Show alert
            invoke MessageBoxA, [hwnd], gameover_message, gameover_title, MB_RETRYCANCEL | MB_ICONINFORMATION
            if _ax, "==", IDRETRY
                fcall game_start, [hwnd]
            else
                invoke DestroyWindow, [hwnd]
            end_if

            end_local
            return 0

            %undef window_data
            %undef index

        .wm_lbuttondown:
            local window_data, POINTER_size, \
                x, DWORD_size, \
                y, DWORD_size

            ; Get window data
            invoke GetWindowLongPtrA, [hwnd], GWLP_USERDATA
            mov [window_data], _ax

            ; Get mouse position
            movzx eax, word [lParam]
            mov [x], eax

            mov eax, [lParam]
            shr eax, 16
            mov [y], eax

            ; Check position is in red square
            mov _si, [window_data]
            mov eax, [x]
            if eax, ">=", [_si + WindowData.red_square + Square.rect + Rect.x]
                mov eax, [y]
                if eax, ">=", [_si + WindowData.red_square + Square.rect + Rect.y]
                    mov eax, [x]
                    mov ecx, [_si + WindowData.red_square + Square.rect + Rect.x]
                    add ecx, [_si + WindowData.red_square + Square.rect + Rect.width]
                    if eax, "<", ecx
                        mov eax, [y]
                        mov ecx, [_si + WindowData.red_square + Square.rect + Rect.y]
                        add ecx, [_si + WindowData.red_square + Square.rect + Rect.height]
                        if eax, "<", ecx
                            ; Start dragging
                            mov dword [_si + WindowData.is_dragging], TRUE

                            mov eax, [x]
                            sub eax, [_si + WindowData.red_square + Square.rect + Rect.x]
                            mov [_si + WindowData.red_square + Square.vx], eax

                            mov eax, [y]
                            sub eax, [_si + WindowData.red_square + Square.rect + Rect.y]
                            mov [_si + WindowData.red_square + Square.vy], eax
                        end_if
                    end_if
                end_if
            end_if

            end_local
            return 0

            %undef window_data

        .wm_mousemove:
            local window_data, POINTER_size

            ; Get window data
            invoke GetWindowLongPtrA, [hwnd], GWLP_USERDATA
            mov [window_data], _ax

            ; Check is dragging
            mov _si, [window_data]
            mov eax, [_si + WindowData.is_dragging]
            if eax, "==", TRUE
                ; Update x position
                movzx eax, word [lParam]
                sub eax, [_si + WindowData.red_square + Square.vx]
                mov [_si + WindowData.red_square + Square.rect + Rect.x], eax

                ; Update y position
                mov eax, [lParam]
                shr eax, 16
                sub eax, [_si + WindowData.red_square + Square.vy]
                mov [_si + WindowData.red_square + Square.rect + Rect.y], eax
            end_if

            end_local
            return 0

            %undef window_data

        .wm_lbuttonup:
            local window_data, POINTER_size

            ; Get window data
            invoke GetWindowLongPtrA, [hwnd], GWLP_USERDATA
            mov [window_data], _ax

            ; Stop dragging
            mov _di, [window_data]
            mov dword [_di + WindowData.is_dragging], FALSE

            ; Check footer click
            mov eax, [lParam]
            shr eax, 16
            mov ecx, [window_height]
            sub ecx, 16 + 20 + 16
            if eax, ">=", ecx
                invoke ShellExecuteA, [hwnd], open_operation, website_url, NULL, NULL, SW_SHOWNORMAL
            end_if

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
            mov _ax, [lParam]
            mov dword [_ax + MINMAXINFO.ptMinTrackSize + POINT.x], 320
            mov dword [_ax + MINMAXINFO.ptMinTrackSize + POINT.y], 240

            return 0

        .wm_erasebkgnd:
            ; Draw no background
            return TRUE

        .wm_paint:
            ; Draw a centered text in the window
            local window_data, POINTER_size, \
                paint_struct, PAINTSTRUCT_size, \
                hdc_buffer, POINTER_size, \
                bitmap_buffer, POINTER_size, \
                graphics, POINTER_size, \
                brush, POINTER_size, \
                index, DWORD_size, \
                hdc_bitmap_buffer, POINTER_size, \
                font, POINTER_size, \
                stats_buffer, 128

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

            ; Create graphics object
            invoke GdipCreateFromHDC, [hdc_buffer], addr graphics

            ; Draw background color
            mov _si, [window_data]
            invoke GdipGraphicsClear, [graphics], [_si + WindowData.background_color]

            ; Draw border
            invoke GdipCreateSolidFill, 0x22ffffff, addr brush

            mov esi, [window_width]
            sub esi, 16 + 16
            mov edi, [window_height]
            sub edi, 16 + 20 + 16 + 20 + 16 + 16 + 20 + 16
            invoke GdipFillRectangleI, [graphics], [brush], 16, 16 + 20 + 16 + 20 + 16, _si, _di

            invoke GdipDeleteBrush, [brush]

            ; Draw blue squares
            invoke GdipCreateSolidFill, 0xdd1155ff, addr brush

            mov dword [index], 0
            mov ecx, [index]
            while ecx, "!=", BLUE_SQUARES_COUNT
                mov _si, [window_data]
                imul edi, ecx, Square_size
                lea _di, [_si + WindowData.blue_squares + _di]

                invoke GdipFillRectangleI, [graphics], [brush], [_di + Square.rect + Rect.x], [_di + Square.rect + Rect.y], \
                    [_di + Square.rect + Rect.width], [_di + Square.rect + Rect.height]

                inc dword [index]
                mov ecx, [index]
            end_while

            invoke GdipDeleteBrush, [brush]

            ; Draw red square
            invoke GdipCreateSolidFill, 0xddee3322, addr brush

            mov _si, [window_data]
            invoke GdipFillRectangleI, [graphics], [brush], [_si + WindowData.red_square + Square.rect + Rect.x], [_si + WindowData.red_square + Square.rect + Rect.y], \
                    [_si + WindowData.red_square + Square.rect + Rect.width], [_si + WindowData.red_square + Square.rect + Rect.height]

            invoke GdipDeleteBrush, [brush]

            ; Delete GDI+ graphics object
            invoke GdipDeleteGraphics, [graphics]

            ; Draw bassiebas bitmap
            invoke CreateCompatibleDC, [hdc_buffer]
            mov [hdc_bitmap_buffer], _ax

            mov _si, [window_data]
            invoke SelectObject, [hdc_bitmap_buffer], [_si + WindowData.bassiebas_bitmap]

            mov _si, [window_width]
            sub _si, 56 + 16
            invoke BitBlt, [hdc_buffer], _si, 16, 56, 56, [hdc_bitmap_buffer], 0, 0, SRCCOPY

            invoke DeleteDC, [hdc_bitmap_buffer]

            ; Draw stats label
            invoke CreateFontA, 20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, \
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name
            mov [font], _ax

            invoke SelectObject, [hdc_buffer], [font]
            invoke SetBkMode, [hdc_buffer], TRANSPARENT
            invoke SetTextColor, [hdc_buffer], 0x00ffffff

            mov _si, [window_data]
            mov eax, [_si + WindowData.time]
            xor edx, edx
            mov ecx, FPS
            idiv ecx
            cinvoke wsprintfA, addr stats_buffer, stats_label, [_si + WindowData.score], _ax, [_si + WindowData.level]
            invoke TextOutA, [hdc_buffer], 16, 16, addr stats_buffer, _ax

            invoke DeleteObject, [font]

            ; Draw help label
            invoke CreateFontA, 20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, \
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name
            mov [font], _ax
            invoke SelectObject, [hdc_buffer], [font]

            fcall strlen, addr help_label
            invoke TextOutA, [hdc_buffer], 16, 16 + 20 + 16, help_label, _ax

            ; Draw footer label
            invoke SetTextAlign, [hdc_buffer], TA_CENTER

            fcall strlen, addr footer_label
            mov esi, [window_width]
            shr esi, 1
            mov edi, [window_height]
            sub edi, 16 + 20
            invoke TextOutA, [hdc_buffer], _si, _di, footer_label, _ax

            invoke DeleteObject, [font]

            ; Draw and delete back buffer
            invoke BitBlt, [paint_struct + PAINTSTRUCT.hdc], 0, 0, [window_width], [window_height], [hdc_buffer], 0, 0, SRCCOPY
            invoke DeleteObject, [bitmap_buffer]
            invoke DeleteDC, [hdc_buffer]

            ; End paint
            invoke EndPaint, [hwnd], addr paint_struct

            end_local
            return 0

            %undef window_data

        .wm_destroy:
            local window_data, POINTER_size

            ; Get window data
            invoke GetWindowLongPtrA, [hwnd], GWLP_USERDATA
            mov [window_data], _ax

            ; Delete bassiebas bitmap
            mov _si, [window_data]
            invoke DeleteObject, [_si + WindowData.bassiebas_bitmap]

            ; Free window data
            fcall free, [window_data]

            ; Close process
            invoke PostQuitMessage, 0

            end_local
            return 0

        .default:
            invoke DefWindowProcA, [hwnd], [uMsg], [wParam], [lParam]
            return

        %undef hwnd
        %undef bassiebas_bitmap

    ; Main entry point
    entrypoint
        local initCommonControlsEx, INITCOMMONCONTROLSEX_size, \
            gdiplusToken, DWORD_size, \
            gdiplusStartupInput, GdiplusStartupInput_size, \
            window_class, WNDCLASSEX_size, \
            hwnd, POINTER_size, \
            message, MSG_size

        ; Init common controls for modern control style
        mov dword [initCommonControlsEx + INITCOMMONCONTROLSEX.dwSize], INITCOMMONCONTROLSEX_size
        mov dword [initCommonControlsEx + INITCOMMONCONTROLSEX.dwICC], ICC_WIN95_CLASSES
        invoke InitCommonControlsEx, addr initCommonControlsEx

        ; Startup GDI+
        mov dword [gdiplusStartupInput + GdiplusStartupInput.GdiplusVersion], 1
        mov pointer [gdiplusStartupInput + GdiplusStartupInput.DebugEventCallback], NULL
        mov dword [gdiplusStartupInput + GdiplusStartupInput.SuppressBackgroundThread], FALSE
        mov dword [gdiplusStartupInput + GdiplusStartupInput.SuppressExternalCodecs], FALSE
        invoke GdiplusStartup, addr gdiplusToken, addr gdiplusStartupInput, NULL

        ; Register the window class
        mov dword [window_class + WNDCLASSEX.cbSize], WNDCLASSEX_size

        mov dword [window_class + WNDCLASSEX.style], 0

        mov pointer [window_class + WNDCLASSEX.lpfnWndProc], WindowProc

        mov dword [window_class + WNDCLASSEX.cbClsExtra], 0

        mov dword [window_class + WNDCLASSEX.cbWndExtra], 0

        invoke GetModuleHandleA, NULL
        mov [window_class + WNDCLASSEX.hInstance], _ax

        invoke LoadImageA, [window_class + WNDCLASSEX.hInstance], ICON_ID, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_SHARED
        mov [window_class + WNDCLASSEX.hIcon], _ax

        invoke LoadCursorA, NULL, IDC_ARROW
        mov [window_class + WNDCLASSEX.hCursor], _ax

        mov pointer [window_class + WNDCLASSEX.hbrBackground], NULL

        mov pointer [window_class + WNDCLASSEX.lpszMenuName], NULL

        mov pointer [window_class + WNDCLASSEX.lpszClassName], window_class_name

        invoke GetSystemMetrics, SM_CXSMICON
        mov _si, _ax
        invoke GetSystemMetrics, SM_CYSMICON
        mov _di, _ax
        invoke LoadImageA, [window_class + WNDCLASSEX.hInstance], ICON_ID, IMAGE_ICON, _si, _di, LR_DEFAULTCOLOR | LR_SHARED
        mov [window_class + WNDCLASSEX.hIconSm], _ax

        invoke RegisterClassExA, addr window_class

        ; Create the window
        invoke CreateWindowExA, 0, window_class_name, window_title, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX, \
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

        ; Shutdown GDI+
        invoke GdiplusShutdown, [gdiplusToken]

        invoke ExitProcess, [message + MSG.wParam]

        end_local
end_code_section

data_section
    ; String constants
    window_class_name db "redsquare", 0
    window_title db "RedSquare", 0
    font_name db "Tahoma", 0
    stats_label db "Score: %06d  -  Time: %02d s  -  Level: %02d", 0
    help_label db "Help: move the red square avoid the edge and the blue squares", 0
    footer_label db "Made by Bastiaan van der Plaat with a lot of love for you, the Windows API is weird but in a cool way!", 0
    open_operation db "open", 0
    website_url db "https://bplaat.nl/", 0
    gameover_title db "Game over!", 0
    gameover_message db "You are game over!", 0

    ; Global variables
    rand_seed dd 0
    window_width dd 800
    window_height dd 600

    ; Import table
    import_table
        library comctl_table, "COMCTL32.DLL", \
            gdi_table, "GDI32.DLL", \
            gdiplus_table, "gdiplus.dll", \
            kernel_table, "KERNEL32.DLL", \
            shell_table, "SHELL32.DLL", \
            user_table, "USER32.DLL"

        import comctl_table, \
            InitCommonControlsEx, "InitCommonControlsEx"

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

        import gdiplus_table, \
            GdipCreateFromHDC, "GdipCreateFromHDC", \
            GdipCreateSolidFill, "GdipCreateSolidFill", \
            GdipDeleteBrush, "GdipDeleteBrush", \
            GdipDeleteGraphics, "GdipDeleteGraphics", \
            GdipFillRectangleI, "GdipFillRectangleI", \
            GdipGraphicsClear, "GdipGraphicsClear", \
            GdipSetSmoothingMode, "GdipSetSmoothingMode", \
            GdiplusShutdown, "GdiplusShutdown", \
            GdiplusStartup, "GdiplusStartup"

        import kernel_table, \
            ExitProcess, "ExitProcess", \
            GetLocalTime, "GetLocalTime", \
            GetModuleHandleA, "GetModuleHandleA", \
            GetProcessHeap, "GetProcessHeap", \
            HeapAlloc, "HeapAlloc", \
            HeapFree, "HeapFree"

        import shell_table, \
            ShellExecuteA, "ShellExecuteA"

        import user_table, \
            BeginPaint, "BeginPaint", \
            CreateWindowExA, "CreateWindowExA", \
            DefWindowProcA, "DefWindowProcA", \
            DestroyWindow, "DestroyWindow", \
            DispatchMessageA, "DispatchMessageA", \
            EndPaint, "EndPaint", \
            FillRect, "FillRect", \
            GetClientRect, "GetClientRect", \
            GetMessageA, "GetMessageA", \
            GetSystemMetrics, "GetSystemMetrics", \
            GetWindowLongPtrA, GetWindowLongPtrAString, \
            KillTimer, "KillTimer", \
            InvalidateRect, "InvalidateRect", \
            LoadBitmapA, "LoadBitmapA", \
            LoadCursorA, "LoadCursorA", \
            LoadImageA, "LoadImageA", \
            MessageBoxA, "MessageBoxA", \
            PostQuitMessage, "PostQuitMessage", \
            RegisterClassExA, "RegisterClassExA", \
            SetTimer, "SetTimer", \
            SetWindowLongPtrA, SetWindowLongPtrAString, \
            SetWindowPos, "SetWindowPos", \
            ShowWindow, "ShowWindow", \
            TranslateMessage, "TranslateMessage", \
            UpdateWindow, "UpdateWindow", \
            wsprintfA, "wsprintfA"
    end_import_table
end_data_section

; resources_section
;     directory RT_BITMAP, bitmaps, \
;         RT_VERSION, versions, \
;         RT_MANIFEST, manifests

;     resource bitmaps, \
;         BASSIEBAS_BITMAP_ID, LANG_NEUTRAL, bassiebas_bitmap

;     resource versions, \
;         1, LANG_ENGLISH + SUBLANG_ENGLISH_US, version

;     resource manifests, \
;         1, LANG_NEUTRAL, app_manifest

;     bitmap bassiebas_bitmap, "bassiebas.bmp"

;     %ifdef WIN64
;         versioninfo version, \
;             0x0001000000000000, 0x0001000000000000, \
;             VOS__WINDOWS32, VFT_APP, VFT2_UNKNOWN, \
;             LANG_ENGLISH + SUBLANG_ENGLISH_US, \
;             CompanyName, "PlaatSoft", \
;             FileDescription, "RedSquare", \
;             FileVersion, "1.0.0.0", \
;             InternalName, "RedSquare", \
;             LegalCopyright, "Copyright (c) 2021 PlaatSoft", \
;             OriginalFilename, "redsquare-x64.exe", \
;             ProductName, "RedSquare", \
;             ProductVersion, "1.0.0.0"
;     %else
;         versioninfo version, \
;             0x0001000000000000, 0x0001000000000000, \
;             VOS__WINDOWS32, VFT_APP, VFT2_UNKNOWN, \
;             LANG_ENGLISH + SUBLANG_ENGLISH_US, \
;             CompanyName, "PlaatSoft", \
;             FileDescription, "RedSquare", \
;             FileVersion, "1.0.0.0", \
;             InternalName, "RedSquare", \
;             LegalCopyright, "Copyright (c) 2021 PlaatSoft", \
;             OriginalFilename, "redsquare-x86.exe", \
;             ProductName, "RedSquare", \
;             ProductVersion, "1.0.0.0"
;     %endif

;     manifest app_manifest, "redsquare.manifest"
; end_resources_section
