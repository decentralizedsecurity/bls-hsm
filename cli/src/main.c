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
char public_keys_hex_store[960];
int keys_counter = 0;

LOG_MODULE_REGISTER(app);

#include "utils.c"

static int cmd_keygen(const struct shell *shell, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

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

        ikm_sk(&keys_counter, info);
        
        //The secret key allow us to generate the associated public key
        blst_p1 pk;
        byte out[48];
        char public_key_hex[96];
        sk_to_pk(&pk);
        pk_serialize(out, pk);

        printf("Public key: \n");
        if(bin2hex(out, sizeof(out), public_key_hex, sizeof(public_key_hex)) == 0) {
          printf("Failed converting binary key to string\n");
        }
        
        store_pk(public_key_hex);
        print_pk(public_key_hex);

	return 0;
}


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

static int cmd_signature_message(const struct shell *shell, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
        //Message examples
        //char * msg_hex = "5656565656565656565656565656565656565656565656565656565656565656";
        //char * msg_hex = "b6bb8f3765f93f4f1e7c7348479289c9261399a3c6906685e320071a1a13955c";

        int offset = parse(argv[1], 96);

        if(offset != -1){
            if(public_key_to_sk(argv[1], public_keys_hex_store, keys_counter, offset) != -1){
                uint8_t msg_bin[32];
                if(msg_parse(argv[2], msg_bin) != 1){
                    blst_p2 hash;
                    get_point_from_msg(&hash, msg_bin);

                    //The secret key allow us to perform a sign operation
                    blst_p2 sig;
                    byte sig_bin[96];
                    char sig_hex[192];

                    sign_pk(&sig, &hash);
                    sig_serialize(sig_bin, sig);
        
                    printf("Signature: \n");
                    if(bin2hex(sig_bin, sizeof(sig_bin), sig_hex, sizeof(sig_hex)) == 0) {
                      printf("Failed converting binary signature to string\n");
                    }
        
                    print_sig(sig_hex);
                }
            }else{
                printf("Public key isn't stored\n");
            }
        }else{
            printf("Incorrect public key length. It must be 96 characters long\n");
        }

	return 0;
}

static int cmd_signature_verification(const struct shell *shell, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

        char dst[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP"; //IETF BLS Signature V4

        blst_p1_affine pk;
        blst_p2_affine sig;
        uint8_t msg_bin[32];
        
        if((pk_parse(argv[1], &pk) || msg_parse(argv[2], msg_bin) || sig_parse(argv[3], &sig)) != 1){
            if(blst_core_verify_pk_in_g1(&pk, &sig, 1, msg_bin, 32, dst, sizeof(dst), NULL, 0) != BLST_SUCCESS){
              printf("Error\n");
            }
            else {
              printf("Success\n");
            }
        }

	return 0;
}

static int cmd_get_keys(const struct shell *shell, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
        if(keys_counter != 0){
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
        }else{
            printf("There are no keys stored\n");
        }

	return 0;
}

static int cmd_benchmark(const struct shell *shell, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

        k_timer_start(&my_timer, K_MSEC(3000), K_NO_WAIT);
        // key_info is an optional parameter.  This parameter MAY be used to derive
        // multiple independent keys from the same IKM.  By default, key_info is the empty string.
        char info[] = {
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        
        ikm_sk_bm(info);

        //The secret key allow us to generate the associated public key
        blst_p1 pk;
        byte pk_bin[48];
        char public_key_hex[96];
        sk_to_pk(&pk);
        pk_serialize(pk_bin, pk);

        uint32_t time_left;
        time_left = k_timer_remaining_get(&my_timer);
        k_timer_stop(&my_timer);

        printf("Public key: \n");
        if(bin2hex(pk_bin, sizeof(pk_bin), public_key_hex, sizeof(public_key_hex)) == 0) {
          printf("Failed converting binary key to string\n");
        }
        
        print_pk(public_key_hex);

        printf("\nBenchmark for key generation: %u ms\n", 3000-time_left);

        k_timer_start(&my_timer, K_MSEC(3000), K_NO_WAIT);
        
        char * msg_hex = "5656565656565656565656565656565656565656565656565656565656565656";
        uint8_t msg_bin[32];
             
        if(hex2bin(msg_hex, strlen(msg_hex), msg_bin, sizeof(msg_bin)) == 0) {
          printf("Failed converting message to binary array\n");
        }

        printf("\nMessage: \n");
        printf("0x");
        printf("%s\n", msg_hex);

        blst_p2 hash;
        get_point_from_msg(&hash, msg_bin);

        time_left = k_timer_remaining_get(&my_timer);
        k_timer_stop(&my_timer);
        printf("\nBenchmark for message hash: %u ms\n", 3000-time_left);

        k_timer_start(&my_timer, K_MSEC(3000), K_NO_WAIT);
        
        blst_p2 sig;
        byte sig_bin[96];
        char sig_hex[192];
        
        sign_pk_bm(&sig, &hash); 
        sig_serialize(sig_bin, sig);
        
        time_left = k_timer_remaining_get(&my_timer);
        k_timer_stop(&my_timer);
        
        printf("\nSignature: \n");
        if(bin2hex(sig_bin, sizeof(sig_bin), sig_hex, sizeof(sig_hex)) == 0) {
          printf("Failed converting binary signature to string\n");
        }

        print_sig(sig_hex);

        printf("\nBenchmark for signature: %u ms\n", 3000-time_left);

	return 0;
}

SHELL_CMD_ARG_REGISTER(keygen, NULL, "Generates secret key and public key", cmd_keygen, 1, 1);

SHELL_CMD_ARG_REGISTER(publickey, NULL, "Shows the last public key that has been generated", cmd_public_key, 1, 0);

SHELL_CMD_ARG_REGISTER(signature, NULL, "Signs a message with a specific public key", cmd_signature_message, 3, 0);

SHELL_CMD_ARG_REGISTER(verify, NULL, "Verifies the signature", cmd_signature_verification, 4, 0);

SHELL_CMD_ARG_REGISTER(getkeys, NULL, "Returns the identifiers of the keys available to the signer", cmd_get_keys, 1, 0);

SHELL_CMD_ARG_REGISTER(benchmark, NULL, "Benchmark for key generation and signature proccess", cmd_benchmark, 1, 0);

void main(void)
{

}