#include "common.h"
#include <stdlib.h>
#include <string.h>
#include "blst.h"


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


/*
Get offset to first char of hex string 'str' with expected length 'len'
If length is incorrect returns -1. If string contains not hexadecimal characters returns -2
*/
int parse_hex(char* str, int len){
        int offset;

        int strl = strlen(str);

        if((str[0] == '0') && (str[1] == 'x')){
            if(strl == len+2){
                 offset = 2;
            }else{
                offset = BADLEN;
            }
        }else{
            if(strl == len){
                 offset = 0;
            }else{
                offset = BADLEN;
            }
        }
		if(offset >= 0){
			if(char_chk(str + offset, len)){
				offset = BADFORMAT;
			}
		}

        return offset;
}

/*
Checks if string 'aux' with length 'len' contains hexadecimal characters.
On error returns 1
*/
int char_chk(char* aux, int len){
        int error = 0;

        for(int i = 0; (i < len) && (error == 0); i++){
            if(!((aux[i] >= '0' && aux[i] <= '9') || (aux[i] >= 'a' && aux[i] <= 'f') || (aux[i] >= 'A' && aux[i] <= 'F'))){
                error = 1;
            }
        }

        return error;
}

/*
Adds public key 'public_key_hex' to given buffer 'buff'
*/
void print_pk(char* public_key_hex, char* buff){
        strcat(buff, "0x");
		for(int i = 0; i < 96; i++) {
			char str[2] = {public_key_hex[i], '\0'};
        	strcat(buff, str);
		}
        strcat(buff, "\n");
}

/*
Get effective length of hexadecimal string 'msg'
*/
int msg_len(char* msg){
    int len;
    if(msg[0] == '0' && msg[1] == 'x'){
        len = strlen(msg + 2);
    }else{
        len = strlen(msg);
    }
    return len;
}

