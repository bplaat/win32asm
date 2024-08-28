#!/bin/sh
# A simple Windows application which has a WinRT XAML island
# Toolchain: tcc, magick, ResourceHacker, minify-xml (npm)

rm -rf target
mkdir -p target/res

# Resources
magick res/icon.png -define icon:auto-resize="16,32,48,256" target/res/icon.ico
minify-xml res/app.manifest > target/res/app.min.manifest
ResourceHacker -open res/resource.rc -save target/resource.res -action compile -log NUL

# Executable
tcc src/island.c -lcombase -o target/island.exe
ResourceHacker -open target/island.exe -save target/island.exe -action addskip -res target/resource.res -log NUL

./target/island.exe
