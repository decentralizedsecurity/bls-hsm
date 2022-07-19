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

#include "tfm_memory_utils.h"

psa_status_t tfm_req_mngr_init(void){
	return PSA_SUCCESS;
}

int tfm_get_keystore_size_req(){
	return get_keystore_size();
}

void tfm_store_pk_req(psa_invec *in_vec, size_t in_len,
				      psa_outvec *pk, size_t out_len){
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
				      psa_outvec *pk, size_t out_len){
	sk_to_pk(pk[0].base);
}