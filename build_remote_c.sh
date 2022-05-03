./build_libs.sh 

mkdir remote-c/build

#git clone https://github.com/wolfssl/wolfssl.git
#cd wolfssl
#./autogen.sh
#./configure --enable-scrypt --enable-static --enable-aesctr
#make
#cd ..

#mv wolfssl/src/.libs/libwolfssl.a lib/
#mv wolfssl/wolfssl/wolfcrypt/pwdbased.h lib/
#mv wolfssl/wolfssl/wolfcrypt/aes.h lib/
#mv wolfssl/wolfssl/wolfcrypt/sha256.h lib/

#rm -rf wolfssl


#gcc json.c
gcc keystoredecrypt/import_keystore.c lib/cJSON.c lib/libwolfssl.a -o import_keystore -Wno-implicit-function-declaration
gcc remote-c/main.c lib/bls_hsm.c lib/common.c lib/picohttpparser.c lib/cJSON.c lib/libblst.a lib/libwolfssl.a -o remote-c/build/server -Wno-implicit-function-declaration
gcc remote-c/client.c -o remote-c/build/client -Wno-implicit-function-declaration