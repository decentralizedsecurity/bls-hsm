int pk_in_keystore(char * public_key_hex, int offset);
void ikm_sk(char* info);
void sk_to_pk(blst_p1* pk);
void sign_pk(blst_p2* sig, blst_p2* hash);
void reset();
void store_pk(char* public_key_hex);
int get_keystore_size();
void getkeys(char* public_keys_hex_store_ns);

void print_pk(char* public_key_hex){
        printf("0x");
        for(int i = 0; i < 96; i++) {
          printf("%c", public_key_hex[i]);
        }
        printf("\n");
}

void print_sig(char* sig_hex){
        printf("0x");
        for(int i = 0; i < 192; i++) {
          printf("%c", sig_hex[i]);
        }
        printf("\n");
}

void pk_serialize(byte* out, blst_p1 pk){
        blst_p1_compress(out, &pk);
}

void sig_serialize(byte* out2, blst_p2 sig){
        blst_p2_compress(out2, &sig);
}

void get_point_from_msg(blst_p2* hash, uint8_t* msg_bin){
        char dst[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP"; //IETF BLS Signature V4
        //Obtain the point from a message
        blst_hash_to_g2(hash, msg_bin, 32, dst, sizeof(dst), NULL, 0);
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

int pk_parse(char* pk_hex, blst_p1_affine* pk){
        byte pk_bin[48];
        int offset = parse(pk_hex, 96);
        int error = 0;

        if(offset == -1){
            printf("Incorrect public key length. It must be 96 characters long.\n");
            error = 1;
        }else{
            if(char_chk(pk_hex + offset, 96)){
                printf("Public key contains incorrect characters.\n");
                error = 1;
            }else{
                if(hex2bin(pk_hex + offset, 96, pk_bin, 48) == 0) {
                    printf("Failed converting public key to binary array\n");
                    error = 1;
                }else{
                    blst_p1_uncompress(pk, pk_bin);
                }
            }
        }
        
        return error;
}

int msg_parse(char* msg, uint8_t* msg_bin){

        int offset = parse(msg, 64);
        int error = 0;

        if(offset == -1){
            printf("Incorrect message length. It must be 64 characters long.\n");
            error = 1;
        }else{
            if(char_chk(msg + offset, 64)){
                printf("Message contains incorrect characters.\n");
                error = 1;
            }else{
                if(hex2bin(msg + offset, 64, msg_bin, 32) == 0) {
                    printf("Failed converting message to binary array\n");
                    error = 1;
                }
            }
        }

        return error;
}

int sig_parse(char* sig_hex, blst_p2_affine* sig){
        byte sig_bin[96];
        int offset = parse(sig_hex, 192);
        int error = 0;

        if(offset == -1){
            printf("Incorrect signature length. It must be 192 characters long.\n");
            error = 1;
        }else{
            if(char_chk(sig_hex + offset, 192)){
                printf("Signature contains incorrect characters.\n");
                error = 1;
            }else{
                if(hex2bin(sig_hex + offset, 192, sig_bin, 96) == 0) {
                    printf("Failed converting signature to binary array\n");
                    error = 1;
                }else{
                    blst_p2_uncompress(sig, sig_bin);
                }
            }
        }
        return error;
}