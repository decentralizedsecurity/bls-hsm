/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <psa/crypto.h>
#include <stdbool.h>
#include <stdint.h>
//#include <sys/printk.h>
#include "tfm_secure_api.h"
#include "tfm_api.h"

#include "../lib/bls_hsm.h"

#define NUM_SECRETS 5

struct dp_secret {
	uint8_t secret[16];
};

struct dp_secret secrets[NUM_SECRETS] = {
	{ {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15} },
	{ {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15} },
	{ {2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15} },
	{ {3, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15} },
	{ {4, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15} },
};

typedef void (*psa_write_callback_t)(void *handle, uint8_t *digest,
				     uint32_t digest_size);

static psa_status_t tfm_dp_secret_digest(uint32_t secret_index,
			size_t digest_size, size_t *p_digest_size,
			psa_write_callback_t callback, void *handle)
{
	uint8_t digest[32];
	psa_status_t status;

	/* Check that secret_index is valid. */
	if (secret_index >= NUM_SECRETS) {
		return PSA_ERROR_INVALID_ARGUMENT;
	}

	/* Check that digest_size is valid. */
	if (digest_size != sizeof(digest)) {
		return PSA_ERROR_INVALID_ARGUMENT;
	}

	status = psa_hash_compute(PSA_ALG_SHA_256, secrets[secret_index].secret,
				sizeof(secrets[secret_index].secret), digest,
				digest_size, p_digest_size);

	if (status != PSA_SUCCESS) {
		return status;
	}
	if (*p_digest_size != digest_size) {
		return PSA_ERROR_PROGRAMMER_ERROR;
	}

	callback(handle, digest, digest_size);

	return PSA_SUCCESS;
}

#include "tfm_memory_utils.h"

void psa_write_digest(void *handle, uint8_t *digest, uint32_t digest_size)
{
	tfm_memcpy(handle, digest, digest_size);
}

psa_status_t tfm_dp_secret_digest_req(psa_invec *in_vec, size_t in_len,
				      psa_outvec *out_vec, size_t out_len)
{
	uint32_t secret_index;

	if ((in_len != 1) || (out_len != 1)) {
		/* The number of arguments are incorrect */
		return PSA_ERROR_PROGRAMMER_ERROR;
	}

	if (in_vec[0].len != sizeof(secret_index)) {
		/* The input argument size is incorrect */
		return PSA_ERROR_PROGRAMMER_ERROR;
	}

	secret_index = *((uint32_t *)in_vec[0].base);

	return tfm_dp_secret_digest(secret_index, out_vec[0].len,
				    &out_vec[0].len, psa_write_digest,
				    (void *)out_vec[0].base);
}


psa_status_t tfm_dp_req_mngr_init(void){
	return PSA_SUCCESS;
}

psa_status_t tfm_get_keystore_size_req(psa_invec *in_vec, size_t in_len,
				      psa_outvec *out_vec, size_t size_len){
	uint32_t ksize = get_keystore_size();
	tfm_memcpy((void*) out_vec[0].base, &ksize, out_vec[0].len);
	return PSA_SUCCESS;
}

static uint32_t GetFreeMemorySize()
{
  uint32_t  i;
  uint32_t  len;
  uint8_t*  ptr;
 
  for(i=1;;i++)
  {
    len = i * 1024;
    ptr = (uint8_t*)malloc(len);
    if(!ptr){
      break;
    }
    free(ptr);
	printf("GetFreeMemorySize (from TF-M) -> i = %d\n", i);
  }
 
  return i;
}

uint32_t tfm_get_free_memory_size_req(psa_invec *in_vec, size_t in_len,
				      psa_outvec *out_vec, size_t size_len){
	printf("Executing GetFreeMemorySize from TF-M\n");
	return GetFreeMemorySize();
}

psa_status_t tfm_secure_keygen_req(psa_invec *in_vec, size_t in_len,
				      psa_outvec *out_vec, size_t size_len){
	uint32_t index = secure_keygen(in_vec[0].base);
	tfm_memcpy((void*) out_vec[0].base, &index, out_vec[0].len);
	return PSA_SUCCESS;
}

psa_status_t tfm_sign_pk_req(psa_invec *in_vec, size_t in_len,
				      psa_outvec *out_vec, size_t size_len){
	//int ret = sign_pk(in_vec[0].base, in_vec[1].base, (void*) out_vec[0].base);
	//const char* pk = in_vec[0].base;
	//const char* msg = in_vec[1].base;
	char sign[193] = "";
	int ret = sign_pk(in_vec[0].base, in_vec[1].base, sign);
	tfm_memcpy((void*)out_vec[0].base, sign, out_vec[0].len);
	return ret;
}

psa_status_t tfm_verify_sign_req(psa_invec *in_vec, size_t in_len,
				      psa_outvec *out_vec, size_t size_len){
	uint32_t ret = verify_sign(in_vec[0].base, in_vec[1].base, in_vec[2].base);
	//tfm_memcpy((void*) out_vec[0].base, ret, out_vec[0].len);
	return ret;
}

psa_status_t tfm_get_key_req(psa_invec *in_vec, size_t in_len,
				      psa_outvec *out_vec, size_t size_len){
	uint32_t index = *((uint32_t *)in_vec[0].base);
	char pk[96];
	get_key(index, pk);
	tfm_memcpy((void*) out_vec[0].base, pk, out_vec[0].len);
	return PSA_SUCCESS;
}

psa_status_t tfm_get_keys_req(psa_invec *in_vec, size_t in_len,
				      psa_outvec *out_vec, size_t size_len){
	//char keys[1][96];
	//get_keys(keys);
	//char public_keys_hex_store[10][96];
	//get_keys(public_keys_hex_store);
	//tfm_memcpy((void*) out_vec[0].base, /*keys[0]*/"12345678901234567890", 15);
	int keystore_size = get_keystore_size();
    char public_keys_hex_store[keystore_size][96];
	get_keys(public_keys_hex_store);
	printf("tfm_get_keys_req executed\n");
	//get_keys((void*) out_vec[0].base);
	return PSA_SUCCESS;
}

psa_status_t tfm_reset_req(psa_invec *in_vec, size_t in_len,
				      psa_outvec *out_vec, size_t size_len){
	reset();
	return PSA_SUCCESS;
}