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

#define MAX 1500
#define PORT 8080
#define SA struct sockaddr

struct nodo{
    char buff[MAX];
    struct nodo* siguiente;
};

void func(int sockfd)
{
    char* argv[4];
    int argc;

    // infinite loop for chat
    for (;;) {
        // //read the message from client and copy it in buffer
        // struct nodo* head = NULL;
        // char buff[MAX];

        // int listSize = 0;
        // int lastSize = 0;
        // int leidos = 0;
        // struct nodo* iteration = NULL;

        // //read http request
        // //readHeader(httpReuest)
        // //header.typer -> post, get

        // do{
        //     leidos = read(sockfd, buff, sizeof(buff));

        //     if(leidos != 0){
        //         lastSize = leidos;

        //         if(listSize == 0){
        //             head = (struct nodo*) malloc(sizeof(struct nodo));
        //             iteration = head;

        //             for(int i= 0; i < leidos; ++i){
        //                 (*iteration).buff[i] = buff[i];
        //             }
        //             iteration->siguiente = NULL;//Por si acaso
        //         }else{
        //             iteration->siguiente = (struct nodo*) malloc(sizeof(struct nodo));
        //             iteration = iteration->siguiente;

        //             for(int i= 0; i < leidos; ++i){
        //                 (*iteration).buff[i] = buff[i];
        //             }
        //             iteration->siguiente = NULL;//Por si acaso
        //         }

        //         ++listSize;
        //     }
        // }while(leidos == MAX);

        
        // char peticion[MAX*(listSize - 1) + lastSize + 1];//el + 1 es para meter el /0
        // iteration = head;
        // int j = 0;

        // for(int i = 0; i < listSize; ++i){
        //     if(i == (listSize - 1)){
        //         for(;(j - (i * MAX)) < lastSize; ++j){
        //             peticion[j] = iteration -> buff[(j - (i * MAX))];
        //         }
        //     }else{
        //         for(;(j - (i * MAX)) < MAX; ++j){
        //             peticion[j] = iteration -> buff[(j - (i * MAX))];
        //         }
        //     }

        //     struct nodo* borrar = iteration;
        //     iteration = iteration -> siguiente;

        //     free(borrar);
        // }

        // peticion[j] = '\0';

        // // print buffer which contains the client contents 
        // printf("%s", peticion);
        // fflush(stdout);
        
        // //State-machine

        // if(j >= 3){
        //     if(strncmp(peticion, "GET", 3) == 0){//Peticion GET
        //     //Solo nos interesa la frase que va despues del GET para saber el tipo de peticion
        //     int inicio = 3;
        //     int final = 3;

        //     while((peticion[inicio] != '/') && (peticion[inicio] != '\0')){
        //         ++inicio;
        //     }

        //     final = inicio;
        //     if(peticion[inicio] != '\0'){
        //         while((peticion[final] != ' ') && (peticion[final] != '\n' && peticion[final] != '\0')){
        //             ++final;
        //         }

        //         if(strncmp(peticion + inicio, "/upcheck", final - inicio) == 0){
        //             write(sockfd, "HTTP/1.1 200 OK\ncontent-type: text/plain; charset=utf-8\ncontent-length: 2\n\nOK", 77);
        //         }else if(strncmp(peticion + inicio, "/api/v1/eth2/publicKeys", final - inicio) == 0){
        //             //el formato tiene que ser ["key1","key2"]
        //             ;
        //         }
        //     }

        //     }else if(strncmp(peticion, "POST", 4) == 0){//Peticion POST
        //         //Vamos a sacar el json de la peticion POST
        //         int inicio = -1;
        //         int fin = -1;
        //         int corchetesAbiertos = 0;

        //         for(int i = 0; (peticion[i] != 0) && (fin < 0); ++i){
        //             if(peticion[i] == '{'){
        //                 if(inicio < 0){
        //                     inicio = i;
        //                 }

        //                 ++corchetesAbiertos;
        //             }

        //             if(peticion[i] =='}'){
        //                 --corchetesAbiertos;

        //                 if(corchetesAbiertos == 0){
        //                     fin = i;
        //                 }
        //             }
        //         }

        //         char[inicio - fin + 2] jsonStr;

        //         for(int i = 0; i < (inicio - fin + 1); ++i){
        //             jsontStr[i] = peticion[inicio + i];
        //         }

        //         jsonStr[inicio - fin + 1] = '\0';

        //         printf(jsonStr);

        //         json_object* json = json_tokener_parse(jsonStr);

        //         printf(json->type);
        //     }
        // }
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