#!/bin/sh
# build.sh

mkdir libs
cd libs

wget https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip
unzip -n glfw-3.4.zip
rm glfw-3.4.zip
cd glfw-3.4
cmake -S . -B build
cd build
make
cd ../..

mkdir glad
cd glad
pip install selenium
pip install pyvirtualdisplay
wget `python ../../scripts/generateGladURL.py`
unzip -n glad.zip
rm glad.zip
cd src
mv glad.c glad.cpp
sed -i 's@#include <glad/glad.h>@#include "../include/glad/glad.h"@g' glad.cpp
cd ../..

wget https://github.com/g-truc/glm/releases/download/0.9.8.5/glm-0.9.8.5.zip
unzip -n glm-0.9.8.5.zip
rm glm-0.9.8.5.zip

mkdir stb_image
cd stb_image
wget https://github.com/nothings/stb/raw/master/stb_image.h
cd ..

#wget https://download.savannah.gnu.org/releases/freetype/freetype-2.10.1.tar.xz
#tar -xvf freetype-2.10.1.tar.xz
#rm -rf freetype-2.10.1.tar.xz
#cd freetype-2.10.1
#cmake -E make_directory build
#cmake -E chdir build cmake ..
#cd build
#make
#cd ../..