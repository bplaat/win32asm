# Simple build script to build the Windows application with MinGW and ResourceHacker
rm -rf build
mkdir build

# magick convert res/icon.png -define icon:auto-resize="16,32,48,256" res/icon.ico

windres res/resource.rc -o build/resource.res

gcc -static -s -Os src/bimg.c -lgdi32 -lcomctl32 -Wl,--subsystem,windows -o build/bimg.exe

ResourceHacker -open build/bimg.exe -save build/bimg.exe -action addoverwrite -res build/resource.res -log NUL

rm build/resource.res

./build/bimg "bassiebas 2.0.jpg"
