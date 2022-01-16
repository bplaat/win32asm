# A simple Windows application which has a WinRT XAML island
# Simple build script to build the Windows application with Tiny C Compiler and ResourceHacker
rm -rf build
mkdir build

# magick convert res/icon.png -define icon:auto-resize="16,32,48,256" res/icon.ico
# minify-xml res/app.manifest > res/app.min.manifest

if [[ $1 != 'release' ]]; then
    tcc -lcombase src/island.c -Wl,-subsystem=console -o build/island-x64.exe
else
    tcc -lcombase src/island.c -o build/island-x64.exe
fi
ResourceHacker -open res/resource-x64.rc -save build/resource-x64.res -action compile -log NUL
ResourceHacker -open build/island-x64.exe -save build/island-x64.exe -action addskip -res build/resource-x64.res -log NUL
rm build/resource-x64.res

if [[ $1 != 'release' ]]; then
    ./build/island-x64
fi
