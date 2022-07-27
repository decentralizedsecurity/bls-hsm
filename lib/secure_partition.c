#include <tfm_veneers.h>
#include <tfm_ns_interface.h>

#include "secure_partition.h"
#include "blst.h"
#include "bls_hsm.h"


psa_status_t tfm_get_keystore_size(int* size)
{
	psa_status_t status;

    psa_outvec out_vec[] = {
		{ .base = size, .len = sizeof(int) }
	};

	status = tfm_ns_interface_dispatch(
				(veneer_fn)tfm_get_keystore_size_req_veneer,
				NULL,  0,
				(uint32_t)out_vec, IOVEC_LEN(out_vec));

	return status;
}

psa_status_t tfm_store_pk(char* pk, size_t pksize)
{
	psa_status_t status;

    psa_invec in_vec[] = {
		{ .base = pk, .len = pksize }
	};

	status = tfm_ns_interface_dispatch(
				(veneer_fn)tfm_store_pk_req_veneer,
				(uint32_t)in_vec, IOVEC_LEN(in_vec),
				NULL, 0);

	return status;
}

psa_status_t tfm_ikm_sk(char* info, size_t infosize)
{
	psa_status_t status;

    psa_invec in_vec[] = {
		{ .base = info, .len = infosize }
	};

	status = tfm_ns_interface_dispatch(
				(veneer_fn)tfm_ikm_sk_req_veneer,
				(uint32_t)in_vec, IOVEC_LEN(in_vec),
				NULL, 0);

	return status;
}

psa_status_t tfm_sk_to_pk(char* pk, size_t pksize)
{
	psa_status_t status;

    psa_invec in_vec[] = {
		{ .base = pk, .len = pksize }
	};

	status = tfm_ns_interface_dispatch(
				(veneer_fn)tfm_sk_to_pk_req_veneer,
				(uint32_t)in_vec, IOVEC_LEN(in_vec),
				NULL, 0);

	return status;
}

psa_status_t tfm_getkeys(char** keys, size_t size)
{
	psa_status_t status;

    psa_outvec out_vec[] = {
		{ .base = keys, .len = size }
	};

	status = tfm_ns_interface_dispatch(
				(veneer_fn)tfm_getkeys_req_veneer,
				NULL, 0,
				(uint32_t)out_vec, IOVEC_LEN(out_vec));

	return status;
}

psa_status_t tfm_hash(uint8_t* out, uint8_t* in, size_t size)
{
	psa_status_t status;

	psa_invec in_vec[] = {
		{ .base = in, .len = size }
	};

    psa_outvec out_vec[] = {
		{ .base = out, .len = sizeof(out) }
	};

	status = tfm_ns_interface_dispatch(
				(veneer_fn)tfm_hash_req_veneer,
				(uint32_t)in_vec, IOVEC_LEN(in_vec),
				(uint32_t)out_vec, IOVEC_LEN(out_vec));

	return status;
}

psa_status_t tfm_aes128ctr(uint8_t* key, uint8_t* iv, uint8_t* in, uint8_t* out)
{
	psa_status_t status;

	psa_invec in_vec[] = {
		{ .base = key, .len = sizeof(key) },
		{ .base = iv, .len = sizeof(iv) },
		{ .base = in, .len = sizeof(in) }
	};

    psa_outvec out_vec[] = {
		{ .base = out, .len = sizeof(out) }
	};

	status = tfm_ns_interface_dispatch(
				(veneer_fn)tfm_aes128ctr_req_veneer,
				(uint32_t)in_vec, IOVEC_LEN(in_vec),
				(uint32_t)out_vec, IOVEC_LEN(out_vec));

	return status;
}

psa_status_t tfm_pk_in_keystore(char * public_key_hex, int offset, int* ret)
{
	psa_status_t status;

	psa_invec in_vec[] = {
		{ .base = public_key_hex, .len = sizeof(public_key_hex) },
		{ .base = &offset, .len = sizeof(offset) }
	};

    psa_outvec out_vec[] = {
		{ .base = ret, .len = sizeof(int) }
	};

	status = tfm_ns_interface_dispatch(
				(veneer_fn)tfm_pk_in_keystore_req_veneer,
				(uint32_t)in_vec, IOVEC_LEN(in_vec),
				(uint32_t)out_vec, IOVEC_LEN(out_vec));

	return status;
}

psa_status_t tfm_sign_pk(blst_p2* sig, blst_p2* hash)
{
	psa_status_t status;

	psa_invec in_vec[] = {
		{ .base = sig, .len = sizeof(sig) },
		{ .base = hash, .len = sizeof(hash) }
	};

	status = tfm_ns_interface_dispatch(
				(veneer_fn)tfm_sign_pk_req_veneer,
				(uint32_t)in_vec, IOVEC_LEN(in_vec),
				NULL, 0);

	return status;
}

psa_status_t tfm_reset()
{
	psa_status_t status;

	status = tfm_ns_interface_dispatch(
				(veneer_fn)tfm_reset_req_veneer,
				NULL, 0,
				NULL, 0);

	return status;
}

psa_status_t tfm_import_sk(blst_scalar* sk_imp, int* ret)
{
	psa_status_t status;

	psa_invec in_vec[] = {
		{ .base = sk_imp, .len = sizeof(sk_imp) }
	};

    psa_outvec out_vec[] = {
		{ .base = ret, .len = sizeof(int) }
	};

	status = tfm_ns_interface_dispatch(
				(veneer_fn)tfm_import_sk_req_veneer,
				(uint32_t)in_vec, IOVEC_LEN(in_vec),
				(uint32_t)out_vec, IOVEC_LEN(out_vec));

	return status;
}