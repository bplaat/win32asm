if [[ $1 == "resources" ]]; then
    ./layout-convert.py res/layout.xml res/layout.bin
fi

if [[ $1 == "clean" ]]; then
    rm res/layout.bin
fi
