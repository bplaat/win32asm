gcc -I . -m32 -Os -nostdlib $1.c -o $1-x86.s -S -masm=intel &&
./convert.py x86 $1-x86.s $1-x86.asm &&
nasm -f bin $1-x86.asm -o $1-x86.exe

gcc -I . -DWIN64 -Os -nostdlib $1.c -o $1-x64.s -S -masm=intel &&
./convert.py x64 $1-x64.s $1-x64.asm &&
nasm -DWIN64 -f bin $1-x64.asm -o $1-x64.exe

if [[ $2 != "keep" ]]; then
    rm $1-x86.s $1-x86.asm $1-x64.s $1-x64.asm
fi
