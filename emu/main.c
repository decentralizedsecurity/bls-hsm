/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "../blst/bindings/blst.h"

#include "utils.c"
#include "mainsec.c"

#define MAX 500
#define PORT 8080
#define SA struct sockaddr

static int cmd_keygen(int argc, char **argv, char* buff)
{
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

            //printf("Public key: \n");
            strcat(buff, "Public key: \n");
            if(bin2hex(out, sizeof(out), public_key_hex, sizeof(public_key_hex)) == 0) {
              //printf("Failed converting binary key to string\n");
              strcat(buff, "Failed converting binary key to string\n");
            }
        
            store_pk(public_key_hex);
            print_pk(public_key_hex, buff);

            return 0;
        }else{
            //printf("Can't generate more keys. Limit reached.\n");
            strcat(buff, "Can't generate more keys. Limit reached.\n");
        }
        return 0;
}

static int cmd_signature_message(int argc, char **argv, char* buff)
{
        //Message examples
        //char * msg_hex = "5656565656565656565656565656565656565656565656565656565656565656";
        //char * msg_hex = "b6bb8f3765f93f4f1e7c7348479289c9261399a3c6906685e320071a1a13955c";

        int offset = parse(argv[1], 96);

        if(offset != -1){
            if(pk_in_keystore(argv[1], offset) != -1){
                uint8_t msg_bin[32];
                if(msg_parse(argv[2], msg_bin, buff) != 1){
                    blst_p2 hash;
                    get_point_from_msg(&hash, msg_bin);

                    blst_p2 sig;
                    byte sig_bin[96];
                    char sig_hex[192];

                    sign_pk(&sig, &hash);
                    sig_serialize(sig_bin, sig);
        
                    //printf("Signature: \n");
                    strcat(buff, "Signature: \n");
                    if(bin2hex(sig_bin, sizeof(sig_bin), sig_hex, sizeof(sig_hex)) == 0) {
                      //printf("Failed converting binary signature to string\n");
                      strcat(buff, "Failed converting binary signature to string\n");
                    }
        
                    print_sig(sig_hex, buff);
                }
            }else{
                //printf("Public key isn't stored\n");
                strcat(buff, "Public key isn't stored\n");
            }
        }else{
            //printf("Incorrect public key length. It must be 96 characters long\n");
            strcat(buff, "Incorrect public key length. It must be 96 characters long\n");
        }

	return 0;
}

static int cmd_signature_verification(int argc, char **argv, char* buff)
{

        char dst[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP"; //IETF BLS Signature V4

        blst_p1_affine pk;
        blst_p2_affine sig;
        uint8_t msg_bin[32];
        
        if((pk_parse(argv[1], &pk, buff) || msg_parse(argv[2], msg_bin, buff) || sig_parse(argv[3], &sig, buff)) != 1){
            if(blst_core_verify_pk_in_g1(&pk, &sig, 1, msg_bin, 32, dst, sizeof(dst), NULL, 0) != BLST_SUCCESS){
              //printf("Error\n");
              strcat(buff, "Error\n");
            }
            else {
              //printf("Success\n");
              strcat(buff, "Success\n");
            }
        }

	return 0;
}

static int cmd_get_keys(int argc, char **argv, char* buff)
{
        int keystore_size = get_keystore_size();
        char public_keys_hex_store[96*keystore_size];
        getkeys(public_keys_hex_store);
        if(keystore_size != 0){
            int j = 0;
            int cont = keystore_size - 1;
            int counter = keystore_size;
        
            //printf("{'keys':['");
            strcat(buff, "{'keys':['");
            for(int i = 0; i < 96 * cont + 96; i++){
                //printf("%c", public_keys_hex_store[i]);
                char str[2] = {public_keys_hex_store[i], '\0'};
                strcat(buff, str);
                j++;
                if (j == 96){
                    if(counter > 1) {
                        //printf("'\n'");
                        strcat(buff, "'\n'");
                    } else {
                        //printf("']}\n");
                        strcat(buff, "']}\n");
                    }               
                    j = 0;
                    counter--;
                }           
            }
        }else{
            //printf("There are no keys stored\n");
            strcat(buff, "There are no keys stored\n");
        }

	return 0;
}

static int cmd_reset(int argc, char **argv, char* buff){

        reset();
        //printf("Keys deleted\n");
        strcat(buff, "Keys deleted\n");
        return 0;
}

void func(int sockfd)
{
    char buff[MAX];
    char* argv[4];
    int argc;
    // infinite loop for chat
    for (;;) {
        bzero(buff, MAX);
   
        // read the message from client and copy it in buffer
        read(sockfd, buff, sizeof(buff));
        // print buffer which contains the client contents
        printf("%s", buff);
        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }

        char* token;                  //split command into separate strings
        char* cmd = strdup(buff);
        token = strtok(cmd," ");
        argc = 0;
        while(token!=NULL){
            char* c = strchr(token, '\n');
            if(c != NULL){
                *c = '\0';
            }
            argv[argc] = token;
            printf("%s\n", token);
            token = strtok(NULL," ");
            argc++;
        }
        bzero(buff, MAX);
        if(strstr(argv[0], "keygen") != NULL){
            cmd_keygen(argc, argv, buff);
        }else if(strstr(argv[0], "signature") != NULL){
            if(argc != 3){
                strcat(buff, "Incorrect arguments\n");
            }else{
                cmd_signature_message(argc, argv, buff);
            }
        }else if(strstr(argv[0], "verify") != NULL){
            if(argc != 4){
                strcat(buff, "Incorrect arguments\n");
            }else{
                cmd_signature_verification(argc, argv, buff);
            }
        }else if(strstr(argv[0], "getkeys") != NULL){
            cmd_get_keys(argc, argv, buff);
        }else if(strstr(argv[0], "reset") != NULL){
            cmd_reset(argc, argv, buff);
        }
        char* end = strrchr(buff, '\n');
        bzero(end+1, MAX-(end-buff));
        printf("%s", buff);
        write(sockfd, buff, strlen(buff));
    }
}

void main(void)
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;
   
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
   
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
   
    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");
   
    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);
   
    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("server accept failed...\n");
        exit(0);
    }
    else
        printf("server accept the client...\n");
   
    // Function for chatting between client and server
    func(connfd);
   
    // After chatting close the socket
    close(sockfd);
}