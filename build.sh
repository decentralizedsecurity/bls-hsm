#!/usr/bin/bash

bls=`pwd`

while getopts ":c::b:" opt; do
  case $opt in
    c) comp="$OPTARG"
    ;;
    b) board="$OPTARG"
    ;;
  esac
done

if [ -z $comp ]; then
	echo "Compiler path required"
	exit 1;
fi

if [ -z $board ]; then
	echo "Board identifier required"
	exit 1;
fi

echo "export ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb" > ~/.zephyrrc
echo "export GNUARMEMB_TOOLCHAIN_PATH=${comp%bin*}" >> ~/.zephyrrc

source ~/ncs/zephyr/zephyr-env.sh

cd $bls/cli
sudo rm -r build
west build -b $board