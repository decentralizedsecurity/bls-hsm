#include "tfm_api.h"

psa_status_t tfm_get_keystore_size(int* size);

psa_status_t tfm_ikm_sk(char* info, size_t infosize); // secure_keygen

psa_status_t tfm_getkeys(char** keys, size_t size);

psa_status_t tfm_pk_in_keystore(char * public_key_hex, int offset, int* ret);

psa_status_t tfm_sign_pk(char* sig, char* hash, char* pk);

psa_status_t tfm_reset();