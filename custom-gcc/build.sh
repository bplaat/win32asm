gcc -Os -nostdlib $1.c -m32 -o $1.s -S -masm=intel
./convert.py $1.s $1.asm
nasm -f bin $1.asm -o $1.exe
