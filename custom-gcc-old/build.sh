# x86
gcc -I . -Wall -Wextra -Wpedantic -Werror --std=c99 \
    -m32 -mno-sse -Os -nostdlib $1.c -o $1-x86.s -S -masm=intel &&
./convert.py x86 $1-x86.s $1-x86.asm &&
nasm -f bin $1-x86.asm -o $1-x86.exe

# x64
gcc -I . -Wall -Wextra -Wpedantic -Werror --std=c99 \
    -DWIN64 -Os -nostdlib $1.c -o $1-x64.s -S -masm=intel &&
./convert.py x64 $1-x64.s $1-x64.asm &&
nasm -DWIN64 -f bin $1-x64.asm -o $1-x64.exe

# Clean up
if [[ $2 != "keep" ]]; then
    rm $1-x86.s $1-x86.asm $1-x64.s $1-x64.asm
fi

$1-x64.exe
