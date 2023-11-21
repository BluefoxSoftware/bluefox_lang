alias b := build
alias c := clean
alias r := run

build: clean
    mkdir build
    cmake -G Ninja -B build . 
    cmake --build build

clean:
    rm -r build || true      #ignore if fail

run: build
    ./build/bluefox_lang ./examples/hello_world/hello_world.bfl