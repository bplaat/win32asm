    ; controls.asm - An pure 32-bit and 64-bit win32 assembly GUI controls program
    ; Made by Bastiaan van der Plaat (https://bastiaan.ml/)
    ; 32-bit: nasm -f bin controls.asm -o controls-x86.exe && ./controls-x86
    ; 64-bit: nasm -DWIN64 -f bin controls.asm -o controls-x64.exe && ./controls-x64

%include "libwindows.inc"

header

struct WindowData, \
    play_button_hwnd, POINTER_size, \
    about_button_hwnd, POINTER_size, \
    exit_button_hwnd, POINTER_size, \
    list_view_hwnd, POINTER_size

code_section
    %define PLAY_BUTTON_ID 1
    %define ABOUT_BUTTON_ID 2
    %define EXIT_BUTTON_ID 3

    ; A function allocates memory
    function malloc, size
        invoke GetProcessHeap
        invoke HeapAlloc, _ax, 0, [size]
        return
        %undef size

    ; A function that frees memory
    function free, ptr
        invoke GetProcessHeap
        invoke HeapFree, _ax, 0, [ptr]
        return

    ; A function that zero's out some memory
    function ZeroMemory, address, size
        mov _di, [address]
        xor _cx, _cx
    .repeat:
        cmp _cx, [size]
        je .done
        mov byte [_di + _cx], 0
        inc _cx
        jmp .repeat
    .done:
        return

    ; Set font function
    function SetFont, hwnd, font
        invoke SendMessageA, [hwnd], WM_SETFONT, [font], TRUE
        return TRUE
        %undef hwnd

    ; Window procedure function
    function WindowProc, hwnd, uMsg, wParam, lParam
        mov eax, [uMsg]
        cmp eax, WM_CREATE
        je .wm_create
        cmp eax, WM_SIZE
        je .wm_size
        cmp eax, WM_COMMAND
        je .wm_command
        cmp eax, WM_GETMINMAXINFO
        je .wm_getminmaxinfo
        cmp eax, WM_DESTROY
        je .wm_destroy
        jmp .default

        .wm_create:
            local window_data, POINTER_size, \
                window_rect, RECT_size, \
                new_window_rect, Rect_size, \
                list_item, LVITEM_size, \
                item_buffer, 128

            ; Allocate WindowData structure
            fcall malloc, WindowData_size
            mov [window_data], _ax

            invoke SetWindowLongPtrA, [hwnd], GWLP_USERDATA, [window_data]

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

            ; Create window controls
            invoke CreateWindowExA, 0, button_class_name, play_button, WS_CHILD | WS_VISIBLE, \
                0, 0, 0, 0, [hwnd], PLAY_BUTTON_ID, NULL, NULL
            mov _di, [window_data]
            mov [_di + WindowData.play_button_hwnd], _ax

            invoke CreateWindowExA, 0, button_class_name, about_button, WS_CHILD | WS_VISIBLE, \
                0, 0, 0, 0, [hwnd], ABOUT_BUTTON_ID, NULL, NULL
            mov _di, [window_data]
            mov [_di + WindowData.about_button_hwnd], _ax

            invoke CreateWindowExA, 0, button_class_name, exit_button, WS_CHILD | WS_VISIBLE, \
                0, 0, 0, 0, [hwnd], EXIT_BUTTON_ID, NULL, NULL
            mov _di, [window_data]
            mov [_di + WindowData.exit_button_hwnd], _ax

            invoke CreateWindowExA, 0, listview_class_name, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_LIST | LVS_SORTASCENDING, \
                0, 0, 0, 0, [hwnd], EXIT_BUTTON_ID, NULL, NULL
            mov _di, [window_data]
            mov [_di + WindowData.list_view_hwnd], _ax

            ; Add 500 text items to list view
            xor _bx, _bx
        .list_view_repeat:
            cmp _bx, 500
            je .list_view_done

            cinvoke wsprintfA, addr item_buffer, item_format, _bx

            fcall ZeroMemory, addr list_item, LVITEM_size
            mov dword [list_item + LVITEM.mask], LVIF_TEXT
            lea _ax, [item_buffer]
            mov pointer [list_item + LVITEM.pszText], _ax

            mov _si, [window_data]
            invoke SendMessageA, [_si + WindowData.list_view_hwnd], LVM_INSERTITEMA, NULL, addr list_item

            inc _bx
            jmp .list_view_repeat
        .list_view_done:

            ; Change font to default font
            invoke GetStockObject, DEFAULT_GUI_FONT
            invoke EnumChildWindows, [hwnd], SetFont, _ax

            end_local
            jmp .leave

            %undef window_data
            %undef window_rect

        .wm_size:
            local window_data, POINTER_size, \
                rect, Rect_size

            ; Get WindowData structure
            invoke GetWindowLongPtrA, [hwnd], GWLP_USERDATA
            mov [window_data], _ax

            ; Save new window size
            movzx eax, word [lParam]
            mov [window_width], eax

            mov eax, dword [lParam]
            shr eax, 16
            mov [window_height], eax

            ; Resize the window controls
            %define padding 16

            ; Resize the play button
            mov eax, padding
            mov [rect + Rect.x], eax

            mov eax, padding
            mov [rect + Rect.y], eax

            mov eax, [window_width]
            shr eax, 1
            sub eax, padding * 2 - padding / 2
            mov [rect + Rect.width], eax

            mov eax, [window_height]
            sub eax, padding * 4
            xor edx, edx
            mov ecx, 3
            idiv ecx
            mov [rect + Rect.height], eax

            mov _si, [window_data]
            invoke SetWindowPos, [_si + WindowData.play_button_hwnd], NULL, [rect + Rect.x], [rect + Rect.y], [rect + Rect.width], [rect + Rect.height], SWP_NOZORDER

            ; Resize the about button
            mov eax, [rect + Rect.y]
            add eax, [rect + Rect.height]
            add eax, padding
            mov [rect + Rect.y], eax

            mov _si, [window_data]
            invoke SetWindowPos, [_si + WindowData.about_button_hwnd], NULL, [rect + Rect.x], [rect + Rect.y], [rect + Rect.width], [rect + Rect.height], SWP_NOZORDER

            ; Resize the exit button
            mov eax, [rect + Rect.y]
            add eax, [rect + Rect.height]
            add eax, padding
            mov [rect + Rect.y], eax

            mov _si, [window_data]
            invoke SetWindowPos, [_si + WindowData.exit_button_hwnd], NULL, [rect + Rect.x], [rect + Rect.y], [rect + Rect.width], [rect + Rect.height], SWP_NOZORDER

            ; Resize the list view
            mov eax, [window_width]
            shr eax, 1
            add eax, padding - padding / 2
            mov [rect + Rect.x], eax

            mov eax, padding
            mov [rect + Rect.y], eax

            mov eax, [window_height]
            sub eax, padding * 2
            mov [rect + Rect.height], eax

            mov _si, [window_data]
            invoke SetWindowPos, [_si + WindowData.list_view_hwnd], NULL, [rect + Rect.x], [rect + Rect.y], [rect + Rect.width], [rect + Rect.height], SWP_NOZORDER

            end_local
            jmp .leave

        .wm_command:
            movzx eax, word [wParam]
            cmp eax, PLAY_BUTTON_ID
            je .play_button
            cmp eax, ABOUT_BUTTON_ID
            je .about_button
            cmp eax, EXIT_BUTTON_ID
            je .exit_button
            jmp .leave

        .play_button:
            invoke ShellExecuteA, [hwnd], open_operation, website_url, NULL, NULL, SW_SHOWNORMAL
            jmp .leave

        .about_button:
            invoke MessageBoxA, [hwnd], about_message, about_button, MB_OK | MB_ICONINFORMATION
            jmp .leave

        .exit_button:
            invoke DestroyWindow, [hwnd]
            jmp .leave

        .wm_getminmaxinfo:
            ; Set window min size
            mov _ax, [lParam]
            mov dword [_ax + MINMAXINFO.ptMinTrackSize + POINT.x], 320
            mov dword [_ax + MINMAXINFO.ptMinTrackSize + POINT.y], 240
            jmp .leave

        .wm_destroy:
            ; Free WindowData structure
            invoke GetWindowLongPtrA, [hwnd], GWLP_USERDATA
            fcall free, _ax

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
        local initCommonControlsEx, INITCOMMONCONTROLSEX_size, \
            window_class, WNDCLASSEX_size, \
            hwnd, POINTER_size, \
            message, MSG_size

        ; Init common controls for modern control style
        mov dword [initCommonControlsEx + INITCOMMONCONTROLSEX.dwSize], INITCOMMONCONTROLSEX_size
        mov dword [initCommonControlsEx + INITCOMMONCONTROLSEX.dwICC], ICC_WIN95_CLASSES
        invoke InitCommonControlsEx, addr initCommonControlsEx

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

        mov dword [window_class + WNDCLASSEX.hbrBackground], COLOR_WINDOW + 1

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

        end_local
end_code_section

data_section
    ; String constants
    window_class_name db "controls-test", 0
    %ifdef WIN64
        window_title db "This is a test controls window (64-bit)", 0
    %else
        window_title db "This is a test controls window (32-bit)", 0
    %endif

    button_class_name db "BUTTON", 0
    play_button db "Play", 0
    about_button db "About", 0
    exit_button db "Exit", 0

    open_operation db "open", 0
    about_message db "Made by Bastiaan van der Plaat", 13, 10
        website_url db "https://bastiaan.ml/", 0

    listview_class_name db "SysListView32", 0
    item_format db "Item %d", 0

    ; Global variables
    window_width dd 1024
    window_height dd 768

    ; Import table
    import_table
        library comctl_table, "COMCTL32.DLL", \
            gdi_table, "GDI32.DLL", \
            kernel_table, "KERNEL32.DLL", \
            shell_table, "SHELL32.DLL", \
            user_table, "USER32.DLL"

        import comctl_table, \
            InitCommonControlsEx, "InitCommonControlsEx"

        import gdi_table, \
            GetStockObject, "GetStockObject"

        import kernel_table, \
            ExitProcess, "ExitProcess", \
            HeapAlloc, "HeapAlloc", \
            HeapFree, "HeapFree", \
            GetModuleHandleA, "GetModuleHandleA", \
            GetProcessHeap, "GetProcessHeap"

        import shell_table, \
            ShellExecuteA, "ShellExecuteA"

        import user_table, \
            CreateWindowExA, "CreateWindowExA", \
            DefWindowProcA, "DefWindowProcA", \
            DestroyWindow, "DestroyWindow", \
            DispatchMessageA, "DispatchMessageA", \
            EnumChildWindows, "EnumChildWindows", \
            GetClientRect, "GetClientRect", \
            GetMessageA, "GetMessageA", \
            GetSystemMetrics, "GetSystemMetrics", \
            GetWindowLongPtrA, GetWindowLongPtrAString, \
            LoadCursorA, "LoadCursorA", \
            LoadIconA, "LoadIconA", \
            MessageBoxA, "MessageBoxA", \
            PostQuitMessage, "PostQuitMessage", \
            RegisterClassExA, "RegisterClassExA", \
            SendMessageA, "SendMessageA", \
            SetWindowLongPtrA, SetWindowLongPtrAString, \
            SetWindowPos, "SetWindowPos", \
            ShowWindow, "ShowWindow", \
            TranslateMessage, "TranslateMessage", \
            UpdateWindow, "UpdateWindow", \
            wsprintfA, "wsprintfA"
    end_import_table
end_data_section
