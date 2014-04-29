//
//  CHttpResponse.cpp
//  TSHttpServer
//
//  Created by zhaoxy on 14-4-23.
//  Copyright (c) 2014年 tsinghua. All rights reserved.
//

#include "CHttpResponse.h"
#include "CHttpRequest.h"
#include <sys/socket.h>
#include "define.h"
#include <string.h>

#define HTTP_RESPONSE_404 "<html><head><title>404 Not Found</title></head><body><h1>Not Found</h1><p>The requested URL was not found on this server.</p></body></html>"

std::string getStringFromTime(time_t time) {
    char timeBuff[64];
    struct tm tm = *gmtime(&time);
    strftime(timeBuff, sizeof timeBuff, "%a, %d %b %Y %H:%M:%S %Z", &tm);
    return std::string(timeBuff);
}

CHttpResponse::CHttpResponse(CHttpRequest *request) {
    m_request = request;
    if (m_request->method.compare(TS_HTTP_METHOD_GET_S)==0 || m_request->method.compare(TS_HTTP_METHOD_HEAD_S)==0) {
        std::string path = ROOT_PATH;
        if (m_request->path.compare("/")==0) {
            path += ROOT_HTML;
        }else {
            path += m_request->path;
        }
        
        m_statusCode = 0;
        //if file exist
        if (isFileExist(path.c_str())) {
            //if receive modified time
            if (!m_request->modifiedTime.empty()) {
                time_t time = fileModifiedTime(path.c_str());
                if (getStringFromTime(time) == m_request->modifiedTime) {
                    m_statusCode = TS_HTTP_STATUS_NOT_MODIFIED;
                    m_statusMsg = TS_HTTP_STATUS_NOT_MODIFIED_S;
                }
            }
            //if file modified
            if (m_statusCode == 0) {
                if (m_request->fileStart || m_request->fileEnd) {
                    long long fileSize = getFileSize(path.c_str());
                    //if request range satisfied
                    if (m_request->fileStart<fileSize && m_request->fileEnd<fileSize) {
                        m_statusCode = TS_HTTP_STATUS_PARTIAL_CONTENT;
                        m_statusMsg = TS_HTTP_STATUS_PARTIAL_CONTENT_S;
                        m_sendFilePath = path;
                    }else {
                        m_statusCode = TS_HTTP_STATUS_REQUEST_RANGE_NOT_SATISFIABLE;
                        m_statusMsg = TS_HTTP_STATUS_REQUEST_RANGE_NOT_SATISFIABLE_S;
                    }
                }else {
                    m_statusCode = TS_HTTP_STATUS_OK;
                    m_statusMsg = TS_HTTP_STATUS_OK_S;
                    m_sendFilePath = path;
                }
            }
        } else {
            m_statusCode = TS_HTTP_STATUS_NOT_FOUND;
            m_statusMsg = TS_HTTP_STATUS_NOT_FOUND_S;
            m_sendStr = HTTP_RESPONSE_404;
        }
    }
}

bool CHttpResponse::response(int connFd) {
    bool result = true;
    std::stringstream responseStream;
    
    responseStream<<m_request->version<<" "<<m_statusMsg<<"\r\n";
    //time
    responseStream<<"Date: "<<getStringFromTime(time(0))<<"\r\n";
    
    //server name
    responseStream<<"Server: "<<SERVER_NAME<<"\r\n";
    
    //keep alive
    responseStream<<"Connection: "<<m_request->connection<<"\r\n";
    
    //content length
    long long contentLength = 0;
    //if file exist
    if (!m_sendFilePath.empty()) {
        //if define file end
        if (m_request->fileEnd) {
            contentLength = m_request->fileEnd - m_request->fileStart + 1;
        }
        //if define file start
        else if (m_request->fileStart) {
            contentLength = getFileSize(m_sendFilePath.c_str()) - m_request->fileStart + 1;
        }
        //if undefine start or end
        else {
            contentLength = getFileSize(m_sendFilePath.c_str());
        }
    } else if (!m_sendStr.empty()) {
        contentLength = m_sendStr.length();
    }
    if (contentLength) {
        responseStream<<"Content-Length: "<<contentLength<<"\r\n";
    }
    
    //last modified
    if (!m_sendFilePath.empty()) {
        responseStream<<"Last-Modified: "<<getStringFromTime(fileModifiedTime(m_sendFilePath.c_str()))<<"\r\n";
        
        responseStream<<"Accept-Ranges: "<<"bytes"<<"\r\n";
    }
    
    //content type
    if (!m_sendFilePath.empty()) {
        char path[256];
        strcpy(path, m_sendFilePath.c_str());
        char *ext = strtok(path, ".");
        char *lastExt = ext;
        while (ext!=NULL) {
            ext = strtok(NULL, ".");
            if (ext) lastExt = ext;
        }
        for (int i=0; i<38; i++) {
            if (strcmp(mmt[i].ext, lastExt)==0) {
                responseStream<<"Content-Type: "<<mmt[i].type<<"\r\n";
                break;
            }
        }
    }
    
    //other
    switch (m_statusCode) {
        case TS_HTTP_STATUS_UNAUTHORIZED:
            responseStream<<"WWW-Authenticate: Basic realm=\"zhaoxy.com\"\r\n";
            break;
        case TS_HTTP_STATUS_FOUND:
            responseStream<<"Location: /index.html\r\n";
            break;
        case TS_HTTP_STATUS_PARTIAL_CONTENT:
            responseStream<<"Content-Range: "<<"bytes "<<m_request->fileStart<<"-"<<(m_request->fileEnd==0?contentLength:m_request->fileEnd)<<"/"<<getFileSize(m_sendFilePath.c_str())<<"\r\n";
            break;
        default:
            break;
    }
    
    //seperator
    responseStream<<"\r\n";
    
    //send response header
    std::string responseStr = responseStream.str();
    std::cout<<responseStr<<std::endl;
    
    send(connFd, responseStr.c_str(), responseStr.length(), 0);
    
    //content
    
    //if not head method
    if (m_request->method.compare(TS_HTTP_METHOD_HEAD_S)!=0) {
        if (!m_sendFilePath.empty()) {
            std::ifstream file(m_sendFilePath);
            file.seekg(m_request->fileStart, std::ifstream::beg);
            while(file.tellg() != -1)
            {
                char *p = new char[1024];
                bzero(p, 1024);
                file.read(p, 1024);
                int n = (int)send(connFd, p, 1024, 0);
                if (n < 0) {
                    std::cout<<"ERROR writing to socket"<<std::endl;
                    result = false;
                    break;
                }
                delete p;
            }
            file.close();
        }else {
            send(connFd, m_sendStr.c_str(), m_sendStr.length(), 0);
        }
    }
    
    return result;
}
