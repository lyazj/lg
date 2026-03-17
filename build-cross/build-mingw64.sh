#!/bin/bash

cmake -S . -B build-mingw64 -DCMAKE_TOOLCHAIN_FILE=${PWD}/mingw64.cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${PWD}/install-mingw64 &&
cmake --build build-mingw64 --parallel $(nproc) &&
cmake --install build-mingw64
