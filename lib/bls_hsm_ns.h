
#ifndef bls_hsm_ns_h
#define bls_hsm_ns_h

  
#include <stdint.h>

/*int pk_in_keystore(char * public_key_hex, int offset);
int secure_keygen(char* info);
int sign_pk(char* pk, char* msg, char* sign);
void reset();
void store_pk(char* public_key_hex);
int get_keystore_size();
int get_key(int index, char* public_key_hex);
void get_keys(char** public_keys_hex_store_ns);
int import_sk(char* sk);
int PBKDF2(uint8_t* salt, uint8_t* password, int it_cnt, uint8_t* key);
void aes128ctr(uint8_t* key, uint8_t* iv, uint8_t* in, uint8_t* out);*/

int get_keystore_length();
int keygen(char* data, char* buff);
void get_signature(char* pk, char* msg, char* signature);
int signature(char* pk, char* msg, char* buff);
int verify(char* pk, char* msg, char* sig, char* buff);
int get_pk(int index, char* pk_hex);
// Replacing the function "void dump_keys(char* buff)" with "int print_keys_Json(char* buff)"
int print_keys_Json(char* buff);
void resetc(char* buff);

int import(char* sk, char* buff);


int get_decryption_key_scrypt(char* password, int dklen, int n,  int r, int p, char* salt_hex, unsigned char* decryption_key);
//int get_decryption_key_pbkdf2(char* password, int dklen, int c, char* prf, char* salt_hex, unsigned char* decryption_key);
int verify_password(char* checksum_message_hex, char* cipher_message_hex, unsigned char* decription_key);
int get_private_key(char* cipher_message, char* iv_str, unsigned char* decription_key, char* private_key);

#endif
