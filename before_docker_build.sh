#!/bin/bash

[[ -f cmake-3.9.2.tar.gz ]] || wget https://cmake.org/files/v3.9/cmake-3.9.2.tar.gz
[[ -f gcc-arm-none-eabi-6-2017-q1-update-linux.tar.bz2 ]] || wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/6_1-2017q1/gcc-arm-none-eabi-6-2017-q1-update-linux.tar.bz2

if [[ ! -f JLink_Linux_V634_x86_64.deb ]]; then
echo "================================================================================================================"
echo "Please go to https://www.segger.com/downloads/jlink/ and download JLink_Linux_V634_x86_64.deb to this directory."
echo "================================================================================================================"
fi
