# A simple shell script the assembles all programs

# Hello World! example
nasm -f bin hello.asm -o hello-x86.exe
nasm -DWIN64 -f bin hello.asm -o hello-x64.exe

# Window example
nasm -f bin window.asm -o window-x86.exe
nasm -DWIN64 -f bin window.asm -o window-x64.exe

# Graphics example
nasm -f bin graphics.asm -o graphics-x86.exe
nasm -DWIN64 -f bin graphics.asm -o graphics-x64.exe