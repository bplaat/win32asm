    ; console.asm - An pure 32-bit and 64-bit win32 console program
    ; Made by Bastiaan van der Plaat (https://bastiaan.ml/)
    ; 32-bit: nasm -f bin console.asm -o console-x86.exe && ./console-x86
    ; 64-bit: nasm -DWIN64 -f bin console.asm -o console-x64.exe && ./console-x64

%include "libwindows.inc"

header HEADER_CONSOLE

code_section
    entrypoint
        %define name_buffer_size 64

        local console_out, POINTER_size, \
            console_in, POINTER_size, \
            name_buffer, name_buffer_size, \
            name_bytes_read, DWORD_size, \
            answer_buffer, 64

        ; Print question string
        invoke GetStdHandle, STD_OUTPUT_HANDLE
        mov [console_out], _ax
        invoke WriteConsoleA, [console_out], question, question_size, NULL, 0

        ; Read name answer string
        invoke GetStdHandle, STD_INPUT_HANDLE
        mov [console_in], _ax
        invoke ReadConsoleA, [console_in], addr name_buffer, name_buffer_size, addr name_bytes_read, NULL

        ; Cut trailing CRLF enter characters
        mov eax, [name_bytes_read]
        mov byte [name_buffer + _ax - 2], 0

        ; Check if name is empty if so replace with a question mark
        mov al, [name_buffer]
        test al, al
        jne .skip
        mov byte [name_buffer], '?'
        mov byte [name_buffer + 1], 0
    .skip:

        ; Print formated answer string
        cinvoke wsprintfA, addr answer_buffer, answer, addr name_buffer
        invoke WriteConsoleA, [console_out], addr answer_buffer, _ax, NULL, 0

        ; Exit successfull
        invoke ExitProcess, EXIT_SUCCESS

    end_local
end_code_section

data_section
    ; String constants
    question db "Hello, what is your name: "
    question_size equ $ - question
    answer db "Hello %s, it is nice to meet you!", 13, 10, 0

    ; Import table
    import_table
        library kernel_table, "KERNEL32.DLL", \
            user_table, "USER32.DLL"

        import kernel_table, \
            ExitProcess, "ExitProcess", \
            GetStdHandle, "GetStdHandle", \
            ReadConsoleA, "ReadConsoleA", \
            WriteConsoleA, "WriteConsoleA"

        import user_table, \
            wsprintfA, "wsprintfA"
    end_import_table
end_data_section
