#!/usr/bin/bash

mkdir ~/gnuarmemb
cd ~/gnuarmemb
wget 'https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2019q4/gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2'
tar xvjf gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2
rm gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2

wget https://apt.kitware.com/kitware-archive.sh
sudo bash kitware-archive.sh
rm kitware-archive.sh

sudo apt install --no-install-recommends git cmake ninja-build gperf \
  ccache dfu-util device-tree-compiler wget \
  python3-dev python3-pip python3-setuptools python3-tk python3-wheel xz-utils file \
  make gcc gcc-multilib g++-multilib libsdl2-dev
  
  
mkdir ~/ncs
cd ~/ncs
pip3 install --user west
export PATH=~/.local/bin:"$PATH"

west init -m https://github.com/nrfconnect/sdk-nrf --mr v1.8.0
west update
west zephyr-export

pip3 install --user -r zephyr/scripts/requirements.txt
pip3 install --user -r nrf/scripts/requirements.txt
pip3 install --user -r bootloader/mcuboot/scripts/requirements.txt

if ! command -v nrfjprog &> /dev/null || ! command -v JLinkConfig &> /dev/null; then
	if ! command -v unzip &> /dev/null; then
		sudo apt install unzip
	fi
	cd ~
	wget https://www.nordicsemi.com/-/media/Software-and-other-downloads/Desktop-software/nRF-command-line-tools/sw/Versions-10-x-x/10-15-4/nrf-command-line-tools-10.15.4_amd64.zip
	unzip nrf-command-line-tools-10.15.4_amd64.zip -d tools
	cd tools
	tar xvf nrf-command-line-tools-10.15.4_Linux-amd64.tar.gz
	sudo dpkg -i --force-overwrite JLink_Linux_V758b_x86_64.deb
	sudo dpkg -i --force-overwrite nrf-command-line-tools_10.15.4_amd64.deb
	cd ~
	rm -r ~/tools
	rm ~/nrf-command-line-tools-10.15.4_amd64.zip
fi