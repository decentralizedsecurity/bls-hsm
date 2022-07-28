
#ifndef bls_hsm_ns_h
#define bls_hsm_ns_h

  
#ifdef TFM
#include "secure_partition_interface.h"
#else
#include "bls_hsm.h"
#endif


int keygen(char* data, char* buff);
void get_signature(char* pk, char* msg, char* signature);
int signature(char* pk, char* msg, char* buff);
int verify(char* pk, char* msg, char* sig, char* buff);
// Replacing the function "void dump_keys(char* buff)" with "int print_keys_Json(char* buff)"
int print_keys_Json(char* buff);
void resetc(char* buff);

int import(char* sk, char* buff);


int get_decryption_key_scrypt(char* password, int dklen, int n,  int r, int p, char* salt_hex, unsigned char* decryption_key);
//int get_decryption_key_pbkdf2(char* password, int dklen, int c, char* prf, char* salt_hex, unsigned char* decryption_key);
int verify_password(char* checksum_message_hex, char* cipher_message_hex, unsigned char* decription_key);
int get_private_key(char* cipher_message, char* iv_str, unsigned char* decription_key, char* private_key);

#endif
