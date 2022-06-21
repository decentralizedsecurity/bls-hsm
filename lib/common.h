#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <string.h>

#define BADLEN -1
#define BADFORMAT -2
#define OK  0
#define NOKEYS 1
#define PKNOTFOUND 2
#define BADPKLEN 3
#define EXISTINGKEY 4
#define BADSKLEN 5
#define KEYSLIMIT 6
#define HEX2BINERR 7
#define BIN2HEXERR 8
#define BADSIGLEN 9
#define BADJSON 10
#define BLSTSUCCESS 11
#define BLSTFAIL 12
#define INVPASSWORD 13
#define BADJSONFORMAT 14

#ifndef NRF
#include "blst.h"

int char2hex(char c, uint8_t *x);

int hex2char(uint8_t x, char *c);

size_t bin2hex(const uint8_t *buf, size_t buflen, char *hex, size_t hexlen);

size_t hex2bin(const char *hex, size_t hexlen, uint8_t *buf, size_t buflen);

#endif

int parse_hex(char* str, int len);

int char_chk(char* aux, int len);

void print_pk(char* public_key_hex, char* buff);

void print_sig(char* sig_hex, char* buff);

int msg_len(char* msg);

#endif