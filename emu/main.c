/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define EMU

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "../blst/bindings/blst.h"
#include "../cli/include/common.h"

#include "../secure_module/zephyr/spm/src/main.c"

#define MAX 500
#define PORT 8080
#define SA struct sockaddr

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
            keygen(argc, argv, buff);
        }else if(strstr(argv[0], "signature") != NULL){
            if(argc != 3){
                strcat(buff, "Incorrect arguments\n");
            }else{
                signature(argc, argv, buff);
            }
        }else if(strstr(argv[0], "verify") != NULL){
            if(argc != 4){
                strcat(buff, "Incorrect arguments\n");
            }else{
                verify(argc, argv, buff);
            }
        }else if(strstr(argv[0], "getkeys") != NULL){
            get_keys(argc, argv, buff);
        }else if(strstr(argv[0], "reset") != NULL){
            resetc(argc, argv, buff);
        }else if(strstr(argv[0], "import") != NULL){
            import(argc, argv, buff);
        }else{
            strcat(buff, "Command not found\n");
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