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

#include "../lib/httpRemote.h"

#include "../lib/bls_hsm.h"

#define PORT 8080
#define SA struct sockaddr
#define MAX 65535

void func(int sockfd)
{
    char* argv[4];
    int argc;

    // infinite loop for chat
    for (;;) {
        char bu[6555];
        import("0c512bb087a03ac827d63a8242d0e69d0692f5b86019bcd8fee0a1dcfcb5cdf7",bu);
        int bytesRead;
        char bufferRequest[MAX];

        bytesRead = read(sockfd, (void*) bufferRequest, MAX);
        printf("%.*s\n\n\n\n", bytesRead, bufferRequest);
        fflush(stdout);
        struct boardRequest reply;

        if(parseRequest(bufferRequest, (size_t) bytesRead, &reply) == 0){
            int bytesToWrite;
            char bufferReply[MAX];

            if((bytesToWrite = dumpHttpResponse(bufferReply, &reply)) > 0){
                int bytesWritten = 0;
                do{
                    bytesWritten += write(sockfd, (void*) (bufferReply + bytesWritten), bytesToWrite - bytesWritten);
                }while(bytesWritten < bytesToWrite);
                printf("\n\n%s\n\n", bufferReply);
                fflush(stdout);
            }else{
                printf("Unsuccessful response.\n");
            }
        }
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