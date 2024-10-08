cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(untitled)

target_sources(app PRIVATE src/main.c)

add_definitions(-DGPL_LICENSE_TERMS_ACCEPTED -DSCRYPT_SUPPORT -DPBKDF_SUPPORT -DSALSA20_SUPPORT)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources}
  ../lib/cyclone_crypto/kdf/scrypt.c
  ../lib/cyclone_crypto/kdf/pbkdf.c
  ../lib/cyclone_crypto/mac/hmac.c
  ../lib/cyclone_crypto/hash/sha256.c
  ../lib/cyclone_crypto/cipher/salsa20.c
  ../lib/common/os_port_zephyr.c
  ../lib/common/cpu_endian.c
)

zephyr_library_include_directories(
  ../lib/
  ../lib/cyclone_crypto/
  ../lib/common/
)