./build_libs.sh 

mkdir remote-c/build

if [[ ! -f ./lib/libwolfssl.a ]] || [[ ! -d ./wolfssl ]]; then
    git clone https://github.com/wolfssl/wolfssl.git
    cd wolfssl
    ./autogen.sh
    ./configure --enable-scrypt --enable-static --enable-aesctr
    make
    cd ..

    mv wolfssl/src/.libs/libwolfssl.a lib/
    #mv wolfssl/wolfssl/wolfcrypt/pwdbased.h lib/
    #mv wolfssl/wolfssl/wolfcrypt/aes.h lib/
    #mv wolfssl/wolfssl/wolfcrypt/sha256.h lib/
fi

#gcc json.c
gcc -Iwolfssl/wolfssl/wolfcrypt remote-c/main.c lib/bls_hsm_ns.c lib/common.c lib/picohttpparser.c lib/cJSON.c lib/libblst.a lib/libwolfssl.a -o remote-c/build/server -Wno-implicit-function-declaration
gcc remote-c/client.c -o remote-c/build/client -Wno-implicit-function-declaration
