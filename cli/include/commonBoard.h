#ifndef COMMONBOARD_H
#define COMMONBOARD_H

#include "blst.h"
#include <stdlib.h>
#include <errno.h>

/*
    All functions must return -1 on error and 0 on success, also functions must add the error code to the variable errno.
*/

/*
********************************************************ERRORS*****************************************************************
*/
#define noKeys 0
#define noKey 1
#define incorrectPubKeyLength 2
#define failBinaryToStr 3
#define failHexToBin 4
#define failBinToHex 5
#define keyAlreadyIn 6
#define incorrectCharacters 7
#define incorrectPrivKeyLength 8
#define tooManyKeys 9
#define keystoreTooBig 10
#define passwordTooBig 11
#define notSamePasswordsKeystores 12

void print_error(){
    switch(errno){
        case noKeys:
            printf("Error %d: No keys stored.\n", noKeys);
            break;
        case noKey:
            printf("Error %d: Keys is not stored.\n", noKey);
            break;
        case incorrectPubKeyLength:
            printf("Error %d: incorrect Public Key length.\n", incorrectPubKeyLength);
            break;
        case failBinaryToStr:
            printf("Error %d: fail converting binary to string.\n", failBinaryToStr);
            break;
        case failHexToBin:
            printf("Error %d: fail converting hex to binary.\n", failHexToBin);
            break;
        case failBinToHex:
            printf("Error %d: fail converting binary to hex.\n", failBinToHex);
            break;
        case keyAlreadyIn:
            printf("Error %d: key already imported.\n", keyAlreadyIn);
            break;
        case incorrectCharacters:
            printf("Error %d: Private Key with incorrect characters.\n", incorrectCharacters);
            break;
        case incorrectPrivKeyLength:
            printf("Error %d: incorrect Private Key length.\n", incorrectPrivKeyLength);
            break;
        case tooManyKeys:
            printf("Error %d: too many Keys already imported.\n", tooManyKeys);
            break;
        case keystoreTooBig:
            printf("Error %d: keystore too big.\n", keystoreTooBig);
            break;
        case passwordTooBig:
            printf("Error %d: password for keystore too big.\n", passwordTooBig);
            break;
        case notSamePasswordsKeystores:
            printf("Error %d: there are not the same number of passwords than keystores.\n", notSamePasswordsKeystores);
            break;
        deafult:
            printf("Error unknown.\n");
    }
}

/*
*******************************************************************************************************************************
*/

int char2hex(char c, uint8_t *x)
{
	if (c >= '0' && c <= '9') {
		*x = c - '0';
	} else if (c >= 'a' && c <= 'f') {
		*x = c - 'a' + 10;
	} else if (c >= 'A' && c <= 'F') {
		*x = c - 'A' + 10;
	} else {
		return -22;
	}

	return 0;
}

int hex2char(uint8_t x, char *c)
{
	if (x <= 9) {
		*c = x + '0';
	} else  if (x <= 15) {
		*c = x - 10 + 'a';
	} else {
		return -22;
	}

	return 0;
}

size_t bin2hex(const uint8_t *buf, size_t buflen, char *hex, size_t hexlen)
{
	if ((hexlen + 1) < buflen * 2) {
		return 0;
	}

	for (size_t i = 0; i < buflen; i++) {
		if (hex2char(buf[i] >> 4, &hex[2 * i]) < 0) {
			return 0;
		}
		if (hex2char(buf[i] & 0xf, &hex[2 * i + 1]) < 0) {
			return 0;
		}
	}

	hex[2 * buflen] = '\0';
	return 2 * buflen;
}

size_t hex2bin(const char *hex, size_t hexlen, uint8_t *buf, size_t buflen)
{
	uint8_t dec;

	if (buflen < hexlen / 2 + hexlen % 2) {
		return 0;
	}

	/* if hexlen is uneven, insert leading zero nibble */
	if (hexlen % 2) {
		if (char2hex(hex[0], &dec) < 0) {
			return 0;
		}
		buf[0] = dec;
		hex++;
		buf++;
	}

	/* regular hex conversion */
	for (size_t i = 0; i < hexlen / 2; i++) {
		if (char2hex(hex[2 * i], &dec) < 0) {
			return 0;
		}
		buf[i] = dec << 4;

		if (char2hex(hex[2 * i + 1], &dec) < 0) {
			return 0;
		}
		buf[i] += dec;
	}

	return hexlen / 2 + hexlen % 2;
}

int pk_in_keystore(char * public_key_hex, int offset);
void ikm_sk(char* info);
void sk_to_pk(blst_p1* pk);
void sign_pk(blst_p2* sig, blst_p2* hash);
void reset();
void store_pk(char* public_key_hex);
int get_keystore_size();
void getkeys(char* public_keys_hex_store_ns);
int import_sk(blst_scalar* sk_imp);

void pk_serialize(byte* out, blst_p1 pk){
        blst_p1_compress(out, &pk);
}

void sig_serialize(byte* out2, blst_p2 sig){
        blst_p2_compress(out2, &sig);
}

void get_point_from_msg(blst_p2* hash, uint8_t* msg_bin, int len){
        char dst[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_"; //IETF BLS Signature V4
        //Obtain the point from a message
        blst_hash_to_g2(hash, msg_bin, len, dst, sizeof(dst)-1, NULL, 0);
}

int parse(char* str, int len){
        int offset;

        int strl = strlen(str);

        if((str[0] == '0') && (str[1] == 'x')){
            if(strl == len+2){
                 offset = 2;
            }else{
                offset = -1;
            }
        }else{
            if(strl == len){
                 offset = 0;
            }else{
                offset = -1;
            }
        }

        return offset;
}

int char_chk(char* aux, int len){
        int error = 0;

        for(int i = 0; (i < len) && (error == 0); i++){
            if(!((aux[i] >= '0' && aux[i] <= '9') || (aux[i] >= 'a' && aux[i] <= 'f') || (aux[i] >= 'A' && aux[i] <= 'F'))){
                error = 1;
            }
        }

        return error;
}

void print_pk(char* public_key_hex, char* buff){
        strcat(buff, "0x");
		for(int i = 0; i < 96; i++) {
			char str[2] = {public_key_hex[i], '\0'};
        	strcat(buff, str);
		}
}

void print_sig(char* sig_hex, char* buff){    
        strcat(buff, "0x");
        for(int i = 0; i < 192; i++) {
          char str[2] = {sig_hex[i], '\0'};
          strcat(buff, str);
        }
}

int pk_parse(char* pk_hex, blst_p1_affine* pk, char* buff){
        byte pk_bin[48];
        int offset = parse(pk_hex, 96);
        int error = 0;

        if(offset == -1){
            strcat(buff, "Incorrect public key length. It must be 96 characters long.\n");
            error = 1;
        }else{
            if(char_chk(pk_hex + offset, 96)){
                strcat(buff, "Public key contains incorrect characters.\n");
                error = 1;
            }else{
                if(hex2bin(pk_hex + offset, 96, pk_bin, 48) == 0) {
                    strcat(buff, "Failed converting public key to binary array\n");
                    error = 1;
                }else{
                    blst_p1_uncompress(pk, pk_bin);
                }
            }
        }
        
        return error;
}

int msg_parse(char* msg, uint8_t* msg_bin, int len, char* buff){

        int offset;
        if(msg[0] == '0' && msg[1] == 'x'){
            offset = 2;
        }else{
            offset = 0;
        }
        int error = 0;

        if(char_chk(msg + offset, len)){
            strcat(buff, "Message contains incorrect characters.\n");
            error = 1;
        }else{
            if(hex2bin(msg + offset, len, msg_bin, len/2 + len%2) == 0) {
                strcat(buff, "Failed converting message to binary array\n");
                error = 1;
            }
        }


        return error;
}

int sig_parse(char* sig_hex, blst_p2_affine* sig, char* buff){
        byte sig_bin[96];
        int offset = parse(sig_hex, 192);
        int error = 0;

        if(offset == -1){
            strcat(buff, "Incorrect signature length. It must be 192 characters long.\n");
            error = 1;
        }else{
            if(char_chk(sig_hex + offset, 192)){
                strcat(buff, "Signature contains incorrect characters.\n");
                error = 1;
            }else{
                if(hex2bin(sig_hex + offset, 192, sig_bin, 96) == 0) {
                    strcat(buff, "Failed converting signature to binary array\n");
                    error = 1;
                }else{
                    blst_p2_uncompress(sig, sig_bin);
                }
            }
        }
        return error;
}

int msg_len(char* msg){
    int len;
    if(msg[0] == '0' && msg[1] == 'x'){
        len = strlen(msg + 2);
    }else{
        len = strlen(msg);
    }
    return len;
}

void keygen(int argc, char** argv, char* buff){
    int keystore_size = get_keystore_size();

    if(keystore_size < 10){
        // key_info is an optional parameter.  This parameter MAY be used to derive
        // multiple independent keys from the same IKM.  By default, key_info is the empty string.
        char info[] = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    
        if(argc == 2){
                if(strlen(argv[1]) <= strlen(info)){
                        strcpy(info, argv[1]);
                }else{
                        strncpy(info, argv[1], sizeof(info));
                }
        }

        ikm_sk(info);
    
        //The secret key allow us to generate the associated public key
        blst_p1 pk;
        byte out[48];
        char public_key_hex[96];
        sk_to_pk(&pk);
        pk_serialize(out, pk);
        strcat(buff, "Public key: \n");
        if(bin2hex(out, sizeof(out), public_key_hex, sizeof(public_key_hex)) == 0) {
            strcat(buff, "Failed converting binary key to string\n");
        }
    
        store_pk(public_key_hex);
        print_pk(public_key_hex, buff);
    }else{
        strcat(buff, "Can't generate more keys. Limit reached.\n");
    }
}

int signature(char* pubKey, char* signingRoot, char* responseSigned){
    //Message examples
    //char * msg_hex = "5656565656565656565656565656565656565656565656565656565656565656";
    //char * msg_hex = "b6bb8f3765f93f4f1e7c7348479289c9261399a3c6906685e320071a1a13955c";

    int offset = parse(pubKey, 96);

    responseSigned[0] = '\0';

    if(offset != -1){
        if(pk_in_keystore(pubKey, offset) != -1){
            int len = msg_len(signingRoot);
            uint8_t msg_bin[len/2 + len%2];
            if(msg_parse(signingRoot, msg_bin, len, responseSigned) != 1){
                blst_p2 hash;
                get_point_from_msg(&hash, msg_bin, len/2 + len%2);

                blst_p2 sig;
                byte sig_bin[96];
                char sig_hex[192];

                sign_pk(&sig, &hash);
                sig_serialize(sig_bin, sig);

                if(bin2hex(sig_bin, sizeof(sig_bin), sig_hex, sizeof(sig_hex)) == 0) {
                    errno = failBinaryToStr;
                    return -1;
                }
                print_sig(sig_hex, responseSigned);
            }
        }else{
            errno = noKey;
            return -1;
        }
    }else{
        errno = incorrectPubKeyLength;
        return -1;
    }

    return 0;
}

void verify(int argc, char** argv, char* buff){
    char dst[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_"; //IETF BLS Signature V4

    blst_p1_affine pk;
    blst_p2_affine sig;
    int len = msg_len(argv[2]);
    uint8_t msg_bin[len/2 + len%2];
    if((pk_parse(argv[1], &pk, buff) || msg_parse(argv[2], msg_bin, len, buff) || sig_parse(argv[3], &sig, buff)) != 1){
        if(blst_core_verify_pk_in_g1(&pk, &sig, 1, msg_bin, len/2 + len%2, dst, sizeof(dst)-1, NULL, 0) != BLST_SUCCESS){
            strcat(buff, "Error\n");
        }
        else {
            strcat(buff, "Success\n");
        }
    }
}

int get_keys(char** keys, int* nKeys){
    int keystore_size = get_keystore_size();
    *nKeys = keystore_size;

    char public_keys_hex_store[96*keystore_size];
    getkeys(public_keys_hex_store);
    if(keystore_size != 0){
        for(int j = 0; j < keystore_size; ++j){
            keys[j][0] = '0';
            keys[j][1] = 'x';

            for(int i = 0; i < 96; ++i){
                keys[j][i + 2] = public_keys_hex_store[(96*j) + i];
            }

            keys[j][98] = '\0';
        }
    }else{
        errno = noKeys;
        return -1;
    }

    return 0;
}

void resetc(int argc, char** argv, char* buff){
    reset();
    strcat(buff, "Keys deleted\n");
}

int import(char* privateKey){
    if(get_keystore_size() < 10){
        int offset = parse(privateKey, 64);

        if(offset != -1){
            if(!char_chk(privateKey + offset, 64)){
                byte sk_bin[32];
                if(hex2bin(privateKey + offset, 64, sk_bin, 32) == 0){
                    errno = failHexToBin;
                    return -1;
                }else{
                    blst_scalar sk_imp;
                    blst_scalar_from_bendian(&sk_imp, sk_bin);
                    if(import_sk(&sk_imp) == 0){

                        blst_p1 pk;
                        sk_to_pk(&pk);
                        byte pk_bin[48];
                        pk_serialize(pk_bin, pk);
                        char pk_hex[96];
                        if(bin2hex(pk_bin, 48, pk_hex, 96) == 0){
                            errno = failBinToHex;
                            return -1;
                        }else{
                            store_pk(pk_hex);
                        }
                    }else{
                        errno = keyAlreadyIn;
                        return -1;
                    }
                }
            }else{
                errno = incorrectCharacters;
                return -1;
            }
        }else{
            errno = incorrectPrivKeyLength;
            return -1;
        }
    }else{
        errno = tooManyKeys;
        return -1;
    }
}

int import_from_keystore(char** keystores, char** passwords, int nKeys){
    
    
    return 0;
}

#endif