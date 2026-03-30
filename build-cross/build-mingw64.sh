#!/bin/bash

cmake -S .. -B build-mingw64 -DCMAKE_TOOLCHAIN_FILE=${PWD}/mingw64.cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${PWD}/install-mingw64 -DFIND_GLM=OFF -DFIND_STB=OFF -DUSE_MAGICK=OFF &&
cmake --build build-mingw64 --parallel $(nproc) &&
cmake --install build-mingw64 --strip
