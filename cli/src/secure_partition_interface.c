/*
 * Copyright (c) 2021 Nordic Semiconductor ASA.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <tfm_veneers.h>
#include <tfm_ns_interface.h>

#include "secure_partition_interface.h"

psa_status_t dp_secret_digest(uint32_t secret_index,
			void *p_digest,
			size_t digest_size)
{
	psa_status_t status;
	psa_invec in_vec[] = {
		{ .base = &secret_index, .len = sizeof(secret_index) },
	};

	psa_outvec out_vec[] = {
		{ .base = p_digest, .len = digest_size }
	};

	status = tfm_ns_interface_dispatch(
				(veneer_fn)tfm_dp_secret_digest_req_veneer,
				(uint32_t)in_vec,  IOVEC_LEN(in_vec),
				(uint32_t)out_vec, IOVEC_LEN(out_vec));

	return status;
}

int tfm_get_keystore_size(){
	uint32_t size;
	psa_status_t status;
	psa_invec out_vec[] = {
		{ .base = &size, .len = sizeof(size) },
	};

	status = tfm_ns_interface_dispatch(
				(veneer_fn)tfm_get_keystore_size_req_veneer,
				(uint32_t) NULL,  0,
				(uint32_t)out_vec, IOVEC_LEN(out_vec));

	return size;
}

int tfm_get_free_memory_size(){
	uint32_t size = tfm_ns_interface_dispatch(
				(veneer_fn)tfm_get_free_memory_size_req_veneer,
				(uint32_t) NULL, 0,
				(uint32_t) NULL, 0);

	return size;
}

int tfm_store_pk(char* public_key_hex){
	return 0;
}

int tfm_get_key(uint32_t index, char* public_key_hex){
	psa_status_t status;
	psa_invec in_vec[] = {
		{ .base = &index, .len = sizeof(index) },
	};

	psa_outvec out_vec[] = {
		{ .base = public_key_hex, .len = 96 }
	};

	status = tfm_ns_interface_dispatch(
				(veneer_fn)tfm_get_key_req_veneer,
				(uint32_t)in_vec,  IOVEC_LEN(in_vec),
				(uint32_t)out_vec, IOVEC_LEN(out_vec));

	return status;
}

int tfm_get_keys(char public_keys_hex_store_ns[10][96]){
	psa_status_t status;

	psa_outvec out_vec[] = {
		{ .base = public_keys_hex_store_ns, .len = sizeof(char)*1*96 },
	};

	status = tfm_ns_interface_dispatch(
				(veneer_fn)tfm_get_keys_req_veneer,
				(uint32_t) NULL, 0,
				(uint32_t)out_vec, IOVEC_LEN(out_vec));

	return status;
}

int tfm_secure_keygen(char* info, size_t infosize){
	uint32_t index;
	psa_status_t status;

    psa_invec in_vec[] = {
		{ .base = info, .len = infosize }
	};

	psa_outvec out_vec[] = {
		{ .base = &index, .len = sizeof(index) },
	};

	status = tfm_ns_interface_dispatch(
				(veneer_fn)tfm_secure_keygen_req_veneer,
				(uint32_t)in_vec, IOVEC_LEN(in_vec),
				(uint32_t)out_vec, IOVEC_LEN(out_vec));

	return index; //return status;
}

int tfm_sign_pk(char* pk, char* msg, char* sign){
	psa_status_t status;

    psa_invec in_vec[] = {
		{ .base = pk, .len = /*sizeof(pk)*/97 },
		{ .base = msg, .len = /*sizeof(msg)*/65 }/**/
	};

	psa_outvec out_vec[] = {
		{ .base = sign, .len = /*sizeof(sign)*/193 },
	};

	status = tfm_ns_interface_dispatch(
				(veneer_fn)tfm_sign_pk_req_veneer,
				(uint32_t)in_vec, IOVEC_LEN(in_vec),
				(uint32_t)out_vec, IOVEC_LEN(out_vec));

	/*printk("(tfm_sign_pk) pk: %s\n", pk);
	printk("(tfm_sign_pk) msg: %s\n", msg);
	printk("(tfm_sign_pk) sign: %s\n", sign);
	printk("(tfm_sign_pk) ret: %d\n", status);*/

	return status;
}

int tfm_verify_sign(char* pk, char* msg, char* sig){
	psa_status_t status;
	uint32_t ret;

	psa_invec in_vec[] = {
		{ .base = pk, .len = /*sizeof(pk)*/96 },
		{ .base = msg, .len = /*sizeof(msg)*/64 },
		{ .base = sig, .len = /*sizeof(msg)*/192 }
	};

	psa_outvec out_vec[] = {
		{ .base = &ret, .len = sizeof(ret) },
	};

	status = tfm_ns_interface_dispatch(
				(veneer_fn)tfm_verify_sign_req_veneer,
				(uint32_t)in_vec, IOVEC_LEN(in_vec),
				(uint32_t)out_vec, IOVEC_LEN(out_vec));

	return ret;
}

int tfm_reset(){
	psa_status_t status;

	status = tfm_ns_interface_dispatch(
				(veneer_fn)tfm_reset_req_veneer,
				(uint32_t) NULL, 0,
				(uint32_t) NULL, 0);

	return status;
}

int tfm_import_sk(char* sk){
	return 0;
}
