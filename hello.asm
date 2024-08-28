    ; hello.asm - An pure 32-bit and 64-bit win32 assembly hello world program
    ; Made by Bastiaan van der Plaat (https://bplaat.nl/)
    ; 32-bit: nasm -f bin hello.asm -o hello-x86.exe && ./hello-x86
    ; 64-bit: nasm -DWIN64 -f bin hello.asm -o hello-x64.exe && ./hello-x64

%include "libwindows.inc"

header

code_section
    entrypoint
        frame

        ; Show Hello World! messagebox
        invoke MessageBoxA, HWND_DESKTOP, message, message, MB_OK

        ; Exit successfully
        invoke ExitProcess, EXIT_SUCCESS

        end_frame
end_code_section

data_section
    ; String constants
    %ifdef WIN64
        message db "Hello World! (64-bit)", 0
    %else
        message db "Hello World! (32-bit)", 0
    %endif

    ; Import Table
    import_table
        library kernel_table, "KERNEL32.DLL", \
            user_table, "USER32.DLL"

        import kernel_table, \
            ExitProcess, "ExitProcess"

        import user_table, \
            MessageBoxA, "MessageBoxA"
    end_import_table
end_data_section
