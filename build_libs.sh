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
  printf -- "--- blst/src/e2.c       2022-11-10 12:20:17.768998400 +0100
+++ e2.c        2022-11-10 12:21:23.980661100 +0100
@@ -45,7 +45,7 @@
 },
 { { ONE_MONT_P }, { 0 } }
 };
-
+POINTonE2 table[4][1<<(5-1)];
 const POINTonE2 BLS12_381_NEG_G2 = { /* negative generator [in Montgomery] */
 { /* (0x024aa2b2f08f0a91260805272dc51051c6e47ad4fa403b02
         b4510b647ae3d1770bac0326a805bbefd48056c8c121bdb8 << 384) %% P */
@@ -497,7 +497,7 @@

     {
         const byte *scalars[2] = { val.s, NULL };
-        POINTonE2 table[4][1<<(5-1)];   /* 18KB */
+        //POINTonE2 table[4][1<<(5-1)];   /* 18KB */
         size_t i;

         POINTonE2_precompute_w5(table[0], in);" | patch blst/src/e2.c

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
