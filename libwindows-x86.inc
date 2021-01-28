    ; libwindows-x86.inc - A 32-bit win32 assembly helper library
    ; Made by Bastiaan van der Plaat (https://bastiaan.ml/)

    _base equ 0x400000
    _alignment equ 0x200

    bits 32
    org _base

%define RVA(address) (address - _base)

; Header
HEADER_GUI equ 2
HEADER_CONSOLE equ 3
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
        dw 0x014C                     ; Machine
        dw 2                          ; NumberOfSections
        dd __?POSIX_TIME?__           ; TimeDateStamp
        dd 0                          ; PointerToSymbolTable
        dd 0                          ; NumberOfSymbols
        dw 224                        ; SizeOfOptionalHeader
        dw 0x030F                     ; Characteristics

        ; Standard COFF Fields
        dw 0x010B                     ; Magic
        db 0                          ; MajorLinkerVersion
        db 0                          ; MinorLinkerVersion
        dd _code_section_aligned_size ; SizeOfCode
        dd _data_section_aligned_size ; SizeOfInitializedData
        dd 0                          ; SizeOfUninitializedData
        dd RVA(_entrypoint)           ; AddressOfEntryPoint
        dd RVA(_code_section)         ; BaseOfCode
        dd RVA(_data_section)         ; BaseOfData

        ; Windows-Specific Fields
        dd _base                      ; ImageBase
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
        dd 0x100000                   ; SizeOfStackReserve
        dd 0x1000                     ; SizeOfStackCommit
        dd 0x100000                   ; SizeOfHeapReserve
        dd 0x1000                     ; SizeOfHeapCommit
        dd 0                          ; LoaderFlags
        dd 16                         ; NumberOfRvaAndSizes

        ; Data Directories
        dd 0, 0
        dd RVA(_import_table), _import_table_size
        times 14 dd 0, 0

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
        mov ebp, esp
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
            %1 dd RVA(_%1)
            %rotate 2
        %endrep
        dd 0

        %rotate 1
        %rep (%0 - 1) / 2
            _%1 db 0, 0, %2, 0
            %rotate 2
        %endrep
%endmacro

; Code helpers

; Virtual registers
%idefine _ax eax
%idefine _bx ebx
%idefine _cx ecx
%idefine _dx edx
%idefine _si esi
%idefine _di edi

; Windows Library sizes
BYTE_size equ 1
WORD_size equ 2
DWORD_size equ 4
QWORD_size equ 8
%idefine pointer dword
POINTER_size equ DWORD_size

%macro struct 3-*
    %1:
        %assign position 0
        %rotate 1
        %rep (%0 - 1) / 2
            .%1 equ position
            %assign position position + %2
            %rotate 2
        %endrep
        %1_size equ position
%endmacro

%macro function 1-*
    %1:
        ._arguments_size equ (%0 - 1) * DWORD_size

        push ebp
        mov ebp, esp

        %assign position 2
        %rotate 1
        %rep %0 - 1
            %xdefine %1 ebp + position * DWORD_size
            %assign position position + 1
            %rotate 1
        %endrep
%endmacro

%macro return 1
    mov eax, %1
    mov esp, ebp
    pop ebp
    ret ._arguments_size
%endmacro
%macro return 0
    mov esp, ebp
    pop ebp
    ret ._arguments_size
%endmacro

%macro local 2-*
    %assign size 0
    %rep %0 / 2
        %assign size size + %2
        %rotate 2
    %endrep

    sub esp, size

    %assign position 0
    %rep %0 / 2
        %assign position position + %2
        %xdefine %1 ebp - position
        %rotate 2
    %endrep
%endmacro

; Frame macro's are only for 64-bit fastcall
%macro frame 0
%endmacro

%macro end_frame 0
%endmacro

%define float 'f',
%define addr '&',

%macro push_reverse 1-*
    %assign index %0 - 1
    %rep %0
        %if index < 0
            %exitrep
        %endif

        %rotate -2

        %assign isHandled 0

        %ifstr %1
            ; Load address helper
            %if %1 == '&'
                %rotate 1
                lea eax, [%1]
                push eax
                %rotate -1
                %assign index index - 2
                %assign isHandled 1
            %endif

            ; Ignore float helper only for 64-bit fastcall
            %if %1 == 'f'
                %rotate 1
                push dword %1
                %rotate -1
                %assign index index - 2
                %assign isHandled 1
            %endif
        %endif

        ; Or just push the value
        %if isHandled == 0
            %rotate 1
            push dword %1
            %assign index index - 1
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

%macro fcall 1+
    stdcall %1
%endmacro

%macro invoke 2+
    stdcall [%1], %2
%endmacro
%macro invoke 1
    stdcall [%1]
%endmacro

%macro cinvoke 2+
    cdeclcall [%1], %2
%endmacro
%macro cinvoke 1
    cdeclcall [%1]
%endmacro

; Include the shared libwindows stuff
%include "libwindows-shared.inc"
