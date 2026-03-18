#!/bin/bash

cmake -S . -B build-linux64 -DCMAKE_TOOLCHAIN_FILE=${PWD}/linux64.cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${PWD}/install-linux64 &&
cmake --build build-linux64 --parallel $(nproc) &&
cmake --install build-linux64 --strip
