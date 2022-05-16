#ifndef bls_hsm_ns_h
#define bls_hsm_ns_h

#include "blst.h"

int pk_in_keystore(char * public_key_hex, int offset);
void ikm_sk(char* info);
void sk_to_pk(blst_p1* pk);
void sign_pk(blst_p2* sig, blst_p2* hash);
void reset();
void store_pk(char* public_key_hex);
int get_keystore_size();
int import_sk(blst_scalar* sk_imp);

void pk_serialize(byte* out, blst_p1 pk);
void sig_serialize(byte* out2, blst_p2 sig);
void get_point_from_msg(blst_p2* hash, uint8_t* msg_bin, int len);
int pk_parse(char* pk_hex, blst_p1_affine* pk, char* buff);
int msg_parse(char* msg, uint8_t* msg_bin, int len, char* buff);
int sig_parse(char* sig_hex, blst_p2_affine* sig, char* buff);
int keygen(char* data, char* buff);
void get_signature(char* pk, char* msg, char* signature);
int signature(char* pk, char* msg, char* buff);
int verify(char* pk, char* msg, char* sig, char* buff);
// Replacing the function "void dump_keys(char* buff)" with "int print_keys_Json(char* buff)"
int print_keys_Json(char* buff);
void resetc(char* buff);

int import(char* sk, char* buff);
#endif