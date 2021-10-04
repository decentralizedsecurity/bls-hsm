#!/usr/bin/bash

while getopts ":c:" opt; do
  case $opt in
    c) comp="$OPTARG"
    ;;
  esac
done

if [ -z $comp ]; then
	echo "Compiler path required"
	exit 1;
fi

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