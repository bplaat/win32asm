# RedSquare game
magick convert icon.png -define icon:auto-resize="16,32,48,256" icon.ico

ResourceHacker -open redsquare-x86.rc -save redsquare-x86.res -action compile -log NUL
nasm -f bin redsquare.asm -o redsquare-x86.exe
ResourceHacker -open redsquare-x86.exe -save redsquare-x86.exe -action add -res redsquare-x86.res -log NUL
rm -f redsquare-x86.res

ResourceHacker -open redsquare-x64.rc -save redsquare-x64.res -action compile -log NUL
nasm -DWIN64 -f bin redsquare.asm -o redsquare-x64.exe
ResourceHacker -open redsquare-x64.exe -save redsquare-x64.exe -action add -res redsquare-x64.res -log NUL
rm -f redsquare-x64.res
