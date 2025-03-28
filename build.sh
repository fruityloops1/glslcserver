#!/bin/bash

clang++ -target aarch64-linux-gnu --sysroot=/usr/aarch64-linux-gnu/ main.cpp wrap.cpp -o glslcserver libglslc.so -g