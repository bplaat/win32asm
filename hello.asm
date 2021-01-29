    ; hello.asm - An pure 32-bit and 64-bit win32 assembly hello world program
    ; Made by Bastiaan van der Plaat (https://bastiaan.ml/)
    ; 32-bit: nasm -f bin hello.asm -o hello-x86.exe && ./hello-x86
    ; 64-bit: nasm -DWIN64 -f bin hello.asm -o hello-x64.exe && ./hello-x64

%ifdef WIN64
    %include "libwindows-x64.inc"
%else
    %include "libwindows-x86.inc"
%endif

header

code_section
    entrypoint
        frame

        invoke MessageBoxA, HWND_DESKTOP, message, message, MB_OK

        invoke ExitProcess, EXIT_SUCCESS

        end_frame
end_code_section

data_section
    %ifdef WIN64
        message db "Hello World! (64-bit)", 0
    %else
        message db "Hello World! (32-bit)", 0
    %endif

    import_table
        library kernel_table, "KERNEL32.DLL", \
            user_table, "USER32.DLL"

        import kernel_table, \
            ExitProcess, "ExitProcess"

        import user_table, \
            MessageBoxA, "MessageBoxA"
    end_import_table
end_data_section
