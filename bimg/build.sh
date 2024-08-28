#!/bin/sh
# Simple build script to build the Windows application
# Toolchain: tcc, magick, ResourceHacker, minify-xml (npm)

rm -rf target
mkdir -p target/res

magick res/icon.png -define icon:auto-resize="16,32,48,256" target/res/icon.ico
minify-xml res/app.manifest > target/res/app.min.manifest
ResourceHacker -open res/resource-x64.rc -save target/resource-x64.res -action compile -log NUL

if [[ $1 == 'release' ]]; then
    ResourceHacker -open res/resource-x86.rc -save target/resource-x86.res -action compile -log NUL
    tcc -m32 -lole32 -lcomdlg32 -lshell32 src/bimg.c -o target/bimg-x86.exe
    ResourceHacker -open target/bimg-x86.exe -save target/bimg-x86.exe -action addskip -res target/resource-x86.res -log NUL
fi

tcc -lole32 -lcomdlg32 -lshell32 src/bimg.c -o target/bimg-x64.exe
ResourceHacker -open target/bimg-x64.exe -save target/bimg-x64.exe -action addskip -res target/resource-x64.res -log NUL

./target/bimg-x64
