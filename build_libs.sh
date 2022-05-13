

usage(){
  echo "Usage: $0 [-c \"compiler path\"] [-h]
  -c \"compiler path\": define path of the arm compiler (arm-none-eabi-gcc file)
  -h: display this info"
  exit 1;
}

while getopts ":c::h" opt; do
  case $opt in
    c) comp="$OPTARG"
    ;;
    h) usage
    ;;
  esac
done


if [[ ! -f ./lib/libblst.a ]] || [[ ! -f ./lib/blst.h ]] || [[ ! -f ./lib/blst_aux.h ]]; then
  echo "Cloning blst library"
  #Use working version of blst module
  git clone https://github.com/supranational/blst.git

  if [ -z $comp ]; then
	  echo "No compiler was selected. Blst library will be built with default compiler."
    ./blst/build.sh
  else
    ./blst/build.sh CC=$comp -mcpu=cortex-m33 flavour=elf -fno-pie
  fi
  ret=$?

  if [ $ret -eq 0 ]; then
    echo "Blst library built"
    mv ./libblst.a ./lib/
    cp ./blst/bindings/blst.h ./blst/bindings/blst_aux.h ./lib/
  else
    echo "Error building blst library"
  fi
fi

if [[ ! -f ./lib/picohttpparser.c ]] || [[ ! -f ./lib/picohttpparser.h ]]; then
  echo "Cloning picohttpparser library"
  git clone https://github.com/h2o/picohttpparser.git
  cp ./picohttpparser/picohttpparser.c ./picohttpparser/picohttpparser.h ./lib/
fi

if [[ ! -f ./lib/cJSON.c ]] || [[ ! -f ./lib/cJSON.h ]]; then
  echo "Cloning cJSON library"
  git clone https://github.com/DaveGamble/cJSON.git
  cp ./cJSON/cJSON.c ./cJSON/cJSON.h ./lib/
fi

echo "Done"
