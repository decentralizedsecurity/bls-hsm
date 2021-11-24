#!/usr/bin/bash
../blst/build.sh
sudo cp ../blst/bindings/blst.h ../blst/bindings/blst_aux.h ../cli/include/
gcc main.c libblst.a -o server
gcc client.c -o client
