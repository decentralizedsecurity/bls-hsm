#!/bin/bash

usage(){
  echo "Usage: $0 [-c \"compiler path\"] [-i]
  -c \"compiler path\": define path of the arm compiler (arm-none-eabi-gcc file)
  -i: check if GNU ARM Embedded Toolchain is installed. Install it otherwise"
  exit 1;
}

while getopts ":c::i" opt; do
  case $opt in
    c) comp="$OPTARG"
    ;;
    i) sudo apt install gcc-arm-none-eabi
    comp="/usr/bin/arm-none-eabi-gcc"
    ;;
    *) usage
    ;;
  esac
done

if [ -z $comp ]; then
  usage
fi

echo "Compiler selected: $comp"

./blst/build.sh CC=$comp -mcpu=cortex-m33 flavour=elf -fno-pie
ret=$?

if [ $ret -eq 0 ]; then
  echo "Blst library built"
  rm -f ./cli/include/blst.h
  rm -f ./cli/include/blst_aux.h
  rm -f ./cli/lib/libblst.a
  mkdir -p ./cli/lib
  mv ./libblst.a ./cli/lib/
  mkdir -p ./cli/include
  cp ./blst/bindings/blst.h ./blst/bindings/blst_aux.h ./cli/include/
else
  echo "Error building blst library"
fi