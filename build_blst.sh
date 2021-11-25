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
cd ./blst
git checkout master
cd ..

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