#!/bin/bash

sudo dnf install -y gcc gcc-c++

if [[ ! -d ~/Downloads/cmake-3.9.2 ]]; then
	if [[ ! -f ~/Downloads/cmake-3.9.2.tar.gz ]]; then
		echo "Please download cmake:"
		echo "wget https://cmake.org/files/v3.9/cmake-3.9.2.tar.gz -P ~/Downloads/"
		exit
	else
		tar -xzf cmake-3.9.2.tar.gz
	fi
fi

cd ~/Downloads/cmake-3.9.2 && ./bootstrap && make && sudo make install


if [[ ! -f gcc-arm-none-eabi-6-2017-q1-update-linux.tar.bz2 ]]; then
		echo "Please download arm tools:"
		echo "wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/6_1-2017q1/gcc-arm-none-eabi-6-2017-q1-update-linux.tar.bz2 -P ~/Downloads/"
		exit
fi

sudo tar -xf gcc-arm-none-eabi-6-2017-q1-update-linux.tar.bz2 --strip 1 -C /usr/local/
