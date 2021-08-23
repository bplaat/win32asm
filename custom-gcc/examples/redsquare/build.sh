if [[ $1 == "resources" ]]; then
    magick convert res/icon.png -define icon:auto-resize="16,32,48,256" res/icon.ico
    magick convert res/paper.jpg -colors 16 -define bmp:format=bmp3 res/paper.bmp
    magick convert res/paper-dark.jpg -colors 16 -define bmp:format=bmp3 res/paper-dark.bmp
    ./header-convert.py include/resources.h res/resources.inc
    cd res
    nasm -f bin layout.asm -o layout.bin
fi

if [[ $1 == "clean" ]]; then
    rm res/icon.ico res/paper.bmp res/paper-dark.bmp res/resources.inc res/layout.bin
fi
