#include "bls_hsm_ns.h"
#include "common.h"
#include <stdlib.h>
#include <string.h>

/*
Converts input 'pk' to binary 'out'
*/
void pk_serialize(byte* out, blst_p1 pk){
        blst_p1_compress(out, &pk);
}

/*
Converts input 'sig' to binary 'out2'
*/
void sig_serialize(byte* out2, blst_p2 sig){
        blst_p2_compress(out2, &sig);
}

/*
Get 'hash' from binary string 'msg_bin' with length 'len'
*/
void get_point_from_msg(blst_p2* hash, uint8_t* msg_bin, int len){
        char dst[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_"; //IETF BLS Signature V4
        //Obtain the point from a message
        blst_hash_to_g2(hash, msg_bin, len, dst, sizeof(dst)-1, NULL, 0);
}

/*
Derive 'pk' from hexadecimal string 'pk_hex'. Errors are dumped to 'buff'
On error returns 1
*/
int pk_parse(char* pk_hex, blst_p1_affine* pk, char* buff){
        byte pk_bin[48];
        int offset = parse_hex(pk_hex, 96);
        int error = 0;

        if(offset == BADLEN){
            strcat(buff, "Incorrect public key length. It must be 96 characters long.\n");
            error = BADPKLEN;
        }else if(offset == BADFORMAT){
                strcat(buff, "Public key contains incorrect characters.\n");
                error = offset;
        }else{
            if(hex2bin(pk_hex + offset, 96, pk_bin, 48) == 0) {
                strcat(buff, "Failed converting public key to binary array\n");
                error = HEX2BINERR;
            }else{
                blst_p1_uncompress(pk, pk_bin);
            }
        }
        
        return error;
}

/*
Converts hexadecimal string 'msg' with length 'len' to binary string 'msg_bin'. Errors are dumped to 'buff'.
On error returns 1
*/
int msg_parse(char* msg, uint8_t* msg_bin, int len, char* buff){

        int offset = parse_hex(msg, len);
        int error = 0;
        if(offset == BADFORMAT){
            strcat(buff, "Message contains incorrect characters.\n");
            error = offset;
        }else if(offset == BADLEN){
            strcat(buff, "Message contains incorrect characters.\n");
            error = offset;
        }else{
            if(hex2bin(msg + offset, len, msg_bin, len/2 + len%2) == 0) {
                strcat(buff, "Incorrect message length.\n");
                error = HEX2BINERR;
            }
        }

        return error;
}

/*
Derive 'sig' from hexadecimal string 'sig_hex'. Errors are dumped to 'buff'
On error returns 1
*/
int sig_parse(char* sig_hex, blst_p2_affine* sig, char* buff){
        byte sig_bin[96];
        int offset = parse_hex(sig_hex, 192);
        int error = 0;

        if(offset == BADLEN){
            strcat(buff, "Incorrect signature length. It must be 192 characters long.\n");
            error = BADSIGLEN;
        }else if(offset == BADFORMAT){
            strcat(buff, "Signature contains incorrect characters.\n");
            error = offset;
        }else{
            if(hex2bin(sig_hex + offset, 192, sig_bin, 96) == 0) {
                strcat(buff, "Failed converting signature to binary array\n");
                error = HEX2BINERR;
            }else{
                blst_p2_uncompress(sig, sig_bin);
            }
        }
        return error;
}

/*
Generates random key. Response is dumped to 'buff'
*/
/*
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
*/

int keygen(char* data, char* buff){
    int keystore_size = get_keystore_size();

    if(keystore_size < 10){
        // key_info is an optional parameter.  This parameter MAY be used to derive
        // multiple independent keys from the same IKM.  By default, key_info is the empty string.
        char info[] = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

       //check if data string is empty
        if(strlen(data) != 0){
            if(strlen(data) <= strlen(info)){
                    strcpy(info, data);
            }else{
                    strncpy(info, data, sizeof(info));
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
        return BIN2HEXERR;
        }
        else
        {
        store_pk(public_key_hex);
        print_pk(public_key_hex, buff);
        }
        
    }else{
        strcat(buff, "Can't generate more keys. Limit reached.\n");
        return KEYSLIMIT;
    }
}

/*
Gets hexadecimal string 'signature' from given public key 'pk' and message 'msg' 
*/
/*
void get_signature(char* pk, char* msg, char* signature){
    int offset = parse_hex(pk, 96);
    pk_in_keystore(pk, offset);
    int len = msg_len(msg);
    uint8_t msg_bin[len/2 + len%2];
    offset = parse_hex(msg, len);
    hex2bin(msg + offset, len, msg_bin, len/2 + len%2);
    blst_p2 hash;
    get_point_from_msg(&hash, msg_bin, len/2 + len%2);

    blst_p2 sig;
    byte sig_bin[96];
    char sig_hex[192];

    sign_pk(&sig, &hash);
    sig_serialize(sig_bin, sig);
    bin2hex(sig_bin, sizeof(sig_bin), signature, 192);
}
*/
/*
Signs message with given public key. Public key must be stored. Response is dumped to 'buff'
*/
int signature(char* pk, char* msg, char* buff){
    //Message examples
    //char * msg_hex = "5656565656565656565656565656565656565656565656565656565656565656";
    //char * msg_hex = "b6bb8f3765f93f4f1e7c7348479289c9261399a3c6906685e320071a1a13955c";

    int offset = parse_hex(pk, 96);

    if(offset >= 0){
        if(pk_in_keystore(pk, offset) != -1){
            int len = msg_len(msg);
            uint8_t msg_bin[len/2 + len%2];
            if(msg_parse(msg, msg_bin, len, buff) != 1){
                blst_p2 hash;
                get_point_from_msg(&hash, msg_bin, len/2 + len%2);

                blst_p2 sig;
                byte sig_bin[96];
                char sig_hex[192];

                sign_pk(&sig, &hash);
                sig_serialize(sig_bin, sig);
                if(bin2hex(sig_bin, sizeof(sig_bin), sig_hex, sizeof(sig_hex)) == 0) {
                    strcat(buff, "Failed converting binary signature to string\n");
                }
                strcpy(buff, sig_hex);
            }
        }else{
            strcat(buff, "Public key isn't stored\n");
            return PKNOTFOUND;
        }
    }else if(offset == BADLEN){
        strcat(buff, "Incorrect public key length. It must be 96 characters long\n");
        return BADPKLEN;
    }else{
        strcat(buff, "Public key contains incorrect characters.\n");
        return BADFORMAT;
    }
return OK;
}

/*
Verifies signature of given message and public key
*/
/*
void verify(char** argv, char* buff){
    char dst[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_"; //IETF BLS Signature V4

    blst_p1_affine pk;
    blst_p2_affine sig;
    int len = msg_len(argv[2]);
    uint8_t msg_bin[len/2 + len%2];
    if((pk_parse(argv[1], &pk, buff) || msg_parse(argv[2], msg_bin, len, buff) || sig_parse(argv[3], &sig, buff)) == 0){
        if(blst_core_verify_pk_in_g1(&pk, &sig, 1, msg_bin, len/2 + len%2, dst, sizeof(dst)-1, NULL, 0) != BLST_SUCCESS){
            strcat(buff, "Error\n");
        }
        else {
            strcat(buff, "Success\n");
        }
    }
}
*/
/*
Verifies signature of given message and public key
*/
int verify(char* pk, char* msg, char* sig, char* buff){
    char dst[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_"; //IETF BLS Signature V4

    blst_p1_affine pk_bin;
    blst_p2_affine sig_bin;
    int len = msg_len(msg);
    uint8_t msg_bin[len/2 + len%2];
    if((pk_parse(pk, &pk_bin, buff) || msg_parse(msg, msg_bin, len, buff) || sig_parse(sig, &sig_bin, buff)) == 0){
        if(blst_core_verify_pk_in_g1(&pk_bin, &sig_bin, 1, msg_bin, len/2 + len%2, dst, sizeof(dst)-1, NULL, 0) != BLST_SUCCESS){
            strcat(buff, "BLSTFAIL\n");
            return BLSTFAIL;
        }
        else {
            strcat(buff, "BLSTSUCCESS\n");
            return BLSTSUCCESS;
        }
    }
}
/*
Get array of stored public keys in buffer 'buff'
*/
void dump_keys(char* buff){
    int keystore_size = get_keystore_size();
    char public_keys_hex_store[keystore_size][96];
    getkeys(public_keys_hex_store);
    if(keystore_size != 0){
        strcat(buff, "{\"keys\":[\"");
        for(int i = 0; i < keystore_size; i++){
            for(int j = 0; j < 96; j++){
                char str[2] = {public_keys_hex_store[i][j], '\0'};
                strcat(buff, str);
            }
            if (i + 1 < keystore_size){
                strcat(buff, "\",\n\"");
            } else {
                strcat(buff, "\"]}\n");
            }                        
        }
    }else{
        strcat(buff, "There are no keys stored\n");
    }
}

/*
Delete all stored public and secret keys. Response is dumped to 'buff'
*/
/*
void resetc(char* buff){
    reset();
    strcat(buff, "Keys deleted\n");
}
*/

void resetc(char* buff){
    reset();
    strcat(buff, "Keys deleted\n");
}

/*
Import given secret key. Derived public key and errors are dumped to 'buff'
*/
/*
void import(char* sk, char* buff){
    if(get_keystore_size() < 10){
        int offset = parse_hex(sk, 64);

        if(offset >= 0){
            byte sk_bin[32];
            if(hex2bin(sk + offset, 64, sk_bin, 32) == 0){
                strcat(buff, "Failed converting hex to bin\n");
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
                        strcat(buff, "Failed converting bin to hex\n");
                    }else{
                        store_pk(pk_hex);
                        print_pk(pk_hex, buff);
                    }
                }else{
                        strcat(buff, "Key already imported\n");
                }
            }
        }else if(offset == BADFORMAT){
            strcat(buff, "Incorrect characters\n");
        }else{
            strcat(buff, "Incorrect secret key length\n");
        }
    }else{
            strcat(buff, "Limit reached\n");
           
    }
}
*/

/*
Import given secret key. Derived public key and errors are dumped to 'buff'
*/
int import(char* sk, char* buff){
    if(get_keystore_size() < 10){
        int offset = parse_hex(sk, 64);

        if(offset >= 0){
            byte sk_bin[32];
            if(hex2bin(sk + offset, 64, sk_bin, 32) == 0){
                strcat(buff, "Failed converting hex to bin\n");

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
                        strcat(buff, "Failed converting bin to hex\n");
                    }else{
                        store_pk(pk_hex);
                        print_pk(pk_hex, buff);
                        return OK;

                    }
                }else{
                        strcat(buff, "Key already imported\n");
                }
            }
        }else if(offset == BADFORMAT){
            strcat(buff, "Incorrect characters\n");
        }else{
            strcat(buff, "Incorrect secret key length\n");
            return BADSKLEN;  

        }
    }else{
            strcat(buff, "Limit reached\n");
            return KEYSLIMIT;

    }
}