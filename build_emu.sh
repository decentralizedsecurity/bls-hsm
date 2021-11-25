git submodule init
git submodule update
blst/build.sh
cp blst/bindings/blst.h blst/bindings/blst_aux.h cli/include/
mkdir emu/lib
mkdir emu/build
mv libblst.a emu/lib/
gcc emu/main.c emu/lib/libblst.a -o emu/build/server -Wno-implicit-function-declaration
gcc emu/client.c -o emu/build/client -Wno-implicit-function-declaration
