//
//  CHttpRequest.h
//  TSHttpServer
//
//  Created by zhaoxy on 14-4-23.
//  Copyright (c) 2014年 tsinghua. All rights reserved.
//

#ifndef __TSHttpServer__CHttpRequest__
#define __TSHttpServer__CHttpRequest__

#include <iostream>
#include <sstream>
#include <map>

//http method
#define TS_HTTP_METHOD_CONNECT_S                       "CONNECT"
#define TS_HTTP_METHOD_DELETE_S                        "DELETE"
#define TS_HTTP_METHOD_GET_S                           "GET"
#define TS_HTTP_METHOD_HEAD_S                          "HEAD"
#define TS_HTTP_METHOD_OPTIONS_S                       "OPTIONS"
#define TS_HTTP_METHOD_POST_S                          "POST"
#define TS_HTTP_METHOD_PUT_S                           "PUT"
#define TS_HTTP_METHOD_TRACE_S                         "TRACE"

//http header
#define TS_HTTP_HEADER_USER_AGENT                       "User-Agent:"
#define TS_HTTP_HEADER_ACCEPT                           "Accept:"
#define TS_HTTP_HEADER_ACCEPT_LANGUAGE                  "Accept-Language:"
#define TS_HTTP_HEADER_ACCEPT_ENCODING                  "Accept-Encoding:"
#define TS_HTTP_HEADER_ACCEPT_CHARSET                   "Accept-Charset:"
#define TS_HTTP_HEADER_PROXY_CONNECTION                 "Proxy-Connection:"
#define TS_HTTP_HEADER_CONNECTION                       "Connection:"
#define TS_HTTP_HEADER_COOKIE                           "Cookie:"
#define TS_HTTP_HEADER_HOST                             "Host:"
#define TS_HTTP_HEADER_CACHE_CONTROL                    "Cache-Control:"
#define TS_HTTP_HEADER_CONTENT_TYPE                     "Content-Type:"
#define TS_HTTP_HEADER_CONTENT_LENGTH                   "Content-Length:"
#define TS_HTTP_HEADER_CONTENT_ENCODING                 "Content-Encoding:"
#define TS_HTTP_HEADER_SERVER                           "Server:"
#define TS_HTTP_HEADER_DATE                             "Date:"
#define TS_HTTP_HEADER_RANGE                            "Range:"
#define TS_HTTP_HEADER_ETAG                             "Etag:"
#define TS_HTTP_HEADER_EXPIRES                          "Expires:"
#define TS_HTTP_HEADER_REFERER                          "Referer:"
#define TS_HTTP_HEADER_LAST_MODIFIED                    "Last-Modified:"
#define TS_HTTP_HEADER_IF_MOD_SINCE                     "If-Modified-Since:"
#define TS_HTTP_HEADER_IF_NONE_MATCH                    "If-None-Match:"
#define TS_HTTP_HEADER_ACCEPT_RANGES                    "Accept-Ranges:"
#define TS_HTTP_HEADER_TRANSFER_ENCODING                "Transfer-Encoding:"
#define TS_HTTP_HEADER_AUTHORIZATION                    "Authorization:"

class CHttpRequest {
public:
    std::map<std::string, void(CHttpRequest::*)(char*)> fieldMap;
    
    std::string method;
    std::string path;
    std::string version;
    
    std::string connection;
    std::string authorize;
    std::string modifiedTime;
    long fileStart;
    long fileEnd;
    
    CHttpRequest();
    void handleRequest(char *header);
private:
    void handleConnection(char *);
    void handleAuthorization(char *);
    void handleRange(char *);
    void handleIfModSince(char *);
};

#endif /* defined(__TSHttpServer__CHttpRequest__) */
