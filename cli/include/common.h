#ifndef COMMON_H
#define COMMON_H

#include "blst.h"
#include <stdlib.h>

#ifdef EMU

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

#endif

int pk_in_keystore(char * public_key_hex, int offset);
void ikm_sk(char* info);
void sk_to_pk(blst_p1* pk);
void sign_pk(blst_p2* sig, blst_p2* hash);
void reset();
void store_pk(char* public_key_hex);
int get_keystore_size();
void getkeys(char* public_keys_hex_store_ns);
void import_sk(blst_scalar* sk_imp);

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
#ifdef EMU
        strcat(buff, "0x");
		for(int i = 0; i < 96; i++) {
			char str[2] = {public_key_hex[i], '\0'};
        	strcat(buff, str);
		}
        strcat(buff, "\n");
#else
		printf("0x");
		for(int i = 0; i < 96; i++) {
			printf("%c", public_key_hex[i]);
		}
		printf("\n");
#endif  
}

void print_sig(char* sig_hex, char* buff){
#ifdef EMU
        
        strcat(buff, "0x");
        for(int i = 0; i < 192; i++) {
          char str[2] = {sig_hex[i], '\0'};
          strcat(buff, str);
        }
        strcat(buff, "\n");
#else
		printf("0x");
		for(int i = 0; i < 192; i++) {
          printf("%c", sig_hex[i]);
        }
        printf("\n");
#endif
}

int pk_parse(char* pk_hex, blst_p1_affine* pk, char* buff){
        byte pk_bin[48];
        int offset = parse(pk_hex, 96);
        int error = 0;

        if(offset == -1){
#ifndef EMU
            printf("Incorrect public key length. It must be 96 characters long.\n");
#else
            strcat(buff, "Incorrect public key length. It must be 96 characters long.\n");
#endif
            error = 1;
        }else{
            if(char_chk(pk_hex + offset, 96)){
#ifndef EMU
                printf("Public key contains incorrect characters.\n");
#else
                strcat(buff, "Public key contains incorrect characters.\n");
#endif
                error = 1;
            }else{
                if(hex2bin(pk_hex + offset, 96, pk_bin, 48) == 0) {
#ifndef EMU
                    printf("Failed converting public key to binary array\n");
#else
                    strcat(buff, "Failed converting public key to binary array\n");
#endif
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
#ifndef EMU
            printf("Message contains incorrect characters.\n");
#else
            strcat(buff, "Message contains incorrect characters.\n");
#endif
            error = 1;
        }else{
            if(hex2bin(msg + offset, len, msg_bin, len/2 + len%2) == 0) {
#ifndef EMU
                printf("Failed converting message to binary array\n");
#else
                strcat(buff, "Failed converting message to binary array\n");
#endif
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
#ifndef EMU
            printf("Incorrect signature length. It must be 192 characters long.\n");
#else
            strcat(buff, "Incorrect signature length. It must be 192 characters long.\n");
#endif
            error = 1;
        }else{
            if(char_chk(sig_hex + offset, 192)){
#ifndef EMU
                printf("Signature contains incorrect characters.\n");
#else
                strcat(buff, "Signature contains incorrect characters.\n");
#endif
                error = 1;
            }else{
                if(hex2bin(sig_hex + offset, 192, sig_bin, 96) == 0) {
#ifndef EMU
                    printf("Failed converting signature to binary array\n");
#else
                    strcat(buff, "Failed converting signature to binary array\n");
#endif
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
#ifndef EMU
        printf("Public key: \n");
#else
        strcat(buff, "Public key: \n");
#endif
        if(bin2hex(out, sizeof(out), public_key_hex, sizeof(public_key_hex)) == 0) {
#ifndef EMU
            printf("Failed converting binary key to string\n");
#else
            strcat(buff, "Failed converting binary key to string\n");
#endif
        }
    
        store_pk(public_key_hex);
#ifndef EMU
        print_pk(public_key_hex, NULL);
#else
        print_pk(public_key_hex, buff);
#endif
    }else{
#ifndef EMU
        printf("Can't generate more keys. Limit reached.\n");
#else
        strcat(buff, "Can't generate more keys. Limit reached.\n");
#endif
    }
}

void signature(int argc, char** argv, char* buff){
    //Message examples
    //char * msg_hex = "5656565656565656565656565656565656565656565656565656565656565656";
    //char * msg_hex = "b6bb8f3765f93f4f1e7c7348479289c9261399a3c6906685e320071a1a13955c";

    int offset = parse(argv[1], 96);

    if(offset != -1){
        if(pk_in_keystore(argv[1], offset) != -1){
            int len = msg_len(argv[2]);
            uint8_t msg_bin[len/2 + len%2];
#ifndef EMU
            if(msg_parse(argv[2], msg_bin, len, NULL) != 1){
#else
            if(msg_parse(argv[2], msg_bin, len, buff) != 1){
#endif
                blst_p2 hash;
                get_point_from_msg(&hash, msg_bin, len/2 + len%2);

                blst_p2 sig;
                byte sig_bin[96];
                char sig_hex[192];

                sign_pk(&sig, &hash);
                sig_serialize(sig_bin, sig);
#ifndef EMU
                printf("Signature: \n");
#else
                strcat(buff, "Signature: \n");
#endif
                if(bin2hex(sig_bin, sizeof(sig_bin), sig_hex, sizeof(sig_hex)) == 0) {
#ifndef EMU
                    printf("Failed converting binary signature to string\n");
#else
                    strcat(buff, "Failed converting binary signature to string\n");
#endif
                }
#ifndef EMU
                print_sig(sig_hex, NULL);
#else
                print_sig(sig_hex, buff);
#endif
            }
        }else{
#ifndef EMU
            printf("Public key isn't stored\n");
#else
            strcat(buff, "Public key isn't stored\n");
#endif
        }
    }else{
#ifndef EMU
        printf("Incorrect public key length. It must be 96 characters long\n");
#else
        strcat(buff, "Incorrect public key length. It must be 96 characters long\n");
#endif
    }
}

void verify(int argc, char** argv, char* buff){
    char dst[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_"; //IETF BLS Signature V4

    blst_p1_affine pk;
    blst_p2_affine sig;
    int len = msg_len(argv[2]);
    uint8_t msg_bin[len/2 + len%2];
#ifndef EMU
    if((pk_parse(argv[1], &pk, NULL) || msg_parse(argv[2], msg_bin, len, NULL) || sig_parse(argv[3], &sig, NULL)) != 1){
        if(blst_core_verify_pk_in_g1(&pk, &sig, 1, msg_bin, len/2 + len%2, dst, sizeof(dst)-1, NULL, 0) != BLST_SUCCESS){
            printf("Error\n");
        }
        else {
            printf("Success\n");
        }
    }
#else
    if((pk_parse(argv[1], &pk, buff) || msg_parse(argv[2], msg_bin, len, buff) || sig_parse(argv[3], &sig, buff)) != 1){
        if(blst_core_verify_pk_in_g1(&pk, &sig, 1, msg_bin, len/2 + len%2, dst, sizeof(dst)-1, NULL, 0) != BLST_SUCCESS){
            strcat(buff, "Error\n");
        }
        else {
            strcat(buff, "Success\n");
        }
    }
#endif
}

void get_keys(int argc, char** argv, char* buff){
    int keystore_size = get_keystore_size();
    char public_keys_hex_store[96*keystore_size];
    getkeys(public_keys_hex_store);
    if(keystore_size != 0){
        int j = 0;
        int cont = keystore_size - 1;
        int counter = keystore_size;
#ifndef EMU
        printf("{'keys':['");
        for(int i = 0; i < 96 * cont + 96; i++){
            printf("%c", public_keys_hex_store[i]);
            j++;
            if (j == 96){
                if(counter > 1) {
                    printf("'\n'");
                } else {
                    printf("']}\n");
                }               
                j = 0;
                counter--;
            }           
        }
    }else{
        printf("There are no keys stored\n");
    }
#else
        strcat(buff, "{'keys':['");
        for(int i = 0; i < 96 * cont + 96; i++){
            char str[2] = {public_keys_hex_store[i], '\0'};
            strcat(buff, str);
            j++;
            if (j == 96){
                if(counter > 1) {
                    strcat(buff, "'\n'");
                } else {
                    strcat(buff, "']}\n");
                }               
                j = 0;
                counter--;
            }           
        }
    }else{
        strcat(buff, "There are no keys stored\n");
    }
#endif
}

void resetc(int argc, char** argv, char* buff){
    reset();
#ifndef EMU
    printf("Keys deleted\n");
#else
    strcat(buff, "Keys deleted\n");
#endif
}

void import(int argc, char** argv, char* buff){
    int offset = parse(argv[1], 64);

    if(offset != -1){
        if(!char_chk(argv[1] + offset, 64)){
            byte sk_bin[32];
            if(hex2bin(argv[1] + offset, 64, sk_bin, 32) == 0){
#ifndef EMU
                printf("Failed converting hex to bin\n");
#else
                strcat(buff, "Failed converting hex to bin\n");
#endif
            }else{
                blst_scalar sk_imp;
                blst_scalar_from_bendian(&sk_imp, sk_bin);
                import_sk(&sk_imp);

                blst_p1 pk;
                sk_to_pk(&pk);
                byte pk_bin[48];
                pk_serialize(pk_bin, pk);
                char pk_hex[96];
                if(bin2hex(pk_bin, 48, pk_hex, 96) == 0){
#ifndef EMU
                    printf("Failed converting bin to hex\n");
#else
                    strcat(buff, "Failed converting bin to hex\n");
#endif
                }else{
                    store_pk(pk_hex);
#ifndef EMU
                    print_pk(pk_hex, NULL);
#else
                    print_pk(pk_hex, buff);
#endif
                }
            }
        }else{
#ifndef EMU
            printf("Incorrect characters\n");
#else
            strcat(buff, "Incorrect characters\n");
#endif
        }
    }else{
#ifndef EMU
        printf("Incorrect secret key length\n");
#else
        strcat(buff, "Incorrect secret key length\n");
#endif
    }
}

#endif