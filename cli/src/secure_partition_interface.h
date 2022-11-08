/*
 * Copyright (c) 2021 Nordic Semiconductor ASA.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "tfm_api.h"

psa_status_t dp_secret_digest(uint32_t secret_index,
			      void *p_digest,
			      size_t digest_size);
int tfm_get_keystore_size();
int tfm_get_free_memory_size(); // For temporary use
int tfm_store_pk(char* public_key_hex);
int tfm_get_key(uint32_t index, char* public_key_hex);
int tfm_get_keys(char public_keys_hex_store_ns[10][96]);
int tfm_secure_keygen(char* info, size_t infosize);
int tfm_sign_pk(char* pk, char* msg, char* sign);
int tfm_verify_sign(char* pk, char* msg, char* sig);
int tfm_reset();
int tfm_import_sk(char* sk);