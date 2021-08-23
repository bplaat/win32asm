#!/bin/env python

import os
import re
import shutil
import sys

path = '.'
conf = 'debug'
arch = 'x64'
libs = [ 'win32' ]
for arg in sys.argv[1:]:
    if arg.startswith('--path='):
        path = arg[len('--path='):]

    if arg.startswith('--arch='):
        arch = arg[len('--arch='):]
    if arg == '--x86':
        arch = 'x86'

    if arg.startswith('--conf='):
        conf = arg[len('--conf='):]
    if arg == '--release':
        conf = 'release'

    if arg.startswith('--lib='):
        libs.append(arg[len('--lib='):])
    if arg == '--dpi':
        libs.append('dpi')
    if arg == '--henk':
        libs.append('henk')

    if arg == '--clean':
        shutil.rmtree(path + '/target', ignore_errors = True)
        exit(0)

project_name = os.path.basename(os.path.realpath(path))
script_folder = os.path.dirname(os.path.realpath(__file__))
if arch != 'x86' and arch != 'x64':
    print('[ERROR] Arch must be: x86 or x64')
    exit(1)
if conf != 'debug' and conf != 'release':
    print('[ERROR] Conf must be: debug or release')
    exit(1)

# Assemble c files
if not os.path.isdir(path + '/src/'):
    print('[ERROR] No src folder found')
    exit(1)

os.makedirs(path + '/target/' + arch + '/' + conf + '/', exist_ok=True)

assembly_files = []
source_files = []
for file in os.listdir(path + '/src'):
    source_files.append(path + '/src/' + file)
for file in libs:
    source_files.append(script_folder + '/src/' + file + '.c')
for file in source_files:
    if file.endswith('.c'):
        file = file[:-2]
        filename = os.path.basename(file)
        if conf == 'release':
            if arch == 'x64':
                if os.system('gcc -Iinclude -I"' + script_folder + '/include" -Os -nostdlib -DWIN64 ' + file + '.c -o ' + path + '/target/x64/release/' + filename + '.s -S -masm=intel') != 0:
                    exit(1)
            else:
                if os.system('gcc -Iinclude -I"' + script_folder + '/include" -Os -nostdlib -m32 -mno-sse ' + file + '.c -o ' + path + '/target/x86/release/' + filename + '.s -S -masm=intel') != 0:
                    exit(1)
        else:
            if arch == 'x64':
                if os.system('gcc -Iinclude -I"' + script_folder + '/include" -Wall -Wextra -Wpedantic -Werror --std=c99 -nostdlib -DWIN64 -DDEBUG ' + file + '.c -o ' + path + '/target/x64/debug/' + filename + '.s -S -masm=intel') != 0:
                    exit(1)
            else:
                if os.system('gcc -Iinclude -I"' + script_folder + '/include" -Wall -Wextra -Wpedantic -Werror --std=c99 -nostdlib -DDEBUG -m32 -mno-sse ' + file + '.c -o ' + path + '/target/x86/debug/' + filename + '.s -S -masm=intel') != 0:
                    exit(1)
        assembly_files.append(path + '/target/' + arch + '/' + conf + '/' + filename + '.s')

if len(assembly_files) == 0:
    print('[ERROR] No C source files found')
    shutil.rmtree(path + '/target')
    exit(1)

# Link assembly files
libraries = {
    'KERNEL32.DLL': [
        'CloseHandle', 'CreateFileW', 'CreateMutexW', 'ExitProcess', 'FindClose', 'FindFirstFileW', 'FindNextFileW', 'FindResourceW',
        'GetFileSize', 'GetFullPathNameW', 'GetLastError', 'GetLocalTime', 'GetModuleHandleW', 'GetProcAddress', 'GetProcessHeap',
        'GetStdHandle', 'GetVersionExW', 'HeapAlloc', 'HeapFree', 'HeapReAlloc', 'LoadLibraryW', 'LoadResource', 'LockResource',
        'MulDiv', 'MultiByteToWideChar', 'ReadFile', 'ReleaseMutex', 'SetFilePointer', 'SetThreadLocale', 'SetThreadUILanguage',
        'SizeofResource', 'Sleep', 'WideCharToMultiByte', 'WriteConsoleW', 'WriteFile'
    ],
    'USER32.DLL': [
        'AdjustWindowRectEx', 'BeginPaint', 'CreateWindowExW', 'DefWindowProcW', 'DestroyWindow', 'DispatchMessageW', 'DrawTextW',
        'EndPaint', 'EnumChildWindows', 'FillRect', 'FindWindowW', 'FrameRect', 'GetClientRect', 'GetDC', 'GetDlgItem', 'GetMessageW',
        'GetMonitorInfoW', 'GetSystemMenu', 'GetSystemMetrics', 'GetWindowPlacement', 'GetWindowRect', 'GetWindowTextW', 'InsertMenuW',
        'InvalidateRect', 'IsIconic', 'KillTimer', 'LoadAcceleratorsW', 'LoadBitmapW', 'LoadCursorW', 'LoadIconW', 'LoadImageW',
        'LoadStringW', 'MessageBeep', 'MessageBoxW', 'MonitorFromWindow', 'PeekMessageW', 'PostQuitMessage', 'RegisterClassExW',
        'ReleaseCapture', 'SendMessageW', 'SetCapture', 'SetForegroundWindow', 'SetMenu', 'SetTimer', 'SetWindowPos',
        'SetWindowTextW', 'ShowWindow', 'TranslateAcceleratorW', 'TranslateMessage', 'UpdateWindow', 'wsprintfW', 'wvsprintfW'
    ],
    'GDI32.DLL': [
        'BeginPath', 'BitBlt', 'CloseFigure', 'CreateBitmap', 'CreateCompatibleBitmap', 'CreateCompatibleDC', 'CreateFontW', 'CreatePen',
        'CreateRectRgn', 'CreateSolidBrush', 'DeleteDC', 'DeleteObject', 'EndPath', 'ExtTextOutW', 'FillPath', 'GdiAlphaBlend',
        'GetDeviceCaps', 'GetStockObject', 'GetTextExtentPoint32W', 'LineTo', 'MoveToEx', 'Rectangle', 'SelectClipRgn', 'SelectObject',
        'SetBkMode', 'SetStretchBltMode', 'SetTextAlign', 'SetTextColor', 'StretchBlt', 'StrokePath', 'StrokeAndFillPath', 'TextOutW'
    ],
    'SHELL32.DLL': [
        'DragFinish', 'DragQueryFileW', 'SHGetFolderPathW', 'ShellExecuteW'
    ],
    'COMCTL32.DLL': [
        'InitCommonControlsEx'
    ],
    'COMDLG32.DLL': [
        'GetOpenFileNameW', 'GetSaveFileNameW'
    ],
    'ADVAPI32.DLL': [
        'GetUserNameW'
    ],
    'WINMM.DLL': [
        'PlaySoundW'
    ],
    'gdiplus.dll': [
        'GdipCreateFromHDC', 'GdipCreatePen1', 'GdipCreateSolidFill', 'GdipDeleteBrush', 'GdipDeleteGraphics', 'GdipDeletePen',
        'GdipDrawRectangle', 'GdipDrawRectangleI', 'GdipFillRectangle', 'GdipFillRectangleI', 'GdipGraphicsClear', 'GdipSetSmoothingMode',
        'GdiplusShutdown', 'GdiplusStartup'
    ],
    'dwmapi.dll': [
        'DwmExtendFrameIntoClientArea'
    ],
    'd2d1.dll': [
        'D2D1CreateFactory'
    ],
    'dwrite.dll': [
        'DWriteCreateFactory'
    ]
}

if arch == 'x64':
    libraries['USER32.DLL'].extend(['SetWindowLongPtrW', 'GetWindowLongPtrW', 'SetWindowLongPtrW', 'GetWindowLongPtrW'])
else:
    libraries['USER32.DLL'].extend(['SetWindowLongW', 'GetWindowLongW', 'SetWindowLongW', 'GetWindowLongW'])

registers = ['eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp', 'esp']
if arch == 'x64':
    registers.extend([
        'rax', 'rbx', 'rcx', 'rdx', 'rsi', 'rdi', 'rbp', 'rsp',
        'r8', 'r8d', 'r9', 'r9d', 'r10', 'r10d', 'r11', 'r11d',
        'r12', 'r12d', 'r13', 'r13d', 'r14', 'r14d', 'r15', 'r15d'
    ])

text = ''
data = ''
symbols = []
imports = []

# Process GNU assembly style files
for filename in assembly_files:
    modulename = os.path.splitext(os.path.basename(filename))[0]
    with open(filename, 'r') as file:
        output = file.read()
        output = output.replace('\n\t', '\n    ')
        output = output.replace('\t', ' ')

        output = re.sub(r'\s*\.file.*\n', '\n', output)
        output = re.sub(r'\s*\.intel_syntax.*\n', '\n', output)
        output = re.sub(r'\s*\.ident.*\n', '\n', output)

        output = re.sub(r'\s*\.align (.+)\n', '\n    align \\1, db 0\n', output)
        output = re.sub(r'\s*\.space (.+)\n', '\n    times \\1 db 0\n', output)
        output = output.replace('.ascii', 'db')
        output = output.replace('.word', 'dw')
        output = output.replace('.long', 'dd')
        output = output.replace('.quad', 'dq')
        output = output.replace(' PTR ', ' ')
        output = re.sub(r' \[DWORD (.+)\]\n', ' DWORD \\1\n', output)
        output = output.replace(' OFFSET FLAT:', ' ')
        output = output.replace('`', '\`')
        output = output.replace('"', '`')

        output = re.sub(r'DWORD LC(.+)\n', 'DWORD [LC\\1]\n', output)
        output = re.sub(r'TBYTE \[(.+)\]\n', 'TWORD [\\1]\n', output)
        output = re.sub(r'TBYTE LC(.+)\n', 'TWORD [LC\\1]\n', output)
        output = re.sub(r'st\((\d+)\)', 'st\\1', output)
        output = output.replace('st,', 'st0,')
        output = output.replace('st\n', 'st0\n')

        if arch == 'x64':
            output = re.sub(r'\s*\.linkonce.*\n', '\n', output)
            output = re.sub(r'\s*\.linkonce.*\n', '\n', output)
            output = re.sub(r'\s*\.seh_.*\n', '\n', output)
            output = re.sub(r'\.LC([0-9]+)', 'LC\\1', output)
            output = re.sub(r' \[QWORD (.+)\]\n', ' QWORD \\1\n', output)
            output = output.replace('movabs r', 'mov r')
            output = output.replace('.refptr.', '')
            output = re.sub(r'([a-zA-Z_][a-zA-Z0-9_]*)\[rip\]', '[rel \\1]', output)
            output = output.replace(' XMMWORD ', ' OWORD ')
            output = output.replace('rex.W ', ' ')

        output = re.sub(r' L([0-9]+)', ' ' + modulename + '_L\\1', output)
        output = re.sub(r'\[L([0-9]+)', '[' + modulename + '_L\\1', output)
        output = re.sub(r'\nL([0-9]+)', '\n' + modulename + '_L\\1', output)

        output = re.sub(r' LC([0-9]+)', ' ' + modulename + '_LC\\1', output)
        output = re.sub(r'\[LC([0-9]+)', '[' + modulename + '_LC\\1', output)
        output = re.sub(r'\nLC([0-9]+)', '\n' + modulename + '_LC\\1', output)

        for register in registers:
            output = output.replace('shl ' + register + '\n', 'shl ' + register + ', 1\n')
            output = output.replace('shr ' + register + '\n', 'shr ' + register + ', 1\n')
            output = output.replace('sar ' + register + '\n', 'sar ' + register + ', 1\n')

        isText = True
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
                if name in imports:
                    imports.remove(name)
            elif strippedLine.startswith('.def'):
                name = strippedLine.split(';')[0].split(' ')[1]
                if not (name in symbols or name in imports):
                    imports.append(name)
            else:
                if isText:
                    text += line + '\n'
                else:
                    data += line + '\n'

for symbol in sorted(symbols, key=len, reverse=True):
    text = re.sub(r'DWORD ' + symbol + '([^,\n]*)', 'DWORD [' + symbol + '\\1]', text)

def symbolRealName(name):
    if arch == 'x64':
        return name
    else:
        return name[1:].split('@')[0]

# Generate import table info
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

# Generate import table assembly
importTable = ''
for table in sortedTables:
    importTable += '    dd 0, 0, 0, _%s - _base, %s - _base\n' % (table, table)
importTable += '    dd 0, 0, 0, 0, 0\n'
for table in sortedTables:
    importTable += '    _%s db \'%s\', 0\n' % (table, table)

dp = arch == 'x64' and 'dq' or 'dd'
for table, funcs in tables.items():
    funcs = sorted(funcs)
    importTable += '\n%s:\n' % (table)
    for func in funcs:
        importTable += '    %s %s _%s - _base\n' % (func, dp, func)
    importTable += '    %s 0\n' % (dp)
    for func in funcs:
        importTable += '    _%s db 0, 0, \'%s\', 0\n' % (func, symbolRealName(func))

# Write final linked assembly file with PE headers for NASM
with open(path + '/target/' + arch + '/' + conf + '/' + project_name + '.asm', 'w') as file:
    file.write("""[bits %d]

_base equ 0x400000
_alignment equ 0x1000
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
    dw %d                         ; Subsystem
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

    _sections_count equ ($ - _sections) / 40

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
    dp, conf == 'release' and 2 or 3, dp, dp, dp, dp,
    text, data, importTable)
)

# Assemble file with NASM
os.system('nasm -f bin ' + path + '/target/' + arch + '/' + conf + '/' + project_name + '.asm -o ' + path + '/target/' + arch + '/' + conf + '/' + project_name + '.exe')

# Clean up if release build
if conf == 'release':
    for file in assembly_files:
        os.remove(file)
    os.remove(path + '/target/' + arch + '/' + conf + '/' + project_name + '.asm')

# Zip final folder if release build
if conf == 'release':
    os.chdir(path + '/target/' + arch)
    zip_name = project_name + '-' + arch
    os.system('mv release ' + zip_name)
    os.system('7z a ' + zip_name + '.zip ' + zip_name)
    os.system('mv ' + zip_name + ' release')

# Run executable when debug
if conf == 'debug':
    os.chdir(path + '/target/' + arch + '/' + conf)
    os.system(project_name + '.exe')
