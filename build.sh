#!/usr/bin/env bash
set -e

BUILD_DIR=cmake-build-debug

rm -rf "$BUILD_DIR"

cmake -S . -B "$BUILD_DIR" \
  -DCMAKE_BUILD_TYPE=Release \
  -DPython3_EXECUTABLE="$(which python3)"

cmake --build "$BUILD_DIR" -j
