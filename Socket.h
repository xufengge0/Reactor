#pragma once

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>// TCP NODELAY需要包含这个头文件。
#include "InetAddress.h"

// 创建一个非阻塞的socket
int creatnonblocking();

class Socket
{
private:
    const int fd_;
    const char* ip_;
    uint16_t port_;
public:
    Socket(int fd):fd_(fd){};
    ~Socket(){::close(fd_);};

    const int fd();
    void setSO_REUSEADDR(bool on);
    void setTCP_NODELAY(bool on);
    void setSO_REUSEPORT(bool on);
    void setSO_KEEPALIVE(bool on);
    
    void bind(const InetAdress& serveraddr);
    void listen(int nn=128);
    int accept(InetAdress& clientaddr);
    void setipport(const char* ip,uint16_t port);   // 设置IP和端口成员变量

    const char* ip();   // 返回ip成员变量
    const uint16_t port(); // 返回端口成员变量
};




