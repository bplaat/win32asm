# A simple shell script the assembles all programs

# Hello World! example
nasm -f bin hello.asm -o hello-x86.exe
nasm -DWIN64 -f bin hello.asm -o hello-x64.exe

# Console example
nasm -f bin console.asm -o console-x86.exe
nasm -DWIN64 -f bin console.asm -o console-x64.exe

# Socket example (not yet finished)
nasm -f bin socket.asm -o socket-x86.exe
nasm -DWIN64 -f bin socket.asm -o socket-x64.exe

# Window example
nasm -f bin window.asm -o window-x86.exe
nasm -DWIN64 -f bin window.asm -o window-x64.exe

# Graphics example
nasm -f bin graphics.asm -o graphics-x86.exe
nasm -DWIN64 -f bin graphics.asm -o graphics-x64.exe

# Controls example
nasm -f bin controls.asm -o controls-x86.exe
nasm -DWIN64 -f bin controls.asm -o controls-x64.exe
