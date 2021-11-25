#!/usr/bin/bash

bls=`pwd`

source ~/ncs/zephyr/zephyr-env.sh

cd $bls/cli
if  [ ! command -v nrfjprog &> /dev/null ] || [ ! command -v JLinkConfig &> /dev/null ]; then
	cd ~
	wget https://www.nordicsemi.com/-/media/Software-and-other-downloads/Desktop-software/nRF-command-line-tools/sw/Versions-10-x-x/10-13-0/nRF-Command-Line-Tools_10_13_0_Linux64.zip
	unzip nRF-Command-Line-Tools_10_13_0_Linux64.zip -d tools
	cd tools/nRF-Command-Line-Tools_10_13_0_Linux64
	tar xvf nRF-Command-Line-Tools_10_13_0_Linux-amd64.tar.gz
	sudo dpkg -i --force-overwrite JLink_Linux_V750a_x86_64.deb
	sudo dpkg -i --force-overwrite nRF-Command-Line-Tools_10_13_0_Linux-amd64.deb
	cd $bls/cli
	rm -r ~/tools
	rm ~/nRF-Command-Line-Tools_10_13_0_Linux64.zip
fi

west flash --erase