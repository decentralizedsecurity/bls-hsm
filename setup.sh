#!/bin/bash

while getopts ":c:" opt; do
  case $opt in
    c) comp="$OPTARG"
    ;;
    \?) echo "Invalid option -$OPTARG" >&2
    exit 1
    ;;
  esac

  case $OPTARG in
    -*) echo "Option $opt needs a valid argument"
    exit 1
    ;;
  esac
done

echo "Compiler selected: $comp"

./blst/build.sh CC=$comp -mcpu=cortex-m33 flavour=elf -fno-pie
ret=$?

if [ $ret -eq 0 ];
then
echo "Blst library builded"
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