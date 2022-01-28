/*
    A library to handle http requests to the remote signer

    IMPORTANT comment about MAXHeaders and MAXKeys(maximum keystore)

    Ask about maximum size of an ethereum signature
*/

#ifndef httpRemote_h
#define httpRemote_h

#include "./picohttpparser.h"
#include "./cJSON.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../cli/include/common.h"

#define signatureOffset 12//due to  Signature: \n

#define MAXSizeEthereumSignature 205 //MAXSizeEthereumSignature + 12 (due to Signature: \n) + 1 (due to \0)
#define MAX 65535
#define MAXHeaders 100
#define MAXKeys 10 //Maximum numbers of keys to store
#define keySize 96

#define sign 0
#define upcheck 1
#define getKeys 2

char upcheckStr[] = "/upcheck";
char getKeysStr[] = "/api/v1/eth2/publicKeys";
char signRequestStr[] = "/api/v1/eth2/sign/0x";
char contentLengthStr[] = "content-length";

/*
**********************************************************RESPONSES****************************************************************
*/
char upcheckResponse[] = "HTTP/1.1 200 OK\r\n"
   "content-type: text/p"
   "lain; charset=utf-8"
   "\r\n"
   "content-length: 2\r"
   "\n\r\n"
   "OK";

/*
We got to add later the size of the json in text, 2 \n and the json with the publick Keys
json format: ["0xkey", "0xkey", ....]
keys are in hex
*/
char getKeysResponse[] = "HTTP/1.1 200 OK\r\n"
   "content-type: applic"
   "ation/json; charset="
   "utf-8\r\n"
   "content-length: ";

/*
We got to add later the size of the signature, 2 \n and the signature
signature format 0xsignature
signature is in hex
*/
char signResponse[] = "HTTP/1.1 200 OK\r\n"
   "content-type: text/p"
   "lain; charset=utf-8"
   "\r\n"
   "content-length: ";

/*
************************************************************************************************************************************
*/

struct boardRequest{
    int method; //Board
    char* json;
    char* keyToSign;//Size is always of keySize bytes
    char publicKeys[MAXKeys][keySize];//In hex
    int nKeys;//number of keys
    int jsonLen;//In fact we won't need this field because there will be a \0 at the end of the json, but just in case 
};

struct httpRequest{
    char* method;
    char* path;
    char* body;
    size_t requestLen;
    size_t methodLen;
    size_t pathLen;
    size_t bodyLen;
    int minorVersion;
    struct phr_header headers[MAXHeaders];//We can only process this number of headers
    size_t numHeaders;   
};

/*
    On success returns 0
    On error returns -1
*/
int copyKeys(struct boardRequest* request){
    char buffer[MAXKeys * 99 + 12];
    get_keys(0, NULL, buffer);
    if((strlen(buffer) == 25) && (strncmp(buffer, "There are no keys stored\n", 25) == 0)){
        return -1;
    }else{
        cJSON* json = cJSON_Parse(buffer);
        cJSON* keys = json->child;

        request->nKeys = 0;
        do{
            ++request->nKeys;

            if(request->nKeys >= MAXKeys){
                request->nKeys = 0;
                return -1;
            }
            
            strncpy(request->publicKeys[request->nKeys - 1], keys->valuestring, keySize);

            keys = keys->next;
        }while(keys != NULL);
    }

    return 0;
}

void getBody(char* buffer, size_t bufferSize, struct httpRequest* request){
    int bodyLengthPosition;//Where is content-length in request->headers
    int contentLengthStrSize = strlen(contentLengthStr);

    //14 is the size of content-length
    for(bodyLengthPosition = 0; 
    (bodyLengthPosition < (int) request->numHeaders) && (request->headers[bodyLengthPosition].name != NULL) &&
    !((request->headers[bodyLengthPosition].name_len == contentLengthStrSize) 
    && (strncmp(contentLengthStr, request->headers[bodyLengthPosition].name, contentLengthStrSize))); 
    ++bodyLengthPosition){}

    if(bodyLengthPosition < request->numHeaders){
        char bodyLenChar[(int) request->headers[bodyLengthPosition].value_len + 1];
        strncpy(bodyLenChar, request->headers[bodyLengthPosition].value, request->headers[bodyLengthPosition].value_len);
        bodyLenChar[(int) request->headers[bodyLengthPosition].value_len] = '\0';

        request->bodyLen = (size_t) atoi(bodyLenChar);

        if((int) request->bodyLen > 0){
            request->body = buffer + (int) bufferSize - (int) request->bodyLen;
            printf("%s", request->body);
        }else{
            request->body = NULL;
        }
    }
}

/*
    On succes returns 0
    On error returns -1
*/
int checkKey(struct boardRequest* request){
    if(copyKeys(request) == -1){
        return -1;
    }else{
        for(int i = 0; i < request->nKeys; ++i){
            if(strncmp(request->keyToSign, request->publicKeys[i], keySize) == 0){ 
                return 0;
            }
        }
    }

    return -1;
}

/*   
    On succes returns 0
    On error returns -1
    We are only going to support GET and POST requests, so if we get another type of request we are going to handle it like an error.
*/
int parseRequest(char* buffer, size_t bufferSize, struct boardRequest* reply){//boardRequest out, buffer in
    buffer[bufferSize] = '\0';
    struct httpRequest request;

    request.requestLen = phr_parse_request(buffer, bufferSize, (const char**) &(request.method), &(request.methodLen), 
    (const char**) &(request.path), &(request.pathLen), &(request.minorVersion), request.headers, &(request.numHeaders), 0);

    if((int) request.requestLen < 0){
        return -1;
    }

    getBody(buffer, bufferSize, &request);

    if((request.methodLen == 3) && (strncmp(request.method, "GET", 3) == 0)){
        if((request.pathLen == strlen(upcheckStr)) && (strncmp(request.path, upcheckStr, strlen(upcheckStr)) == 0)){
            reply->method = upcheck;
        }else if((request.pathLen == strlen(getKeysStr)) && (strncmp(request.path, getKeysStr, strlen(getKeysStr)) == 0)){
            reply->method = getKeys;
        }else{
            return -1;
        }
    }else if((request.methodLen == 4) && (strncmp(request.method, "POST", 4) == 0)){
        if((request.pathLen == (strlen(signRequestStr) + keySize)) && (strncmp(request.path, signRequestStr, strlen(signRequestStr)) == 0)){
            reply->keyToSign = request.path + strlen(signRequestStr);
                        
            reply->json = request.body;
            reply->jsonLen = request.bodyLen;

            reply->method = sign;        
        }else{
            return -1;
        }
    }else{
        return -1;
    }

    return 0;
}

/*
    Returns size of buffer
*/
int upcheckResponseStr(char* buffer){
    strcpy(buffer, upcheckResponse);
    return strlen(upcheckResponse);
}

/*
    Returns size of buffer
*/
int getKeysResponseStr(char* buffer, struct boardRequest* request){
    int jsonKeysSize = 5*request->nKeys + request->nKeys*keySize + 3;

    strcpy(buffer, getKeysResponse);

    char nKeysStr[100];
    sprintf(nKeysStr, "%d", jsonKeysSize);

    strcat(buffer, nKeysStr);
    strcat(buffer, "\n\n[\n");

    for(int i = 0; i < request->nKeys; ++i){
        strcat(buffer, "\"0x");
        strncat(buffer, request->publicKeys[i], keySize);
        strcat(buffer, "\"");
        strcat(buffer, "\n");
    }
    strcat(buffer, "]");

    return (strlen(buffer));
}

/*
    Returns size of buffer
*/
int signResponseStr(char* buffer, struct boardRequest* request){
    cJSON* json = cJSON_Parse(request->json);
    cJSON* signingroot = cJSON_GetObjectItemCaseSensitive(json, "signingRoot");

    char* argv[] = {NULL, request->keyToSign, signingroot->valuestring};
    char responseSigned[MAXSizeEthereumSignature];//¿Maximum size of ethereum siganture?
    responseSigned[0] = '\0';
    signature(0, argv, responseSigned);

    strcpy(buffer, signResponse);

    int signatureLen = strlen(responseSigned + signatureOffset) + 2;//+2 due to 0x
    char signatureLenStr[100];
    sprintf(signatureLenStr, "%d", signatureLen);

    strcat(buffer, signatureLenStr);
    strcat(buffer, "\n\n0x");
    strcat(buffer, responseSigned + signatureOffset);

    return strlen(buffer);
}

/*
    On succes returns the number of bytes in buffer
    On error retuns -1
*/
int dumpHttpResponse(char* buffer, struct boardRequest* request){//boardRequest in, buffer out
    switch(request->method){
        case sign:
            if(checkKey(request) == -1){
                return -1;
            }
            return signResponseStr(buffer, request);
            break;
        case upcheck:
            return upcheckResponseStr(buffer);
            break;
        case getKeys:
            if(copyKeys(request) == -1){
                return -1;
            }
            return getKeysResponseStr(buffer, request);
            break;
        default:
            return -1;
    }
}

#endif