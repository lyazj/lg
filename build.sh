#!/bin/bash

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${PWD}/install &&
cmake --build build --parallel $(nproc) &&
cmake --install build
