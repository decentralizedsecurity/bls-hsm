/*
 * Copyright (c) 2021 Nordic Semiconductor ASA.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <tfm_veneers.h>
#include <tfm_ns_interface.h>

#include "secure_partition_interface.h"

#include "psa/client.h"
#include "psa_manifest/sid.h"

psa_status_t dp_secret_digest(uint32_t secret_index,
			void *p_digest,
			size_t digest_size)
{
	psa_status_t status;
	psa_handle_t handle;

	psa_invec in_vec[] = {
		{ .base = &secret_index, .len = sizeof(secret_index) },
	};

	psa_outvec out_vec[] = {
		{ .base = p_digest, .len = digest_size }
	};

	handle = psa_connect(TFM_DP_SECRET_DIGEST_SID,
				TFM_DP_SECRET_DIGEST_VERSION);
	if (!PSA_HANDLE_IS_VALID(handle)) {
		return PSA_ERROR_GENERIC_ERROR;
	}

	status = psa_call(handle, PSA_IPC_CALL, in_vec, IOVEC_LEN(in_vec),
			out_vec, IOVEC_LEN(out_vec));

	psa_close(handle);

	return status;
}

psa_status_t tfm_init(){
	psa_status_t status;
	psa_handle_t handle;

	handle = psa_connect(TFM_INIT_SID,
				TFM_INIT_VERSION);
	if (!PSA_HANDLE_IS_VALID(handle)) {
		return PSA_ERROR_GENERIC_ERROR;
	}

	status = psa_call(handle, PSA_IPC_CALL, NULL, 0,
			NULL, 0);

	psa_close(handle);

	return status;
}

int tfm_get_keystore_size(){
	uint32_t size;
	psa_status_t status;
	psa_handle_t handle;

	psa_outvec out_vec[] = {
		{ .base = &size, .len = sizeof(size) },
	};

	handle = psa_connect(TFM_GET_KEYSTORE_SIZE_SID,
				TFM_GET_KEYSTORE_SIZE_VERSION);
	if (!PSA_HANDLE_IS_VALID(handle)) {
		return PSA_ERROR_GENERIC_ERROR;
	}

	status = psa_call(handle, PSA_IPC_CALL, NULL, 0,
			out_vec, IOVEC_LEN(out_vec));

	psa_close(handle);

	return size;
}

int tfm_get_free_memory_size(){
	psa_status_t status;
	psa_handle_t handle;

	uint32_t size;

	psa_outvec out_vec[] = {
		{ .base = &size, .len = sizeof(size) },
	};

	handle = psa_connect(TFM_GET_FREE_MEMORY_SIZE_SID,
				TFM_GET_FREE_MEMORY_SIZE_VERSION);
	if (!PSA_HANDLE_IS_VALID(handle)) {
		return PSA_ERROR_GENERIC_ERROR;
	}

	status = psa_call(handle, PSA_IPC_CALL, NULL, 0,
			out_vec, IOVEC_LEN(out_vec));

	psa_close(handle);

	return size;
}

int tfm_store_pk(char* public_key_hex){
	return 0;
}

psa_status_t tfm_get_key(uint32_t index, char* public_key_hex){
	psa_status_t status;
	psa_handle_t handle;

	psa_invec in_vec[] = {
		{ .base = &index, .len = sizeof(index) },
	};

	psa_outvec out_vec[] = {
		{ .base = public_key_hex, .len = 96 }
	};

	handle = psa_connect(TFM_GET_KEY_SID,
				TFM_GET_KEY_VERSION);
	if (!PSA_HANDLE_IS_VALID(handle)) {
		return PSA_ERROR_GENERIC_ERROR;
	}

	status = psa_call(handle, PSA_IPC_CALL, in_vec, IOVEC_LEN(in_vec),
			out_vec, IOVEC_LEN(out_vec));

	psa_close(handle);

	return status;
}

int tfm_get_keys(char public_keys_hex_store_ns[10][96]){
	psa_status_t status;
	psa_handle_t handle;
	
	int ksize = tfm_get_keystore_size();
	psa_outvec out_vec[] = {
		{ .base = public_keys_hex_store_ns, .len = sizeof(char)*ksize*96 },
	};

	handle = psa_connect(TFM_GET_KEYS_SID,
				TFM_GET_KEYS_VERSION);
	if (!PSA_HANDLE_IS_VALID(handle)) {
		return PSA_ERROR_GENERIC_ERROR;
	}

	status = psa_call(handle, PSA_IPC_CALL, NULL, 0,
			out_vec, IOVEC_LEN(out_vec));

	psa_close(handle);

	return status;
}

psa_status_t tfm_secure_keygen(char* info, size_t infosize){
	uint32_t index;
	psa_status_t status;
	psa_handle_t handle;

    psa_invec in_vec[] = {
		{ .base = info, .len = infosize }
	};

	psa_outvec out_vec[] = {
		{ .base = &index, .len = sizeof(index) },
	};

	handle = psa_connect(TFM_SECURE_KEYGEN_SID,
				TFM_SECURE_KEYGEN_VERSION);
	if (!PSA_HANDLE_IS_VALID(handle)) {
		return PSA_ERROR_GENERIC_ERROR;
	}

	status = psa_call(handle, PSA_IPC_CALL, in_vec, IOVEC_LEN(in_vec),
			out_vec, IOVEC_LEN(out_vec));

	psa_close(handle);

	return index; //return status;
}

psa_status_t tfm_sign_pk(char* pk, char* msg, char* sign){
	psa_status_t status;
	psa_handle_t handle;

    psa_invec in_vec[] = {
		{ .base = pk, .len = /*sizeof(pk)*/97 },
		{ .base = msg, .len = /*sizeof(msg)*/65 }/**/
	};

	psa_outvec out_vec[] = {
		{ .base = sign, .len = /*sizeof(sign)*/193 },
	};

	handle = psa_connect(TFM_SIGN_PK_SID,
				TFM_SIGN_PK_VERSION);
	if (!PSA_HANDLE_IS_VALID(handle)) {
		return PSA_ERROR_GENERIC_ERROR;
	}

	status = psa_call(handle, PSA_IPC_CALL, in_vec, IOVEC_LEN(in_vec),
			out_vec, IOVEC_LEN(out_vec));

	psa_close(handle);

	/*printk("(tfm_sign_pk) pk: %s\n", pk);
	printk("(tfm_sign_pk) msg: %s\n", msg);
	printk("(tfm_sign_pk) sign: %s\n", sign);
	printk("(tfm_sign_pk) ret: %d\n", status);*/

	return status;
}

int tfm_verify_sign(char* pk, char* msg, char* sig){
	psa_status_t status;
	psa_handle_t handle;
	uint32_t ret;

	psa_invec in_vec[] = {
		{ .base = pk, .len = /*sizeof(pk)*/96 },
		{ .base = msg, .len = /*sizeof(msg)*/64 },
		{ .base = sig, .len = /*sizeof(msg)*/192 }
	};

	psa_outvec out_vec[] = {
		{ .base = &ret, .len = sizeof(ret) },
	};

	handle = psa_connect(TFM_VERIFY_SIGN_SID,
				TFM_VERIFY_SIGN_VERSION);
	if (!PSA_HANDLE_IS_VALID(handle)) {
		return PSA_ERROR_GENERIC_ERROR;
	}

	status = psa_call(handle, PSA_IPC_CALL, in_vec, IOVEC_LEN(in_vec),
			out_vec, IOVEC_LEN(out_vec));

	psa_close(handle);

	return ret;
}

psa_status_t tfm_reset(){
	psa_status_t status;
	psa_handle_t handle;

	handle = psa_connect(TFM_RESET_SID,
				TFM_RESET_VERSION);
	if (!PSA_HANDLE_IS_VALID(handle)) {
		return PSA_ERROR_GENERIC_ERROR;
	}

	status = psa_call(handle, PSA_IPC_CALL, NULL, 0,
			NULL, 0);

	psa_close(handle);

	return status;
}

int tfm_import_sk(char* sk){
	psa_status_t status;
	psa_handle_t handle;

	int ret;

	psa_outvec in_vec[] = {
		{ .base = sk, .len = 64 },
	};

	psa_outvec out_vec[] = {
		{ .base = &ret, .len = sizeof(ret) },
	};

	handle = psa_connect(TFM_IMPORT_SK_SID,
				TFM_IMPORT_SK_VERSION);
	if (!PSA_HANDLE_IS_VALID(handle)) {
		return PSA_ERROR_GENERIC_ERROR;
	}

	status = psa_call(handle, PSA_IPC_CALL, in_vec, IOVEC_LEN(in_vec),
			out_vec, IOVEC_LEN(out_vec));

	psa_close(handle);

	return ret;
}
