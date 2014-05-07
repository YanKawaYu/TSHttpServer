//
//  main.cpp
//  TSHttpServer
//
//  Created by zhaoxy on 14-4-19.
//  Copyright (c) 2014年 tsinghua. All rights reserved.
//

#include <iostream>
#include <set>
#include "CHttpRequest.h"
#include "CHttpResponse.h"

#include <signal.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#include <sys/mman.h>
#include <pthread.h>

#define GET_ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))
#define PROCESS_NUM 1

//创建socket
int startup(int port) {
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    //协议域（ip地址和端口）
    servAddr.sin_family = AF_INET;
    //绑定默认网卡
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //端口
    servAddr.sin_port = htons(port);
    int listenFd;
    //创建套接字
    if ((listenFd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
        return 0;
    }
    unsigned value = 1;
    setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
    //绑定套接字
    if (bind(listenFd, (struct sockaddr *)&servAddr, sizeof(servAddr))) {
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
        return 0;
    }
    //开始监听，设置最大连接请求
    if (listen(listenFd, 10) == -1) {
        printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
        return 0;
    }
    return listenFd;
}

//管理子进程的数组，数组多大就有几个子进程
static int processArr[PROCESS_NUM];
//创建若干个子进程，返回当前进程是否父进程
bool createSubProcess() {
    for (int i=0; i<GET_ARRAY_LEN(processArr); i++) {
        int pid = fork();
        //如果是子进程，返回0
        if (pid == 0) {
            return false;
        }
        //如果是父进程，继续fork
        else if (pid >0){
            processArr[i] = pid;
            continue;
        }
        //如果出错
        else {
            fprintf(stderr,"can't fork ,error %d\n",errno);
            return true;
        }
    }
    return true;
}

//信号处理
void handleTerminate(int signal) {
    for (int i=0; i<GET_ARRAY_LEN(processArr); i++) {
        kill(processArr[i], SIGTERM);
    }
    exit(0);
}

//处理http请求
bool handleRequest(int connFd) {
    if (connFd<=0) return false;
    //读取缓存
    char buff[4096];
    //读取http header
    int len = (int)recv(connFd, buff, sizeof(buff), 0);
    if (len<=0) {
        return false;
    }
    buff[len] = '\0';
    std::cout<<buff<<std::endl;

    CHttpRequest *httpRequest = new CHttpRequest(connFd);
    httpRequest->handleRequest(buff);
    CHttpResponse *httpResponse = new CHttpResponse(httpRequest);
    bool result = httpResponse->response(connFd);
    
    std::string transformConnection(httpRequest->connection);
    std::transform(transformConnection.begin(), transformConnection.end(), transformConnection.begin(), ::tolower);
    return transformConnection == "Keep-Alive" && result;
}

//互斥量
pthread_mutex_t *mutex;
//创建共享的mutex
void initMutex()
{
    //设置互斥量为进程间共享
    mutex=(pthread_mutex_t*)mmap(NULL, sizeof(pthread_mutex_t), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);
    if( MAP_FAILED==mutex) {
        perror("mutex mmap failed");
        exit(1);
    }
    //设置attr的属性
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    int ret = pthread_mutexattr_setpshared(&attr,PTHREAD_PROCESS_SHARED);
    if(ret != 0) {
        fprintf(stderr, "mutex set shared failed");
        exit(1);
    }
    pthread_mutex_init(mutex, &attr);
}

int main(int argc, const char * argv[])
{
    int listenFd;
    
    initMutex();
    //设置端口号
    listenFd = startup(8080);
    
//    //创建若干个子进程
//    bool isParent = createSubProcess();
//    //如果是父进程
//    if (isParent) {
//        while (1) {
//            //注册信号处理
//            signal(SIGTERM, handleTerminate);
//            //挂起等待信号
//            pause();
//        }
//    }
//    //如果是子进程
//    else {
        //套接字集合
        fd_set rset;
        //最大套接字
        int maxFd = listenFd;
        std::set<int> fdArray;
        //循环处理事件
        while (1) {
            FD_ZERO(&rset);
            FD_SET(listenFd, &rset);
            //重新设置每个需要监听的套接字
            for (std::set<int>::iterator iterator=fdArray.begin();iterator!=fdArray.end();iterator++) {
                FD_SET(*iterator, &rset);
            }
            //开始监听
            if (select(maxFd+1, &rset, NULL, NULL, NULL)<0) {
                fprintf(stderr, "select error: %s(errno: %d)\n",strerror(errno),errno);
                continue;
            }
            
            //遍历每个连接套接字
            for (std::set<int>::iterator iterator=fdArray.begin();iterator!=fdArray.end();) {
                int currentFd = *iterator;
                if (FD_ISSET(currentFd, &rset)) {
                    if (!handleRequest(currentFd)) {
                        close(currentFd);
                        fdArray.erase(iterator++);
                        continue;
                    }
                }
                ++iterator;
            }
            //检查连接监听套接字
            if (FD_ISSET(listenFd, &rset)) {
                if (pthread_mutex_trylock(mutex)==0) {
                    int newFd = accept(listenFd, (struct sockaddr *)NULL, NULL);
                    if (newFd<=0) {
                        fprintf(stderr, "accept socket error: %s(errno: %d)\n",strerror(errno),errno);
                        continue;
                    }
                    std::cout<<"new client"<<std::endl;
                    //忽略SIGPIPE信号，避免客户端关闭端口导致异常
                    int set = 1;
                    setsockopt(newFd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
                    //更新最大的套接字
                    if (newFd>maxFd) {
                        maxFd = newFd;
                    }
                    fdArray.insert(newFd);
                    pthread_mutex_unlock(mutex);
                }
            }
        }
//    }

    close(listenFd);
    return 0;
}

