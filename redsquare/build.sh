# RedSquare game build script

# Assets
magick convert icon.png -define icon:auto-resize="16,32,48,256" icon.ico
magick convert bassiebas.png -colors 16 -define bmp:format=bmp3 bassiebas.bmp

# x86
nasm -f bin redsquare.asm -o redsquare-x86.exe
windres -UWIN64 redsquare.rc -o redsquare-x86.res
ResourceHacker -open redsquare-x86.exe -save redsquare-x86.exe -action addskip -res redsquare-x86.res -log NUL

# x64
nasm -DWIN64 -f bin redsquare.asm -o redsquare-x64.exe
windres redsquare.rc -o redsquare-x64.res
ResourceHacker -open redsquare-x64.exe -save redsquare-x64.exe -action addskip -res redsquare-x64.res -log NUL

# Clean up
rm redsquare-x86.res redsquare-x64.res icon.ico bassiebas.bmp

./redsquare-x64
