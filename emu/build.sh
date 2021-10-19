#!/usr/bin/bash
../blst/build.sh
gcc main.c libblst.a -o server
gcc client.c -o client
