#!/usr/bin/bash

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

west init -m https://github.com/nrfconnect/sdk-nrf --mr v1.7.0
west update
west zephyr-export

pip3 install --user -r zephyr/scripts/requirements.txt
pip3 install --user -r nrf/scripts/requirements.txt
pip3 install --user -r bootloader/mcuboot/scripts/requirements.txt