    ; libwindows-x64.inc - A 64-bit win32 assembly helper library
    ; Made by Bastiaan van der Plaat (https://bastiaan.ml/)

    _base equ 0x400000
    _alignment equ 0x200

    bits 64
    org _base

%define RVA(address) (address - _base)

; Header
%macro header 0
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
        dw 0x8664                     ; Machine
        dw 2                          ; NumberOfSections
        dd __?POSIX_TIME?__           ; TimeDateStamp
        dd 0                          ; PointerToSymbolTable
        dd 0                          ; NumberOfSymbols
        dw 240                        ; SizeOfOptionalHeader
        dw 0x030F                     ; Characteristics

        ; Standard COFF Fields
        dw 0x020B                     ; Magic
        db 0                          ; MajorLinkerVersion
        db 0                          ; MinorLinkerVersion
        dd _code_section_aligned_size ; SizeOfCode
        dd _data_section_aligned_size ; SizeOfInitializedData
        dd 0                          ; SizeOfUninitializedData
        dd RVA(_entrypoint)           ; AddressOfEntryPoint
        dd RVA(_code_section)         ; BaseOfCode

        ; Windows-Specific Fields
        dq _base                      ; ImageBase
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
        dw 2                          ; Subsystem
        dw 0                          ; DllCharacteristics
        dq 0x100000                   ; SizeOfStackReserve
        dq 0x1000                     ; SizeOfStackCommit
        dq 0x100000                   ; SizeOfHeapReserve
        dq 0x1000                     ; SizeOfHeapCommit
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
        sub rsp, 8
        mov rbp, rsp
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
            %1 dq RVA(_%1)
            %rotate 2
        %endrep
        dq 0

        %rotate 1
        %rep (%0 - 1) / 2
            _%1 db 0, 0, %2, 0
            %rotate 2
        %endrep
%endmacro

; Code helpers

; Virtual registers constants
%define _ax rax
%define _bx rbx
%define _cx rcx
%define _dx rdx
%define _si rsi
%define _di rdi

; Windows Library structures
%define BYTE_size 1
%define WORD_size 2
%define DWORD_size 4
%define QWORD_size 8
%idefine pointer qword
%define POINTER_size QWORD_size

%macro struct 3-*
    %1:
        %assign position 0
        %rotate 1
        %rep (%0 - 1) / 2
            .%1 equ position
            %assign position position + %2
            %rotate 2
        %endrep
        %xdefine %1_size position
%endmacro

%macro function 1-*
    %1:
        push rbp
        mov rbp, rsp

        %assign i 2
        %rotate 1
        %rep %0 - 1
            %xdefine %1 rbp + i * QWORD_size

            %if i == 2
                mov [%1], rcx
            %endif
            %if i == 3
                mov [%1], rdx
            %endif
            %if i == 4
                mov [%1], r8
            %endif
            %if i == 5
                mov [%1], r9
            %endif

            %assign i i + 1
            %rotate 1
        %endrep
%endmacro

%macro return 1
    mov rax, %1
    mov rsp, rbp
    pop rbp
    ret
%endmacro

%macro return 0
    mov rsp, rbp
    pop rbp
    ret
%endmacro

%macro local 2-*
    %assign size 0
    %rep %0 / 2
        %assign size size + %2
        %rotate 2
    %endrep

    sub rsp, (size + 15) & (~15)

    %assign position 0
    %rep %0 / 2
        %assign position position + %2
        %xdefine %1 rbp - position
        %rotate 2
    %endrep
%endmacro

%macro fastcall 1-*
    ; Count good arguments
    %assign arguments_count 0
    %rotate 1
    %rep %0 - 1
        %ifnstr %1
            %assign arguments_count arguments_count + 1
        %endif
        %rotate 1
    %endrep

    ; Reserve stack space 16 byte aligned
    %if arguments_count > 4
        sub rsp, ((arguments_count * QWORD_size) + 15) & (~15)
    %else
        sub rsp, 4 * QWORD_size
    %endif

    ; Loop over arguments
    %assign index %0 - 1
    %assign position arguments_count
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

                %if position == 1
                    lea rcx, [%1]
                %elif position == 2
                    lea rdx, [%1]
                %elif position == 3
                    lea r8, [%1]
                %elif position == 4
                    lea r9, [%1]
                %else
                    lea rcx, [%1]
                    mov [rsp + (position - 1) * QWORD_size], rcx
                %endif

                %rotate -1
                %assign index index - 2
                %assign isHandled 1
            %endif

            ; Float helper
            %if %1 == 'f'
                %rotate 1

                %if position == 1
                    movss xmm0, %1
                %elif position == 2
                    movss xmm1, %1
                %elif position == 3
                    movss xmm2, %1
                %elif position == 4
                    movss xmm3, %1
                %else
                    mov rcx, %1
                    mov [rsp + (position - 1) * QWORD_size], rcx
                %endif

                %rotate -1
                %assign index index - 2
                %assign isHandled 1
            %endif
        %endif

        ; Or just a normal value
        %if isHandled == 0
            %rotate 1

            %if position == 0
                call %1
            %elif position == 1
                mov rcx, %1
            %elif position == 2
                mov rdx, %1
            %elif position == 3
                mov r8, %1
            %elif position == 4
                mov r9, %1
            %else
                mov rcx, %1
                mov [rsp + (position - 1) * QWORD_size], rcx
            %endif

            %assign index index - 1
        %endif

        %assign position position - 1
    %endrep

    ; Free stack space 16 byte aligned
    %if arguments_count > 4
        add rsp, ((arguments_count * QWORD_size) + 15) & (~15)
    %else
        add rsp, 4 * QWORD_size
    %endif
%endmacro

%macro fcall 1+
    fastcall %1
%endmacro

%macro invoke 2+
    fastcall [%1], %2
%endmacro
%macro invoke 1
    fastcall [%1]
%endmacro

%macro cinvoke 2+
    fastcall [%1], %2
%endmacro
%macro cinvoke 1
    fastcall [%1]
%endmacro

; Include the shared libwindows stuff
%include "libwindows-shared.inc"
