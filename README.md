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

**Windows / PowerShell / VS Code**
If you are running ByteRacers on a Windows device using Visual Studio or PowerShell:
1. Install Visual Studio 2022 with the Desktop C++ development workload.
2. Install CMake and make sure to add it to your PATH.
3. Install vcpkg:
  In terminal:
    git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg
    .\bootstrap-vcpkg.bat

4. Install SDL3 and SDL3_image via vcpkg:
  In terminal:
    .\vcpkg install sdl3 sdl3-image
    .\vcpkg integrate install

5. Open the ByteRacers project in VS Code:
6. Create or edit workspace settings in .vscode/settings.json and add:
    In vscode > settings.json:
      {
        "cmake.configureArgs": [
        "-DCMAKE_TOOLCHAIN_FILE=${workspaceFolder}/../vcpkg/scripts/buildsystems/vcpkg.cmake"
        ],
        "cmake.sourceDirectory": "${workspaceFolder}",
        "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools"
      }
  
7. Build the project in VS Code:
  - Open the CMake sidebar → Select Kit → choose your Visual Studio Community x64 kit.
  - Click Configure to generate build files. (Only need to rebuild when changes in code have been made).
  - Click Build to compile the project.
    In terminal:
      cd build
      cmake --build

8. Run the game:
    In terminal:
      .\Debug\ByteRacers.exe
