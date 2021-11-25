#!/usr/bin/bash

usage(){
  echo "Usage: $0 -c \"compiler path\" -b \"board identifier\"
  -c \"compiler path\": define path of the arm compiler (arm-none-eabi-gcc file)
  -b: \"board identifier\""
  exit 1;
}

bls=`pwd`

while getopts ":c::b:" opt; do
  case $opt in
    c) comp="$OPTARG"
    ;;
    b) board="$OPTARG"
    ;;
  esac
done

if [ -z $comp ] || [ -z $board ]; then
	usage
fi

echo "export ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb" > ~/.zephyrrc
echo "export GNUARMEMB_TOOLCHAIN_PATH=${comp%bin*}" >> ~/.zephyrrc

source ~/ncs/zephyr/zephyr-env.sh

cd $bls/cli
west build -p -b $board