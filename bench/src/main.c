#include <zephyr/kernel.h>

#include <kdf/scrypt.h>
#include <kdf/pbkdf.h>
#include <hash/sha256.h>


#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <common.h>

#include "mbedtls/asn1.h"
#include "mbedtls/cipher.h"
#include "mbedtls/oid.h"
#include "mbedtls/pkcs5.h"
#include <mbedtls/aes.h>

void main(void)
{
	char* pass = "123456789";
	char* salt_str = "8123ea083eae312143c724a8063ea9ec53b4818d34726b28a20fafa6107b2900";
	char* iv_str = "7ea5abd19a7747ddac97b3951ade63a5";
	char* cm_str = "ac099b2371f36ed9356e03918ddbcb231b3d018ef5d3b5b5041ecc2b83d56a2a"; 
	uint8_t iv[16];
	uint8_t salt[32];
	uint8_t cm[32];
	mbedtls_aes_context ctx;
	
    if(hex2bin(salt_str, 64, salt, 32) == 0){
        return HEX2BINERR;
    }
	if(hex2bin(iv_str, 32, iv, 16) == 0){
        return HEX2BINERR;
    }
	if(hex2bin(cm_str, 64, cm, 32) == 0){
        return HEX2BINERR;
    }
	uint8_t dk[32] = "";
	uint8_t sk[32] = "";
	unsigned char stream_block[16];
	size_t nc_off = 0;


	int64_t start = k_uptime_get();
	error_tc ret = scrypt(pass, salt, 32, 256, 8, 1, dk, 32);
	int64_t elapsed = k_uptime_delta(&start);
	if(ret == 0){
		printf("Scrypt success in %d ms\n", (int) elapsed);
	}else{
		printf("Scrypt failed\n");
	}

	start = k_uptime_get();
	ret = mbedtls_pkcs5_pbkdf2_hmac_ext(MBEDTLS_MD_SHA256, pass, 9, salt, 32, 262144, 32, dk);
	elapsed = k_uptime_delta(&start);

	if(ret == 0){
		printf("Native PBKDF2 success in %d s\n", (int) elapsed/1000);
	}else{
		printf("Native PBKDF2 failed\n");
	}


	start = k_uptime_get();
	ret = pbkdf2(SHA256_HASH_ALGO, pass, 9, salt, 32, 262144, dk, 32);
	elapsed = k_uptime_delta(&start);

	if(ret == 0){
		printf("Cyclone PBKDF2 success in %d s\n", (int) elapsed/1000);
	}else{
		printf("Cyclone PBKDF2 failed\n");
	}
	
	mbedtls_aes_init(&ctx);
	mbedtls_aes_setkey_dec(&ctx, dk, 128);

	start = k_uptime_get();
	ret = mbedtls_aes_crypt_ctr(&ctx, 32, &nc_off, iv, stream_block, dk, sk);
	elapsed = k_uptime_delta(&start);

	if(ret == 0){
		printf("AES-128-CTR success in %d ms\n", (int) elapsed);
	}else{
		printf("AES-128-CTR failed\n");
	}
}