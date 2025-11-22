#!/usr/bin/bash
# simple script to build Release & debug
# Then also runs both builds' tests
# makes easier to catch error between debug and release builds

# stop on first error
set -e 

if [ ! -d build ]; then
    mkdir build
fi

if [ ! -d build/debug ]; then
    cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug 
fi

if [ ! -d build/release ]; then
    cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release
fi

export LD_LIBRARY_PATH=/lib:$(pwd)/submodules/simavr/simavr/obj-x86_64-pc-linux-gnu/

printf "\nDEBUG BUILD:\n\n"
cmake --build build/debug --config CMAKE_BUILD_TYPE=Debug -j$(nproc) --clean-first
printf "\nRELEASE BUILD:\n\n"
cmake --build build/release --config CMAKE_BUILD_TYPE=Release -j$(nproc) --clean-first

printf "\nDEBUG Test:\n\n"
ctest --test-dir build/debug --output-on-failure -j$(nproc)
printf "\nRELEASE Test:\n\n"
ctest --test-dir build/release --output-on-failure -j$(nproc)