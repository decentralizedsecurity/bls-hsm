#ifndef bls_hsm_h
#define bls_hsm_h
#include "blst.h"
#include "common.h"
#ifdef NRF
#include <zephyr/sys/util.h>
#endif
#include <stdio.h>
#include <string.h>

// LOG System Wrapper
#if defined(NO_LOGS)
void do_nothing(){
}
#define ERR_LOG(msg) do_nothing()
#define WRNG_LOG(msg) do_nothing()
#define INF_LOG(msg) do_nothing()
#define DEBUG_LOG(msg) do_nothing()
#elif defined(NRF) && defined(TFM) //&& 0 // 0 due to errors building
#define ERR_MSG 1
#define WRNG_MSG 2
void print_colored_error(const char * msg, int size, int msg_type){
    char buffer[size+12];
    if(msg_type == ERR_MSG) sprintf(buffer, "\033[1;31m%s\033[0m", msg);
    else if (msg_type == WRNG_MSG) sprintf(buffer, "\033[1;33m%s\033[0m", msg);
    else sprintf(buffer, "%s", msg);
    SPMLOG_ERRMSG(buffer);
} 
#define ERR_LOG(msg) print_colored_error(msg, strlen(msg), ERR_MSG)
#define WRNG_LOG(msg) print_colored_error(msg, strlen(msg), WRNG_MSG)
#define INF_LOG(msg) SPMLOG_INFMSG(msg)
#define DEBUG_LOG(msg) SPMLOG_DBGMSG(msg)
#elif defined(NRF) && !defined(TFM)
#define ERR_LOG(msg) TODO(msg)
#define WRNG_LOG(msg) TODO(msg)
#define INF_LOG(msg) TODO(msg)
#define DEBUG_LOG(msg) TODO(msg)
#else
#define ERR_LOG(msg) printf(msg);
#define WRNG_LOG(msg) printf(msg);
#define INF_LOG(msg) printf(msg);
#define DEBUG_LOG(msg) printf(msg);
#endif

#define MAX_KEYSTORE_SIZE 10

blst_scalar secret_keys_store[MAX_KEYSTORE_SIZE];
char public_keys_hex_store[MAX_KEYSTORE_SIZE][96];
int keystore_size = 0;

// MAYBE SHOULD BE DELETED
int char2hex_todo(char c, uint8_t *x)
{
	if (c >= '0' && c <= '9') {
		*x = c - '0';
	} else if (c >= 'a' && c <= 'f') {
		*x = c - 'a' + 10;
	} else if (c >= 'A' && c <= 'F') {
		*x = c - 'A' + 10;
	} else {
		return -22;
	}

	return 0;
}
int hex2char_todo(uint8_t x, char *c)
{
	if (x <= 9) {
		*c = x + '0';
	} else  if (x <= 15) {
		*c = x - 10 + 'a';
	} else {
		return -22;
	}

	return 0;
}
size_t bin2hex_todo(const uint8_t *buf, size_t buflen, char *hex, size_t hexlen)
{
	if ((hexlen + 1) < buflen * 2) {
		return 0;
	}

	for (size_t i = 0; i < buflen; i++) {
		if (hex2char_todo(buf[i] >> 4, &hex[2 * i]) < 0) {
			return 0;
		}
		if (hex2char_todo(buf[i] & 0xf, &hex[2 * i + 1]) < 0) {
			return 0;
		}
	}

	hex[2 * buflen] = '\0';
	return 2 * buflen;
}
size_t hex2bin_todo(const char *hex, size_t hexlen, uint8_t *buf, size_t buflen)
{
	uint8_t dec;

	if (buflen < hexlen / 2 + hexlen % 2) {
		return 0;
	}

	/* if hexlen is uneven, insert leading zero nibble */
	if (hexlen % 2) {
		if (char2hex_todo(hex[0], &dec) < 0) {
			return 0;
		}
		buf[0] = dec;
		hex++;
		buf++;
	}

	/* regular hex conversion */
	for (size_t i = 0; i < hexlen / 2; i++) {
		if (char2hex_todo(hex[2 * i], &dec) < 0) {
			return 0;
		}
		buf[i] = dec << 4;

		if (char2hex_todo(hex[2 * i + 1], &dec) < 0) {
			return 0;
		}
		buf[i] += dec;
	}

	return hexlen / 2 + hexlen % 2;
}
// ----------------------------------------------

#ifdef TFM
#include <psa/storage_common.h>
#include <psa/protected_storage.h>
#define UID_KEYSTORE_SIZE 11
#define UID_KEYSTORE 1
psa_status_t status;
size_t bytes_read;
psa_status_t init(){
    // Initialize PSA Crypto
	status = psa_crypto_init();
	if (status != PSA_SUCCESS) return (psa_status_t)-1;

    status = psa_ps_get((psa_storage_uid_t) UID_KEYSTORE_SIZE, 0, sizeof(int), &keystore_size, &bytes_read);
	if (status != PSA_SUCCESS) {
        keystore_size = 0;
        return PSA_SUCCESS;
	} else {
        status =psa_ps_get((psa_storage_uid_t) UID_KEYSTORE, 0, sizeof(blst_scalar)*keystore_size, secret_keys_store, &bytes_read);
        if (status != PSA_SUCCESS){
            return status;
        }
        for(int i = 1; i <= keystore_size; i++){   
            blst_p1 pk;
            uint8_t pk_bin[48];
            uint8_t pk_hex[97];
            blst_sk_to_pk_in_g1(&pk, &secret_keys_store[i-1]);
            blst_p1_compress(pk_bin, &pk);
            if(bin2hex_todo(pk_bin, sizeof(pk_bin), pk_hex, 96) == 0){
                return -BIN2HEXERR;
            }
            memcpy(public_keys_hex_store[i-1], pk_hex, 96);
        }
        return PSA_SUCCESS;
    }
}
#endif

int add_sk(blst_scalar sk){
    memcpy(&secret_keys_store[keystore_size], &sk, sizeof(blst_scalar));
    keystore_size++;
#ifdef TFM
    status = psa_ps_set((psa_storage_uid_t)UID_KEYSTORE, sizeof(secret_keys_store), secret_keys_store, (psa_storage_create_flags_t) PSA_STORAGE_FLAG_NONE);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_ps_set((psa_storage_uid_t)UID_KEYSTORE_SIZE, sizeof(int), &keystore_size, (psa_storage_create_flags_t) PSA_STORAGE_FLAG_NONE);
    if (status != PSA_SUCCESS) {
        return status;
    }
#endif
    return 0;
}

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
        blst_hash_to_g2(hash, msg_bin, len, (byte *) dst, sizeof(dst)-1, NULL, 0);
}

/*
Derive 'pk' from hexadecimal string 'pk_hex'. Errors are dumped to 'buff'
On error returns 1
*/
int pk_parse(const char* pk_hex, blst_p1_affine* pk, char* buff){
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
            if(hex2bin_todo(pk_hex + offset, 96, pk_bin, 48) == 0) {
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
int msg_parse(const char* msg, uint8_t* msg_bin, int len, char* buff){

        int offset = parse_hex(msg, len);
        int error = 0;
        if(offset == BADFORMAT){
            if(buff != NULL) strcat(buff, "Message contains incorrect characters.\n");
            error = offset;
        }else if(offset == BADLEN){
            if(buff != NULL) strcat(buff, "Message contains incorrect characters.\n");
            error = offset;
        }else{
            if(hex2bin_todo(msg + offset, len, msg_bin, len/2 + len%2) == 0) {
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
int sig_parse(const char* sig_hex, blst_p2_affine* sig, char* buff){
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
            if(hex2bin_todo(sig_hex + offset, 192, sig_bin, 96) == 0) {
                if(buff != NULL) strcat(buff, "Failed converting signature to binary array\n");
                error = HEX2BINERR;
            }else{
                blst_p2_uncompress(sig, sig_bin);
            }
        }
        return error;
}

/**
 * @brief Returns the number of stored keys
*/
int get_keystore_size(){
        //DEBUG_LOG("Returned store size\r\n");
        return keystore_size;
}

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
        INF_LOG("Given public key stored\r\n");
}

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

/**
 * @brief Gets all stored keys
 * 
 * @param public_key_hex String array that contains public keys
*/
void get_keys(char public_keys_hex_store_ns[keystore_size][96]){
        for(int i = 0; i < 1; i++){
            for(int j = 0; j < 96; j++){
                public_keys_hex_store_ns[i][j] = public_keys_hex_store[i][j];
            }
        }
        INF_LOG("All keys have been obtained\r\n");
}

#ifdef NRF
#include <psa/crypto.h>
#include <psa/crypto_extra.h>
#include <psa/crypto_values.h>
#endif

void hash(uint8_t* out, uint8_t* in, size_t size){
#ifdef NRF
    uint32_t olen;
	psa_status_t status;

    /* Initialize PSA Crypto */
    status = psa_crypto_init();
	if (status != PSA_SUCCESS)
		return;

	/* Calculate the SHA256 hash */
	status = psa_hash_compute(
		PSA_ALG_SHA_256, in, size, out, 32, &olen);
	if (status != PSA_SUCCESS) {
		return;
	}
#else // TODO:  implement hash in c
        for(int i = 0; i < 32; i++){
            out[i] = in[i];
        }
#endif
}


#ifdef NRF
// TODO

void aes128ctr(uint8_t* key, uint8_t* iv, uint8_t* in, uint8_t* out){
    psa_status_t status;
    psa_key_handle_t key_handle;
    /* Initialize PSA Crypto */
	status = psa_crypto_init();
	if (status != PSA_SUCCESS)
		return;

    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;

	psa_set_key_usage_flags(&key_attributes,
				PSA_KEY_USAGE_ENCRYPT);
	psa_set_key_lifetime(&key_attributes, PSA_KEY_LIFETIME_VOLATILE);
	psa_set_key_algorithm(&key_attributes, PSA_ALG_CTR);
	psa_set_key_type(&key_attributes, PSA_KEY_TYPE_AES);
	psa_set_key_bits(&key_attributes, 128);

    status = psa_import_key(&key_attributes, key, 16, &key_handle);
	if (status != PSA_SUCCESS) {
		return;
	}
    psa_reset_key_attributes(&key_attributes);

    uint32_t olen;
	psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;

	/* Setup the decryption operation */
	status = psa_cipher_encrypt_setup(&operation, key_handle, PSA_ALG_CTR);
	if (status != PSA_SUCCESS) {
		return;
	}

	/* Set the IV to the one generated during encryption */
	status = psa_cipher_set_iv(&operation, iv, 16);
	if (status != PSA_SUCCESS) {
		return;
	}

	/* Perform the decryption */
	status = psa_cipher_update(&operation,
							   in,
							   32,
							   out,
							   32, &olen);
	if (status != PSA_SUCCESS) {
		return;
	}

	/* Finalize the decryption */
	status = psa_cipher_finish(&operation,
							   out + olen,
							   32 - olen,
							   &olen);
	if (status != PSA_SUCCESS) {
		return;
	}
}
#endif

/**
 * @brief Searchs given public key. If found, returns index. If not found, returns-1
 * 
 * @param public_key_hex String that contains public key
 * @param offset Offset to first char to hex string
*/
int pk_in_keystore(const char * public_key_hex, int offset){

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

/**
 * @brief Generates a new pair of public key and secret key. Returns index of the generated pair
 * 
 * @param info Optional parameter. This parameter is used to derive multiple independent keys from the same ikm
*/
int secure_keygen(const char* info){
        if(keystore_size >= MAX_KEYSTORE_SIZE){
            WRNG_LOG("Keystore is full. No more keys can be generated\r\n");
            return -KEYSLIMIT;
        }
        // For security, IKM MUST be infeasible to guess, e.g., generated by a trusted
        // source of randomness. IKM MUST be at least 32 bytes long, but it MAY be longer.
        unsigned char ikm[32];
        blst_p1 pk;
        uint8_t pk_hex[97];
        byte out[48];
        char default_info[] = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#ifdef NRF
	    #ifndef TFM
        for(int i = 0; i < 32; i++){
            ikm[i] = 0;
        }
        #else
	    // Generate the random number
        psa_status_t status = psa_generate_random(ikm, sizeof(ikm));
        if (status != PSA_SUCCESS) {
            ERR_LOG("IKM not generated: psa_generate_random failed\r\n");
            return (psa_status_t)RNGERR;
        }
        #endif
#else
        for(int i = 0; i < 32; i++){
            ikm[i] = rand();
        }
#endif
        DEBUG_LOG("IKM generated successfully\r\n");

        // key_info is an optional parameter.  This parameter MAY be used to derive
        // multiple independent keys from the same IKM.  By default, key_info is the empty string.
        if (info == NULL) info = default_info;

        // Secret key (256-bit scalar)
        blst_scalar sk_gen;
        blst_keygen(&sk_gen, ikm, sizeof(ikm), (byte *) info, sizeof(info));
        add_sk(sk_gen);
        
        // Public key
        blst_sk_to_pk_in_g1(&pk, &sk_gen);
        blst_p1_compress(out, &pk);
        if(bin2hex_todo(out, sizeof(out), pk_hex, 97) == 0){
            ERR_LOG("BIN2HEX error\r\n");
            return -BIN2HEXERR;
        }
        memcpy(&public_keys_hex_store[keystore_size-1], pk_hex, 96);
        INF_LOG("A new pair of secret and private key have been generated\r\n");

        return keystore_size-1;
}

/**
 * @brief Signs given message with given public key. This public key must be stored (with its corresponding secret key). Returns 0 if success
 * 
 * @param pk Public key
 * @param msg Message to be signed
 * @param sign Resulting signature
*/
int sign_pk(const char* pk, const char* msg, char* sign){
        int pk_index = pk_in_keystore(pk, 0);
        if( pk_index != -1){
            int len = msg_len(msg);
            uint8_t msg_bin[len/2 + len%2];
            if(msg_parse(msg, msg_bin, len, sign) != 1){
                blst_p2 hash;
                get_point_from_msg(&hash, msg_bin, len/2 + len%2);

                blst_p2 sig;
                byte sig_bin[96];
                char sig_hex[193];
                //printf("SIGNATURE:\n - pk: %s\n - msg: %s\n", pk, msg);
                blst_sign_pk_in_g1(&sig, &hash, &secret_keys_store[pk_index]);
                sig_serialize(sig_bin, sig);
                if(bin2hex_todo(sig_bin, sizeof(sig_bin), sig_hex, sizeof(sig_hex)) == 0) {
                    ERR_LOG("Error signing message\r\n");
                    return BIN2HEXERR;
                }
                strcpy(sign, sig_hex);/**/
                INF_LOG("Message signed\r\n");
                return 0;
            }
        }else{
            ERR_LOG("Can't sign message. Password not found\r\n");
            return PKNOTFOUND;
        }   
        return -99; // TODO: Warning while building if there is no return at the end

}

/**
 * @brief Verifies signature of given message and public key. Returns BLSTSUCCESS if success and BLSTFAIL otherwise
 * 
 * @param pk Public key
 * @param msg Message signed
 * @param sign Signature
*/
int verify_sign(const char* pk, const char* msg, const char* sig){
        char dst[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_"; //IETF BLS Signature V4

        blst_p1_affine pk_bin;
        blst_p2_affine sig_bin;
        int len = msg_len(msg);
        uint8_t msg_bin[len/2 + len%2];
        if((pk_parse(pk, &pk_bin, NULL) || msg_parse(msg, msg_bin, len, NULL) || sig_parse(sig, &sig_bin, NULL)) == 0){
            if(blst_core_verify_pk_in_g1(&pk_bin, &sig_bin, 1, msg_bin, len/2 + len%2, (byte *) dst, sizeof(dst)-1, NULL, 0) != BLST_SUCCESS){
                return BLSTFAIL;
            }else{
                return BLSTSUCCESS;
            }
        }else{
            return -1;
        }
        return -8;
}

/**
 * @brief Deletes all keys
*/
void reset(){
        memset(secret_keys_store, 0, sizeof(secret_keys_store));        
#ifdef TFM
        int ksize = 0;
        status = psa_ps_set((psa_storage_uid_t)UID_KEYSTORE_SIZE, sizeof(int), &ksize, (psa_storage_create_flags_t) PSA_STORAGE_FLAG_NONE);
        for(psa_storage_uid_t i = 1; i <= keystore_size; i++){
            psa_ps_remove(i);
        }
#endif
        keystore_size = 0;
        memset(public_keys_hex_store, 0, 960);
        INF_LOG("All keys deleted\r\n");
}

/**
 * @brief Generates a public key from given secret and store both. Returns index of stored pair
 * 
 * @param sk Secret key
*/
int import_sk(char* sk){
        if(keystore_size >= MAX_KEYSTORE_SIZE) return -KEYSLIMIT;

        byte sk_bin[32];
        blst_scalar sk_imp;
	if(hex2bin_todo(sk, 64, sk_bin, 32) == 0){
            return -HEX2BINERR;
        }
        blst_scalar_from_bendian(&sk_imp, sk_bin);        
        int ret = 0;
        int c = 0;        

        if(keystore_size == 0){
            add_sk(sk_imp);
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
                        add_sk(sk_imp);
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
        blst_sk_to_pk_in_g1(&pk, &sk_imp);
        byte pk_bin[48];
        uint8_t pk_hex[97];
        blst_p1_compress(pk_bin, &pk);
        if(bin2hex_todo(pk_bin, sizeof(pk_bin), pk_hex, 97) == 0){
            return -BIN2HEXERR;
        }
        memcpy(public_keys_hex_store[keystore_size-1], pk_hex, 96);
        
        return keystore_size - 1;
}
#endif
