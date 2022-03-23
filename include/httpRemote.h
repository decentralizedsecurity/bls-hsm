/*
    A library to handle http requests to the remote signer

    IMPORTANT comment about MAXHeaders and MAXKeys(maximum keystore)

    Ask about maximum size of an ethereum signature
*/

#ifndef httpRemote_h
#define httpRemote_h

#include "./picohttpparser.h"
#include <cJSON.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "bls_hsm.h"

#define signatureOffset 12//due to  Signature: \n

#define MAXSizeEthereumSignature 192
#define MAXBUF 32768
#define MAXHeaders 100
#define MAXKeys 10 //Maximum numbers of keys to store
#define keySize 96

#define sign 0
#define upcheck 1
#define getKeys 2

#define textPlain 0
#define applicationJson 1

char upcheckStr[] = "/upcheck";
char getKeysStr[] = "/api/v1/eth2/publicKeys";
char signRequestStr[] = "/api/v1/eth2/sign/0x";
char contentLengthStr[] = "content-length";
char acceptStr[] = "Accept";

char textPlainStr[] = "text/plain";
char applicationJsonStr[] = "application/json";

/*
**********************************************************RESPONSES****************************************************************
*/
char upcheckResponse[] = "HTTP/1.1 200 OK\r\n"
   "content-type: text/p"
   "lain; charset=utf-8"
   "\r\n"
   "content-length: 0\r\n\r\n";

/*
We got to add later the size of the json in text, 2 \n and the json with the publick Keys
json format: ["0xkey", "0xkey", ....]
keys are in hex
*/
char getKeysResponse[] = "HTTP/1.1 200 OK\r\n"
   "Content-Type: application/json"
   "\r\n"
   "Content-Length: ";

/*
We got to add later the size of the signature, 2 \n and the signature
signature format 0xsignature
signature is in hex
*/
char signResponse[] = "HTTP/1.1 200 OK\r\n"
   "Content-Type: application/json"
   "\r\n"
   "Content-Length: ";

char signResponseText[] = "HTTP/1.1 200 OK\r\n"
   "Content-Type: text/plain"
   "\r\n"
   "Content-Length: ";

char* badRequest = "HTTP/1.1 400 Bad request format\r\n"
"Content-Type: application/json\r\n"
"Content-Length: 0\r\n\r\n";

char* pknf = "HTTP/1.1 404 Public key not found\r\n"
"Content-Type: application/json\r\n"
"Content-Length: 0\r\n\r\n";

/*
************************************************************************************************************************************
*/

struct boardRequest{
    int method; //Board
    int acceptType;
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
    int acceptType;
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
    On empty keystore returns -1
*/
int copyKeys(struct boardRequest* request){
    int ksize = get_keystore_size();
    if(ksize == 0){
        request->nKeys = 0;
        return -1;
    }else{
        char buffer[ksize][96];
        getkeys(buffer);
        request->nKeys = 0;
        for(int i = 0; i < ksize; i++){
            ++(request->nKeys);
            strncpy(request->publicKeys[i], buffer[i], 96);
        }
    }

    return 0;
}

void getAcceptOptions(struct httpRequest* request){
    int acceptTypePosition = 0;
    int acceptStrSize = strlen(acceptStr);

    //6 is the size of Accept
    for(acceptTypePosition = 0;
    (acceptTypePosition < (int) request->numHeaders) && (request->headers[acceptTypePosition].name != NULL) &&
    !((request->headers[acceptTypePosition].name_len == acceptStrSize) 
    && !(strncmp(acceptStr, request->headers[acceptTypePosition].name, acceptStrSize))); 
    ++acceptTypePosition){}

    if(acceptTypePosition < (int) request->numHeaders){
        if((request->headers[acceptTypePosition].value_len == (int) strlen(applicationJsonStr)) &&
        (strncmp(applicationJsonStr, request->headers[acceptTypePosition].value, strlen(applicationJsonStr)) == 0) || 
        (request->headers[acceptTypePosition].value_len == 3) &&
        (strncmp("*/*", request->headers[acceptTypePosition].value, 3) == 0)){
            request->acceptType = applicationJson;
        }else{//If the Accept type is not application/json or */* we select the simplest one
            request->acceptType = textPlain;
        }
    }else{//If there is not an explicit accept header send response as text/plain
        request->acceptType = textPlain;
    }
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
            //printf("%s", request->body);
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

    if((request.methodLen == 3) && (strncmp(request.method, "GET", 3) == 0)){
        if((request.pathLen == strlen(upcheckStr)) && (strncmp(request.path, upcheckStr, strlen(upcheckStr)) == 0)){
            reply->method = upcheck;
        }else if((request.pathLen == strlen(getKeysStr)) && (strncmp(request.path, getKeysStr, strlen(getKeysStr)) == 0)){
            reply->method = getKeys;
        }else{
            return -1;
        }
    }else if((request.methodLen == 4) && (strncmp(request.method, "POST", 4) == 0)){
        getBody(buffer, bufferSize, &request);
        if((request.pathLen == (strlen(signRequestStr) + keySize)) && (strncmp(request.path, signRequestStr, strlen(signRequestStr)) == 0)){
            getAcceptOptions(&request);
            reply->acceptType = request.acceptType;
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
int pknotfoundResponseStr(char* buffer){
    strcpy(buffer, pknf);
    return strlen(pknf);
}

/*
    Returns size of buffer
*/
int getKeysResponseStr(char* buffer, struct boardRequest* request){
    int jsonKeysSize = 6*request->nKeys - 1 + request->nKeys*keySize + 3;
    if(request->nKeys == 0){
        jsonKeysSize = 3;
    }

    strcpy(buffer, getKeysResponse);

    char nKeysStr[100];
    sprintf(nKeysStr, "%d", jsonKeysSize);

    strcat(buffer, nKeysStr);
    strcat(buffer, "\n\n[\n");

    for(int i = 0; i < request->nKeys; ++i){
        strcat(buffer, "\"0x");
        strncat(buffer, request->publicKeys[i], keySize);
        strcat(buffer, "\"");
        if(i + 1 < request->nKeys){
            strcat(buffer, ",");
        }
        strcat(buffer, "\n");
    }
    strcat(buffer, "]");

    return strlen(buffer);
}

/*
    Returns size of buffer
*/
int signResponseStr(char* buffer, struct boardRequest* request){
    cJSON* json = cJSON_Parse(request->json);
    cJSON* signingroot = cJSON_GetObjectItemCaseSensitive(json, "signingRoot");

    char* key = strndup(request->keyToSign, 96);
    char signat[MAXSizeEthereumSignature];//¿Maximum size of ethereum siganture?
    signature(key, signingroot->valuestring, signat);

    char reply[256] = "";
    switch(request->acceptType){
        case textPlain:
            strcat(reply, "0x");
            strncat(reply, signat, MAXSizeEthereumSignature);
            strcpy(buffer, signResponseText);
            break;
        case applicationJson:
            strcat(reply, "{\"signature\": \"0x");
            strncat(reply, signat, 192);
            strcat(reply, "\"}");
            strcpy(buffer, signResponse);
            break;
        default:
            return -1;
    }

    int signatureLen = strlen(reply);
    char signatureLenStr[100];
    sprintf(signatureLenStr, "%d", signatureLen);

    strcat(buffer, signatureLenStr);
    strcat(buffer, "\n\n");
    strcat(buffer, reply);

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
                return pknotfoundResponseStr(buffer);
            }
            return signResponseStr(buffer, request);
            break;
        case upcheck:
            return upcheckResponseStr(buffer);
            break;
        case getKeys:
            copyKeys(request);
            return getKeysResponseStr(buffer, request);
            break;
        default:
            return -1;
    }
}

#endif