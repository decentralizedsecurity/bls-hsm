/*
 * Copyright (c) 2018 Nordic Semiconductor ASA.
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <spm.h>

/*
 * Example code for a Secure Partition Manager application.
 * The application uses the SPM to set the security attributions of
 * the MCU resources (Flash, SRAM and Peripherals). It uses the core
 * TrustZone-M API to prepare the MCU to jump into Non-Secure firmware
 * execution.
 *
 * The following security configuration for Flash and SRAM is applied:
 *
 *                FLASH
 *  1 MB  |---------------------|
 *        |                     |
 *        |                     |
 *        |                     |
 *        |                     |
 *        |                     |
 *        |     Non-Secure      |
 *        |       Flash         |
 *        |                     |
 * 256 kB |---------------------|
 *        |                     |
 *        |     Secure          |
 *        |      Flash          |
 *  0 kB  |---------------------|
 *
 *
 *                SRAM
 * 256 kB |---------------------|
 *        |                     |
 *        |                     |
 *        |                     |
 *        |     Non-Secure      |
 *        |    SRAM (image)     |
 *        |                     |
 * 128 kB |.................... |
 *        |     Non-Secure      |
 *        |  SRAM (BSD Library) |
 *  64 kB |---------------------|
 *        |      Secure         |
 *        |       SRAM          |
 *  0 kB  |---------------------|
 */


#include <aarch32/cortex_m/tz.h>
#include <blst.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/printk.h>

blst_scalar sk;
blst_scalar secret_keys_store[10];

__TZ_NONSECURE_ENTRY_FUNC
void public_key_to_sk(char * public_key_hex, blst_scalar* sk, char* public_keys_hex_store, int keys_counter){
    
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
                *sk = secret_keys_store[i];
                break;
            } else {
                for(int k = 0; k < 96; k++){
                  aux2[k] = public_keys_hex_store[k+96*cont];
                }
            }
        }
}

void main(void)
{
	spm_config();
	spm_jump();
}