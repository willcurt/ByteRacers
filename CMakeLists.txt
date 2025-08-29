cmake_minimum_required(VERSION 3.31)
project(ByteRacers LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

find_package(SDL3 CONFIG REQUIRED)

add_executable(ByteRacers main.cpp)

target_link_libraries(ByteRacers PRIVATE SDL3::SDL3)

set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")

if (EXISTS "${ASSETS_DIR}")
    add_custom_command(TARGET ByteRacers POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E rm -rf "$<TARGET_FILE_DIR:ByteRacers>/assets"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:ByteRacers>/assets"
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${ASSETS_DIR}" "$<TARGET_FILE_DIR:ByteRacers>/assets"
        COMMENT "Copying assets to output directory"
    )
else()
    message(WARNING "Assets directory not found: ${ASSETS_DIR}")
endif()


