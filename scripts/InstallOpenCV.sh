#!/usr/bin/env bash

curl -L -o opencv.tar.gz https://github.com/opencv/opencv/archive/3.3.1.tar.gz
tar -xzf opencv.tar.gz
cd opencv*/
mkdir -p build/
cd build/
cmake ..
make install -j 2
rm -rf opencv*