    ; redsquare.asm - An pure 32-bit and 64-bit win32 assembly RedSquare game
    ; Made by Bastiaan van der Plaat (https://bastiaan.ml/)
    ; 32-bit: nasm -f bin redsquare.asm -o redsquare-x86.exe && ./redsquare-x86
    ; 64-bit: nasm -DWIN64 -f bin redsquare.asm -o redsquare-x64.exe && ./redsquare-x64

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
        xor _ax, _ax
    .repeat:
        cmp byte [_si], 0
        je .done
        inc _si
        inc _ax
        jmp .repeat
    .done:
        return

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
    %define FPS 50
    %define FRAME_TIMER_ID 1
    %define BLUE_SQUARES_COUNT 4

    struct Square, \
        rect, Rect_size, \
        vx, DWORD_size, \
        vy, DWORD_size

    struct WindowData, \
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
        and eax, 0x00808080
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
            local time, SYSTEMTIME_size, \
                window_rect, RECT_size, \
                new_window_rect, Rect_size

            ; Create window data
            fcall malloc, WindowData_size
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
            jmp .leave

        .wm_timer:
            mov eax, [wParam]
            cmp eax, FRAME_TIMER_ID
            je .wm_timer.frame_timer
            jmp .leave

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
            cmp edx, 0
            je .wm_timer.frame_timer.increase_level

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
        .wm_timer.frame_timer.increase_level_done:

            ; Update blue sqaures
            mov dword [index], 0
        .wm_timer.frame_timer.repeat:
            mov ecx, [index]
            cmp ecx, BLUE_SQUARES_COUNT
            je .wm_timer.frame_timer.done

            imul ecx, Square_size
            mov _di, [window_data]

            ; Update blue square position
            mov eax, [_di + WindowData.blue_squares + _cx + Square.vx]
            add [_di + WindowData.blue_squares + _cx + Square.rect + Rect.x], eax

            mov eax, [_di + WindowData.blue_squares + _cx + Square.vy]
            add [_di + WindowData.blue_squares + _cx + Square.rect + Rect.y], eax

            ; Blue square bounds check
            cmp dword [_di + WindowData.blue_squares + _cx + Square.rect + Rect.x], 0
            jl .wm_timer.frame_timer.repeat.invert_vx

            cmp dword [_di + WindowData.blue_squares + _cx + Square.rect + Rect.y], 0
            jl .wm_timer.frame_timer.repeat.invert_vy

            mov eax, [_di + WindowData.blue_squares + _cx + Square.rect + Rect.x]
            add eax, [_di + WindowData.blue_squares + _cx + Square.rect + Rect.width]
            cmp eax, [window_width]
            jg .wm_timer.frame_timer.repeat.invert_vx

            mov eax, [_di + WindowData.blue_squares + _cx + Square.rect + Rect.y]
            add eax, [_di + WindowData.blue_squares + _cx + Square.rect + Rect.height]
            cmp eax, [window_height]
            jg .wm_timer.frame_timer.repeat.invert_vy
        .wm_timer.frame_timer.invert_done:

            ; Check speed increase
            cmp dword [is_leveled], TRUE
            je .wm_timer.frame_timer.increase_speed
        .wm_timer.frame_timer.increase_speed_done:

            ; Check square collision
            mov eax, [_di + WindowData.blue_squares + _cx + Square.rect + Rect.x]
            add eax, [_di + WindowData.blue_squares + _cx + Square.rect + Rect.width]
            cmp [_di + WindowData.red_square + Square.rect + Rect.x], eax
            jge .wm_timer.frame_timer.square_collision_done

            mov eax, [_di + WindowData.red_square + Square.rect + Rect.x]
            add eax, [_di + WindowData.red_square + Square.rect + Rect.width]
            cmp eax, [_di + WindowData.blue_squares + _cx + Square.rect + Rect.x]
            jl .wm_timer.frame_timer.square_collision_done

            mov eax, [_di + WindowData.blue_squares + _cx + Square.rect + Rect.y]
            add eax, [_di + WindowData.blue_squares + _cx + Square.rect + Rect.height]
            cmp [_di + WindowData.red_square + Square.rect + Rect.y], eax
            jge .wm_timer.frame_timer.square_collision_done

            mov eax, [_di + WindowData.red_square + Square.rect + Rect.y]
            add eax, [_di + WindowData.red_square + Square.rect + Rect.height]
            cmp eax, [_di + WindowData.blue_squares + _cx + Square.rect + Rect.y]
            jl .wm_timer.frame_timer.square_collision_done

            jmp .wm_timer.frame_timer.gameover
        .wm_timer.frame_timer.square_collision_done:

            ; Go to next blue square
            inc dword [index]
            jmp .wm_timer.frame_timer.repeat
        .wm_timer.frame_timer.done:
            ; Redraw window
            invoke InvalidateRect, [hwnd], NULL, TRUE

            jmp .leave

        .wm_timer.frame_timer.increase_level:
            inc dword [_si + WindowData.level]
            mov dword [is_leveled], TRUE
            jmp .wm_timer.frame_timer.increase_level_done

        .wm_timer.frame_timer.repeat.invert_vx:
            neg dword [_di + WindowData.blue_squares + _cx + Square.vx]
            jmp .wm_timer.frame_timer.invert_done

        .wm_timer.frame_timer.repeat.invert_vy:
            neg dword [_di + WindowData.blue_squares + _cx + Square.vy]
            jmp .wm_timer.frame_timer.invert_done

        .wm_timer.frame_timer.increase_speed:
            shl dword [_di + WindowData.blue_squares + _cx + Square.vx], 1
            shl dword [_di + WindowData.blue_squares + _cx + Square.vy], 1
            jmp .wm_timer.frame_timer.increase_speed_done

        .wm_timer.frame_timer.gameover:
            ; Stop frame timer
            invoke KillTimer, [hwnd], FRAME_TIMER_ID

            ; Redraw window
            invoke InvalidateRect, [hwnd], NULL, TRUE

            ; Show alert
            invoke MessageBoxA, [hwnd], gameover_message, gameover_title, MB_RETRYCANCEL | MB_ICONINFORMATION
            cmp _ax, IDRETRY
            je .wm_timer.frame_timer.gameover.retry

            ; When cancel close window
            invoke DestroyWindow, [hwnd]

            jmp .leave

        .wm_timer.frame_timer.gameover.retry:
            ; When retry start game
            fcall game_start, [hwnd]

            end_local
            jmp .leave

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
            mov _di, [window_data]
            mov eax, [_di + WindowData.red_square + Square.rect + Rect.x]
            cmp [x], eax
            jl .leave

            mov eax, [_di + WindowData.red_square + Square.rect + Rect.y]
            cmp [y], eax
            jl .leave

            mov eax, [_di + WindowData.red_square + Square.rect + Rect.x]
            add eax, [_di + WindowData.red_square + Square.rect + Rect.width]
            cmp [x], eax
            jge .leave

            mov eax, [_di + WindowData.red_square + Square.rect + Rect.y]
            add eax, [_di + WindowData.red_square + Square.rect + Rect.height]
            cmp [y], eax
            jge .leave

            ; Start dragging
            mov dword [_di + WindowData.is_dragging], TRUE

            mov eax, [x]
            sub eax, [_di + WindowData.red_square + Square.rect + Rect.x]
            mov [_di + WindowData.red_square + Square.vx], eax

            mov eax, [y]
            sub eax, [_di + WindowData.red_square + Square.rect + Rect.y]
            mov [_di + WindowData.red_square + Square.vy], eax

            end_local
            jmp .leave

            %undef window_data

        .wm_mousemove:
            local window_data, POINTER_size

            ; Get window data
            invoke GetWindowLongPtrA, [hwnd], GWLP_USERDATA
            mov [window_data], _ax

            ; Check is dragging
            mov _di, [window_data]
            cmp dword [_di + WindowData.is_dragging], FALSE
            je .leave

            ; Update x position
            movzx eax, word [lParam]
            sub eax, [_di + WindowData.red_square + Square.vx]
            mov [_di + WindowData.red_square + Square.rect + Rect.x], eax

            ; Update y position
            mov eax, [lParam]
            shr eax, 16
            sub eax, [_di + WindowData.red_square + Square.vy]
            mov [_di + WindowData.red_square + Square.rect + Rect.y], eax

            end_local
            jmp .leave

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
            cmp eax, ecx
            jl .leave

            invoke ShellExecuteA, [hwnd], open_operation, website_url, NULL, NULL, SW_SHOWNORMAL

            end_local
            jmp .leave

            %undef window_data

        .wm_size:
            ; Save new window size
            movzx eax, word [lParam]
            mov [window_width], eax

            mov eax, [lParam]
            shr eax, 16
            mov [window_height], eax

            jmp .leave

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
            ; Draw a centered text in the window
            local window_data, POINTER_size, \
                paint_struct, PAINTSTRUCT_size, \
                hdc_buffer, POINTER_size, \
                bitmap_buffer, POINTER_size, \
                brush, POINTER_size, \
                rect, RECT_size, \
                font, POINTER_size, \
                stats_buffer, 128, \
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

            ; Draw border
            mov _si, [window_data]
            mov eax, [_si + WindowData.background_color]
            add eax, 0x00171717
            invoke CreateSolidBrush, _ax
            mov [brush], _ax

            mov dword [rect + RECT.left], 16
            mov dword [rect + RECT.top], 16 + 20 + 16 + 20 + 16
            mov eax, [window_width]
            sub eax, 16
            mov [rect + RECT.right], eax
            mov eax, [window_height]
            sub eax, 16 + 20 + 16
            mov [rect + RECT.bottom], eax

            invoke FillRect, [hdc_buffer], addr rect, [brush]

            invoke DeleteObject, [brush]

            ; Draw blue squares
            invoke CreateSolidBrush, 0x00ff0000
            mov [brush], _ax

            mov dword [index], 0
        .wm_paint.repeat:
            mov ecx, [index]
            cmp ecx, BLUE_SQUARES_COUNT
            je .wm_paint.done

            imul ecx, Square_size
            mov _si, [window_data]
            mov eax, [_si + WindowData.blue_squares + _cx + Square.rect + Rect.x]
            mov dword [rect + RECT.left], eax
            mov eax, [_si + WindowData.blue_squares + _cx + Square.rect + Rect.y]
            mov dword [rect + RECT.top], eax
            mov eax, [_si + WindowData.blue_squares + _cx + Square.rect + Rect.x]
            add eax, [_si + WindowData.blue_squares + _cx + Square.rect + Rect.width]
            mov [rect + RECT.right], eax
            mov eax, [_si + WindowData.blue_squares + _cx + Square.rect + Rect.y]
            add eax, [_si + WindowData.blue_squares + _cx + Square.rect + Rect.height]
            mov [rect + RECT.bottom], eax

            invoke FillRect, [hdc_buffer], addr rect, [brush]

            inc dword [index]
            jmp .wm_paint.repeat
        .wm_paint.done:
            invoke DeleteObject, [brush]

            ; Draw red square
            invoke CreateSolidBrush, 0x000000ff
            mov [brush], _ax

            mov _si, [window_data]
            mov eax, [_si + WindowData.red_square + Square.rect + Rect.x]
            mov dword [rect + RECT.left], eax
            mov eax, [_si + WindowData.red_square + Square.rect + Rect.y]
            mov dword [rect + RECT.top], eax
            mov eax, [_si + WindowData.red_square + Square.rect + Rect.x]
            add eax, [_si + WindowData.red_square + Square.rect + Rect.width]
            mov [rect + RECT.right], eax
            mov eax, [_si + WindowData.red_square + Square.rect + Rect.y]
            add eax, [_si + WindowData.red_square + Square.rect + Rect.height]
            mov [rect + RECT.bottom], eax

            invoke FillRect, [hdc_buffer], addr rect, [brush]

            invoke DeleteObject, [brush]

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
            fcall strlen, addr footer_label
            mov esi, [window_height]
            sub esi, 16 + 20
            invoke TextOutA, [hdc_buffer], 20, _si, footer_label, _ax

            invoke DeleteObject, [font]

            ; Draw and delete back buffer
            invoke BitBlt, [paint_struct + PAINTSTRUCT.hdc], 0, 0, [window_width], [window_height], [hdc_buffer], 0, 0, SRCCOPY
            invoke DeleteObject, [bitmap_buffer]
            invoke DeleteDC, [hdc_buffer]

            ; End paint
            invoke EndPaint, [hwnd], addr paint_struct

            end_local
            jmp .leave

        .wm_destroy:
            ; Free window data
            invoke GetWindowLongPtrA, [hwnd], GWLP_USERDATA
            fcall free, _ax

            ; Close process
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

        ; Register the window class
        mov dword [window_class + WNDCLASSEX.cbSize], WNDCLASSEX_size

        mov dword [window_class + WNDCLASSEX.style], 0

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
        invoke CreateWindowExA, 0, window_class_name, window_title, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX, \
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
    website_url db "https://bastiaan.ml/", 0
    gameover_title db "Game over!", 0
    gameover_message db "You are game over!", 0

    ; Global variables
    rand_seed dd 0
    window_width dd 800
    window_height dd 600

    ; Import table
    import_table
        library gdi_table, "GDI32.DLL", \
            kernel_table, "KERNEL32.DLL", \
            shell_table, "SHELL32.DLL", \
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
            SetTextColor, "SetTextColor", \
            TextOutA, "TextOutA"

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
            LoadCursorA, "LoadCursorA", \
            LoadIconA, "LoadIconA", \
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