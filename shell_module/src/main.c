/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>
#include <shell/shell.h>
#include <version.h>
#include <logging/log.h>
#include <stdlib.h>
#include <kernel.h>
#include <stdio.h>
#include <secure_services.h>
#include <pm_config.h>
#include <fw_info.h>

#include <blst.h>

#define CONFIG_SPM_SERVICE_RNG

struct k_timer my_timer;
K_TIMER_DEFINE(my_timer, NULL, NULL);

//Global variables
blst_scalar sk;
blst_p2 hash;
blst_p1_affine pk2;
blst_p2_affine sig2;
uint8_t msg_bin[32];
blst_scalar secret_keys_store[10];
blst_p1 public_keys_store[10];
char public_keys_hex_store[960];
int keys_counter = 0;

LOG_MODULE_REGISTER(app);


static int cmd_keygen(const struct shell *shell, size_t argc, char **argv);


static int cmd_public_key(const struct shell *shell, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

        printf("Public key: \n");
        
        printf("0x");
        int cont = keys_counter - 1;
        for(int i = 96 * cont; i < 96 * cont + 96; i++) {
          printf("%c", public_keys_hex_store[i]);
        }
        printf("\n");

	return 0;
}

void public_key_to_sk(char * public_key_hex, blst_scalar sk){
    
        char aux[96];
        char aux2[96];
        for(int i = 0; i < 96; i++){
           aux2[i] = public_keys_hex_store[i];
        }

        //This is for taking out "0x" from the string
        /*
        for(int i = 2; i < strlen(public_key_hex); i++){
           aux[i-2] = public_key_hex[i];
        }
        */

        int j = 0;
        int cont = keys_counter - 1;

        for(int i = 0; i < keys_counter; i++){
            if (strcmp(aux, aux2) == 0){
                sk = secret_keys_store[i];
                break;
            } else {
                for(int k = 0; k < 96; k++){
                  aux2[k] = public_keys_hex_store[k+96*cont];
                }
            }
        }
}

static int cmd_signature_message(const struct shell *shell, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
        //Message examples
        //char * msg_hex = "5656565656565656565656565656565656565656565656565656565656565656";
        //char * msg_hex = "b6bb8f3765f93f4f1e7c7348479289c9261399a3c6906685e320071a1a13955c";

        blst_scalar sk_sign;
        public_key_to_sk(argv[1], sk_sign);
        char * msg_hex = argv[2];

        //Hash message
        if(hex2bin(msg_hex, strlen(msg_hex), &msg_bin, sizeof(msg_bin)) == 0) {
          printf("Error4\n");
        }

        char message[64];
        if(bin2hex(&msg_bin, sizeof(msg_bin), &message, sizeof(message)) == 0) {
          printf("Error5\n");
        }
               
        char dst[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP"; //IETF BLS Signature V4
        //Obtain the point from a message
        blst_hash_to_g2(&hash, msg_bin, sizeof(msg_bin), dst, sizeof(dst), NULL, 0);

        //The secret key allow us to perform a sign operation
        blst_p2 sig;
        byte out2[96];
        char sig_hex[192];

        blst_sign_pk_in_g1(&sig, &hash, &sk_sign); 
        blst_p2_to_affine(&sig2, &sig);
        blst_p2_compress(out2, &sig);
        
        printf("Signature: \n");
        if(bin2hex(&out2, sizeof(out2), sig_hex, sizeof(sig_hex)) == 0) {
          printf("Error6\n");
        }
        
        printf("0x");
        for(int i = 0; i < 192; i++) {
          printf("%c", sig_hex[i]);
        }
        printf("\n\n");

	return 0;
}

static int cmd_signature_verification(const struct shell *shell, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

        char dst[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP"; //IETF BLS Signature V4

        if(blst_core_verify_pk_in_g1(&pk2, &sig2, 1, msg_bin, sizeof(msg_bin), dst, sizeof(dst), NULL, 0) != BLST_SUCCESS){
          printf("Error\n");
        }
        else {
          printf("Success\n");
        }

	return 0;
}

static int cmd_get_keys(const struct shell *shell, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

        int j = 0;
        int cont = keys_counter - 1;
        int counter = keys_counter;
        
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

	return 0;
}

static int cmd_benchmark(const struct shell *shell, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

        unsigned char ikm[32];
	const int random_number_len = 144;     
        uint8_t random_number[random_number_len];
        size_t olen = random_number_len;
        int ret;

        ret = spm_request_random_number(random_number, random_number_len, &olen);
        if (ret != 0) {
           printk("Could not get random number (err: %d)\n", ret);
        }

        for(int i = 0; i < sizeof(ikm); i++){
          ikm[i] = random_number[i];
        } 

        // key_info is an optional parameter.  This parameter MAY be used to derive
        // multiple independent keys from the same IKM.  By default, key_info is the empty string.
        char info[] = {
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

        k_timer_start(&my_timer, K_MSEC(3000), K_NO_WAIT);
        
        //Secret key (256-bit scalar)
        blst_keygen(&sk, ikm, sizeof(ikm), info, sizeof(info));
                
        uint8_t priv_key_bin[32];
        blst_bendian_from_scalar(priv_key_bin, &sk);

        char priv_key_hex2[64];
        if(bin2hex(priv_key_bin, sizeof(priv_key_bin), priv_key_hex2, sizeof(priv_key_hex2)) == 0) {
          printf("Error2\n");
        }

        printf("Secret key: \n");
        printf("0x");
        printf("%s\n", priv_key_hex2);

        //The secret key allow us to generate the associated public key
        blst_p1 pk;
        byte out[48];
        char public_key_hex[96];
        blst_sk_to_pk_in_g1(&pk, &sk);
        blst_p1_to_affine(&pk2, &pk);
        blst_p1_compress(out, &pk);

        uint32_t time_left;
        time_left = k_timer_remaining_get(&my_timer);
        k_timer_stop(&my_timer);

        printf("Public key: \n");
        if(bin2hex(&out, sizeof(out), public_key_hex, sizeof(public_key_hex)) == 0) {
          printf("Error3\n");
        }
           
        printf("0x");
        for(int i = 0; i < 96; i++) {
          printf("%c", public_key_hex[i]);
        }
        printf("\n");

        printf("\nBenchmark for key generation: %u ms\n", 3000-time_left);

        k_timer_start(&my_timer, K_MSEC(3000), K_NO_WAIT);
        
        blst_p2 hash;
        char * msg_hex = "5656565656565656565656565656565656565656565656565656565656565656";
        uint8_t msg_bin[32];
             
        if(hex2bin(msg_hex, strlen(msg_hex), &msg_bin, sizeof(msg_bin)) == 0) {
          printf("Error4\n");
        }

        char message[64];
        if(bin2hex(&msg_bin, sizeof(msg_bin), &message, sizeof(message)) == 0) {
          printf("Error5\n");
        }

        printf("\nMessage: \n");
        printf("0x");
        printf("%s\n", message);
               
        char dst[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP"; //IETF BLS Signature V4
        //Obtain the point from a message
        blst_hash_to_g2(&hash, msg_bin, sizeof(msg_bin), dst, sizeof(dst), NULL, 0);

        time_left = k_timer_remaining_get(&my_timer);
        k_timer_stop(&my_timer);
        printf("\nBenchmark for message hash: %u ms\n", 3000-time_left);

        k_timer_start(&my_timer, K_MSEC(3000), K_NO_WAIT);
        
        blst_p2 sig;
        blst_p2_affine sig2;
        byte out2[96];
        char sig_hex[192];

        blst_sign_pk_in_g1(&sig, &hash, &sk); 
        blst_p2_to_affine(&sig2, &sig);
        blst_p2_compress(out2, &sig);
        
        time_left = k_timer_remaining_get(&my_timer);
        k_timer_stop(&my_timer);
        
        printf("\nSignature: \n");
        if(bin2hex(&out2, sizeof(out2), sig_hex, sizeof(sig_hex)) == 0) {
          printf("Error6\n");
        }
        
        printf("0x");
        for(int i = 0; i < 192; i++) {
          printf("%c", sig_hex[i]);
        }
        printf("\n");

        printf("\nBenchmark for signature: %u ms\n", 3000-time_left);

	return 0;
}

SHELL_CMD_ARG_REGISTER(keygen, NULL, "Generates secret key and public key", cmd_keygen, 1, 0);

SHELL_CMD_ARG_REGISTER(publickey, NULL, "Shows the last public key that has been generated", cmd_public_key, 1, 0);

SHELL_CMD_ARG_REGISTER(signature, NULL, "Signs a message with a specific public key", cmd_signature_message, 1, 2);

SHELL_CMD_ARG_REGISTER(verify, NULL, "Verifies the signature", cmd_signature_verification, 1, 0);

SHELL_CMD_ARG_REGISTER(getkeys, NULL, "Returns the identifiers of the keys available to the signer", cmd_get_keys, 1, 0);

SHELL_CMD_ARG_REGISTER(benchmark, NULL, "Benchmark for key generation and signature proccess", cmd_benchmark, 1, 0);

void main(void)
{

}
