# Simple build script to build the Windows application with Tiny C Compiler and ResourceHacker
rm -rf build
mkdir build

# magick convert res/icon.png -define icon:auto-resize="16,32,48,256" res/icon.ico

if [[ $1 == 'release' ]]; then
    tcc -m32 -lcomctl32 -lcomdlg32 -lshell32 src/bimg.c -o build/bimg-x86.exe
    ResourceHacker -open res/resource-x86.rc -save build/resource-x86.res -action compile -log NUL
    ResourceHacker -open build/bimg-x86.exe -save build/bimg-x86.exe -action addskip -res build/resource-x86.res -log NUL
    rm build/resource-x86.res
fi

tcc -lcomctl32 -lcomdlg32 -lshell32 src/bimg.c -o build/bimg-x64.exe
ResourceHacker -open res/resource-x64.rc -save build/resource-x64.res -action compile -log NUL
ResourceHacker -open build/bimg-x64.exe -save build/bimg-x64.exe -action addskip -res build/resource-x64.res -log NUL
rm build/resource-x64.res

if [[ $1 != 'release' ]]; then
    ./build/bimg-x64
fi
