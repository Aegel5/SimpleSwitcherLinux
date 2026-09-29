#!/bin/zsh
cmake -B release -DCMAKE_BUILD_TYPE=Release
cmake --build release
