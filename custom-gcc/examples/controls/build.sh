if [[ $1 == "resources" ]]; then
    magick convert res/icon.png -define icon:auto-resize="16,32,48,256" res/icon.ico
fi

if [[ $1 == "clean" ]]; then
    rm res/icon.ico
fi
