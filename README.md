# ByteRacers
A 2D maze-racing game made using SDL3, C++, CMake

**macOS & UNIX:**
- Install homebrew
- Install SDL3 on system
- Install SDL3_image

- make sure SDL3 and SDL3_image include directories are in /usr/local/include directory

- Download CMakeLists.txt, main.cpp, assets, build.sh into ByteRacers directory using: % `git clone https://github.com/willcurt/ByteRacers.git`

In terminal:
- % `chmod +x build.sh`
- Run: % `RUN_AFTER_BUILD=1 ./build.sh`
    - Overrides (example):
      % `RUN_AFTER_BUILD=1 \
      SDL3_DIR=/usr/local/opt/sdl3/lib/cmake/SDL3 \
      SDL3_image_DIR=/usr/local/opt/sdl3_image/lib/cmake/SDL3_image \
      ./build.sh`


