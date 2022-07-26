#include "tfm_api.h"
#include "blst.h"

psa_status_t tfm_get_keystore_size(int* size);

psa_status_t tfm_store_pk(char* pk, size_t pksize);

psa_status_t tfm_ikm_sk(char* info, size_t infosize);

psa_status_t tfm_sk_to_pk(char* pk, size_t pksize);

psa_status_t tfm_getkeys(char** keys, size_t size);

psa_status_t tfm_hash(uint8_t* out, uint8_t* in, size_t size);

psa_status_t tfm_aes128ctr(uint8_t* key, uint8_t* iv, uint8_t* in, uint8_t* out);

psa_status_t tfm_pk_in_keystore(char * public_key_hex, int offset, int* ret);

psa_status_t tfm_sign_pk(blst_p2* sig, blst_p2* hash);

psa_status_t tfm_reset();

psa_status_t tfm_import_sk(blst_scalar* sk_imp, int* ret);