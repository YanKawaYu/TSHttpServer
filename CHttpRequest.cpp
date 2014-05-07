//
//  CHttpRequest.cpp
//  TSHttpServer
//
//  Created by zhaoxy on 14-4-23.
//  Copyright (c) 2014年 tsinghua. All rights reserved.
//

#include "CHttpRequest.h"
#include "define.h"
#include <sys/socket.h>

using namespace std;

CHttpRequest::CHttpRequest(int tConnFd) {
    connFd = tConnFd;
    connection = "Close";
    modifiedTime = "";
    fileStart = 0;
    fileEnd = 0;
    
    fieldMap[TS_HTTP_HEADER_CONNECTION] = &CHttpRequest::handleConnection;
    fieldMap[TS_HTTP_HEADER_AUTHORIZATION] = &CHttpRequest::handleAuthorization;
    fieldMap[TS_HTTP_HEADER_RANGE] = &CHttpRequest::handleRange;
    fieldMap[TS_HTTP_HEADER_IF_MOD_SINCE] = &CHttpRequest::handleIfModSince;
    fieldMap[TS_HTTP_HEADER_CONTENT_LENGTH] = &CHttpRequest::handleContentLength;
}

void CHttpRequest::handleRequest(char *header) {
    stringstream stream;
    stream<<header;
    
    int count = 0;
    while (1) {
        if (stream.eof()) {
            break;
        }
        char line[1024];
        stream.getline(line, sizeof(line));
        if (strcmp(line, "")==0) {
            continue;
        }
        
        stringstream lineStream;
        lineStream<<line;
        //first line
        if (count == 0) {
            lineStream>>method;
            lineStream>>path;
            lineStream>>version;
        }else {
            string fieldName;
            lineStream>>fieldName;
            //remove \r
            line[strlen(line)-1] = '\0';
            void(CHttpRequest::*func)(char*) = fieldMap[fieldName];
            if (func!=NULL) {
                (this->*func)(line+fieldName.length()+1);
            }
        }
        count++;
    }
    
    //if method is post, continue receiving data
    if (method.compare(TS_HTTP_METHOD_POST_S)==0) {
        long recvLen = 0;
        while (recvLen<contentLength) {
            //读取缓存
            char buff[4096];
            //读取http header
            int len = (int)recv(connFd, buff, sizeof(buff), 0);
            buff[len] = '\0';
            std::cout<<buff;
            if (len<0) {
                break;
            }
            recvLen+=len;
        }
    }
}

void CHttpRequest::handleConnection(char *field) {
    if (ENABLE_KEEP_ALIVE) {
        connection = string(field);
    }
}

void CHttpRequest::handleAuthorization(char *field) {
    char authName[10], authInfo[256];
    sscanf(field, "%s %s", authName, authInfo);
    authorize = string(authInfo);
}

void CHttpRequest::handleRange(char *field) {
    if (strstr(field, "bytes=")==field) {
        char *start = strtok(field+strlen("bytes="), "-");
        fileStart = start==NULL?0:atol(start);
        char *end = strtok(NULL, "-");
        fileEnd = end==NULL?0:atol(end);
    }
}

void CHttpRequest::handleIfModSince(char *field) {
    modifiedTime = string(field);
}

void CHttpRequest::handleContentLength(char *field) {
    contentLength = atol(field);
}