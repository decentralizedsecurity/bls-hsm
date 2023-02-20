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
#include <psa/crypto.h>
#include <psa/crypto_extra.h>
#include <psa/crypto_values.h>

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

#include "psa/service.h"
#include "psa_manifest/tfm_secure_partition.h"

typedef psa_status_t (*dp_func_t)(psa_msg_t *);

static void psa_write_digest(void *handle, uint8_t *digest,
			     uint32_t digest_size)
{
	psa_write((psa_handle_t)handle, 0, digest, digest_size);
}

static psa_status_t tfm_dp_secret_digest_ipc(psa_msg_t *msg)
{
	size_t num = 0;
	uint32_t secret_index;

	if (msg->in_size[0] != sizeof(secret_index)) {
		/* The size of the argument is incorrect */
		return PSA_ERROR_PROGRAMMER_ERROR;
	}

	num = psa_read(msg->handle, 0, &secret_index, msg->in_size[0]);
	if (num != msg->in_size[0]) {
		return PSA_ERROR_PROGRAMMER_ERROR;
	}

	return tfm_dp_secret_digest(secret_index, msg->out_size[0],
				    &msg->out_size[0], psa_write_digest,
				    (void *)msg->handle);
}

static psa_status_t tfm_init_ipc(psa_msg_t *msg){
	psa_status_t status = init();
	return status;
}

static psa_status_t tfm_get_keystore_size_ipc(psa_msg_t *msg){
	uint32_t ksize = get_keystore_size();
	psa_write((psa_handle_t)msg->handle, 0, &ksize, sizeof(ksize));
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

static psa_status_t tfm_get_free_memory_size_ipc(psa_msg_t *msg){
	int ms = GetFreeMemorySize();
	psa_write((psa_handle_t)msg->handle, 0, &ms, sizeof(ms));
	return PSA_SUCCESS;
}

static psa_status_t tfm_secure_keygen_ipc(psa_msg_t *msg){
	size_t size;
	char info[32] = "";
	size = psa_read(msg->handle, 0, info, msg->in_size[0]);
	if (size != msg->in_size[0]) {
		return PSA_ERROR_PROGRAMMER_ERROR;
	}
	uint32_t index = secure_keygen(info);
	psa_write((psa_handle_t)msg->handle, 0, &index, sizeof(index));
	return PSA_SUCCESS;
}

static psa_status_t tfm_sign_pk_ipc(psa_msg_t *msg){
	//int ret = sign_pk(in_vec[0].base, in_vec[1].base, (void*) out_vec[0].base);
	//const char* pk = in_vec[0].base;
	//const char* msg = in_vec[1].base;
	char sign[193] = "";
	char pk[97];
	char m[65];
	size_t size = psa_read(msg->handle, 0, pk, msg->in_size[0]);
	if (size != msg->in_size[0]) {
		return PSA_ERROR_PROGRAMMER_ERROR;
	}

	size = psa_read(msg->handle, 1, m, msg->in_size[1]);
	if (size != msg->in_size[1]) {
		return PSA_ERROR_PROGRAMMER_ERROR;
	}

	int ret = sign_pk(pk, m, sign);
	psa_write((psa_handle_t)msg->handle, 0, sign, sizeof(sign));
	return ret;
}

static psa_status_t tfm_verify_sign_ipc(psa_msg_t *msg){
	char pk[96];
	char m[64];
	char sign[192];

	size_t size = psa_read(msg->handle, 0, pk, msg->in_size[0]);
	if (size != msg->in_size[0]) {
		return PSA_ERROR_PROGRAMMER_ERROR;
	}

	size = psa_read(msg->handle, 1, m, msg->in_size[1]);
	if (size != msg->in_size[1]) {
		return PSA_ERROR_PROGRAMMER_ERROR;
	}

	size = psa_read(msg->handle, 2, sign, msg->in_size[2]);
	if (size != msg->in_size[2]) {
		return PSA_ERROR_PROGRAMMER_ERROR;
	}

	uint32_t ret = verify_sign(pk, m, sign);
	psa_write((psa_handle_t)msg->handle, 0, &ret, sizeof(ret));
	//tfm_memcpy((void*) out_vec[0].base, ret, out_vec[0].len);
	return ret;
}

static psa_status_t tfm_get_key_ipc(psa_msg_t *msg){
	uint32_t index;

	size_t size = psa_read(msg->handle, 0, &index, msg->in_size[0]);
	if (size != msg->in_size[0]) {
		return PSA_ERROR_PROGRAMMER_ERROR;
	}

	char pk[96];
	get_key(index, pk);
	psa_write((psa_handle_t)msg->handle, 0, pk, sizeof(pk));
	return PSA_SUCCESS;
}

static psa_status_t tfm_get_keys_ipc(psa_msg_t *msg){
	//char keys[1][96];
	//get_keys(keys);
	//char public_keys_hex_store[10][96];
	//get_keys(public_keys_hex_store);
	//tfm_memcpy((void*) out_vec[0].base, /*keys[0]*/"12345678901234567890", 15);
	int keystore_size = get_keystore_size();
    char public_keys_hex_store[keystore_size][96];
	get_keys(public_keys_hex_store);
	printf("tfm_get_keys_req executed\n");
	psa_write((psa_handle_t)msg->handle, 0, public_keys_hex_store, keystore_size*96);
	//get_keys((void*) out_vec[0].base);
	return PSA_SUCCESS;
}

static psa_status_t tfm_reset_ipc(psa_msg_t *msg){
	reset();
	return PSA_SUCCESS;
}

static psa_status_t tfm_import_sk_ipc(psa_msg_t *msg){
	char sk[64];
	size_t size = psa_read(msg->handle, 0, sk, msg->in_size[0]);
	if (size != msg->in_size[0]) {
		return PSA_ERROR_PROGRAMMER_ERROR;
	}

	int ret = import_sk(sk);

	psa_write((psa_handle_t)msg->handle, 0, &ret, sizeof(ret));
	return PSA_SUCCESS;
}

static void sp_signal_handle(psa_signal_t signal, dp_func_t pfn)
{
	psa_status_t status;
	psa_msg_t msg;

	status = psa_get(signal, &msg);
	switch (msg.type) {
	case PSA_IPC_CONNECT:
		psa_reply(msg.handle, PSA_SUCCESS);
		break;
	case PSA_IPC_CALL:
		status = pfn(&msg);
		psa_reply(msg.handle, status);
		break;
	case PSA_IPC_DISCONNECT:
		psa_reply(msg.handle, PSA_SUCCESS);
		break;
	default:
		psa_panic();
	}
}

psa_status_t tfm_sp_req_mngr_init(void)
{
	psa_signal_t signals = 0;

	while (1) {
		signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
		if (signals & TFM_DP_SECRET_DIGEST_SIGNAL) {
			sp_signal_handle(TFM_DP_SECRET_DIGEST_SIGNAL,
					 tfm_dp_secret_digest_ipc);
		} else if (signals & TFM_INIT_SIGNAL) {
			sp_signal_handle(TFM_INIT_SIGNAL,
			tfm_init_ipc);
		} else if (signals & TFM_GET_KEYSTORE_SIZE_SIGNAL) {
			sp_signal_handle(TFM_GET_KEYSTORE_SIZE_SIGNAL,
			tfm_get_keystore_size_ipc);
		} else if (signals & TFM_GET_FREE_MEMORY_SIZE_SIGNAL) {
			sp_signal_handle(TFM_GET_FREE_MEMORY_SIZE_SIGNAL,
			tfm_get_free_memory_size_ipc);
		} else if (signals & TFM_SECURE_KEYGEN_SIGNAL) {
			sp_signal_handle(TFM_SECURE_KEYGEN_SIGNAL,
			tfm_secure_keygen_ipc);
		} else if (signals & TFM_SIGN_PK_SIGNAL) {
			sp_signal_handle(TFM_SIGN_PK_SIGNAL,
			tfm_sign_pk_ipc);
		} else if (signals & TFM_VERIFY_SIGN_SIGNAL) {
			sp_signal_handle(TFM_VERIFY_SIGN_SIGNAL,
			tfm_verify_sign_ipc);
		} else if (signals & TFM_GET_KEY_SIGNAL) {
			sp_signal_handle(TFM_GET_KEY_SIGNAL,
			tfm_get_key_ipc);
		} else if (signals & TFM_GET_KEYS_SIGNAL) {
			sp_signal_handle(TFM_GET_KEYS_SIGNAL,
			tfm_get_keys_ipc);
		} else if (signals & TFM_RESET_SIGNAL) {
			sp_signal_handle(TFM_RESET_SIGNAL,
			tfm_reset_ipc);
		} else if (signals & TFM_IMPORT_SK_SIGNAL) {
			sp_signal_handle(TFM_IMPORT_SK_SIGNAL,
			tfm_import_sk_ipc);
		} else {
			psa_panic();
		}
	}

	return PSA_ERROR_SERVICE_FAILURE;
}