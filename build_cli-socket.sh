


./build_libs.sh
mkdir cli-socket/build
gcc cli-socket/main.c  lib/common.c lib/bls_hsm_ns.c  lib/libblst.a -o cli-socket/build/server -Wno-implicit-function-declaration
gcc cli-socket/client.c -o cli-socket/build/client -Wno-implicit-function-declaration


