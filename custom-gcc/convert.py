#!/bin/env python

import sys
import re

libraries = {
    'KERNEL32.DLL': ['GetModuleHandleA', 'ExitProcess'],
    'USER32.DLL': [
        'MessageBoxA', 'PostQuitMessage', 'DefWindowProcA', 'LoadIconA', 'LoadCursorA', 'RegisterClassExA',
        'CreateWindowExA', 'ShowWindow', 'UpdateWindow', 'GetMessageA', 'TranslateMessage', 'DispatchMessageA', 'GetClientRect',
        'GetSystemMetrics', 'SetWindowPos', 'BeginPaint', 'EndPaint', 'FillRect'
    ],
    'GDI32.DLL': ['CreateCompatibleDC', 'CreateCompatibleBitmap', 'CreateSolidBrush', 'SelectObject', 'DeleteObject', 'DeleteDC', 'BitBlt']
}

with open(sys.argv[1], 'r') as file:
    output = file.read()

    output = re.sub(r'\s*\.file.*\n', '\n', output)
    output = re.sub(r'\s*\.intel_syntax.*\n', '\n', output)
    output = re.sub(r'\s*\.ident.*\n', '\n', output)

    output = re.sub(r'\s*\.align (.*)\n', '\nalign \\1, db 0\n', output)
    output = re.sub(r'\:\n\t.long\t(.+)\n', ' dd \\1\n', output)
    output = output.replace('.ascii', 'db')
    output = output.replace(' PTR ', ' ')
    output = output.replace(' OFFSET FLAT:', ' ')
    output = output.replace('\\0"', '", 0')
    output = output.replace('shr\teax\n', 'shr eax, 1\n')
    output = output.replace('shr\tebx\n', 'shr ebx, 1\n')
    output = output.replace('shr\tecx\n', 'shr ecx, 1\n')
    output = output.replace('shr\tedx\n', 'shr edx, 1\n')
    output = output.replace('shr\tesi\n', 'shr esi, 1\n')
    output = output.replace('shr\tedi\n', 'shr edi, 1\n')

    data = ''
    text = ''
    isText = True
    symbols = []
    imports = []

    for line in output.split('\n'):
        strippedLine = line.strip()
        if strippedLine == '.text':
            isText = True
        elif strippedLine == '.data' or strippedLine.startswith('.section .rdata'):
            isText = False
        elif strippedLine.startswith('.globl'):
            name = strippedLine.split('\t')[1]
            symbols.append(name)
        elif strippedLine.startswith('.def'):
            name = strippedLine.split(';')[0].split('\t')[1]
            if not (name in symbols):
                imports.append(name)
        else:
            if isText:
                text += line + '\n'
            else:
                data += line + '\n'

    for symbol in symbols:
        text = text.replace('DWORD ' + symbol, 'DWORD [' + symbol + ']')

    tables = {}
    for name in imports:
        realName = name[1:].split('@')[0]

        text = text.replace('\tjmp\t' + name + '\n', '\tjmp\t[' + name + ']\n')
        text = text.replace('\tcall\t' + name + '\n', '\tcall\t[' + name + ']\n')

        found = False
        for library, funcs in libraries.items():
            for func in funcs:
                if func == realName:
                    if not (library in tables):
                        tables[library] = []
                    tables[library].append(name)
                    found = True
                    break
            if found:
                break

    sortedTables = {}
    for table in sorted(tables):
        sortedTables[table] = tables[table]
    tables = sortedTables

    importTable = ''
    for table in tables:
        importTable += '    dd 0, 0, 0, RVA(_%s), RVA(%s)\n' % (table, table)
    importTable += '    dd 0, 0, 0, 0, 0\n'
    for table in tables:
        importTable += '    _%s db \'%s\', 0\n' % (table, table)

    for table, funcs in tables.items():
        funcs = sorted(funcs)
        importTable += '\n%s:\n' % (table)
        for func in funcs:
            importTable += '    %s dd RVA(_%s)\n' % (func, func)
        importTable += '    dd 0\n'
        for func in funcs:
            realName = func[1:].split('@')[0]
            importTable += '    _%s db 0, 0, \'%s\', 0\n' % (func, realName)

    with open (sys.argv[2], 'w') as outFile:
        outFile.write("""[bits 32]
%%define _base 0x400000
%%define _alignment 0x200
[org _base]
%%define RVA(_address) (_address - _base)

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

    db "PE", 0, 0                 ; Signature
    dw 0x014C                     ; Machine
    dw _sections_count            ; NumberOfSections
    dd __?POSIX_TIME?__           ; TimeDateStamp
    dd 0                          ; PointerToSymbolTable
    dd 0                          ; NumberOfSymbols
    dw _optional_header_size      ; SizeOfOptionalHeader
    dw 0x030F                     ; Characteristics

    ; Standard COFF Fields
_optional_header:
    dw 0x010B                     ; Magic
    db 0                          ; MajorLinkerVersion
    db 0                          ; MinorLinkerVersion
    dd _text_section_aligned_size ; SizeOfCode
    dd _data_section_aligned_size ; SizeOfInitializedData
    dd 0                          ; SizeOfUninitializedData
    dd RVA(__start)               ; AddressOfEntryPoint
    dd RVA(_text_section)         ; BaseOfCode
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
    dd _header_aligned_size + _text_section_aligned_size + _data_section_aligned_size ; SizeOfImage
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

    ; Data Directories
    dd 0, 0
    dd RVA(_import_table), _import_table_size
    times 14 dd 0, 0

    _optional_header_size equ $ - _optional_header

    ; Section Table
_sections:
    ; Text Section
    db ".text", 0, 0, 0           ; Name
    dd _text_section_size         ; VirtualSize
    dd RVA(_text_section)         ; VirtualAddress
    dd _text_section_aligned_size ; SizeOfRawData
    dd RVA(_text_section)         ; PointerToRawData
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

    _sections_count equ ($ - _sections) / (8 + 4 * 6 + 2 * 2 + 4)

_header_size equ $ - _header
    align _alignment, db 0
_header_aligned_size equ $ - _header

_text_section:
%s_text_section_size equ $ - _text_section
    align _alignment, db 0
_text_section_aligned_size equ $ - _text_section

_data_section:
%s_import_table:
%s
_import_table_size equ $ - _import_table

_data_section_size equ $ - _data_section
    align _alignment, db 0
_data_section_aligned_size equ $ - _data_section
""" % (text, data, importTable))