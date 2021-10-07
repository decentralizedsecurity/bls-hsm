#!/usr/bin/bash
chmod +rwx *

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
    ;;
    i)comp="/usr/bin/arm-none-eabi-gcc"
    control=1
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

if [ $control -eq 1 ]; then
	sudo apt install gcc-arm-none-eabi
fi

comp=`realpath $comp`

echo "Compiler selected: $comp"

./build_blst.sh -c $comp

./dependencies.sh

./build.sh -c $comp -b $board

./flash.sh