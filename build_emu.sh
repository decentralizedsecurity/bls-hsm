git submodule init
git submodule update
blst/build.sh
cp blst/bindings/blst.h blst/bindings/blst_aux.h cli/include/
mkdir remote-c/lib
mkdir remote-c/build
mv libblst.a remote-c/lib/
#gcc json.c
gcc remote-c/main.c remote-c/picohttpparser.c remote-c/cJSON.c remote-c/lib/libblst.a -o remote-c/build/server -Wno-implicit-function-declaration
gcc remote-c/client.c -o remote-c/build/client -Wno-implicit-function-declaration
