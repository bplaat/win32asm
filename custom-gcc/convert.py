#!/bin/env python

import sys
import re

arch = sys.argv[1]
dp = arch == 'x64' and 'dq' or 'dd'

libraries = {
    'KERNEL32.DLL': [
        'GetModuleHandleA', 'ExitProcess', 'GetProcessHeap', 'HeapAlloc', 'HeapReAlloc', 'HeapFree', 'GetLocalTime'
    ],
    'USER32.DLL': [
        'MessageBoxA', 'PostQuitMessage', 'DefWindowProcA', 'LoadIconA', 'LoadCursorA', 'RegisterClassExA',
        'CreateWindowExA', 'ShowWindow', 'UpdateWindow', 'GetMessageA', 'TranslateMessage', 'DispatchMessageA', 'GetClientRect',
        'GetSystemMetrics', 'SetWindowPos', 'BeginPaint', 'EndPaint', 'FillRect', 'SetWindowLongA', 'GetWindowLongA'
    ],
    'GDI32.DLL': [
        'CreateCompatibleDC', 'CreateCompatibleBitmap', 'CreateSolidBrush', 'SelectObject', 'DeleteObject',
        'DeleteDC', 'BitBlt', 'CreateFontA', 'SetBkMode', 'SetTextColor', 'SetTextAlign', 'TextOutA'
    ]
}

registers = ['eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp', 'esp']
if arch == 'x64':
    registers.extend([
        'rax', 'rbx', 'rcx', 'rdx', 'rsi', 'rdi', 'rbp', 'rsp',
        'r8', 'r8d', 'r9', 'r9d', 'r10', 'r10d', 'r11', 'r11d',
        'r12', 'r12d', 'r13', 'r13d', 'r14', 'r14d', 'r15', 'r15d'
    ])

with open(sys.argv[2], 'r') as file:
    output = file.read()
    output = output.replace('\n\t', '\n    ')
    output = output.replace('\t', ' ')

    output = re.sub(r'\s*\.file.*\n', '\n', output)
    output = re.sub(r'\s*\.intel_syntax.*\n', '\n', output)
    output = re.sub(r'\s*\.ident.*\n', '\n', output)

    output = re.sub(r'\s*\.align (.+)\n', '\n    align \\1, db 0\n', output)
    output = re.sub(r'\s*\.space (.+)\n', '\n    times \\1 db 0\n', output)
    output = re.sub(r'\:\n    \.long (.+)\n', ' dd \\1\n', output)
    output = output.replace('.ascii', 'db')
    output = output.replace(' PTR ', ' ')
    output = re.sub(r' \[DWORD (.+)\]\n', ' DWORD \\1\n', output)
    output = output.replace(' OFFSET FLAT:', ' ')
    output = output.replace('\\0"', '", 0')

    if arch == 'x64':
        output = re.sub(r'\s*\.seh_.*\n', '\n', output)
        output = re.sub(r'\:\n    \.quad (.+)\n', ' dq \\1\n', output)
        output = re.sub(r'\.LC([0-9]+)', 'LC\\1', output)
        output = re.sub(r' \[QWORD (.+)\]\n', ' QWORD \\1\n', output)
        output = output.replace('movabs r', 'mov r')
        output = re.sub(r'([a-zA-Z_][a-zA-Z0-9_]*)\[rip\]', '[rel \\1]', output)

    for register in registers:
        output = output.replace('shl ' + register + '\n', 'shl ' + register + ', 1\n')
        output = output.replace('shr ' + register + '\n', 'shr ' + register + ', 1\n')

    data = ''
    text = ''
    isText = True
    symbols = []
    imports = []

    for line in output.split('\n'):
        strippedLine = line.strip()
        if strippedLine == '.text':
            isText = True
        elif (
            strippedLine == '.data' or
            strippedLine == '.bss' or
            strippedLine.startswith('.section .rdata')
        ):
            isText = False
        elif strippedLine.startswith('.globl'):
            name = strippedLine.split(' ')[1]
            symbols.append(name)
        elif strippedLine.startswith('.def'):
            name = strippedLine.split(';')[0].split(' ')[1]
            if not (name in symbols):
                imports.append(name)
        else:
            if isText:
                text += line + '\n'
            else:
                data += line + '\n'

    for symbol in sorted(symbols, key=len, reverse=True):
        text = text.replace('DWORD ' + symbol, 'DWORD [' + symbol + ']')

    def symbolRealName(name):
        if arch == 'x86':
            return name[1:].split('@')[0]
        else:
            return name

    tables = {}
    for name in imports:
        text = text.replace('jmp ' + name + '\n', 'jmp [' + name + ']\n')
        text = text.replace('call ' + name + '\n', 'call [' + name + ']\n')

        found = False
        realName = symbolRealName(name)
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
        importTable += '    dd 0, 0, 0, _%s - _base, %s - _base\n' % (table, table)
    importTable += '    dd 0, 0, 0, 0, 0\n'
    for table in tables:
        importTable += '    _%s db \'%s\', 0\n' % (table, table)

    for table, funcs in tables.items():
        funcs = sorted(funcs)
        importTable += '\n%s:\n' % (table)
        for func in funcs:
            importTable += '    %s %s _%s - _base\n' % (func, dp, func)
        importTable += '    %s 0\n' % (dp)
        for func in funcs:
            importTable += '    _%s db 0, 0, \'%s\', 0\n' % (func, symbolRealName(func))

    with open (sys.argv[3], 'w') as outFile:
        outFile.write("""[bits %d]

_base equ 0x400000
_alignment equ 0x200
[org _base]

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
    dw %s                         ; Machine
    dw _sections_count            ; NumberOfSections
    dd __?POSIX_TIME?__           ; TimeDateStamp
    dd 0                          ; PointerToSymbolTable
    dd 0                          ; NumberOfSymbols
    dw _optional_header_size      ; SizeOfOptionalHeader
    dw 0x030F                     ; Characteristics

    ; Standard COFF Fields
_optional_header:
    dw %s                         ; Magic
    db 0                          ; MajorLinkerVersion
    db 0                          ; MinorLinkerVersion
    dd _text_section_aligned_size ; SizeOfCode
    dd _data_section_aligned_size ; SizeOfInitializedData
    dd 0                          ; SizeOfUninitializedData
    dd %s - _base                 ; AddressOfEntryPoint
    dd _text_section - _base      ; BaseOfCode
    %s

    ; Windows-Specific Fields
    %s _base                      ; ImageBase
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
    %s 0x100000                   ; SizeOfStackReserve
    %s 0x1000                     ; SizeOfStackCommit
    %s 0x100000                   ; SizeOfHeapReserve
    %s 0x1000                     ; SizeOfHeapCommit
    dd 0                          ; LoaderFlags
    dd 16                         ; NumberOfRvaAndSizes

    ; Data Directories
    dd 0, 0
    dd _import_table - _base, _import_table_size
    times 14 dd 0, 0

    _optional_header_size equ $ - _optional_header

    ; Section Table
_sections:
    ; Text Section
    db ".text", 0, 0, 0           ; Name
    dd _text_section_size         ; VirtualSize
    dd _text_section - _base      ; VirtualAddress
    dd _text_section_aligned_size ; SizeOfRawData
    dd _text_section - _base      ; PointerToRawData
    dd 0                          ; PointerToRelocations
    dd 0                          ; PointerToLinenumbers
    dw 0                          ; NumberOfRelocations
    dw 0                          ; NumberOfLinenumbers
    dd 0x60000020                 ; Characteristics

    ; Data Section
    db ".data", 0, 0, 0           ; Name
    dd _data_section_size         ; VirtualSize
    dd _data_section - _base      ; VirtualAddress
    dd _data_section_aligned_size ; SizeOfRawData
    dd _data_section - _base      ; PointerToRawData
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
""" % (
    arch == 'x64' and 64 or 32,
    (arch == 'x64' and '0x8664' or '0x014C'),
    (arch == 'x64' and '0x020B' or '0x010B'),
    (arch == 'x64' and '_start' or '__start'),
    (arch != 'x64' and 'dd _data_section - _base ; BaseOfData' or ''),
    dp, dp, dp, dp, dp,
    text, data, importTable)
)
