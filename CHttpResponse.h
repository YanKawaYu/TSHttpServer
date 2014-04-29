//
//  CHttpResponse.h
//  TSHttpServer
//
//  Created by zhaoxy on 14-4-23.
//  Copyright (c) 2014年 tsinghua. All rights reserved.
//

#ifndef __TSHttpServer__CHttpResponse__
#define __TSHttpServer__CHttpResponse__

#include <iostream>
#include <fstream>
#include <sys/stat.h>

//http response status code
#define TS_HTTP_STATUS_CONTINUE                        100
#define TS_HTTP_STATUS_SWITCHING_PROTOCOLS             101

#define TS_HTTP_STATUS_OK                              200
#define TS_HTTP_STATUS_CREATED                         201
#define TS_HTTP_STATUS_ACCEPTED                        202
#define TS_HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION   203
#define TS_HTTP_STATUS_NO_CONTENT                      204
#define TS_HTTP_STATUS_RESET_CONTENT                   205
#define TS_HTTP_STATUS_PARTIAL_CONTENT                 206

#define TS_HTTP_STATUS_MULTIPLE_CHOICES                300
#define TS_HTTP_STATUS_MOVED_PERMANENTLY               301
#define TS_HTTP_STATUS_FOUND                           302
#define TS_HTTP_STATUS_SEE_OTHER                       303
#define TS_HTTP_STATUS_NOT_MODIFIED                    304
#define TS_HTTP_STATUS_USE_PROXY                       305
#define TS_HTTP_STATUS_TEMPORARY_REDIRECT              307

#define TS_HTTP_STATUS_BAD_REQUEST                     400
#define TS_HTTP_STATUS_UNAUTHORIZED                    401
#define TS_HTTP_STATUS_PAYMENT_REQUIRED                402
#define TS_HTTP_STATUS_FORBIDDEN                       403
#define TS_HTTP_STATUS_NOT_FOUND                       404
#define TS_HTTP_STATUS_METHOD_NOT_ALLOWED              405
#define TS_HTTP_STATUS_NOT_ACCEPTABLE                  406
#define TS_HTTP_STATUS_PROXY_AUTHENICATION_REQUIRED    407
#define TS_HTTP_STATUS_REQUEST_TIME_OUT                408
#define TS_HTTP_STATUS_CONFLICT                        409
#define TS_HTTP_STATUS_GONE                            410
#define TS_HTTP_STATUS_LENGTH_REQUIRED                 411
#define TS_HTTP_STATUS_PRECONDITION_FAILED             412
#define TS_HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE        413
#define TS_HTTP_STATUS_REQUEST_URI_TOO_LARGE           414
#define TS_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE          415
#define TS_HTTP_STATUS_REQUEST_RANGE_NOT_SATISFIABLE   416
#define TS_HTTP_STATUS_EXPECTATION_FAILED              417

#define TS_HTTP_STATUS_INTERNAL_SERVER_ERROR           500
#define TS_HTTP_STATUS_NOT_IMPLEMENTED                 501
#define TS_HTTP_STATUS_BAD_GATEWAY                     502
#define TS_HTTP_STATUS_SERVICE_UNAVAILABLE             503
#define TS_HTTP_STATUS_GATEWAY_TIME_OUT                504
#define TS_HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED      505

//http response status
#define TS_HTTP_STATUS_CONTINUE_S                      "100 Continue"
#define TS_HTTP_STATUS_SWITCHING_PROTOCOLS_S           "101 Switching Protocols"

#define TS_HTTP_STATUS_OK_S                            "200 OK"
#define TS_HTTP_STATUS_CREATED_S                       "201 Created"
#define TS_HTTP_STATUS_ACCEPTED_S                      "202 Accepted"
#define TS_HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION_S "203 Non-Authoritative Information"
#define TS_HTTP_STATUS_NO_CONTENT_S                    "204 No Content"
#define TS_HTTP_STATUS_RESET_CONTENT_S                 "205 Reset Content"
#define TS_HTTP_STATUS_PARTIAL_CONTENT_S               "206 Partial Content"

#define TS_HTTP_STATUS_MULTIPLE_CHOICES_S              "300 Multiple Choices"
#define TS_HTTP_STATUS_MOVED_PERMANENTLY_S             "301 Moved Permanently"
#define TS_HTTP_STATUS_FOUND_S                         "302 Found"
#define TS_HTTP_STATUS_SEE_OTHER_S                     "303 See Other"
#define TS_HTTP_STATUS_NOT_MODIFIED_S                  "304 Not Modified"
#define TS_HTTP_STATUS_USE_PROXY_S                     "305 Use Proxy"
#define TS_HTTP_STATUS_TEMPORARY_REDIRECT_S            "307 Temporary Redirect"

#define TS_HTTP_STATUS_BAD_REQUEST_S                   "400 Bad Request"
#define TS_HTTP_STATUS_UNAUTHORIZED_S                  "401 Unauthorized"
#define TS_HTTP_STATUS_PAYMENT_REQUIRED_S              "402 Payment Required"
#define TS_HTTP_STATUS_FORBIDDEN_S                     "403 Forbidden"
#define TS_HTTP_STATUS_NOT_FOUND_S                     "404 Not Found"
#define TS_HTTP_STATUS_METHOD_NOT_ALLOWED_S            "405 Not Allowed"
#define TS_HTTP_STATUS_NOT_ACCEPTABLE_S                "406 Not Acceptable"
#define TS_HTTP_STATUS_PROXY_AUTHENICATION_REQUIRED_S  "407 Authenication Required"
#define TS_HTTP_STATUS_REQUEST_TIME_OUT_S              "408 Request Time-out"
#define TS_HTTP_STATUS_CONFLICT_S                      "409 Conflict"
#define TS_HTTP_STATUS_GONE_S                          "410 Gone"
#define TS_HTTP_STATUS_LENGTH_REQUIRED_S               "411 Length Required"
#define TS_HTTP_STATUS_PRECONDITION_FAILED_S           "412 Precondition Required"
#define TS_HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE_S      "413 Request Enity Too Large"
#define TS_HTTP_STATUS_REQUEST_URI_TOO_LARGE_S         "414 Request-URI Too Large"
#define TS_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE_S        "415 Unsupported Media Type"
#define TS_HTTP_STATUS_REQUEST_RANGE_NOT_SATISFIABLE_S "416 Requested range not satisfiable"
#define TS_HTTP_STATUS_EXPECTATION_FAILED_S            "417 Expectation Failed"

#define TS_HTTP_STATUS_INTERNAL_SERVER_ERROR_S         "500 Internal Server Error"
#define TS_HTTP_STATUS_NOT_IMPLEMENTED_S               "501 Not Implemented"
#define TS_HTTP_STATUS_BAD_GATEWAY_S                   "502 Bad Gateway"
#define TS_HTTP_STATUS_SERVICE_UNAVAILABLE_S           "503 Service Unavailable"
#define TS_HTTP_STATUS_GATEWAY_TIME_OUT_S              "504 Gateway Time-out"
#define TS_HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED_S    "505 HTTP Version not supported"

//content type
static const struct {
	const char *ext;
	const char *type;
} mmt[] = {
	{ "html",	"text/html"			},
	{ "htm",	"text/html"			},
	{ "shtm",	"text/html"			},
	{ "shtml",	"text/html"			},
	{ "css",	"text/css"			},
	{ "js",		"application/x-javascript"	},
	{ "ico",	"image/x-icon"			},
	{ "gif",	"image/gif"			},
	{ "jpg",	"image/jpeg"			},
	{ "jpeg",	"image/jpeg"			},
	{ "png",	"image/png"			},
	{ "svg",	"image/svg+xml"			},
	{ "torrent",	"application/x-bittorrent"	},
	{ "wav",	"audio/x-wav"			},
	{ "mp3",	"audio/x-mp3"			},
	{ "mid",	"audio/mid"			},
	{ "m3u",	"audio/x-mpegurl"		},
	{ "ram",	"audio/x-pn-realaudio"		},
	{ "ra",		"audio/x-pn-realaudio"		},
	{ "doc",	"application/msword",		},
	{ "exe",	"application/octet-stream"	},
	{ "zip",	"application/x-zip-compressed"	},
	{ "xls",	"application/excel"		},
	{ "tgz",	"application/x-tar-gz"		},
	{ "tar.gz",	"application/x-tar-gz"		},
	{ "tar",	"application/x-tar"		},
	{ "gz",		"application/x-gunzip"		},
	{ "arj",	"application/x-arj-compressed"	},
	{ "rar",	"application/x-arj-compressed"	},
	{ "rtf",	"application/rtf"		},
	{ "pdf",	"application/pdf"		},
	{ "swf",	"application/x-shockwave-flash"	},
	{ "mpg",	"video/mpeg"			},
	{ "mpeg",	"video/mpeg"			},
	{ "asf",	"video/x-ms-asf"		},
	{ "avi",	"video/x-msvideo"		},
	{ "bmp",	"image/bmp"			},
	{ NULL,		NULL				}
};

class CHttpRequest;
class CHttpResponse {
private:
    CHttpRequest *m_request;
    
    int m_statusCode;
    std::string m_statusMsg;
    std::string m_sendFilePath;
    std::string m_sendStr;
    
    inline bool isFileExist(const char *fileName) { std::ifstream infile(fileName); return infile.good(); }
    inline long long getFileSize(const char *fileName) { std::ifstream file(fileName); file.seekg(0,std::ios::end); long long fileSize=file.tellg(); file.close(); return fileSize;}
    inline time_t fileModifiedTime(const char *fileName) { struct stat attr; stat(fileName, &attr); return attr.st_mtime;}
public:
    CHttpResponse(CHttpRequest *request);
    bool response(int connFd);
};

#endif /* defined(__TSHttpServer__CHttpResponse__) */
