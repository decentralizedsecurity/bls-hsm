#include "bls_hsm_ns.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef NRF
#include <ctype.h>
#include <wolfssl/wolfcrypt/pwdbased.h>
#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/sha256.h>

#define WOLFSSL_AES_COUNTER
#endif

#ifdef NRF
#include "mbedtls/asn1.h"
#include "mbedtls/cipher.h"
#include "mbedtls/oid.h"
#include "mbedtls/pkcs5.h"
int PBKDF2(uint8_t* salt, uint8_t* password, int it_cnt, uint8_t* key)
{   
    size_t plen = strlen(password);
    /*ocrypto_pbkdf2_hmac_sha256(
    key, 16,
    password, plen,
    salt, 32,
    it_cnt);*/

	mbedtls_md_context_t sha256_ctx;
    const mbedtls_md_info_t *info_sha256;

	int ret;
    
    mbedtls_md_init( &sha256_ctx );
    info_sha256 = mbedtls_md_info_from_type( MBEDTLS_MD_SHA256 );
    if( info_sha256 == NULL )
    {	return -1;
    }
    
    if( ( ret = mbedtls_md_setup( &sha256_ctx, info_sha256, 1 ) ) != 0 )
    {	return -1;
    }

	ret = mbedtls_pkcs5_pbkdf2_hmac( &sha256_ctx, password, plen, salt, 32, it_cnt, 32, key );
	if( ret != 0 )
	{	return -1;
	}
    mbedtls_md_free( &sha256_ctx );
	return 0;
}
#endif

/*
Converts input 'pk' to binary 'out'
*/
void pk_serialize(byte* out, blst_p1 pk){
        blst_p1_compress(out, &pk);
}

/*
Converts input 'sig' to binary 'out2'
*/
void sig_serialize(byte* out2, blst_p2 sig){
        blst_p2_compress(out2, &sig);
}

/*
Get 'hash' from binary string 'msg_bin' with length 'len'
*/
void get_point_from_msg(blst_p2* hash, uint8_t* msg_bin, int len){
        char dst[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_"; //IETF BLS Signature V4
        //Obtain the point from a message
        blst_hash_to_g2(hash, msg_bin, len, dst, sizeof(dst)-1, NULL, 0);
}

/*
Derive 'pk' from hexadecimal string 'pk_hex'. Errors are dumped to 'buff'
On error returns 1
*/
int pk_parse(char* pk_hex, blst_p1_affine* pk, char* buff){
        byte pk_bin[48];
        int offset = parse_hex(pk_hex, 96);
        int error = 0;

        if(offset == BADLEN){
            strcat(buff, "Incorrect public key length. It must be 96 characters long.\n");
            error = BADPKLEN;
        }else if(offset == BADFORMAT){
                strcat(buff, "Public key contains incorrect characters.\n");
                error = offset;
        }else{
            if(hex2bin(pk_hex + offset, 96, pk_bin, 48) == 0) {
                strcat(buff, "Failed converting public key to binary array\n");
                error = HEX2BINERR;
            }else{
                blst_p1_uncompress(pk, pk_bin);
            }
        }
        
        return error;
}

/*
Converts hexadecimal string 'msg' with length 'len' to binary string 'msg_bin'. Errors are dumped to 'buff'.
On error returns 1
*/
int msg_parse(char* msg, uint8_t* msg_bin, int len, char* buff){

        int offset = parse_hex(msg, len);
        int error = 0;
        if(offset == BADFORMAT){
            strcat(buff, "Message contains incorrect characters.\n");
            error = offset;
        }else if(offset == BADLEN){
            strcat(buff, "Message contains incorrect characters.\n");
            error = offset;
        }else{
            if(hex2bin(msg + offset, len, msg_bin, len/2 + len%2) == 0) {
                strcat(buff, "Incorrect message length.\n");
                error = HEX2BINERR;
            }
        }

        return error;
}

/*
Derive 'sig' from hexadecimal string 'sig_hex'. Errors are dumped to 'buff'
On error returns 1
*/
int sig_parse(char* sig_hex, blst_p2_affine* sig, char* buff){
        byte sig_bin[96];
        int offset = parse_hex(sig_hex, 192);
        int error = 0;

        if(offset == BADLEN){
            strcat(buff, "Incorrect signature length. It must be 192 characters long.\n");
            error = BADSIGLEN;
        }else if(offset == BADFORMAT){
            strcat(buff, "Signature contains incorrect characters.\n");
            error = offset;
        }else{
            if(hex2bin(sig_hex + offset, 192, sig_bin, 96) == 0) {
                strcat(buff, "Failed converting signature to binary array\n");
                error = HEX2BINERR;
            }else{
                blst_p2_uncompress(sig, sig_bin);
            }
        }
        return error;
}

/*
Generates random key. Response is dumped to 'buff'
*/


int keygen(char* data, char* buff){
    int keystore_size = get_keystore_size();

    if(keystore_size < 10){
        // key_info is an optional parameter.  This parameter MAY be used to derive
        // multiple independent keys from the same IKM.  By default, key_info is the empty string.
        char info[] = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

       //check if data string is empty
        if(strlen(data) != 0){
            if(strlen(data) <= strlen(info)){
                    strcpy(info, data);
            }else{
                    strncpy(info, data, sizeof(info));
            }
        }

        ikm_sk(info);
    
        //The secret key allow us to generate the associated public key
        blst_p1 pk;
        byte out[48];
        char public_key_hex[96];
        sk_to_pk(&pk);
        pk_serialize(out, pk);
        strcat(buff, "Public key: \n");
        if(bin2hex(out, sizeof(out), public_key_hex, sizeof(public_key_hex)) == 0) {
          strcat(buff, "Failed converting binary key to string\n");
        return BIN2HEXERR;
        }
        else
        {
        store_pk(public_key_hex);
        print_pk(public_key_hex, buff);
        }
        
    }else{
        strcat(buff, "Can't generate more keys. Limit reached.\n");
        return KEYSLIMIT;
    }
}

/*
Signs message with given public key. Public key must be stored. Response is dumped to 'buff'
*/
int signature(char* pk, char* msg, char* buff){
    //Message examples
    //char * msg_hex = "5656565656565656565656565656565656565656565656565656565656565656";
    //char * msg_hex = "b6bb8f3765f93f4f1e7c7348479289c9261399a3c6906685e320071a1a13955c";

    int offset = parse_hex(pk, 96);

    if(offset >= 0){
        if(pk_in_keystore(pk, offset) != -1){
            int len = msg_len(msg);
            uint8_t msg_bin[len/2 + len%2];
            if(msg_parse(msg, msg_bin, len, buff) != 1){
                blst_p2 hash;
                get_point_from_msg(&hash, msg_bin, len/2 + len%2);

                blst_p2 sig;
                byte sig_bin[96];
                char sig_hex[192];

                sign_pk(&sig, &hash);
                sig_serialize(sig_bin, sig);
                if(bin2hex(sig_bin, sizeof(sig_bin), sig_hex, sizeof(sig_hex)) == 0) {
                    strcat(buff, "Failed converting binary signature to string\n");
                }
                strcpy(buff, sig_hex);
            }
        }else{
            strcat(buff, "Public key isn't stored\n");
            return PKNOTFOUND;
        }
    }else if(offset == BADLEN){
        strcat(buff, "Incorrect public key length. It must be 96 characters long\n");
        return BADPKLEN;
    }else{
        strcat(buff, "Public key contains incorrect characters.\n");
        return BADFORMAT;
    }
return OK;
}

/*
Verifies signature of given message and public key
*/

int verify(char* pk, char* msg, char* sig, char* buff){
    char dst[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_"; //IETF BLS Signature V4

    blst_p1_affine pk_bin;
    blst_p2_affine sig_bin;
    int len = msg_len(msg);
    uint8_t msg_bin[len/2 + len%2];
    if((pk_parse(pk, &pk_bin, buff) || msg_parse(msg, msg_bin, len, buff) || sig_parse(sig, &sig_bin, buff)) == 0){
        if(blst_core_verify_pk_in_g1(&pk_bin, &sig_bin, 1, msg_bin, len/2 + len%2, dst, sizeof(dst)-1, NULL, 0) != BLST_SUCCESS){
            strcat(buff, "BLSTFAIL\n");
            return BLSTFAIL;
        }
        else {
            strcat(buff, "BLSTSUCCESS\n");
            return BLSTSUCCESS;
        }
    }
}
/*
Get array of stored public keys in buffer 'buff'
*/

int print_keys_Json(char* buff){
    int keystore_size = get_keystore_size();
    char public_keys_hex_store[keystore_size][96];
    getkeys(public_keys_hex_store);
    
        strcat(buff, "{\"keys\":[\"");
        for(int i = 0; i < keystore_size; i++){
            for(int j = 0; j < 96; j++){
                char str[2] = {public_keys_hex_store[i][j], '\0'};
                strcat(buff, str);
            }
            if (i + 1 < keystore_size){
                strcat(buff, "\",\n\"");
            }                     
        
        }
        strcat(buff, "\"]}\n");
               
    return keystore_size;
}

/*
Delete all stored public and secret keys. Response is dumped to 'buff'
*/


void resetc(char* buff){
     reset();
    strcat(buff, "Keys deleted\n");
}

/*
Import given secret key. Derived public key and errors are dumped to 'buff'
*/

/*
Import given secret key. Derived public key and errors are dumped to 'buff'
*/
int import(char* sk, char* buff){
    if(get_keystore_size() < 10){
        int offset = parse_hex(sk, 64);

        if(offset >= 0){
            byte sk_bin[32];
            if(hex2bin(sk + offset, 64, sk_bin, 32) == 0){
                strcat(buff, "Failed converting hex to bin\n");

            }else{
                blst_scalar sk_imp;
                blst_scalar_from_bendian(&sk_imp, sk_bin);
                if(import_sk(&sk_imp) == 0){
                    blst_p1 pk;
                    sk_to_pk(&pk);
                    byte pk_bin[48];
                    pk_serialize(pk_bin, pk);
                    char pk_hex[96];
                    if(bin2hex(pk_bin, 48, pk_hex, 96) == 0){
                        strcat(buff, "Failed converting bin to hex\n");
                    }else{
                        store_pk(pk_hex);
                        print_pk(pk_hex, buff);
                        return OK;

                    }
                }else{
                        strcat(buff, "Key already imported\n");
                }
            }
        }else if(offset == BADFORMAT){
            strcat(buff, "Incorrect characters\n");
        }else{
            strcat(buff, "Incorrect secret key length\n");
            return BADSKLEN;  

        }
    }else{
            strcat(buff, "Limit reached\n");
            return KEYSLIMIT;

    }
}

/*
    Returns 0 on succes and error number on error
*/
int get_decryption_key_scrypt(char* password, int dklen, int n,  int r, int p, char* salt_hex, unsigned char* decryption_key){
    #ifndef NRF
    int error;

    int salt_len = strlen(salt_hex)/2;
    unsigned char salt_bin[salt_len];

    if(hex2bin(salt_hex, strlen(salt_hex), salt_bin, strlen(salt_hex)/2) != strlen(salt_hex)/2){
        return HEX2BINERR;
    }

    //n are iteration, in this function they convert n to the power of 2, in the other function they take n as the exponent of 2
    wc_scrypt_ex(decryption_key, password, strlen(password), salt_bin, salt_len, (word32) n, r, p, dklen);

    return 0;
    #else
    return -1;
    #endif
}

/*
    Returns 0 on succes and error number on error
*/
int verify_password(char* checksum_message_hex, char* cipher_message_hex, unsigned char* decription_key){
    int error;
  
    int cipher_message_len = strlen(cipher_message_hex)/2;
    unsigned char cipher_message_bin[cipher_message_len];
    if(hex2bin(cipher_message_hex, strlen(cipher_message_hex), cipher_message_bin, strlen(cipher_message_hex)/2) == 0){
        return HEX2BINERR;
    }

    unsigned char pre_image[16 + cipher_message_len];
    for(int i = 0; i < 16; ++i){
        pre_image[i] = decription_key[16 + i];
    }

    for(int i = 16; i < (16 + cipher_message_len); ++i){
        pre_image[i] = cipher_message_bin[i - 16];
    }

    unsigned char checksum_hash[32];

    #ifndef NRF
    Sha256 sha;
    wc_InitSha256(&sha);
    wc_Sha256Update(&sha, pre_image, 16 + cipher_message_len);
    wc_Sha256Final(&sha, checksum_hash);
    #else
    hash(checksum_hash, pre_image, 16 + cipher_message_len);
    #endif

    char checksum_str[64];
    if(bin2hex(checksum_hash, 32, checksum_str, 64) == 0){
        return BIN2HEXERR;
    }
    
    if(strncmp(checksum_message_hex, checksum_str, 64) == 0){
        return 0;
    }else{
        return INVPASSWORD;
    }
}

/*
    Returns 0 on succes and error number on error
*/
int get_private_key(char* cipher_message, char* iv_str, unsigned char* decription_key, char* private_key){
    int error;

    int cipher_message_len = strlen(cipher_message)/2;
    unsigned char cipher_message_bin[cipher_message_len];
    if(hex2bin(cipher_message, strlen(cipher_message), cipher_message_bin, strlen(cipher_message)/2) != strlen(cipher_message)/2){
        return HEX2BINERR;
    }

    int iv_len = strlen(iv_str)/2;
    unsigned char iv_bin[iv_len];
    if(hex2bin(iv_str, strlen(iv_str), iv_bin, strlen(iv_str)/2) != strlen(iv_str)/2){
        return HEX2BINERR;
    }
    #ifndef NRF
    Aes aes;
    wc_AesSetKey(&aes, decription_key, 16, iv_bin, AES_ENCRYPTION);
    wc_AesCtrEncrypt(&aes, private_key, cipher_message_bin, cipher_message_len);
    #else
    aes128ctr(decription_key, iv_bin, cipher_message_bin, private_key);
    #endif

    char private_key_str[65];

    if(bin2hex(private_key, 32, private_key_str, 64) != 64){
        return BIN2HEXERR;
    }
    private_key[64] = '\0';

    char buff[6500];
    printf("\n\n%s\n\n", private_key_str);
    import(private_key_str, buff);

    return 0;
}


