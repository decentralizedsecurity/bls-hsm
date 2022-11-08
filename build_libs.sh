#!/bin/bash
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
  #Use version 0.3.10 of blst module
  git clone --depth 1 --branch v0.3.10 https://github.com/supranational/blst

  #Patch to use HEAP memory instead of STACK
  printf -- "--- ./blst/src/e2.c     2022-11-03 13:49:53.282533000 +0100\n+++ e2.c        2022-11-03 13:49:25.006248700 +0100\n@@ -497,7 +497,8 @@\n\n     {\n         const byte *scalars[2] = { val.s, NULL };\n-        POINTonE2 table[4][1<<(5-1)];   /* 18KB */\n+        //POINTonE2 table[4][1<<(5-1)];   /* 18KB */\n+        POINTonE2 (*table)[1<<(5-1)] = malloc(4*(1<<(5-1))*sizeof(POINTonE2));\n         size_t i;\n\n         POINTonE2_precompute_w5(table[0], in);\n@@ -510,6 +511,7 @@\n         }\n\n         POINTonE2s_mult_w5(out, NULL, 4, scalars, 64, table);\n+        free(table);\n     }\n\n     vec_zero(val.l, sizeof(val));   /* scrub the copy of SK */" | patch blst/src/e2.c

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
