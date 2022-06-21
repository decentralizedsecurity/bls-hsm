git clone https://github.com/wolfssl/wolfssl.git
cd wolfssl
./configure --enable-scrypt
make
sudo make install
cd ..

gcc import_keystore.c cJSON.c -lm -lwolfssl -o import_keystore -Wno-implicit-function-declaration
