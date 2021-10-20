int pk_in_keystore(char * public_key_hex, int offset);
void ikm_sk(char* info);
void sk_to_pk(blst_p1* pk);
void sign_pk(blst_p2* sig, blst_p2* hash);
void reset();
void store_pk(char* public_key_hex);
int get_keystore_size();
void getkeys(char* public_keys_hex_store_ns);

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

void print_pk(char* public_key_hex, char* buff){
        //printf("0x");
        strcat(buff, "0x");
        for(int i = 0; i < 96; i++) {
          //printf("%c", public_key_hex[i]);
          char str[2] = {public_key_hex[i], '\0'};
          strcat(buff, str);
        }
        //printf("\n");
        strcat(buff, "\n");
}

void print_sig(char* sig_hex, char* buff){
        //printf("0x");
        strcat(buff, "0x");
        for(int i = 0; i < 192; i++) {
          //printf("%c", sig_hex[i]);
          char str[2] = {sig_hex[i], '\0'};
          strcat(buff, str);
        }
        //printf("\n");
        strcat(buff, "\n");
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

int pk_parse(char* pk_hex, blst_p1_affine* pk, char* buff){
        byte pk_bin[48];
        int offset = parse(pk_hex, 96);
        int error = 0;

        if(offset == -1){
            //printf("Incorrect public key length. It must be 96 characters long.\n");
            strcat(buff, "Incorrect public key length. It must be 96 characters long.\n");
            error = 1;
        }else{
            if(char_chk(pk_hex + offset, 96)){
                //printf("Public key contains incorrect characters.\n");
                strcat(buff, "Public key contains incorrect characters.\n");
                error = 1;
            }else{
                if(hex2bin(pk_hex + offset, 96, pk_bin, 48) == 0) {
                    //printf("Failed converting public key to binary array\n");
                    strcat(buff, "Failed converting public key to binary array\n");
                    error = 1;
                }else{
                    blst_p1_uncompress(pk, pk_bin);
                }
            }
        }
        
        return error;
}

int msg_parse(char* msg, uint8_t* msg_bin, char* buff){

        int offset = parse(msg, 64);
        int error = 0;

        if(offset == -1){
            //printf("Incorrect message length. It must be 64 characters long.\n");
            strcat(buff, "Incorrect message length. It must be 64 characters long.\n");
            error = 1;
        }else{
            if(char_chk(msg + offset, 64)){
                //printf("Message contains incorrect characters.\n");
                strcat(buff, "Message contains incorrect characters.\n");
                error = 1;
            }else{
                if(hex2bin(msg + offset, 64, msg_bin, 32) == 0) {
                    //printf("Failed converting message to binary array\n");
                    strcat(buff, "Failed converting message to binary array\n");
                    error = 1;
                }
            }
        }

        return error;
}

int sig_parse(char* sig_hex, blst_p2_affine* sig, char* buff){
        byte sig_bin[96];
        int offset = parse(sig_hex, 192);
        int error = 0;

        if(offset == -1){
            //printf("Incorrect signature length. It must be 192 characters long.\n");
            strcat(buff, "Incorrect signature length. It must be 192 characters long.\n");
            error = 1;
        }else{
            if(char_chk(sig_hex + offset, 192)){
                //printf("Signature contains incorrect characters.\n");
                strcat(buff, "Signature contains incorrect characters.\n");
                error = 1;
            }else{
                if(hex2bin(sig_hex + offset, 192, sig_bin, 96) == 0) {
                    //printf("Failed converting signature to binary array\n");
                    strcat(buff, "Failed converting signature to binary array\n");
                    error = 1;
                }else{
                    blst_p2_uncompress(sig, sig_bin);
                }
            }
        }
        return error;
}