    ; libwindows-x86.inc - A 32-bit win32 assembly helper library
    ; Made by Bastiaan van der Plaat (https://bastiaan.ml/)

    ; PE header
    _base equ 0x400000
    _alignment equ 0x200

    [BITS 32]
    [ORG _base]

%define RVA(address) (address - _base)

    ; DOS Stub
    db 0x4D, 0x5A, 0x90, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00
    db 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00
    db 0x0E, 0x1F, 0xBA, 0x0E, 0x00, 0xB4, 0x09, 0xCD, 0x21, 0xB8, 0x01, 0x4C, 0xCD, 0x21, 0x54, 0x68
    db 0x69, 0x73, 0x20, 0x70, 0x72, 0x6F, 0x67, 0x72, 0x61, 0x6D, 0x20, 0x63, 0x61, 0x6E, 0x6E, 0x6F
    db 0x74, 0x20, 0x62, 0x65, 0x20, 0x72, 0x75, 0x6E, 0x20, 0x69, 0x6E, 0x20, 0x44, 0x4F, 0x53, 0x20
    db 0x6D, 0x6F, 0x64, 0x65, 0x2E, 0x0D, 0x0D, 0x0A, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

    db "PE", 0, 0                 ; Signature
    dw 0x014C                     ; Machine
    dw 2                          ; NumberOfSections
    dd 0                          ; TimeDateStamp
    dd 0                          ; PointerToSymbolTable
    dd 0                          ; NumberOfSymbols
    dw 224                        ; SizeOfOptionalHeader
    dw 0x030F                     ; Characteristics

    dw 0x010B                     ; Magic
    db 0                          ; MajorLinkerVersion
    db 0                          ; MinorLinkerVersion
    dd _code_section_aligned_size ; SizeOfCode
    dd _data_section_aligned_size ; SizeOfInitializedData
    dd 0                          ; SizeOfUninitializedData
    dd RVA(_entrypoint)           ; AddressOfEntryPoint
    dd RVA(_code_section)         ; BaseOfCode
    dd RVA(_data_section)         ; BaseOfData

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
    dd _file_size                 ; SizeOfImage
    dd _header_aligned_size       ; SizeOfHeaders
    dd 0                          ; CheckSum
    dw 2                          ; Subsystem
    dw 0                          ; DllCharacteristics
    dd 0x100000                   ; SizeOfStackReserve
    dd 0x1000                     ; SizeOfStackCommit
    dd 0x100000                   ; SizeOfHeapReserve
    dd 0x1000                     ; SizeOfHeapCommit
    dd 0                          ; LoaderFlags
    dd 16                         ; NumberOfRvaAndSizes

    dd 0, 0
    dd RVA(_import_table), _import_table_size
    times 14 dd 0, 0

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

_header_size equ $ - $$
    align _alignment, db 0
_header_aligned_size equ $ - $$

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

    _file_size equ $ - $$
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
        ._arguments_size equ (%0 - 1) * 4

        push ebp
        mov ebp, esp

        %assign i 2
        %rotate 1
        %rep %0 - 1
            %xdefine %1 ebp + i * 4
            %assign i i + 1
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

%macro push_reverse 1-*
    %rep %0
        %rotate -1
        push dword %1
    %endrep
%endmacro

%macro fcall 2+
    push_reverse %2
    call %1
%endmacro

%macro fcall 1
    call %1
%endmacro

%macro invoke 2+
    fcall [%1], %2
%endmacro

%macro invoke 1
    fcall [%1]
%endmacro

; Virtual registers constants
%define _ax eax
%define _bx ebx
%define _cx ecx
%define _dx edx
%define _si esi
%define _di edi

; Windows Library sizes
%define WORD_size 2
%define WORD_size_word word
%define DWORD_size 4
%define DWORD_size_word dword
%define POINTER_size 4
%define POINTER_size_word dword

; Include the shared libwindows stuff
%include "libwindows-shared.inc"
