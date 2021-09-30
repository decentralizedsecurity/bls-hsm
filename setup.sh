#!/usr/bin/bash

usage(){
  echo "Usage: $0 [-c \"compiler path\"] [-i]
  -c \"compiler path\": define path of the arm compiler (arm-none-eabi-gcc file)
  -i: check if GNU ARM Embedded Toolchain is installed. Install it otherwise"
  exit 1;
}

while getopts ":c::i" opt; do
  case $opt in
    c) comp="$OPTARG"
    control=0
    export control
    ;;
    i) sudo apt install gcc-arm-none-eabi
    comp="/usr/bin/arm-none-eabi-gcc"
    control=1
    export control
    ;;
    *) usage
    ;;
  esac
done

if [ -z $comp ]; then
  usage
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
echo 'export PATH=~/.local/bin:"$PATH"'

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
west build -b nrf5340dk_nrf5340_cpuapp_ns