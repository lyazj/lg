#!/bin/bash

cmake -S .. -B build-linux32 -DCMAKE_TOOLCHAIN_FILE=${PWD}/linux32.cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${PWD}/install-linux32 -DFIND_GLM=OFF -DFIND_STB=OFF -DUSE_MAGICK=OFF &&
cmake --build build-linux32 --parallel $(nproc) &&
cmake --install build-linux32 --strip
