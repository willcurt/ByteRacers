#!/usr/bin/env bash
set -euo pipefail

# knobs
BUILD_DIR="${BUILD_DIR:-build}"
BUILD_TYPE="${BUILD_TYPE:-RelWithDebInfo}"   # Debug/Release/RelWithDebInfo
GENERATOR="${GENERATOR:-}"                    # if using one
RUN_AFTER_BUILD="${RUN_AFTER_BUILD:-0}"       # 1 = run after build

# resolve SDL config dirs (env overrides win)
SDL3_DIR_HINT="${SDL3_DIR:-}"
SDL3_IMAGE_DIR_HINT="${SDL3_image_DIR:-}"
if command -v brew >/dev/null 2>&1; then
  [[ -z "${SDL3_DIR_HINT}"        && -d "$(brew --prefix sdl3 2>/dev/null)/lib/cmake/SDL3" ]] \
    && SDL3_DIR_HINT="$(brew --prefix sdl3)/lib/cmake/SDL3"
  [[ -z "${SDL3_IMAGE_DIR_HINT}"  && -d "$(brew --prefix sdl3_image 2>/dev/null)/lib/cmake/SDL3_image" ]] \
    && SDL3_IMAGE_DIR_HINT="$(brew --prefix sdl3_image)/lib/cmake/SDL3_image"
fi

mkdir -p "$BUILD_DIR"
CMAKE_OPTS=(-S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_FIND_PACKAGE_PREFER_CONFIG=TRUE)
[[ -n "$GENERATOR" ]] && CMAKE_OPTS+=(-G "$GENERATOR")
[[ -n "${SDL3_DIR_HINT}"       ]] && CMAKE_OPTS+=(-DSDL3_DIR="$SDL3_DIR_HINT")
[[ -n "${SDL3_IMAGE_DIR_HINT}" ]] && CMAKE_OPTS+=(-DSDL3_image_DIR="$SDL3_IMAGE_DIR_HINT")

echo "Configuring..."
cmake "${CMAKE_OPTS[@]}"

echo "Building..."
cmake --build "$BUILD_DIR" --config "$BUILD_TYPE" ${VERBOSE:+--verbose}

if [[ "$RUN_AFTER_BUILD" == "1" ]]; then
  for exe in "$BUILD_DIR/ByteRacers" "$BUILD_DIR/$BUILD_TYPE/ByteRacers"; do
    [[ -x "$exe" ]] && exec "$exe"
  done
  echo "Built, but couldn't locate executable."
fi

