#ifndef bls_hsm_h
#define bls_hsm_h
#include "blst.h"
#include <string.h>
#include "common.h"

#define MAX_KEYSTORE_SIZE 10

void pk_serialize(byte* out, blst_p1 pk);
void sig_serialize(byte* out2, blst_p2 sig);
void get_point_from_msg(blst_p2* hash, uint8_t* msg_bin, int len);
int pk_parse(char* pk_hex, blst_p1_affine* pk, char* buff);
int msg_parse(char* msg, uint8_t* msg_bin, int len, char* buff);
int sig_parse(char* sig_hex, blst_p2_affine* sig, char* buff);

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

blst_scalar sk;
blst_scalar secret_keys_store[10];
blst_scalar sk_sign;
char public_keys_hex_store[10][96];
int keystore_size = 0;

int get_keystore_size(){
        return keystore_size;
}

void store_pk(char* public_key_hex){
        int cont = keystore_size - 1;
        for(int i = 0; i < 96; i++){
            public_keys_hex_store[cont][i] = public_key_hex[i];
        }
}

int get_key(int index, char* public_key_hex){
        if(index >= keystore_size) return -1;
        for(int i = 0; i < 96; i++){
            public_key_hex[i] = public_keys_hex_store[index][i];
        }
        return 0;
}

void getkeys(char public_keys_hex_store_ns[keystore_size][96]){
        for(int i = 0; i < keystore_size; i++){
            for(int j = 0; j < 96; j++){
                public_keys_hex_store_ns[i][j] = public_keys_hex_store[i][j];
            }
        }
}

void hash(uint8_t* out, uint8_t* in, size_t size){
#ifdef NRF
	    // TODO: ocrypto_sha256 doesnt work (v2.0.0)
        // ocrypto_sha256(out, in, size);
        for(int i = 0; i < 32; i++){
            out[i] = in[i];
        }
#else // TODO:  implement hash in c
        for(int i = 0; i < 32; i++){
            out[i] = in[i];
        }
#endif
}


#ifdef NRF
void aes128ctr(uint8_t* key, uint8_t* iv, uint8_t* in, uint8_t* out){
    // TODO: doesnt work (v2.0.0)
    /*ocrypto_aes_ctr_ctx ctx;
    ocrypto_aes_ctr_init(&ctx, key, 16, iv);
    ocrypto_aes_ctr_decrypt(&ctx, out, in, 32);*/
}
#endif

int pk_in_keystore(char * public_key_hex, int offset){

        int ret = 0;

        int c = 0;

        if(keystore_size == 0){
                ret = -1;
        }

        for(int i = 0; i < keystore_size; i++){
            for(int j = 0; j < 96; j++){
                if(public_key_hex[j + offset] != public_keys_hex_store[i][j]){
                    c = 1;
                    break;
                }
            }
            if (c == 0){
                sk_sign = secret_keys_store[i];
                break;
            } else {
                if((i+1) < keystore_size){
                    c = 0;
                }else{
                    ret = -1;
                }
            }
        }
        return ret;
}

int secure_keygen(char* info){
        if(keystore_size >= MAX_KEYSTORE_SIZE) return -KEYSLIMIT;

        // For security, IKM MUST be infeasible to guess, e.g., generated by a trusted
        // source of randomness. IKM MUST be at least 32 bytes long, but it MAY be longer.
        unsigned char ikm[32];
        blst_p1 pk;
        byte out[48];
#ifdef NRF
	    const int random_number_len = 144;     
        uint8_t random_number[random_number_len];
        size_t olen = random_number_len;
        int ret;

        // TODO:
        /*ret = nrf_cc3xx_platform_ctr_drbg_get(NULL, random_number, random_number_len, &olen);
        
        ocrypto_sha256(ikm, random_number, random_number_len);*/
        for(int i = 0; i < 32; i++){
            ikm[i] = rand();
        }
#else
        for(int i = 0; i < 32; i++){
            ikm[i] = rand();
        }
#endif

        // Secret key (256-bit scalar)
        blst_keygen(secret_keys_store + keystore_size*sizeof(blst_scalar), ikm, sizeof(ikm), info, sizeof(info));

        // Public key
        blst_sk_to_pk_in_g1(&pk, secret_keys_store + keystore_size*sizeof(blst_scalar));
        blst_p1_compress(out, &pk);
        if(bin2hex(out, sizeof(out), public_keys_hex_store[keystore_size], sizeof(public_keys_hex_store[keystore_size])) == 0){
            return -BIN2HEXERR;
        }

        keystore_size++;

        return keystore_size-1;
}

void ikm_sk(char* info){
        // For security, IKM MUST be infeasible to guess, e.g., generated by a trusted
        // source of randomness. IKM MUST be at least 32 bytes long, but it MAY be longer.
        unsigned char ikm[32];
#ifdef NRF_ //TODO: nrf_cc3xx_platform_ctr_drbg_get doesn't work (v2.0.0). 
	    const int random_number_len = 144;     
        uint8_t random_number[random_number_len];
        size_t olen = random_number_len;
        int ret;

        ret = nrf_cc3xx_platform_ctr_drbg_get(NULL, random_number, random_number_len, &olen);
        
        ocrypto_sha256(ikm, random_number, random_number_len);
#else
        for(int i = 0; i < 32; i++){
            ikm[i] = rand();
        }
#endif

        
        //Secret key (256-bit scalar)
        blst_keygen(&sk, ikm, sizeof(ikm), info, sizeof(info));
        secret_keys_store[keystore_size] = sk;
        keystore_size++;
}

void sk_to_pk(blst_p1* pk){
        blst_sk_to_pk_in_g1(pk, &sk);
}

void sign_pk(blst_p2* sig, blst_p2* hash){
        blst_sign_pk_in_g1(sig, hash, &sk_sign);
}

void reset(){
        memset(secret_keys_store, 0, sizeof(secret_keys_store));
        memset(public_keys_hex_store, 0, 960);
        keystore_size = 0;
}

int import_sk(blst_scalar* sk_imp){
        int ret = 0;

        int c = 0;

        if(keystore_size == 0){
                secret_keys_store[keystore_size] = *sk_imp;
                keystore_size++;
                sk = *sk_imp;
        }else{
            for(int i = 0; i < keystore_size; i++){
                for(int x = 0; x < 32; x++){
                    if(secret_keys_store[i].b[x] != (*sk_imp).b[x]){
                        c = 1;
                        break;
                    }
            }
                if (c == 0){
                    ret = -1;
                    break;
                } else {
                    if((i+1) < keystore_size){
                        c = 0;
                    }else{
                        secret_keys_store[keystore_size] = *sk_imp;
                        keystore_size++;
                        sk = *sk_imp;
                        break;
                    }
                }
            }
        }
        return ret;
}
#endif