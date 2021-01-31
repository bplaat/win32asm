; libwindows.inc - A 32-bit and 64-bit win32 assembly helper library
; Made by Bastiaan van der Plaat (https://bastiaan.ml/)
; Use -DWIN64 to enable 64-bit mode

; Setup instruction set
%ifdef WIN64
    [bits 64]
%else
    [bits 32]
%endif

; Setup executable base
%define _base 0x400000
%define _alignment 0x200
[org _base]
%define RVA(_address) (_address - _base)

; Virtual extended registers
%ifdef WIN64
    %idefine _ax rax
    %idefine _bx rbx
    %idefine _cx rcx
    %idefine _dx rdx
    %idefine _si rsi
    %idefine _di rdi
    %idefine _bp rbp
    %idefine _sp rsp
%else
    %idefine _ax eax
    %idefine _bx ebx
    %idefine _cx ecx
    %idefine _dx edx
    %idefine _si esi
    %idefine _di edi
    %idefine _bp ebp
    %idefine _sp esp
%endif

; Sizes
%define BYTE_size 1
%define WORD_size 2
%define DWORD_size 4
%define QWORD_size 8
%ifdef WIN64
    %idefine pointer qword
    %idefine dp dq
    %define POINTER_size QWORD_size
%else
    %idefine pointer dword
    %idefine dp dd
    %define POINTER_size DWORD_size
%endif

; Header
%define HEADER_GUI 2
%define HEADER_CONSOLE 3
%macro header 1
    _header:
        ; MS-DOS Header
        db 0x4D, 0x5A, 0x90, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00
        db 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00

        ; MS-DOS Stub
        db 0x0E, 0x1F, 0xBA, 0x0E, 0x00, 0xB4, 0x09, 0xCD, 0x21, 0xB8, 0x01, 0x4C, 0xCD, 0x21, 0x54, 0x68
        db 0x69, 0x73, 0x20, 0x70, 0x72, 0x6F, 0x67, 0x72, 0x61, 0x6D, 0x20, 0x63, 0x61, 0x6E, 0x6E, 0x6F
        db 0x74, 0x20, 0x62, 0x65, 0x20, 0x72, 0x75, 0x6E, 0x20, 0x69, 0x6E, 0x20, 0x44, 0x4F, 0x53, 0x20
        db 0x6D, 0x6F, 0x64, 0x65, 0x2E, 0x0D, 0x0D, 0x0A, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

        ; COFF Header
        db "PE", 0, 0                 ; Signature
        %ifdef WIN64
            dw 0x8664                 ; Machine
        %else
            dw 0x014C                 ; Machine
        %endif
        dw 2                          ; NumberOfSections
        dd __?POSIX_TIME?__           ; TimeDateStamp
        dd 0                          ; PointerToSymbolTable
        dd 0                          ; NumberOfSymbols
        dw _optional_header_size      ; SizeOfOptionalHeader
        dw 0x030F                     ; Characteristics

        ; Standard COFF Fields
    _optional_header:
        %ifdef WIN64
            dw 0x020B                 ; Magic
        %else
            dw 0x010B                 ; Magic
        %endif
        db 0                          ; MajorLinkerVersion
        db 0                          ; MinorLinkerVersion
        dd _code_section_aligned_size ; SizeOfCode
        dd _data_section_aligned_size ; SizeOfInitializedData
        dd 0                          ; SizeOfUninitializedData
        dd RVA(_entrypoint)           ; AddressOfEntryPoint
        dd RVA(_code_section)         ; BaseOfCode
        %ifndef WIN64
            dd RVA(_data_section)     ; BaseOfData
        %endif

        ; Windows-Specific Fields
        %ifdef WIN64
            dq _base                  ; ImageBase
        %else
            dd _base                  ; ImageBase
        %endif
        dd _alignment                 ; SectionAlignment
        dd _alignment                 ; FileAlignment
        dw 4                          ; MajorOperatingSystemVersion
        dw 0                          ; MinorOperatingSystemVersion
        dw 0                          ; MajorImageVersion
        dw 0                          ; MinorImageVersion
        dw 4                          ; MajorSubsystemVersion
        dw 0                          ; MinorSubsystemVersion
        dd 0                          ; Win32VersionValue
        dd _header_aligned_size + _code_section_aligned_size + _data_section_aligned_size ; SizeOfImage
        dd _header_aligned_size       ; SizeOfHeaders
        dd 0                          ; CheckSum
        dw %1                         ; Subsystem
        dw 0                          ; DllCharacteristics
        %ifdef WIN64
            dq 0x100000               ; SizeOfStackReserve
            dq 0x1000                 ; SizeOfStackCommit
            dq 0x100000               ; SizeOfHeapReserve
            dq 0x1000                 ; SizeOfHeapCommit
        %else
            dd 0x100000               ; SizeOfStackReserve
            dd 0x1000                 ; SizeOfStackCommit
            dd 0x100000               ; SizeOfHeapReserve
            dd 0x1000                 ; SizeOfHeapCommit
        %endif
        dd 0                          ; LoaderFlags
        dd 16                         ; NumberOfRvaAndSizes

        ; Data Directories
        dd 0, 0
        dd RVA(_import_table), _import_table_size
        times 14 dd 0, 0

        _optional_header_size equ $ - _optional_header

        ; Section Table

        ; Code Section
        db ".text", 0, 0, 0           ; Name
        dd _code_section_size         ; VirtualSize
        dd RVA(_code_section)         ; VirtualAddress
        dd _code_section_aligned_size ; SizeOfRawData
        dd RVA(_code_section)         ; PointerToRawData
        dd 0                          ; PointerToRelocations
        dd 0                          ; PointerToLinenumbers
        dw 0                          ; NumberOfRelocations
        dw 0                          ; NumberOfLinenumbers
        dd 0x60000020                 ; Characteristics

        ; Data Section
        db ".data", 0, 0, 0           ; Name
        dd _data_section_size         ; VirtualSize
        dd RVA(_data_section)         ; VirtualAddress
        dd _data_section_aligned_size ; SizeOfRawData
        dd RVA(_data_section)         ; PointerToRawData
        dd 0                          ; PointerToRelocations
        dd 0                          ; PointerToLinenumbers
        dw 0                          ; NumberOfRelocations
        dw 0                          ; NumberOfLinenumbers
        dd 0xC0000040                 ; Characteristics

    _header_size equ $ - _header
        align _alignment, db 0
    _header_aligned_size equ $ - _header
%endmacro
%macro header 0
    header HEADER_GUI
%endmacro

; Code section
%macro code_section 0
    _code_section:
%endmacro

%macro end_code_section 0
    _code_section_size equ $ - _code_section
        align _alignment, db 0
    _code_section_aligned_size equ $ - _code_section
%endmacro

%macro entrypoint 0
    _entrypoint:
        %ifdef WIN64
            sub rsp, QWORD_size
        %endif
        mov _bp, _sp
%endmacro

; Data section
%macro data_section 0
    _data_section:
%endmacro

%macro end_data_section 0
    _data_section_size equ $ - _data_section
        align _alignment, db 0
    _data_section_aligned_size equ $ - _data_section
%endmacro

; Import section
%macro import_table 0
    _import_table:
%endmacro

%macro end_import_table 0
    _import_table_size equ $ - _import_table
%endmacro

%macro library 2-*
    %rep %0 / 2
        dd 0, 0, 0, RVA(_%1), RVA(%1)
        %rotate 2
    %endrep
    dd 0, 0, 0, 0, 0

    %rep %0 / 2
        _%1 db %2, 0
        %rotate 2
    %endrep
%endmacro

%macro import 3-*
    %1:
        %rotate 1
        %rep (%0 - 1) / 2
            %ifdef WIN64
                %1 dq RVA(_%1)
            %else
                %1 dd RVA(_%1)
            %endif
            %rotate 2
        %endrep
        %ifdef WIN64
            dq 0
        %else
            dd 0
        %endif

        %rotate 1
        %rep (%0 - 1) / 2
            _%1 db 0, 0, %2, 0
            %rotate 2
        %endrep
%endmacro

; Code helpers
%macro struct 3-*
    %1:
        %rotate 1
        %assign _index 0
        %assign _position 0
        %rep (%0 - 1) / 2
            .%1 equ _position
            %assign _position _position + %2
            %rotate 2

            ; Pointer align items
            %if _index != ((%0 - 1) / 2 - 1)
                %if (_position % POINTER_size) > 0
                    %ifnum %2
                        %if %2 == POINTER_size
                            %assign _position _position + (_position % POINTER_size)
                        %endif
                    %else
                        %error struct macro argument: %1, %2
                    %endif
                %endif
            %else
                %if (_position % POINTER_size) > 0
                    %assign _position _position + (_position % POINTER_size)
                %endif
            %endif

            %assign _index _index + 1
        %endrep
        %xdefine %1_size _position
%endmacro

%macro function 1-*
    %1:
        push _bp
        mov _bp, _sp

        %ifdef WIN64
            %assign _index 1
            %rotate 1
            %rep %0 - 1
                %xdefine %1 rbp + (_index + 1) * QWORD_size

                %if _index == 1
                    mov [%1], rcx
                %endif
                %if _index == 2
                    mov [%1], rdx
                %endif
                %if _index == 3
                    mov [%1], r8
                %endif
                %if _index == 4
                    mov [%1], r9
                %endif

                %assign _index _index + 1
                %rotate 1
            %endrep
        %else
            %assign _index 1
            %rotate 1
            %rep %0 - 1
                %xdefine %1 ebp + (_index + 1) * DWORD_size
                %assign _index _index + 1
                %rotate 1
            %endrep

            ._arguments_size equ (%0 - 1) * DWORD_size
        %endif
%endmacro

%macro return 0
    mov _sp, _bp
    pop _bp

    %ifdef WIN64
        ret
    %else
        ret ._arguments_size
    %endif
%endmacro
%macro return 1
    mov _ax, %1
    return
%endmacro

%macro local 2-*
    %assign _index 0
    %assign _size 0
    %rep %0 / 2
        %assign _size _size + %2
        %rotate 2

        ; Pointer align items
        %if _index != (%0 / 2 - 1)
            %if (_size % POINTER_size) > 0
                %ifnum %2
                    %if %2 == POINTER_size
                        %assign _size _size + (_size % POINTER_size)
                    %endif
                %else
                    %error local macro argument: %1, %2
                %endif
            %endif
        %endif

        %assign _index _index + 1
    %endrep

    %ifdef WIN64
        %ifndef _fastcall_frame
            %define _fastcall_frame
            %assign _size _size + 4 * QWORD_size
        %endif

        sub rsp, (_size + 15) & (~15)
    %else
        sub esp, (_size + 3) & (~3)
    %endif

    %assign _index 0
    %assign _position 0
    %rep %0 / 2
        %assign _position _position + %2

        %xdefine %1 _bp - _position
        %rotate 2

        ; Pointer align items
        %if _index != (%0 / 2 - 1)
            %if (_position % POINTER_size) > 0
                %ifnum %2
                    %if %2 == POINTER_size
                        %assign _position _position + (_position % POINTER_size)
                    %endif
                %else
                    %error local macro argument: %1, %2
                %endif
            %endif
        %endif

        %assign _index _index + 1
    %endrep
%endmacro

%macro end_local 0
    %ifdef _fastcall_frame
        %undef _fastcall_frame
    %endif
%endmacro

%macro frame 0
    %ifdef WIN64
        %ifndef _fastcall_frame
            %define _fastcall_frame
            sub rsp, 4 * QWORD_size
        %endif
    %endif
%endmacro

%macro end_frame 0
    %ifdef WIN64
        %ifdef _fastcall_frame
            %undef _fastcall_frame
            add rsp, 4 * QWORD_size
        %endif
    %endif
%endmacro

%define float 'f',
%define addr '&',

; Fastcall
%macro fastcall 1-*
    ; Count good arguments
    %assign _arguments_count 0
    %rotate 1
    %rep %0 - 1
        %ifnstr %1
            %assign _arguments_count _arguments_count + 1
        %endif
        %rotate 1
    %endrep

    ; Reserve stack space 16 byte aligned
    %ifdef _fastcall_frame
        %if _arguments_count > 4
            sub rsp, (((_arguments_count - 4) * QWORD_size) + 15) & (~15)
        %endif
    %else
        %if _arguments_count > 4
            sub rsp, ((_arguments_count * QWORD_size) + 15) & (~15)
        %else
            sub rsp, 4 * QWORD_size
        %endif
    %endif

    ; Loop over arguments
    %assign _index %0 - 1
    %assign _position _arguments_count
    %rep %0
        %if _index < 0
            %exitrep
        %endif

        %rotate -2

        %assign _isHandled 0

        %ifstr %1
            ; Load address helper
            %if %1 == '&'
                %rotate 1

                %if _position == 1
                    lea rcx, [%1]
                %elif _position == 2
                    lea rdx, [%1]
                %elif _position == 3
                    lea r8, [%1]
                %elif _position == 4
                    lea r9, [%1]
                %else
                    lea rcx, [%1]
                    mov [rsp + (_position - 1) * QWORD_size], rcx
                %endif

                %rotate -1
                %assign _index _index - 2
                %assign _isHandled 1
            %endif

            ; Float helper
            %if %1 == 'f'
                %rotate 1

                %if _position == 1
                    movss xmm0, %1
                %elif _position == 2
                    movss xmm1, %1
                %elif _position == 3
                    movss xmm2, %1
                %elif _position == 4
                    movss xmm3, %1
                %else
                    %ifnum %1
                        %if %1 == 0
                            xor rcx, rcx
                        %else
                            mov rcx, %1
                        %endif
                    %else
                        mov rcx, %1
                    %endif
                    mov [rsp + (_position - 1) * QWORD_size], rcx
                %endif

                %rotate -1
                %assign _index _index - 2
                %assign _isHandled 1
            %endif
        %endif

        ; Or just a normal value
        %if _isHandled == 0
            %rotate 1

            %if _position == 0
                call %1
            %elif _position == 1
                %ifnum %1
                    %if %1 == 0
                        xor rcx, rcx
                    %else
                        mov rcx, %1
                    %endif
                %else
                    mov rcx, %1
                %endif
            %elif _position == 2
                %ifnum %1
                    %if %1 == 0
                        xor rdx, rdx
                    %else
                        mov rdx, %1
                    %endif
                %else
                    mov rdx, %1
                %endif
            %elif _position == 3
                %ifnum %1
                    %if %1 == 0
                        xor r8, r8
                    %else
                        mov r8, %1
                    %endif
                %else
                    mov r8, %1
                %endif
            %elif _position == 4
                %ifnum %1
                    %if %1 == 0
                        xor r9, r9
                    %else
                        mov r9, %1
                    %endif
                %else
                    mov r9, %1
                %endif
            %else
                %ifnum %1
                    %if %1 == 0
                        xor rcx, rcx
                    %else
                        mov rcx, %1
                    %endif
                %else
                    mov rcx, %1
                %endif
                mov [rsp + (_position - 1) * QWORD_size], rcx
            %endif

            %assign _index _index - 1
        %endif

        %assign _position _position - 1
    %endrep

    ; Free stack space 16 byte aligned
    %ifdef _fastcall_frame
        %if _arguments_count > 4
            add rsp, (((_arguments_count - 4) * QWORD_size) + 15) & (~15)
        %endif
    %else
        %if _arguments_count > 4
            add rsp, ((_arguments_count * QWORD_size) + 15) & (~15)
        %else
            add rsp, 4 * QWORD_size
        %endif
    %endif
%endmacro

; Stdcall and cdeclcall
%macro push_reverse 1-*
    %assign _index %0 - 1
    %rep %0
        %if _index < 0
            %exitrep
        %endif

        %rotate -2

        %assign _isHandled 0

        %ifstr %1
            ; Load address helper
            %if %1 == '&'
                %rotate 1
                lea eax, [%1]
                push eax
                %rotate -1
                %assign _index _index - 2
                %assign _isHandled 1
            %endif

            ; Ignore float helper only for 64-bit fastcall
            %if %1 == 'f'
                %rotate 1
                push dword %1
                %rotate -1
                %assign _index _index - 2
                %assign _isHandled 1
            %endif
        %endif

        ; Or just push the value
        %if _isHandled == 0
            %rotate 1
            push dword %1
            %assign _index _index - 1
        %endif
    %endrep
%endmacro

%macro stdcall 2+
    push_reverse %2
    call %1
%endmacro
%macro stdcall 1
    call %1
%endmacro

%macro cdeclcall 2+
    push_reverse %2
    call %1
    add esp, (%0 - 1) * DWORD_size
%endmacro
%macro cdeclcall 1
    call %1
%endmacro

; Calls and invokes prefixes
%macro fcall 1+
    %ifdef WIN64
        fastcall %1
    %else
        stdcall %1
    %endif
%endmacro

%macro invoke 2+
    %ifdef WIN64
        fastcall [%1], %2
    %else
        stdcall [%1], %2
    %endif
%endmacro
%macro invoke 1
    %ifdef WIN64
        fastcall [%1]
    %else
        stdcall [%1]
    %endif
%endmacro

%macro cinvoke 2+
    %ifdef WIN64
        fastcall [%1], %2
    %else
        cdeclcall [%1], %2
    %endif
%endmacro
%macro cinvoke 1
    %ifdef WIN64
        fastcall [%1]
    %else
        cdeclcall [%1]
    %endif
%endmacro

; Headers

; Windef
%define NULL 0
%define TRUE 1
%define FALSE 0

; Kernel32
%define STD_INPUT_HANDLE -10
%define STD_OUTPUT_HANDLE -11

struct SYSTEMTIME, \
    wYear, WORD_size, \
    wMonth, WORD_size, \
    wDayOfWeek, WORD_size, \
    wDay, WORD_size, \
    wHour, WORD_size, \
    wMinute, WORD_size, \
    wSecond, WORD_size, \
    wMilliseconds, WORD_size

; User32
%define HWND_DESKTOP 0
%define HWND_TOP 0

%define MB_OK 0x00000000
%define MB_ICONINFORMATION 0x00000040

%define CS_VREDRAW 0x0001
%define CS_HREDRAW 0x0002

%define COLOR_WINDOW 5

%define IDI_APPLICATION 32512

%define IDC_ARROW 32512

%define CW_USEDEFAULT 0x80000000

%define WS_CHILD 0x040000000
%define WS_VISIBLE 0x010000000
%define WS_BORDER 000800000h
%define WS_OVERLAPPEDWINDOW 0x00CF0000

%define SW_SHOWNORMAL 1
%define SW_SHOWDEFAULT 10

%define WM_CREATE 0x0001
%define WM_SIZE 0x0005
%define WM_GETMINMAXINFO 0x0024
%define WM_DESTROY 0x0002
%define WM_PAINT 0x000F
%define WM_ERASEBKGND 0x0014
%define WM_SETFONT 0x0030
%define WM_COMMAND 0x0111

%define SM_CXSCREEN 0
%define SM_CYSCREEN 1

%define SWP_NOZORDER 0x0004

%define DT_CENTER 0x00000001
%define DT_VCENTER 0x00000004
%define DT_SINGLELINE 0x00000020

%define GWLP_USERDATA -21

%ifdef WIN64
    %define GetWindowLongPtrAString 'GetWindowLongPtrA'
    %define SetWindowLongPtrAString 'SetWindowLongPtrA'
%else
    %define GetWindowLongPtrAString 'GetWindowLongA'
    %define SetWindowLongPtrAString 'SetWindowLongA'
%endif

struct POINT, \
    x, DWORD_size, \
    y, DWORD_size

struct RECT, \
    left, DWORD_size, \
    top, DWORD_size, \
    right, DWORD_size, \
    bottom, DWORD_size

struct WNDCLASSEX, \
    cbSize, DWORD_size, \
    style, DWORD_size, \
    lpfnWndProc, POINTER_size, \
    cbClsExtra, DWORD_size, \
    cbWndExtra, DWORD_size, \
    hInstance, POINTER_size, \
    hIcon, POINTER_size, \
    hCursor, POINTER_size, \
    hbrBackground, POINTER_size, \
    lpszMenuName, POINTER_size, \
    lpszClassName, POINTER_size, \
    hIconSm, POINTER_size

struct MSG, \
    hwnd, POINTER_size, \
    message, DWORD_size, \
    wParam, POINTER_size, \
    lParam, POINTER_size, \
    time, DWORD_size, \
    pt, POINT_size, \
    lPrivate, DWORD_size

struct MINMAXINFO, \
    ptReserved, POINT_size, \
    ptMaxSize, POINT_size, \
    ptMaxPosition, POINT_size, \
    ptMinTrackSize, POINT_size, \
    ptMaxTrackSize, POINT_size

struct PAINTSTRUCT, \
    hdc, POINTER_size, \
    fErase, DWORD_size, \
    rcPaint, RECT_size, \
    fRestore, DWORD_size, \
    fIncUpdate, DWORD_size, \
    rgbReserved, 32

; Gdi32
%define DEFAULT_GUI_FONT 17

%define CLEARTYPE_QUALITY 5

%define TRANSPARENT 1

; Comctl32
%define ICC_WIN95_CLASSES 0x000000FF

struct INITCOMMONCONTROLSEX, \
    dwSize, DWORD_size, \
    dwICC, DWORD_size

%define LVS_LIST 0x0003
%define LVS_SORTASCENDING 0x0010

%define LVM_FIRST 0x1000
%define LVM_INSERTITEMA LVM_FIRST + 7

%define LVIF_TEXT 0x0001

struct LVITEM, \
    mask, DWORD_size, \
    iItem, DWORD_size, \
    iSubItem, DWORD_size, \
    state, DWORD_size, \
    stateMask, DWORD_size, \
    pszText, POINTER_size, \
    cchTextMax, DWORD_size, \
    iImage, DWORD_size, \
    lParam, POINTER_size, \
    iIndent, DWORD_size, \
    iGroupId, DWORD_size, \
    cColumns, DWORD_size, \
    puColumns, POINTER_size, \
    piColFmt, POINTER_size, \
    iGroup, DWORD_size

; Ws2_32
%define AF_UNSPEC 0

%define SOCK_STREAM 1

%define IPPROTO_TCP 6

%define SD_SEND 1

struct WSADATA, \
    wVersion, WORD_size, \
    wHighVersion, WORD_size, \
    szDescription, 256 + 1, \
    szSystemStatus, 128 + 1, \
    iMaxSockets, WORD_size, \
    iMaxUdpDg, WORD_size, \
    lpVendorInfo, POINTER_size

struct addrinfo, \
    ai_flags, DWORD_size, \
    ai_family, DWORD_size, \
    ai_socktype, DWORD_size, \
    ai_protocol, DWORD_size, \
    ai_addrlen, POINTER_size, \
    ai_canonname, POINTER_size, \
    ai_addr, POINTER_size, \
    ai_next, POINTER_size

; Gdiplus
%define SmoothingModeAntiAlias 5

%define UnitPixel 2

%define FontStyleRegular 0

%define StringAlignmentCenter 1

%define TextRenderingHintClearTypeGridFit 5

struct GdiplusStartupInput, \
    GdiplusVersion, DWORD_size, \
    DebugEventCallback, POINTER_size, \
    SuppressBackgroundThread, DWORD_size, \
    SuppressExternalCodecs, DWORD_size

; Custom
%define EXIT_SUCCESS 0
%define EXIT_FAILURE 1

struct Rect, \
    x, DWORD_size, \
    y, DWORD_size, \
    width, DWORD_size, \
    height, DWORD_size
