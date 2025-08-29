#!/bin/bash
set -e	#stop if any command fails

# create build directory
mkdir -p build
cd build

# run cmake config, point it to SDL3
cmake .. -DSDL3_DIR=/usr/local/opt/sdl3/lib/cmake/SDL3

# build project
cmake --build .

# run executable
./ByteRacers
