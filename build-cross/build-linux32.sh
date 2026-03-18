#!/bin/bash

cmake -S . -B build-linux32 -DCMAKE_TOOLCHAIN_FILE=${PWD}/linux32.cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${PWD}/install-linux32 &&
cmake --build build-linux32 --parallel $(nproc) &&
cmake --install build-linux32 --strip
