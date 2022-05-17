#git submodule init
#git submodule update
#blst/build.sh
#cp blst/bindings/blst.h blst/bindings/blst_aux.h cli/include/
#mkdir cli-socket/lib
#mkdir cli-socket/build
#mv libblst.a cli-socket/lib/
#gcc cli-socket/main.c cli-socket/lib/libblst.a -o cli-socket/build/server -Wno-implicit-function-declaration
#gcc cli-socket/client.c -o cli-socket/build/client -Wno-implicit-function-declaration


./build_libs.sh
#mkdir cli-socket/build
gcc cli-socket/main.c  lib/common.c lib/bls_hsm_ns.c  lib/libblst.a -o cli-socket/build/server -Wno-implicit-function-declaration
gcc cli-socket/client.c -o cli-socket/build/client -Wno-implicit-function-declaration



#gcc remote-c/main.c lib/bls_hsm.c lib/common.c lib/picohttpparser.c lib/cJSON.c lib/libblst.a lib/libwolfssl.a -o remote-c/build/server -Wno-implicit-function-declaration
#gcc remote-c/client.c -o remote-c/build/client -Wno-implicit-function-declaration