/*
    A library to handle http requests to the remote signer

    IMPORTANT comment about MAXHeaders and MAXKeys(maximum keystore)

    Ask about maximum size of an ethereum signature

    Comment modification in common.h to make a json when we get keys
*/

#ifndef httpRemote_h
#define httpRemote_h

#include "./picohttpparser.h"
#include "./cJSON.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../cli/include/commonBoard.h"

#define ethereumSignature 192

#define MAXSizeEthereumSignature 215 //2 (due to 0x) + MAXSizeEthereumSignature + 1 (due to \0) + 20 (a margin for just in case)
#define MAX 65535
#define MAXHeaders 100
#define MAXKeys 10 //Maximum numbers of keys to store
#define keySize 96

#define sign 0
#define upcheck 1
#define getKeys 2
#define importKey 3

#define textPlain 0
#define applicationJson 1

char upcheckStr[] = "/upcheck";
char getKeysStr[] = "/api/v1/eth2/publicKeys";
char signRequestStr[] = "/api/v1/eth2/sign/0x";
char keymanagerStr[] = "/eth/v1/keystores";
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
    int acceptType;
    char* json;
    char keyToSign[keySize + 1 + 2];//+ 1 due to \0 and + 2 due to 0x 
    char publicKeys[MAXKeys][keySize + 1 + 2];//In hex, + 1 due to \0 and + 2 due to 0x 
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
    On error returns -1
*/
int copyKeys(struct boardRequest* request){
    if(get_keys(request->publicKeys, &request->nKeys) == -1){
        return -1;
    }

    return 0;
}

void getAcceptOptions(struct httpRequest* request){
    int acceptTypePosition;
    int acceptStrSize = strlen(acceptStr);

    //6 is the size of Accept
    for(acceptTypePosition = 0;
    (acceptTypePosition < (int) request->numHeaders) && (request->headers[acceptTypePosition].name != NULL) &&
    !((request->headers[acceptTypePosition].name_len == acceptStrSize) 
    && (strncmp(acceptStr, request->headers[acceptTypePosition].name, acceptStrSize))); 
    ++acceptTypePosition){}

    if(acceptTypePosition < (int) request->numHeaders){
        if((request->headers[acceptTypePosition].value_len == (int) strlen(applicationJsonStr)) &&
        (strncmp(applicationJsonStr, request->headers[acceptTypePosition].value, strlen(applicationJsonStr)) == 0)){
            request->acceptType = applicationJson;
        }else{//If the Accept type is not application/json we select the simplest one
            request->acceptType = textPlain;
        }
    }else{//If there is not an explicit accept means accept any kind of response
        request->acceptType = applicationJson;
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
            if(strncmp(request->keyToSign, request->publicKeys[i], keySize + 2) == 0){ 
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

    getAcceptOptions(&request);
    reply->acceptType = request.acceptType;

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
            reply->keyToSign[0] = '0';
            reply->keyToSign[1] = 'x';
            strncpy(reply->keyToSign + 2, request.path + strlen(signRequestStr), keySize);
            reply->keyToSign[keySize + 2] = '\0';
                        
            reply->json = request.body;
            reply->jsonLen = request.bodyLen;

            reply->method = sign;        
        }else if((request.pathLen == strlen(keymanagerStr)) && (strncmp(request.path, keymanagerStr, strlen(keymanagerStr) == 0))){
            reply->json = request.body;
            reply->jsonLen = request.bodyLen;

            reply->method = importKey;
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
    int jsonKeysSize = 5*request->nKeys + request->nKeys*keySize + 1;

    strcpy(buffer, getKeysResponse);

    char nKeysStr[100];
    sprintf(nKeysStr, "%d", jsonKeysSize);

    strcat(buffer, nKeysStr);
    strcat(buffer, "\n\n[");

    for(int i = 0; i < request->nKeys; ++i){
        strcat(buffer, "\"");
        strncat(buffer, request->publicKeys[i], keySize + 2);
        strcat(buffer, "\"");
        if(i <  (request->nKeys - 1)){
            strcat(buffer, ",");
        }
    }
    strcat(buffer, "]");

    return (strlen(buffer));
}

/*
    Returns size of buffer
    -1 on error
*/
int signResponseStr(char* buffer, struct boardRequest* request){
    cJSON* json = cJSON_Parse(request->json);
    if(json == NULL){
        return -1;
    }

    cJSON* signingroot = cJSON_GetObjectItemCaseSensitive(json, "signingRoot");
    if((signingroot == NULL) || (signingroot->valuestring == NULL)){
        return -1;
    }

    buffer[0] = '\0';

    char responseSigned[MAXSizeEthereumSignature];//¿Maximum size of ethereum siganture?
    responseSigned[0] = '\0';

    if(signature(request->keyToSign, signingroot->valuestring, responseSigned) == -1){
        return -1;
    }

    strcpy(buffer, signResponse);

    char body[MAXSizeEthereumSignature + 20];//+ 20 in case is aplication/json and we got to add {"signature":""}
    body[0] = '\0';

    switch(request->acceptType){
        case textPlain:
            strncat(body, responseSigned, ethereumSignature + 2);//+ 2 due to 0x
            break;
        case applicationJson:
            strncat(body, "{\"signature\":\"", 14);
            strncat(body, responseSigned, ethereumSignature + 2);//+2 due to 0x
            strncat(body, "\"}", 2);
            break;
        default:
            return -1;
    }

    int signatureLen = strlen(body);
    char signatureLenStr[100];
    sprintf(signatureLenStr, "%d", signatureLen);//due to \n at the end of signature method

    strcat(buffer, signatureLenStr);
    strcat(buffer, "\n\n");
    strcat(buffer, body);

    return (strlen(buffer));//due to \n at the end of signature method
}

/*
    Returns 0 on success
    -1 on error and set the variable errno to the type of error
*/

int httpImportFromKeystore(char* body){
    cJSON* json= cJSON_Parse(body);
    if(json == NULL){
        return -1;
    }

    cJSON* keystoresJson = cJSON_GetObjectItemCaseSensitive(json, "keystores");
    if((keystoresJson == NULL) || (keystoresJson->child == NULL)){
        return -1;
    }

    cJSON* passwordsJson = cJSON_GetObjectItemCaseSensitive(json, "passwords");
    if((passwordsJson == NULL) || (passwordsJson->child == NULL)){
        return -1;
    }

    int nKeystores = 0;
    int nPasswords = 0;

    keystoresJson = keystoresJson->child;
    passwordsJson = passwordsJson->child;

    char keystores[MAXKeys][1000];//maximum size of keystores 1000
    char passwords[MAXKeys][200];//maximum size of passwords 200

    while(keystoresJson != NULL){
        if(nKeystores < MAXKeys){
            if(strlen(keystoresJson->valuestring) > (((int) sizeof(keystores[nKeystores])) - 1)){
                errno = keystoreTooBig;
                return -1;
            }else{
                keystores[nKeystores][0] = '\0';
                strncpy(keystores[nKeystores], keystoresJson->valuestring, (int) sizeof(keystores[nKeystores]));
            }

            ++nKeystores;
            keystoresJson = keystoresJson->next;
        }else{
            errno = tooManyKeys;
            return -1;
        }
    }

    while(passwordsJson!= NULL){
        if(nPasswords < MAXKeys){
            if(strlen(passwordsJson->valuestring) > (((int) sizeof(passwords[nPasswords])) - 1)){
                errno = passwordTooBig;
                return -1;
            }else{
                passwords[nPasswords][0] = '\0';
                strncpy(passwords[nPasswords], passwordsJson->valuestring, (int) sizeof(passwords[nPasswords]));
            }
        }else{
            errno = tooManyKeys;
            return -1;
        }
    }

    if(nKeystores != nPasswords){
        errno = notSamePasswordsKeystores;
        return -1;
    }

    return import_from_keystore(keystores, passwords, nKeystores);
}

/*
    On succes returns the number of bytes in buffer
    On error retuns -1
*/
int dumpHttpResponse(char* buffer, struct boardRequest* request){//boardRequest in, buffer out
    buffer[0] = '\0';//Clean buffer just in case
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
        case importKey:
            if((copyKeys(request) == -1) && (errno != noKeys)){
                return -1;
            }
            if(httpImportFromKeystore(request->json) == -1){
                return -1;
            }
            break;
        default:
            return -1;
    }
}

#endif