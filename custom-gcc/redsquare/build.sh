# x86
gcc -I .. -m32 -mno-sse -Os -nostdlib redsquare.c -o redsquare-x86.s -S -masm=intel &&
../convert.py x86 redsquare-x86.s redsquare-x86.asm &&
nasm -f bin redsquare-x86.asm -o redsquare-x86.exe

windres -UWIN64 redsquare.rc -o redsquare-x86.res
ResourceHacker -open redsquare-x86.exe -save redsquare-x86.exe -action addskip -res redsquare-x86.res -log NUL

if [[ $2 != "keep" ]]; then
    rm redsquare-x86.s redsquare-x86.asm redsquare-x86.res
fi

# x64
gcc -I .. -DWIN64 -Os -nostdlib redsquare.c -o redsquare-x64.s -S -masm=intel &&
../convert.py x64 redsquare-x64.s redsquare-x64.asm &&
nasm -DWIN64 -f bin redsquare-x64.asm -o redsquare-x64.exe

windres redsquare.rc -o redsquare-x64.res
ResourceHacker -open redsquare-x64.exe -save redsquare-x64.exe -action addskip -res redsquare-x64.res -log NUL

if [[ $2 != "keep" ]]; then
    rm redsquare-x64.s redsquare-x64.asm redsquare-x64.res
fi

./redsquare-x64.exe
