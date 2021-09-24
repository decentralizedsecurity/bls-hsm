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

LOG_MODULE_REGISTER(app);

#include "utils.c"

static int cmd_keygen(const struct shell *shell, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
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

            printf("Public key: \n");
            if(bin2hex(out, sizeof(out), public_key_hex, sizeof(public_key_hex)) == 0) {
              printf("Failed converting binary key to string\n");
            }
        
            store_pk(public_key_hex);
            print_pk(public_key_hex);

            return 0;
        }else{
            printf("Can't generate more keys. Limit reached.\n");
        }
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
            if(pk_in_keystore(argv[1], offset) != -1){
                uint8_t msg_bin[32];
                if(msg_parse(argv[2], msg_bin) != 1){
                    blst_p2 hash;
                    get_point_from_msg(&hash, msg_bin);

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
        int keystore_size = get_keystore_size();
        char public_keys_hex_store[96*keystore_size];
        getkeys(public_keys_hex_store);
        if(keystore_size != 0){
            int j = 0;
            int cont = keystore_size - 1;
            int counter = keystore_size;
        
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

static int cmd_reset(const struct shell *shell, size_t argc, char **argv){
        ARG_UNUSED(argc);
	ARG_UNUSED(argv);

        reset();
        printf("Keys deleted\n");
        return 0;
}

static int cmd_prompt(const struct shell *shell, size_t argc, char **argv){
        ARG_UNUSED(argc);
	ARG_UNUSED(argv);

        if(strcmp(argv[1], "on") == 0){
            shell_prompt_change(shell, "uart:~$ ");
        }else if(strcmp(argv[1], "off") == 0){
            shell_prompt_change(shell, "");
        }else{
            printf("Usage: prompt on/off\n");
        }
        return 0;
}

SHELL_CMD_ARG_REGISTER(keygen, NULL, "Generates secret key and public key", cmd_keygen, 1, 1);

SHELL_CMD_ARG_REGISTER(signature, NULL, "Signs a message with a specific public key", cmd_signature_message, 3, 0);

SHELL_CMD_ARG_REGISTER(verify, NULL, "Verifies the signature", cmd_signature_verification, 4, 0);

SHELL_CMD_ARG_REGISTER(getkeys, NULL, "Returns the identifiers of the keys available to the signer", cmd_get_keys, 1, 0);

SHELL_CMD_ARG_REGISTER(reset, NULL, "Deletes all generated keys", cmd_reset, 1, 0);

SHELL_CMD_ARG_REGISTER(prompt, NULL, "Toggle prompt", cmd_prompt, 2, 0);

void main(void)
{

}