#ifndef bls_hsm_h
#define bls_hsm_h
#include "blst.h"
#include "common.h"

#define MAX_KEYSTORE_SIZE 10

blst_scalar secret_keys_store[MAX_KEYSTORE_SIZE];
char public_keys_hex_store[MAX_KEYSTORE_SIZE][96];
int keystore_size = 0;

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
            if(buff != NULL) strcat(buff, "Incorrect public key length. It must be 96 characters long.\n");
            error = BADPKLEN;
        }else if(offset == BADFORMAT){
                if(buff != NULL) strcat(buff, "Public key contains incorrect characters.\n");
                error = offset;
        }else{
            if(hex2bin(pk_hex + offset, 96, pk_bin, 48) == 0) {
                if(buff != NULL) strcat(buff, "Failed converting public key to binary array\n");
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
            if(buff != NULL) strcat(buff, "Message contains incorrect characters.\n");
            error = offset;
        }else if(offset == BADLEN){
            if(buff != NULL) strcat(buff, "Message contains incorrect characters.\n");
            error = offset;
        }else{
            if(hex2bin(msg + offset, len, msg_bin, len/2 + len%2) == 0) {
                if(buff != NULL) strcat(buff, "Incorrect message length.\n");
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
            if(buff != NULL) strcat(buff, "Incorrect signature length. It must be 192 characters long.\n");
            error = BADSIGLEN;
        }else if(offset == BADFORMAT){
            if(buff != NULL) strcat(buff, "Signature contains incorrect characters.\n");
            error = offset;
        }else{
            if(hex2bin(sig_hex + offset, 192, sig_bin, 96) == 0) {
                if(buff != NULL) strcat(buff, "Failed converting signature to binary array\n");
                error = HEX2BINERR;
            }else{
                blst_p2_uncompress(sig, sig_bin);
            }
        }
        return error;
}

#ifdef NRF
__TZ_NONSECURE_ENTRY_FUNC
#endif
/**
 * @brief Returns the number of stored keys
*/
int get_keystore_size(){
        return keystore_size;
}

#ifdef NRF
__TZ_NONSECURE_ENTRY_FUNC
#endif
/**
 * @brief Store given public key
 * 
 * @param public_key_hex Public key to be stored. Length must be 96
*/
void store_pk(char* public_key_hex){
        int cont = keystore_size - 1;
        for(int i = 0; i < 96; i++){
            public_keys_hex_store[cont][i] = public_key_hex[i];
        }
}

#ifdef NRF
__TZ_NONSECURE_ENTRY_FUNC
#endif
/**
 * @brief Gets the key stored at the given index. Returns 0 on success and -1 if index is incorrect
 * 
 * @param index Index of the key to be obtained
 * @param public_key_hex String that contains public key
*/
int get_key(int index, char* public_key_hex){
        if(index >= keystore_size) return -1;
        for(int i = 0; i < 96; i++){
            public_key_hex[i] = public_keys_hex_store[index][i];
        }
        return 0;
}

#ifdef NRF
__TZ_NONSECURE_ENTRY_FUNC
#endif
/**
 * @brief Gets all stored keys
 * 
 * @param public_key_hex String array that contains public keys
*/
void get_keys(char public_keys_hex_store_ns[keystore_size][96]){
        for(int i = 0; i < keystore_size; i++){
            for(int j = 0; j < 96; j++){
                public_keys_hex_store_ns[i][j] = public_keys_hex_store[i][j];
            }
        }
}

#ifdef NRF
__TZ_NONSECURE_ENTRY_FUNC
#endif
void hash(uint8_t* out, uint8_t* in, size_t size){
#ifdef NRF
	    ocrypto_sha256(out, in, size);
#else // TODO:  implement hash in c
        for(int i = 0; i < 32; i++){
            out[i] = in[i];
        }
#endif
}


#ifdef NRF
__TZ_NONSECURE_ENTRY_FUNC
void aes128ctr(uint8_t* key, uint8_t* iv, uint8_t* in, uint8_t* out){
    ocrypto_aes_ctr_ctx ctx;
    ocrypto_aes_ctr_init(&ctx, key, 16, iv);
    ocrypto_aes_ctr_decrypt(&ctx, out, in, 32);
}
#endif

#ifdef NRF
__TZ_NONSECURE_ENTRY_FUNC
#endif
/**
 * @brief Searchs given public key. If found, returns index. If not found, returns-1
 * 
 * @param public_key_hex String that contains public key
 * @param offset Offset to first char to hex string
*/
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
                return i;
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

#ifdef NRF
__TZ_NONSECURE_ENTRY_FUNC
#endif
/**
 * @brief Generates a new pair of public key and secret key. Returns index of the generated pair
 * 
 * @param info Optional parameter. This parameter is used to derive multiple independent keys from the same ikm
*/
int secure_keygen(char* info){
        if(keystore_size >= MAX_KEYSTORE_SIZE) return -KEYSLIMIT;

        // For security, IKM MUST be infeasible to guess, e.g., generated by a trusted
        // source of randomness. IKM MUST be at least 32 bytes long, but it MAY be longer.
        unsigned char ikm[32];
        blst_p1 pk;
        byte out[48];
        char default_info[] = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#ifdef NRF
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

        // key_info is an optional parameter.  This parameter MAY be used to derive
        // multiple independent keys from the same IKM.  By default, key_info is the empty string.
        if (info == NULL) info = default_info;

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

#ifdef NRF
__TZ_NONSECURE_ENTRY_FUNC
#endif
/**
 * @brief Signs given message with given public key. This public key must be stored (with its corresponding secret key). Returns 0 if success
 * 
 * @param pk Public key
 * @param msg Message to be signed
 * @param sign Resulting signature
*/
int sign_pk(char* pk, char* msg, char* sign){
        int pk_index = pk_in_keystore(pk, 0);
        if( pk_index != -1){
            int len = msg_len(msg);
            uint8_t msg_bin[len/2 + len%2];
            if(msg_parse(msg, msg_bin, len, sign) != 1){
                blst_p2 hash;
                get_point_from_msg(&hash, msg_bin, len/2 + len%2);

                blst_p2 sig;
                byte sig_bin[96];
                char sig_hex[192];
                blst_sign_pk_in_g1(&sig, &hash, secret_keys_store + pk_index*sizeof(blst_scalar));
                sig_serialize(sig_bin, sig);
                if(bin2hex(sig_bin, sizeof(sig_bin), sig_hex, sizeof(sig_hex)) == 0) {
                    return BIN2HEXERR;
                }
                strcpy(sign, sig_hex);
                return 0;
            }
        }else{
            return PKNOTFOUND;
        }      
}

#ifdef NRF
__TZ_NONSECURE_ENTRY_FUNC
#endif
/**
 * @brief Verifies signature of given message and public key. Returns BLSTSUCCESS if success and BLSTFAIL otherwise
 * 
 * @param pk Public key
 * @param msg Message signed
 * @param sign Signature
*/
int verify_sign(char* pk, char* msg, char* sign){
        char dst[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_"; //IETF BLS Signature V4

        blst_p1_affine pk_bin;
        blst_p2_affine sig_bin;
        int len = msg_len(msg);
        uint8_t msg_bin[len/2 + len%2];
        if((pk_parse(pk, &pk_bin, NULL) || msg_parse(msg, msg_bin, len, NULL) || sig_parse(sign, &sig_bin, NULL)) == 0){
            if(blst_core_verify_pk_in_g1(&pk_bin, &sig_bin, 1, msg_bin, len/2 + len%2, dst, sizeof(dst)-1, NULL, 0) != BLST_SUCCESS){
                return BLSTFAIL;
            }else{
                return BLSTSUCCESS;
            }
        }else{
            return -1;
        }
}

#ifdef NRF
__TZ_NONSECURE_ENTRY_FUNC
#endif
/**
 * @brief Deletes all keys
*/
void reset(){
        memset(secret_keys_store, 0, sizeof(secret_keys_store));
        memset(public_keys_hex_store, 0, 960);
        keystore_size = 0;
}

#ifdef NRF
__TZ_NONSECURE_ENTRY_FUNC
#endif
/**
 * @brief Generates a public key from given secret and store both. Returns index of stored pair
 * 
 * @param sk_ Secret key
*/
int import_sk(char* sk_){
        if(keystore_size >= MAX_KEYSTORE_SIZE) return -KEYSLIMIT;

        byte sk_bin[32];
        blst_scalar sk_imp;
        blst_scalar_from_bendian(&sk_imp, sk_bin);        
        int ret = 0;
        int c = 0;

        if(hex2bin(sk_, 64, sk_bin, 32) == 0){
            return -HEX2BINERR;
        }        

        if(keystore_size == 0){
                secret_keys_store[keystore_size] = sk_imp;
        }else{
            for(int i = 0; i < keystore_size; i++){
                for(int x = 0; x < 32; x++){
                    if(secret_keys_store[i].b[x] != (sk_imp).b[x]){
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
                        secret_keys_store[keystore_size] = sk_imp;
                        break;
                    }
                }
            }
        }
        
        // Public key
        if(ret != 0){
            return EXISTINGKEY;
        }

        blst_p1 pk;
        blst_sk_to_pk_in_g1(&pk, secret_keys_store + keystore_size*sizeof(blst_scalar));
        byte pk_bin[48];
        blst_p1_compress(pk_bin, &pk);
        if(bin2hex(pk_bin, sizeof(pk_bin), public_keys_hex_store[keystore_size], sizeof(public_keys_hex_store[keystore_size])) == 0){
            return -BIN2HEXERR;
        }
        
        keystore_size++;
        return keystore_size - 1;
}
#endif