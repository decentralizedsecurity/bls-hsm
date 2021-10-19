#!/usr/bin/bash
../blst/build.sh
gcc main.c -o server
gcc client.c -o client
