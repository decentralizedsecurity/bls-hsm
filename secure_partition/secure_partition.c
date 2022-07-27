/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <psa/crypto.h>
#include <stdbool.h>
#include <stdint.h>
#include "tfm_secure_api.h"
#include "tfm_api.h"
#include "../lib/bls_hsm.h"

#include "tfm_memory_utils.h"

psa_status_t tfm_req_mngr_init(void){
	return PSA_SUCCESS;
}

void tfm_get_keystore_size_req(psa_invec *in_vec, size_t in_len,
				      psa_outvec *out_vec, size_t size_len){
	int ksize = get_keystore_size();
	tfm_memcpy((void*) out_vec[0].base, ksize, out_vec[0].len);
}

void tfm_store_pk_req(psa_invec *in_vec, size_t in_len,
				      psa_outvec *out_vec, size_t out_len){
	store_pk((char *) in_vec[0].base);
}

void tfm_ikm_sk_req(psa_invec *in_vec, size_t in_len,
				      psa_outvec *out_vec, size_t out_len){
	char info[in_vec[0].len];
	tfm_memcpy(info, (void *) in_vec[0].base, sizeof(info));

	ikm_sk(info);
	
	tfm_memcpy((void *)out_vec[0].base, sk.b, 32);
}

void tfm_sk_to_pk_req(psa_invec *in_vec, size_t in_len,
				      psa_outvec *out_vec, size_t out_len){
	sk_to_pk(in_vec[0].base);
}

void tfm_getkeys_req(psa_invec *in_vec, size_t in_len,
				      psa_outvec *out_vec, size_t out_len){
	getkeys(out_vec[0].base);
}

void tfm_hash_req(psa_invec *in_vec, size_t in_len,
				      psa_outvec *out_vec, size_t out_len){
	hash(out_vec[0].base, in_vec[0].base, in_len);
}

void tfm_aes128ctr_req(psa_invec *in_vec, size_t in_len,
				      psa_outvec *out_vec, size_t out_len){
	aes128ctr(in_vec[0].base, in_vec[1].base, in_vec[2].base, out_vec[0].base);
}

void tfm_pk_in_keystore_req(psa_invec *in_vec, size_t in_len,
				      psa_outvec *out_vec, size_t out_len){
	int ret = pk_in_keystore(in_vec[0].base, in_vec[1].base);
	tfm_memcpy(out_vec[0].base, &ret, out_vec[0].len);
}

void tfm_sign_pk_req(psa_invec *in_vec, size_t in_len,
				      psa_outvec *out_vec, size_t out_len){
	sign_pk(in_vec[0].base, in_vec[1].base);
}

void tfm_reset_req(psa_invec *in_vec, size_t in_len,
				      psa_outvec *out_vec, size_t out_len){
	reset();
}

void tfm_import_sk_req(psa_invec *in_vec, size_t in_len,
				      psa_outvec *out_vec, size_t out_len){
	int ret = import_sk(in_vec[0].base);
	tfm_memcpy(out_vec[0].base, &ret, out_vec[0].len);
}