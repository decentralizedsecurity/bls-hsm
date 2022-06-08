#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>
#include <sys/byteorder.h>
#include <cJSON.h>

#define MASK0 (uint64_t) 0xfffffffffffffffe
#define MASK1 (uint64_t) 0xfffffffffffffffc
#define MASK2 (uint64_t) 0xfffffffffffffff0
#define MASK3 (uint64_t) 0xffffffffffffff00
#define MASK4 (uint64_t) 0xffffffffffff0000
#define MASK5 (uint64_t) 0xffffffff00000000

#define BIT0 (uint64_t) 0x1
#define BIT1 (uint64_t) 0x2
#define BIT2 (uint64_t) 0x4
#define BIT3 (uint64_t) 0x8
#define BIT4 (uint64_t) 0x10
#define BIT5 (uint64_t) 0x20

#define MAX_PROPOSER_SLASHINGS (uint64_t) 16
#define MAX_ATTESTER_SLASHINGS (uint64_t) 2
#define MAX_ATTESTATIONS (uint64_t) 128
#define MAX_DEPOSITS (uint64_t) 16
#define MAX_VOLUNTARY_EXITS (uint64_t) 16

#define MAX_COMMITTEES_PER_SLOT (uint64_t) 64
#define TARGET_COMMITTEE_SIZE (uint64_t) 128
#define MAX_VALIDATORS_PER_COMMITTEE (uint64_t) 2048
#define SHUFFLE_ROUND_COUNT (uint64_t) 90
#define HYSTERESIS_QUOTIENT	(uint64_t) 4
#define HYSTERESIS_DOWNWARD_MULTIPLIER (uint64_t) 1
#define HYSTERESIS_UPWARD_MULTIPLIER (uint64_t) 5

#define GENESIS_SLOT (uint64_t) 0
#define GENESIS_EPOCH (uint64_t) 0
#define BASE_REWARDS_PER_EPOCH (uint64_t) 4
#define DEPOSIT_CONTRACT_TREE_DEPTH (uint64_t) 32
#define JUSTIFICATION_BITS_LENGTH (uint64_t) 4

void hash(uint8_t* out, uint8_t* in, size_t size);

uint8_t zerohashes[64][32];

const uint8_t domain_beacon_proposer[4] = {0x00, 0x00, 0x00, 0x00};
const uint8_t domain_beacon_attester[4] = {0x01, 0x00, 0x00, 0x00};
const uint8_t domain_randao[4] = {0x02, 0x00, 0x00, 0x00};
const uint8_t domain_deposit[4] = {0x03, 0x00, 0x00, 0x00};
const uint8_t domain_voluntary_exit[4] = {0x04, 0x00, 0x00, 0x00};
const uint8_t domain_selection_proof[4] = {0x05, 0x00, 0x00, 0x00};
const uint8_t domain_aggregate_and_proof[4] = {0x06, 0x00, 0x00, 0x00};

void htru8(uint8_t t, uint8_t* buf){
	uint8_t tmp[64];
    sprintf(tmp, "%02x", t);
	memset(tmp + 2, '0', 62);
	hex2bin(tmp, 64, buf, 32);
}

void htru16(uint16_t t, uint8_t* buf){
	t = sys_be16_to_cpu(t);
    uint8_t tmp[64];
	sprintf(tmp, "%04"PRIx16, t);
	memset(tmp + 4, '0', 60);
	hex2bin(tmp, 64, buf, 32);
}

void htru32(uint32_t t, uint8_t* buf){
	t = sys_be32_to_cpu(t);
	uint8_t tmp[64];
	sprintf(tmp, "%08"PRIx32, t);
	memset(tmp + 8, '0', 56);
	hex2bin(tmp, 64, buf, 32);
}

void htru64(uint64_t t, uint8_t* buf){
	t = sys_be64_to_cpu(t);
	uint8_t tmp[64];
	sprintf(tmp, "%016"PRIx64, t);
	memset(tmp + 16, '0', 48);
	hex2bin(tmp, 64, buf, 32);
}

void htrbool(bool t, uint8_t* buf){
	if(t){
		sprintf(buf, "%064x", 1);
	}else{
		sprintf(buf, "%064x", 0);
	}
}

void htrversion(uint8_t* out, uint8_t* ver){
	memset(out, 0, 32);
	memcpy(out, ver, 4);
}

uint64_t BitIndex(uint8_t v) {
	uint64_t out = 0;
	if ((v&0xf0) != 0) { // 11110000
		v >>= 4;
		out |= 4;
	}
	if ((v&0x0c) != 0) { // 00001100
		v >>= 2;
		out |= 2;
	}
	if ((v&0x02) != 0) { // 00000010
		v >>= 1;
		out |= 1;
	}
	return out;
}

uint64_t BitlistLen(uint8_t* bitlist, uint64_t byteLen){
	if(byteLen == 0){
		return 0;
	}
	uint8_t last = bitlist[byteLen-1];
	return ((byteLen - 1) << 3) | BitIndex(last);
}

uint8_t CoverDepth(uint64_t v) {
	if (v == 0 || v == 1) {
		return 0;
	}
	return BitIndex(v-1) + 1;
}

void InitZeroHashes(uint8_t size) {
	memset(zerohashes[0], 0, 32);
	for(int i = 0; i < size; i++){
		uint8_t tmp[64];
		memcpy(tmp, zerohashes[i], 32);
		memcpy(tmp+32, zerohashes[i], 32);
		hash(zerohashes[i+1], tmp, 64);
	}
}

void hashsum(uint8_t* out, uint8_t* in1, uint8_t* in2){
	uint8_t bufbin[64];
	memcpy(bufbin, in1, 32);
	memcpy(bufbin + 32, in2, 32);
	hash(out, bufbin, 64);
}

void merge(uint8_t* hArr, uint8_t* tmp, uint8_t size, uint8_t depth, uint64_t i){
	uint8_t jj;
	for(uint8_t j = 0; ; j++){
		if ((i&((uint64_t)1<<j)) == 0 ){
			// if we are at the size, we want to merge in zero-hashes for padding
			if (i == size && j < depth) {
				hashsum(hArr, hArr, zerohashes[j]);
			} else {
				jj = j;
				break;
			}
		} else {
			// keep merging up if we are the right side
			hashsum(hArr, tmp + 32*j, hArr);
		}
	}
	memcpy(tmp + 32*jj, hArr, 32*sizeof(uint8_t));
}

void htru64list(cJSON* list, uint64_t max, uint8_t* htr){
	uint64_t size = cJSON_GetArraySize(list);
	uint8_t depth = CoverDepth(size);
	uint8_t limitdepth = CoverDepth(max);
	if(size == 0){
		hashsum(htr, zerohashes[limitdepth], zerohashes[0]);
	}else{
		uint8_t* tmp = (uint8_t*) calloc(32*(limitdepth + 1), sizeof(uint8_t*));
		uint8_t hArr[32];
		// merge in leaf by leaf.
		for(uint64_t i = 0; i < size; i++) {
			uint64_t num;
			sscanf(cJSON_GetArrayItem(list, i)->valuestring, "%"PRIu64, &num);
			htru64(num, hArr);
			merge(hArr, tmp, size, depth, i);
		}

		// complement with 0 if empty, or if not the right power of 2
		if (((uint64_t) 1 << depth) != size) {
			merge(zerohashes[0], tmp, size, depth, size);
		}

		// the next power of two may be smaller than the ultimate virtual size,
		// complement with zero-hashes at each depth.
		for (uint8_t j = depth; j < limitdepth; j++) {
			hashsum(tmp + 32*(j+1), tmp + 32*j, zerohashes[j]);
		}

		uint8_t htrlimit[32];
		htru64(max, htrlimit);

		hashsum(htr, tmp + 32*limitdepth, htrlimit);

		free(tmp);
	}
}

void htrBitlist(uint8_t* bitlist, uint64_t byteLen, uint64_t bitlimit, uint8_t* htr){
	uint64_t bitLen = BitlistLen(bitlist, byteLen);
	uint64_t chunks = (bitLen + 0xff) >> 8;
	uint64_t chunkLimit = (bitlimit + 0xff) >> 8;
	uint8_t depth = CoverDepth(chunks);
	uint8_t limitDepth = CoverDepth(chunkLimit);

	uint8_t* tmp = (uint8_t*) calloc(32*(limitDepth+1), sizeof(uint8_t));

	uint8_t hArr[32];
	// merge in leaf by leaf.
	for(uint64_t i = 0; i < chunks; i++) {
		memset(hArr, 0, 32*sizeof(uint8_t));
		if(bitLen - (i<<8) >= 256){
			memcpy(hArr, bitlist + (i<<5), 32*sizeof(uint8_t));
		} else {
			memcpy(hArr, bitlist + (i<<5), (byteLen - (i<<5))*sizeof(uint8_t));
		}
		if(((i + 1) << 8) > bitLen) {
			hArr[(bitLen&0xff)>>3] &= ~(1 << (bitLen & 0x7));
		}
		merge(hArr, tmp, chunks, depth, i);
	}

	// complement with 0 if empty, or if not the right power of 2
	if (((uint64_t) 1 << depth) != chunks) {
		merge(zerohashes[0], tmp, chunks, depth, chunks);
	}

	// the next power of two may be smaller than the ultimate virtual size,
	// complement with zero-hashes at each depth.
	for (uint8_t j = depth; j < limitDepth; j++) {
		hashsum(tmp + 32*(j+1), tmp + 32*j, zerohashes[j]);
	}
	
	uint8_t htrlen[32];
	htru64(bitLen, htrlen);

	hashsum(htr, tmp + 32*limitDepth, htrlen);
	free(tmp);
}

void htrAttDat(cJSON* attdat, uint8_t* htr){
	uint64_t slot;
	sscanf(cJSON_GetObjectItem(attdat, "slot")->valuestring, "%"PRIu64, &slot);
	uint64_t index;
	sscanf(cJSON_GetObjectItem(attdat, "index")->valuestring, "%"PRIu64, &index);
	uint8_t* beacon_block_root = cJSON_GetObjectItem(attdat, "beacon_block_root")->valuestring;
	cJSON* src = cJSON_GetObjectItem(attdat, "source");
	uint64_t srcepoch;
	uint8_t* srcroot;
	srcroot = cJSON_GetObjectItem(src, "root")->valuestring;
	sscanf(cJSON_GetObjectItem(src, "epoch")->valuestring, "%"PRIu64, &srcepoch);
	cJSON* tgt = cJSON_GetObjectItem(attdat, "target");
	uint64_t tgtepoch;
	uint8_t* tgtroot;
	sscanf(cJSON_GetObjectItem(tgt, "epoch")->valuestring, "%"PRIu64, &tgtepoch);
	tgtroot = cJSON_GetObjectItem(tgt, "root")->valuestring;
	


	uint8_t tmp[4][32];

	//Compute htr of slot and index and hash them
	uint8_t slothtr[32];
	htru64(slot, slothtr);
	uint8_t indexhtr[32];
	htru64(index, indexhtr);
	hashsum(tmp[0], slothtr, indexhtr);

	// Compute htr of source (h(htr(source root) + htr(source epoch)))
	uint8_t srcepochhtr[32];
	htru64(srcepoch, srcepochhtr);
	uint8_t srcrootbin[32];
	hex2bin(srcroot + 2, 64, srcrootbin, 32);
	hashsum(tmp[1], srcepochhtr, srcrootbin);
	uint8_t bbr[32];
	hex2bin(beacon_block_root + 2, 64, bbr, 32);

	// Compute hash of beacon_block_root htr and source htr
	hashsum(tmp[3], bbr, tmp[1]);

	// Compute hash of h(slot+index) and previous hash
	hashsum(tmp[2], tmp[0], tmp[3]);


	// Compute htr of target
	uint8_t tgtepochhtr[32];
	htru64(tgtepoch, tgtepochhtr);
	uint8_t tgtrootbin[32];
	hex2bin(tgtroot + 2, 64, tgtrootbin, 32);
	hashsum(tmp[0], tgtepochhtr, tgtrootbin);
	// Compute h(htr(target) + htr(zeros))
	hashsum(tmp[1], tmp[0], zerohashes[0]);
	
	// Compute htr of previous hash + h(zeros + zeros)
	hashsum(tmp[3], tmp[1], zerohashes[1]);

	// Compute final htr (h(slot+index) and h(beacon_block_root htr + source htr) with previous hash)
	hashsum(htr, tmp[2], tmp[3]);
}

void getDomain(uint8_t* domain, cJSON* forkinfo, uint8_t* domtype){
	cJSON* fork = cJSON_GetObjectItem(forkinfo, "fork");
	uint8_t forkDataRoot[32];
	uint8_t currentVersion[4];
	uint8_t genesisValidatorsRoot[32];
	uint8_t version[32];
	hex2bin(cJSON_GetObjectItem(fork, "current_version")->valuestring + 2, 8, currentVersion, 4);
	
	htrversion(version, currentVersion);

	hex2bin(cJSON_GetObjectItem(forkinfo, "genesis_validators_root")->valuestring + 2, 64, genesisValidatorsRoot, 32);

	hashsum(forkDataRoot, version, genesisValidatorsRoot);

	memcpy(domain, domtype, 4);
	memcpy(domain + 4, forkDataRoot, 28);
}

void computeSigningRoot(uint8_t* out, uint8_t* htr, uint8_t* domain){
	hashsum(out, htr, domain);
}

void sr(uint8_t* signingRoot, cJSON* fullblock, uint8_t* htr, uint8_t* domtype){
	cJSON* forkinfo = cJSON_GetObjectItem(fullblock, "fork_info");
	uint8_t domain[32];
	getDomain(domain, forkinfo, domtype);
	computeSigningRoot(signingRoot, htr, domain);
}

void htrBLSSignature(uint8_t* sign, uint8_t* htr){
	uint8_t tmp[2][32];
	hashsum(tmp[0], sign, sign + 32);
	hashsum(tmp[1], sign + 64, zerohashes[0]);
	hashsum(htr, tmp[0], tmp[1]);
}

void htrBLSPubkey(uint8_t* pk, uint8_t* htr){
	uint8_t tmp[32];
	memcpy(tmp, pk + 32, 16);
	memset(tmp + 16, 0, sizeof(uint8_t));
	hashsum(htr, pk, tmp);
}

void htrAtt(cJSON* att, uint8_t* htr){
	uint8_t tmp[2][32];
	uint8_t* aggbits = cJSON_GetObjectItem(att, "aggregation_bits")->valuestring + 2;
	uint8_t aggbitslen = strlen(aggbits);
	uint8_t aggbitsbin[257];
	hex2bin(aggbits, aggbitslen, aggbitsbin, aggbitslen/2);

	//Compute htr of aggregation bits
	htrBitlist(aggbitsbin, aggbitslen/2 + aggbitslen%2, MAX_VALIDATORS_PER_COMMITTEE, tmp[0]);
	cJSON* attdata = cJSON_GetObjectItem(att, "data");
	htrAttDat(attdata, tmp[1]);

	// Compute hash of htr(agg bits + data)
	hashsum(tmp[0], tmp[0], tmp[1]);
	uint8_t* blssig = cJSON_GetObjectItem(att, "signature")->valuestring + 2;
	uint8_t blssigbin[96];
	hex2bin(blssig, 96*2, blssigbin, 96);
	
	// Compute htr of signature
	htrBLSSignature(blssigbin, tmp[1]);

	// Compute hash of htr(signature + zeros[0])
	hashsum(tmp[1], tmp[1], zerohashes[0]);

	// Compute final htr
	hashsum(htr, tmp[0], tmp[1]);
}

void htrAggSlot(cJSON* aggslot, uint8_t* htr){
	uint64_t slot;
	sscanf(cJSON_GetObjectItem(aggslot, "slot")->valuestring, "%"PRIu64, &slot);
	htru64(slot, htr);
}

void htrAggAndProof(cJSON* aggandproof, uint8_t* htr){
	uint8_t tmp[2][32];
	uint64_t agind;
	sscanf(cJSON_GetObjectItem(aggandproof, "aggregator_index")->valuestring, "%"PRIu64, &agind);
	htru64(agind, tmp[0]);

	htrAtt(cJSON_GetObjectItem(aggandproof, "aggregate"), tmp[1]);

	hashsum(tmp[0], tmp[0], tmp[1]);

	uint8_t* selproof = cJSON_GetObjectItem(aggandproof, "selection_proof")->valuestring + 2;
	uint8_t selproofbin[96];
	hex2bin(selproof, 96*2, selproofbin, 96);
	htrBLSSignature(selproofbin, tmp[1]);

	hashsum(tmp[1], tmp[1], zerohashes[0]);

	hashsum(htr, tmp[0], tmp[1]);
}

void htrEth1Data(cJSON* eth1data, uint8_t* htr){
	uint8_t* depositroot = cJSON_GetObjectItem(eth1data, "deposit_root")->valuestring + 2;
	uint8_t depositrootbin[32];
	hex2bin(depositroot, 64, depositrootbin, 32);

	uint64_t depositcount;
	sscanf(cJSON_GetObjectItem(eth1data, "deposit_count")->valuestring, "%"PRIu64, &depositcount);

	uint8_t* blockhash = cJSON_GetObjectItem(eth1data, "block_hash")->valuestring + 2;
	uint8_t blockhashbin[32];
	hex2bin(blockhash, 64, blockhashbin, 32);

	uint8_t tmp[2][32];

	htru64(depositcount, tmp[0]);

	hashsum(tmp[0], depositrootbin, tmp[0]);

	hashsum(tmp[1], blockhashbin, zerohashes[0]);

	hashsum(htr, tmp[0], tmp[1]);
}

void htrBeaconBlockHeader(cJSON* bbh, uint8_t* htr){
	uint64_t slot;
	sscanf(cJSON_GetObjectItem(bbh, "slot")->valuestring, "%"PRIu64, &slot);

	uint64_t proposer_index;
	sscanf(cJSON_GetObjectItem(bbh, "proposer_index")->valuestring, "%"PRIu64, &proposer_index);

	uint8_t* parent_root = cJSON_GetObjectItem(bbh, "parent_root")->valuestring + 2;
	uint8_t parent_rootbin[32];
	hex2bin(parent_root, 64, parent_rootbin, 32);

	uint8_t* state_root = cJSON_GetObjectItem(bbh, "state_root")->valuestring + 2;
	uint8_t state_rootbin[32];
	hex2bin(state_root, 64, state_rootbin, 32);

	uint8_t* body_root = cJSON_GetObjectItem(bbh, "body_root")->valuestring + 2;
	uint8_t body_rootbin[32];
	hex2bin(body_root, 64, body_rootbin, 32);

	uint8_t tmp[2][32];
	htru64(slot, tmp[0]);
	htru64(proposer_index, tmp[1]);
	hashsum(tmp[0], tmp[0], tmp[1]);

	hashsum(tmp[1], parent_rootbin, state_rootbin);

	hashsum(tmp[0], tmp[0], tmp[1]);

	hashsum(tmp[1], body_rootbin, zerohashes[0]);

	hashsum(tmp[1], tmp[1], zerohashes[1]);

	hashsum(htr, tmp[0], tmp[1]);
}

void htrSignedBeaconBlockHeader(cJSON* signedbbh, uint8_t* htr){
	uint8_t* signature = cJSON_GetObjectItem(signedbbh, "signature")->valuestring + 2;
	uint8_t signaturebin[96];
	hex2bin(signature, 96*2, signaturebin, 96);

	uint8_t tmp[32];
	htrBeaconBlockHeader(cJSON_GetObjectItem(signedbbh, "message"), htr);

	htrBLSSignature(signaturebin, tmp);

	hashsum(htr, htr, tmp);
	
}

void htrProposerSlashing(cJSON* proposerslashing, uint8_t* htr){
	uint8_t tmp[32];
	htrSignedBeaconBlockHeader(cJSON_GetObjectItem(proposerslashing, "signed_header_1"), htr);

	htrSignedBeaconBlockHeader(cJSON_GetObjectItem(proposerslashing, "signed_header_2"), tmp);

	hashsum(htr, htr, tmp);
}

void htrIndexedAttestation(cJSON* indexatt, uint8_t* htr){
	uint8_t* signature = cJSON_GetObjectItem(indexatt, "signature")->valuestring + 2;
	uint8_t signaturebin[96];
	hex2bin(signature, 96*2, signaturebin, 96);

	uint8_t tmp[32];
	htru64list(cJSON_GetObjectItem(indexatt, "attesting_indices"), (uint64_t) MAX_VALIDATORS_PER_COMMITTEE, htr);

	htrAttDat(cJSON_GetObjectItem(indexatt, "data"), tmp);

	hashsum(htr, htr, tmp);

	htrBLSSignature(signaturebin, tmp);

	hashsum(tmp, tmp, zerohashes[0]);

	hashsum(htr, htr, tmp);
}

void htrAttesterSlashing(cJSON* attesterslashing, uint8_t* htr){
	uint8_t tmp[32];
	htrIndexedAttestation(cJSON_GetObjectItem(attesterslashing, "attestation_1"), htr);

	htrIndexedAttestation(cJSON_GetObjectItem(attesterslashing, "attestation_2"), tmp);

	hashsum(htr, htr, tmp);
}

void htrDepositData(cJSON* depositData, uint8_t* htr){
	uint8_t* pk = cJSON_GetObjectItem(depositData, "pubkey")->valuestring + 2;
	uint8_t pkbin[48];
	hex2bin(pk, 96, pkbin, 48);
	uint8_t* withdrawal_credentials = cJSON_GetObjectItem(depositData, "withdrawal_credentials")->valuestring + 2;
	uint8_t withdrawal_credentialsbin[32];
	hex2bin(withdrawal_credentials, 64, withdrawal_credentialsbin, 32);
	uint64_t amount;
	sscanf(cJSON_GetObjectItem(depositData, "amount")->valuestring, "%"PRIu64, &amount);
	uint8_t* signature = cJSON_GetObjectItem(depositData, "signature")->valuestring + 2;
	uint8_t signaturebin[96];
	hex2bin(signature, 96*2, signaturebin, 96);

	uint8_t tmp[2][32];
	htrBLSPubkey(pkbin, htr);
	hashsum(htr, htr, withdrawal_credentialsbin);

	htru64(amount, tmp[0]);
	htrBLSSignature(signaturebin, tmp[1]);
	hashsum(tmp[0], tmp[0], tmp[1]);

	hashsum(htr, htr, tmp[0]);

}

void htrDeposit(cJSON* deposit, uint8_t* htr){
	cJSON* proof = cJSON_GetObjectItem(deposit, "proof");
	cJSON* data = cJSON_GetObjectItem(deposit, "data");
	
	uint8_t tmp[32];
	htru64list(proof, DEPOSIT_CONTRACT_TREE_DEPTH + 1, htr);

	htrDepositData(data, tmp);

	hashsum(htr, htr, tmp);
}

void htrVoluntaryExit(cJSON* voluntaryexit, uint8_t* htr){
	uint64_t epoch;
	sscanf(cJSON_GetObjectItem(voluntaryexit, "epoch")->valuestring, "%"PRIu64, &epoch);
	uint64_t validator_index;
	sscanf(cJSON_GetObjectItem(voluntaryexit, "voluntary_exit")->valuestring, "%"PRIu64, &validator_index);

	uint8_t tmp[32];
	htru64(epoch, htr);
	htru64(validator_index, tmp);
	hashsum(htr, htr, tmp);
}

void htrSignedVoluntaryExit(cJSON* signedvoluntaryexit, uint8_t* htr){
	uint8_t tmp[32];
	htrVoluntaryExit(cJSON_GetObjectItem(signedvoluntaryexit, "message"), htr);

	htrBLSSignature(cJSON_GetObjectItem(signedvoluntaryexit, "signature")->valuestring, tmp);

	hashsum(htr, htr, tmp);
}

void htrProposerSlashings(cJSON* proposerslashings, uint8_t* htr){
	uint64_t size = cJSON_GetArraySize(proposerslashings);
	uint8_t depth = CoverDepth(size);
	uint8_t limitdepth = CoverDepth(MAX_PROPOSER_SLASHINGS);
	if(size == 0){
		hashsum(htr, zerohashes[limitdepth], zerohashes[0]);
	}else{
		uint8_t* tmp = (uint8_t*) calloc(32*(limitdepth + 1), sizeof(uint8_t*));
		uint8_t hArr[32];
		// merge in leaf by leaf.
		for(uint64_t i = 0; i < size; i++) {
			htrProposerSlashing(cJSON_GetArrayItem(proposerslashings, i), hArr);
			merge(hArr, tmp, size, depth, i);
		}

		// complement with 0 if empty, or if not the right power of 2
		if (((uint64_t) 1 << depth) != size) {
			merge(zerohashes[0], tmp, size, depth, size);
		}

		// the next power of two may be smaller than the ultimate virtual size,
		// complement with zero-hashes at each depth.
		for (uint8_t j = depth; j < limitdepth; j++) {
			hashsum(tmp + 32*(j+1), tmp + 32*j, zerohashes[j]);
		}

		uint8_t htrlimit[32];
		htru64(MAX_PROPOSER_SLASHINGS, htrlimit);

		hashsum(htr, tmp + 32*limitdepth, htrlimit);

		free(tmp);
	}
}

void htrAttesterSlashings(cJSON* attesterslashings, uint8_t* htr){
	uint64_t size = cJSON_GetArraySize(attesterslashings);
	uint8_t depth = CoverDepth(size);
	uint8_t limitdepth = CoverDepth(MAX_ATTESTER_SLASHINGS);
	if(size == 0){
		hashsum(htr, zerohashes[limitdepth], zerohashes[0]);
	}else{
		uint8_t* tmp = (uint8_t*) calloc(32*(limitdepth + 1), sizeof(uint8_t*));
		uint8_t hArr[32];
		// merge in leaf by leaf.
		for(uint64_t i = 0; i < size; i++) {
			htrAttesterSlashing(cJSON_GetArrayItem(attesterslashings, i), hArr);
			merge(hArr, tmp, size, depth, i);
		}

		// complement with 0 if empty, or if not the right power of 2
		if (((uint64_t) 1 << depth) != size) {
			merge(zerohashes[0], tmp, size, depth, size);
		}

		// the next power of two may be smaller than the ultimate virtual size,
		// complement with zero-hashes at each depth.
		for (uint8_t j = depth; j < limitdepth; j++) {
			hashsum(tmp + 32*(j+1), tmp + 32*j, zerohashes[j]);
		}

		uint8_t htrlimit[32];
		htru64(MAX_ATTESTER_SLASHINGS, htrlimit);

		hashsum(htr, tmp + 32*limitdepth, htrlimit);

		free(tmp);
	}
}

void htrAttestations(cJSON* attestations, uint8_t* htr){
	uint64_t size = cJSON_GetArraySize(attestations);
	uint8_t depth = CoverDepth(size);
	uint8_t limitdepth = CoverDepth(MAX_ATTESTATIONS);
	if(size == 0){
		hashsum(htr, zerohashes[limitdepth], zerohashes[0]);
	}else{
		uint8_t* tmp = (uint8_t*) calloc(32*(limitdepth + 1), sizeof(uint8_t*));
		uint8_t hArr[32];
		// merge in leaf by leaf.
		for(uint64_t i = 0; i < size; i++) {
			htrAtt(cJSON_GetArrayItem(attestations, i), hArr);
			merge(hArr, tmp, size, depth, i);
		}

		// complement with 0 if empty, or if not the right power of 2
		if (((uint64_t) 1 << depth) != size) {
			merge(zerohashes[0], tmp, size, depth, size);
		}

		// the next power of two may be smaller than the ultimate virtual size,
		// complement with zero-hashes at each depth.
		for (uint8_t j = depth; j < limitdepth; j++) {
			hashsum(tmp + 32*(j+1), tmp + 32*j, zerohashes[j]);
		}

		uint8_t htrlimit[32];
		htru64(MAX_ATTESTER_SLASHINGS, htrlimit);

		hashsum(htr, tmp + 32*limitdepth, htrlimit);

		free(tmp);
	}
}

void htrDeposits(cJSON* deposits, uint8_t* htr){
	uint64_t size = cJSON_GetArraySize(deposits);
	uint8_t depth = CoverDepth(size);
	uint8_t limitdepth = CoverDepth(MAX_DEPOSITS);
	if(size == 0){
		hashsum(htr, zerohashes[limitdepth], zerohashes[0]);
	}else{
		uint8_t* tmp = (uint8_t*) calloc(32*(limitdepth + 1), sizeof(uint8_t*));
		uint8_t hArr[32];
		// merge in leaf by leaf.
		for(uint64_t i = 0; i < size; i++) {
			htrDeposit(cJSON_GetArrayItem(deposits, i), hArr);
			merge(hArr, tmp, size, depth, i);
		}

		// complement with 0 if empty, or if not the right power of 2
		if (((uint64_t) 1 << depth) != size) {
			merge(zerohashes[0], tmp, size, depth, size);
		}

		// the next power of two may be smaller than the ultimate virtual size,
		// complement with zero-hashes at each depth.
		for (uint8_t j = depth; j < limitdepth; j++) {
			hashsum(tmp + 32*(j+1), tmp + 32*j, zerohashes[j]);
		}

		uint8_t htrlimit[32];
		htru64(MAX_ATTESTER_SLASHINGS, htrlimit);

		hashsum(htr, tmp + 32*limitdepth, htrlimit);

		free(tmp);
	}
}

void htrVoluntaryExits(cJSON* voluntaryexits, uint8_t* htr){
	uint64_t size = cJSON_GetArraySize(voluntaryexits);
	uint8_t depth = CoverDepth(size);
	uint8_t limitdepth = CoverDepth(MAX_VOLUNTARY_EXITS);
	if(size == 0){
		hashsum(htr, zerohashes[limitdepth], zerohashes[0]);
	}else{
		uint8_t* tmp = (uint8_t*) calloc(32*(limitdepth + 1), sizeof(uint8_t*));
		uint8_t hArr[32];
		// merge in leaf by leaf.
		for(uint64_t i = 0; i < size; i++) {
			htrVoluntaryExit(cJSON_GetArrayItem(voluntaryexits, i), hArr);
			merge(hArr, tmp, size, depth, i);
		}

		// complement with 0 if empty, or if not the right power of 2
		if (((uint64_t) 1 << depth) != size) {
			merge(zerohashes[0], tmp, size, depth, size);
		}

		// the next power of two may be smaller than the ultimate virtual size,
		// complement with zero-hashes at each depth.
		for (uint8_t j = depth; j < limitdepth; j++) {
			hashsum(tmp + 32*(j+1), tmp + 32*j, zerohashes[j]);
		}

		uint8_t htrlimit[32];
		htru64(MAX_ATTESTER_SLASHINGS, htrlimit);

		hashsum(htr, tmp + 32*limitdepth, htrlimit);

		free(tmp);
	}
}

//WIP
void htrSyncAggregate(cJSON* syncaggregate, uint8_t* htr){
	uint8_t tmp[32];
	htrBitVector(cJSON_GetObjectItem(syncaggregate, "sync_committee_bits"), htr);
	htrBLSSignature(cJSON_GetObjectItem(syncaggregate, "sync_committee_signature")->valuestring, tmp);

	hashsum(htr, htr, tmp);
}

void htrBeaconBlockBody(cJSON* body, uint8_t* htr){
	uint8_t* randao_reveal = cJSON_GetObjectItem(body, "randao_reveal")->valuestring + 2;
	uint8_t randao_revealbin[96];
	hex2bin(randao_reveal, 96*2, randao_revealbin, 96);

	uint8_t* graffiti = cJSON_GetObjectItem(body, "graffiti")->valuestring + 2;
	uint8_t graffitibin[32];
	hex2bin(graffiti, 64, graffitibin, 32);
	cJSON* proposer_slashings = cJSON_GetObjectItem(body, "proposer_slashings");
	cJSON* attester_slashings = cJSON_GetObjectItem(body, "attester_slashings");
	cJSON* attestations = cJSON_GetObjectItem(body, "attestations");
	cJSON* deposits = cJSON_GetObjectItem(body, "deposits");
	cJSON* voluntary_exits = cJSON_GetObjectItem(body, "voluntary_exits");

	uint8_t tmp[4][32];
	htrBLSSignature(randao_revealbin, tmp[0]);
	htrEth1Data(cJSON_GetObjectItem(body, "eth1_data"), tmp[1]);
	hashsum(tmp[0], tmp[0], tmp[1]);

	htrProposerSlashings(proposer_slashings, tmp[2]);
	hashsum(tmp[1], graffitibin, tmp[2]);

	hashsum(tmp[0], tmp[0], tmp[1]);

	htrAttesterSlashings(attester_slashings, tmp[1]);
	htrAttestations(attestations, tmp[2]);
	hashsum(tmp[1], tmp[1], tmp[2]);

	htrDeposits(deposits, tmp[2]);
	htrVoluntaryExits(voluntary_exits, tmp[3]);

	hashsum(tmp[2], tmp[2], tmp[3]);

	hashsum(tmp[1], tmp[1], tmp[2]);

	hashsum(htr, tmp[0], tmp[1]);
}

void htrBeaconBlock(cJSON* block, uint8_t* htr){
	uint64_t slot;
	sscanf(cJSON_GetObjectItem(block, "slot")->valuestring, "%"PRIu64, &slot);

	uint64_t proposer_index;
	sscanf(cJSON_GetObjectItem(block, "proposer_index")->valuestring, "%"PRIu64, &proposer_index);

	uint8_t* parent_root = cJSON_GetObjectItem(block, "parent_root")->valuestring + 2;
	uint8_t parent_rootbin[32];
	hex2bin(parent_root, 64, parent_rootbin, 32);

	uint8_t* state_root = cJSON_GetObjectItem(block, "state_root")->valuestring + 2;
	uint8_t state_rootbin[32];
	hex2bin(state_root, 64, state_rootbin, 32);

	uint8_t tmp[2][32];
	htru64(slot, tmp[0]);
	htru64(proposer_index, tmp[1]);
	hashsum(tmp[0], tmp[0], tmp[1]);

	hashsum(tmp[1], parent_rootbin, state_rootbin);

	hashsum(tmp[0], tmp[0], tmp[1]);

	htrBeaconBlockBody(cJSON_GetObjectItem(block, "body"), tmp[1]);
	hashsum(tmp[1], tmp[1], zerohashes[0]);

	hashsum(tmp[1], tmp[1], zerohashes[1]);

	hashsum(htr, tmp[0], tmp[1]);
}

//WIP
void htrBitVector(){
	
}

//WIP
void htrBeaconBlockBodyV2(cJSON* body, uint8_t* htr){
	uint8_t* randao_reveal = cJSON_GetObjectItem(body, "randao_reveal")->valuestring + 2;
	uint8_t randao_revealbin[96];
	hex2bin(randao_reveal, 96*2, randao_revealbin, 96);

	uint8_t* graffiti = cJSON_GetObjectItem(body, "graffiti")->valuestring + 2;
	uint8_t graffitibin[32];
	hex2bin(graffiti, 64, graffitibin, 32);
	cJSON* proposer_slashings = cJSON_GetObjectItem(body, "proposer_slashings");
	cJSON* attester_slashings = cJSON_GetObjectItem(body, "attester_slashings");
	cJSON* attestations = cJSON_GetObjectItem(body, "attestations");
	cJSON* deposits = cJSON_GetObjectItem(body, "deposits");
	cJSON* voluntary_exits = cJSON_GetObjectItem(body, "voluntary_exits");
	cJSON* sync_aggregate = cJSON_GetObjectItem(body, "sync_aggregate");

	uint8_t tmp[4][32];
	htrBLSSignature(randao_revealbin, tmp[0]);
	htrEth1Data(cJSON_GetObjectItem(body, "eth1_data"), tmp[1]);
	hashsum(tmp[0], tmp[0], tmp[1]);

	
	htrProposerSlashings(proposer_slashings, tmp[2]);
	hashsum(tmp[1], graffitibin, tmp[2]);

	hashsum(tmp[0], tmp[0], tmp[1]);

	htrAttesterSlashings(attester_slashings, tmp[1]);
	htrAttestations(attestations, tmp[2]);
	hashsum(tmp[1], tmp[1], tmp[2]);

	htrDeposits(deposits, tmp[2]);
	htrVoluntaryExits(voluntary_exits, tmp[3]);

	hashsum(tmp[2], tmp[2], tmp[3]);

	hashsum(tmp[1], tmp[1], tmp[2]);

	hashsum(htr, tmp[0], tmp[1]);
}