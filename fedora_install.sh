#!/bin/bash

ln -s /run/media/${USER}/61AB-FDF1/Downloads/* ~/Downloads/

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

cd ~/Downloads/cmake-3.9.2

./bootstrap && make && sudo make install

if [[ ! -f ~/Downloads/gcc-arm-none-eabi-6-2017-q1-update-linux.tar.bz2 ]]; then
		echo "Please download arm tools:"
		echo "wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/6_1-2017q1/gcc-arm-none-eabi-6-2017-q1-update-linux.tar.bz2 -P ~/Downloads/"
		exit
fi

sudo tar -xf ~/Downloads/gcc-arm-none-eabi-6-2017-q1-update-linux.tar.bz2 --strip 1 -C /usr/local/

sudo rpm -i ~/Downloads/JLink_Linux_V634a_x86_64.rpm

sudo pip install --upgrade pip
sudo dnf install redhat-rpm-config
sudo dnf install python-devel
sudo pip install hidapi

cd ~/

git clone https://github.com/shiftdevices/defcon_workshop.git
