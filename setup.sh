#!/usr/bin/bash

usage(){
  echo "Usage: $0 [-c \"compiler path\"] [-i] -b \"board identifier\"
  -c \"compiler path\": define path of the arm compiler (arm-none-eabi-gcc file)
  -i: check if GNU ARM Embedded Toolchain is installed. Install it otherwise
  -b: board identifier"
  exit 1;
}

while getopts ":c::b::i" opt; do
  case $opt in
    c) comp="$OPTARG"
    control=0
    export control
    ;;
    i)comp="/usr/bin/arm-none-eabi-gcc"
    control=1
    export control
    ;;
    b)board="$OPTARG"
    ;;
    *) usage
    ;;
  esac
done

if [ -z $comp ] || [ -z $board ]; then
  usage
fi

if [ control -eq 1 ]; then
	sudo apt install gcc-arm-none-eabi
fi

comp=`realpath $comp`

echo "Compiler selected: $comp"

./blst/build.sh CC=$comp -mcpu=cortex-m33 flavour=elf -fno-pie
ret=$?

if [ $ret -eq 0 ]; then
  echo "Blst library built"
  sudo rm -f ./cli/include/blst.h
  sudo rm -f ./cli/include/blst_aux.h
  sudo rm -f ./cli/lib/libblst.a
  sudo mkdir -p ./cli/lib
  sudo mv ./libblst.a ./cli/lib/
  sudo mkdir -p ./cli/include
  sudo cp ./blst/bindings/blst.h ./blst/bindings/blst_aux.h ./cli/include/
else
  echo "Error building blst library"
fi

#--------

wget https://apt.kitware.com/kitware-archive.sh
sudo bash kitware-archive.sh
rm kitware-archive.sh

sudo apt install --no-install-recommends git cmake ninja-build gperf \
  ccache dfu-util device-tree-compiler wget \
  python3-dev python3-pip python3-setuptools python3-tk python3-wheel xz-utils file \
  make gcc gcc-multilib g++-multilib libsdl2-dev
  
bls=`pwd`
  
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

echo "export ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb" > ~/.zephyrrc
if [ $control -eq 0 ]; then
  echo "export GNUARMEMB_TOOLCHAIN_PATH=${comp%bin*}" >> ~/.zephyrrc
else
  echo "export GNUARMEMB_TOOLCHAIN_PATH=\"/usr/\"" >> ~/.zephyrrc
fi


source zephyr/zephyr-env.sh

cd $bls/cli
sudo rm -r build
west build -b $board

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

west flash --erase