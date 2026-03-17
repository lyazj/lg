#!/bin/bash

cmake -S . -B build-mingw32 -DCMAKE_TOOLCHAIN_FILE=${PWD}/mingw32.cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${PWD}/install-mingw32 &&
cmake --build build-mingw32 --parallel $(nproc) &&
cmake --install build-mingw32 --strip
