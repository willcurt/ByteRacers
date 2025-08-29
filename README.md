# ByteRacers
A 2D maze-racing game made using SDL3, C++, CMake

**macOS & UNIX:**
- Install SDL3 on system

- Install homebrew
- May need gcc and pkgconf

- Download CMakeLists.txt, main.cpp, assets, build.sh into ByteRacers directory using: % `git clone https://github.com/willcurt/ByteRacers.git`

- Edit build.sh script with sdl3 path:
- In terminal: run %`brew --prefix sdl3`
- `cmake .. -DSDL3_DIR=$(brew --prefix sdl3)/lib/cmake/SDL3`

In terminal:
- Run: % `chmod +x build.sh`
- Run: % `./build.sh`

Just running: % `./build.sh` from now on will build and run the project (should compile, create executable, and run executable per native resources)
