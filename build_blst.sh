#!/usr/bin/bash

usage(){
  echo "Usage: $0 -c \"compiler path\"
  -c \"compiler path\": define path of the arm compiler (arm-none-eabi-gcc file)"
  exit 1;
}

while getopts ":c:" opt; do
  case $opt in
    c) comp="$OPTARG"
    ;;
  esac
done

if [ -z $comp ]; then
	usage
fi

#Use working version of blst module
git clone https://github.com/supranational/blst.git

./blst/build.sh CC=$comp -mcpu=cortex-m33 flavour=elf -fno-pie
ret=$?

if [ $ret -eq 0 ]; then
  echo "Blst library built"
  sudo rm -f ./include/blst.h
  sudo rm -f ./include/blst_aux.h
  sudo rm -f ./lib/libblst.a
  sudo mkdir -p ./lib
  sudo mv ./libblst.a ./lib/
  sudo mkdir -p ./include
  sudo cp ./blst/bindings/blst.h ./blst/bindings/blst_aux.h ./include/
else
  echo "Error building blst library"
fi