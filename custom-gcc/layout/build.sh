# Assets
./layout-convert.py layout.xml layout.bin

# x86
gcc -I .. -m32 -mno-sse -Os -nostdlib layout.c -o layout-x86.s -S -masm=intel &&
../convert.py x86 layout-x86.s layout-x86.asm &&
nasm -f bin layout-x86.asm -o layout-x86.exe

windres -UWIN64 layout.rc -o layout-x86.res
ResourceHacker -open layout-x86.exe -save layout-x86.exe -action addskip -res layout-x86.res -log NUL

# x64
gcc -I .. -DWIN64 -Os -nostdlib layout.c -o layout-x64.s -S -masm=intel &&
../convert.py x64 layout-x64.s layout-x64.asm &&
nasm -DWIN64 -f bin layout-x64.asm -o layout-x64.exe

windres layout.rc -o layout-x64.res
ResourceHacker -open layout-x64.exe -save layout-x64.exe -action addskip -res layout-x64.res -log NUL

# Clean up
if [[ $1 != "keep" ]]; then
    rm layout-x86.s layout-x86.asm layout-x64.s layout-x64.asm *.res
fi

./layout-x64.exe
