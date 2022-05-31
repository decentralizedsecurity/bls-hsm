#include "import_keystore.h"
#include "../lib/cJSON.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <wolfssl/wolfcrypt/pwdbased.h>
#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/sha256.h>


#define WOLFSSL_AES_COUNTER

#define HEX2BINERR 7
#define BIN2HEXERR 8
#define INVPASSWORD 11

char private_key[] = "3604f89dbd5161c5b214dd15afab350048a9c74ca1f657d93b02d14088f55ad0";//Private key from another keystore, don't remember which one
int nKeys = 1;

void tolower_str(char* str){
    for(int i = 0; i < strlen(str); ++i){
        str[i] = (char) tolower((int) str[i]);
    }
}

/*
    Returns 0 on succes and error number on error
*/
int hex_str_to_bin(char* hex_str, unsigned char* byte_str){
    tolower_str(hex_str);
    unsigned char hex_in_binary;

    for(int i = 0; i < strlen(hex_str); ++i){
        if(((hex_str[i] >= '0') && (hex_str[i] <= '9')) || ((hex_str[i] >= 'a') && (hex_str[i] <= 'f'))){
            if((hex_str[i] >= '0') && (hex_str[i] <= '9')){
                hex_in_binary = (unsigned char) (hex_str[i] - '0');
            }else{
                hex_in_binary = (unsigned char) (hex_str[i] - 'a' + 10);
            }

            if((i % 2) == 0){
                byte_str[i / 2] = hex_in_binary << 4;
            }else{
                byte_str[i / 2] |= hex_in_binary;
            }
        }else{
            return HEX2BINERR;
        }
    }
    return 0;
}

char bin_to_hex(unsigned char bin){
    char hex;
    if(bin < 10){
        hex = '0' + bin;
    }else{
        hex = 'a' + bin - 10;
    }
    return hex;
}

void bin_to_hex_str(unsigned char* byte_str, char* hex_str, int byte_str_len){
    char hex;
    for(int i = 0; i < (byte_str_len * 2); ++ i){
        if(i % 2 == 0){
            hex = bin_to_hex(byte_str[i / 2]>>4);
        }else{
            hex = bin_to_hex(byte_str[i / 2]&15);
        }

        hex_str[i] = hex;
    }
    hex_str[byte_str_len * 2] = '\0';
}

/*
    Returns 0 on succes and error number on error
*/
int get_decryption_key(char* keystore, char* password ,unsigned char* decryption_key){
    int error;

    cJSON* json =  cJSON_Parse(keystore);
    cJSON* crypto = cJSON_GetObjectItemCaseSensitive(json, "crypto");
    cJSON* kdf = cJSON_GetObjectItemCaseSensitive(crypto, "kdf");
    //We are only implementing scrypt now, we will implement pbkdf2 later
    cJSON* params = cJSON_GetObjectItemCaseSensitive(kdf, "params");

    //dklen
    int dklen = cJSON_GetObjectItemCaseSensitive(params, "dklen")->valueint;

    //n -> cost
    int n = cJSON_GetObjectItemCaseSensitive(params, "n")->valueint;

    //r -> blockSize
    int r = cJSON_GetObjectItemCaseSensitive(params, "r")->valueint;

    //p -> parallel
    int p = cJSON_GetObjectItemCaseSensitive(params, "p")->valueint;

    //salt
    char* salt_str = cJSON_GetObjectItemCaseSensitive(params, "salt")->valuestring;
    int salt_len = strlen(salt_str)/2;
    unsigned char salt_bin[salt_len];
    if((error = hex_str_to_bin(salt_str, salt_bin)) != 0){
        return error;
    }

    //n are iteration, in this function they convert n to the power of 2, in the other function they take n as the exponent of 2
    wc_scrypt_ex(decryption_key, password, strlen(password), salt_bin, salt_len, (word32) n, r, p, dklen);

    return 0;
}

/*
    Returns 0 on succes and error number on error
*/
int verificate_password(char* keystore, unsigned char* decription_key){
    int error;

    cJSON* json =  cJSON_Parse(keystore);
    cJSON* crypto = cJSON_GetObjectItemCaseSensitive(json, "crypto");
    cJSON* checksum = cJSON_GetObjectItemCaseSensitive(crypto, "checksum");
    cJSON* cipher = cJSON_GetObjectItemCaseSensitive(crypto, "cipher");

    //checksum_message
    char* checksum_message = cJSON_GetObjectItemCaseSensitive(checksum, "message")->valuestring;

    //cipher_message
    char* cipher_message = cJSON_GetObjectItemCaseSensitive(cipher, "message")->valuestring;
    int cipher_message_len = strlen(cipher_message)/2;
    unsigned char cipher_message_bin[cipher_message_len];
    if((error = hex_str_to_bin(cipher_message, cipher_message_bin)) != 0){
        return error;
    }

    unsigned char pre_image[16 + cipher_message_len];
    for(int i = 0; i < 16; ++i){
        pre_image[i] = decription_key[16 + i];
    }

    for(int i = 16; i < (16 + cipher_message_len); ++i){
        pre_image[i] = cipher_message_bin[i - 16];
    }

    char pre_image_str[(16 + cipher_message_len)*2 + 1];

    unsigned char checksum_hash[32];

    Sha256 sha;
    wc_InitSha256(&sha);
    wc_Sha256Update(&sha, pre_image, 16 + cipher_message_len);
    wc_Sha256Final(&sha, checksum_hash);

    char checksum_str[65];
    bin_to_hex_str(checksum_hash, checksum_str, 32);

    
    if(strncmp(checksum_message, checksum_str, 64) == 0){
        return 0;
    }else{
        return INVPASSWORD;
    }
}

/*
    Returns 0 on succes and error number on error
*/
int get_private_key(char* keystore, char* decription_key, char* private_key){
    int error;

    cJSON* json =  cJSON_Parse(keystore);
    cJSON* crypto = cJSON_GetObjectItemCaseSensitive(json, "crypto");
    cJSON* cipher = cJSON_GetObjectItemCaseSensitive(crypto, "cipher");
    char* params = cJSON_GetObjectItemCaseSensitive(cipher, "params");
    char* iv_str = cJSON_GetObjectItemCaseSensitive(params, "iv")->valuestring;

    char* cipher_message = cJSON_GetObjectItemCaseSensitive(cipher, "message")->valuestring;
    int cipher_message_len = strlen(cipher_message)/2;
    unsigned char cipher_message_bin[cipher_message_len];
    if((error = hex_str_to_bin(cipher_message, cipher_message_bin)) != 0){
        return error;
    }

    int iv_len = strlen(iv_str)/2;
    unsigned char iv_bin[iv_len];
    if((error = hex_str_to_bin(iv_str, iv_bin)) != 0){
        return error;
    }

    Aes aes;
    wc_AesSetKey(&aes, decription_key, 16, iv_bin, AES_ENCRYPTION);
    wc_AesCtrEncrypt(&aes, private_key, cipher_message_bin, cipher_message_len);

    char* private_key_str[65];
    bin_to_hex_str(private_key, private_key_str, 32);

    printf(private_key_str);

    return 0;
}

int import_from_keystore(char** keystores, char** passwords, int nKeys){
    int error;
    char decription_key[32];
    char private_key[32];
    for(int i = 0; i < nKeys; ++i){
        if((error = get_decryption_key(keystores[i], passwords[i], decription_key)) != 0){
            return error;
        }

        if((error = verificate_password(keystores[i], decription_key)) != 0){
            return error;
        }

        if((error = get_private_key(keystores[i], decription_key, private_key)) != 0){
            return error;
        }
    }
    
    return 0;
}

int main(){
    char password[]= "123456789";
    char keystore[] = "{\r\n"
                "    \"crypto\": {\r"
                "\n"
                "        \"kdf\": {\r"
                "\n"
                "            \"functi"
                "on\": \"scrypt\",\r"
                "\n"
                "            \"params"
                "\": {\r\n"
                "                \"dk"
                "len\": 32,\r\n"
                "                \"n"
                "\": 262144,\r\n"
                "                \"r"
                "\": 8,\r\n"
                "                \"p"
                "\": 1,\r\n"
                "                \"sa"
                "lt\": \"8123ea083eae"
                "312143c724a8063ea9ec"
                "53b4818d34726b28a20f"
                "afa6107b2900\"\r\n"
                "            },\r\n"
                "            \"messag"
                "e\": \"\"\r\n"
                "        },\r\n"
                "        \"checksum\""
                ": {\r\n"
                "            \"functi"
                "on\": \"sha256\",\r"
                "\n"
                "            \"params"
                "\": {},\r\n"
                "            \"messag"
                "e\": \"2e5411ff47650"
                "1f52899ccc7eaf21a788"
                "e9b49aae3f1baa1e9777"
                "1b2ea0de5da\"\r\n"
                "        },\r\n"
                "        \"cipher\": "
                "{\r\n"
                "            \"functi"
                "on\": \"aes-128-ctr"
                "\",\r\n"
                "            \"params"
                "\": {\r\n"
                "                \"iv"
                "\": \"7ea5abd19a7747"
                "ddac97b3951ade63a5\""
                "\r\n"
                "            },\r\n"
                "            \"messag"
                "e\": \"8dbbb6d422367"
                "6b14b9f7662fe29d35c0"
                "94889913c51a534e80f2"
                "430e2deedcd\"\r\n"
                "        }\r\n"
                "    },\r\n"
                "    \"description\":"
                " \"\",\r\n"
                "    \"pubkey\": \"ae"
                "249bcf645e7470cdd10c"
                "546de97ea87f70a93dbf"
                "8a99e2b77833c9e83a58"
                "33a6d37f73ef8359aa79"
                "f495130697eec2\",\r"
                "\n"
                "    \"path\": \"m/12"
                "381/3600/0/0/0\",\r"
                "\n"
                "    \"uuid\": \"cc26"
                "0592-1cf5-40d7-bc5a-"
                "44eaaa298d06\",\r\n"
                "    \"version\": 4\r"
                "\n"
                "}";


    int error;
    unsigned char decription_key[32];
    unsigned char private_key[32];

    if((error = get_decryption_key(keystore, password, decription_key)) != 0){
        return error;
    }

    char decryption_key_str[65];
    bin_to_hex_str(decription_key, decryption_key_str, 32);

    if((error = verificate_password(keystore, decription_key)) != 0){
        return error;
    }

    if((error = get_private_key(keystore, decription_key, private_key)) != 0){
        return error;
    }

}
