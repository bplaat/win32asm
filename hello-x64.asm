    ; hello-x64.asm - A 64-bit win32 assembly hello world program
    ; Made by Bastiaan van der Plaat (https://bastiaan.ml/)
    ; nasm -f bin hello-x64.asm -o hello-x64.exe && ./hello-x64

%include "libwindows-x64.inc"

code_section
    _start:
        invoke MessageBoxA, HWND_DESKTOP, message, message, MB_OK
        invoke ExitProcess, 0
end_code_section

data_section
    message db "Hello World! (64-bit)", 0

    import_table
        library kernel_table, "KERNEL32.DLL", \
            user_table, "USER32.DLL"

        import kernel_table, \
            ExitProcess, "ExitProcess"

        import user_table, \
            MessageBoxA, "MessageBoxA"
    end_import_table
end_data_section
