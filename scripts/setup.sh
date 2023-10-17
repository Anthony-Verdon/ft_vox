#!/bin/bash

mkdir includes
cd includes

#install glfw
wget https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.zip
unzip glfw-3.3.8.zip
rm glfw-3.3.8.zip
mv glfw-3.3.8 glfw
cd glfw
mkdir build
cmake -S . -B build
cd ..

#install glm
wget https://github.com/g-truc/glm/releases/download/0.9.8.5/glm-0.9.8.5.zip
unzip glm-0.9.8.5.zip
rm glm-0.9.8.5.zip

cd ..