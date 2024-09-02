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
#include"InetAddress.h"
#include"Socket.h"
#include <vector>
#include"Channel.h"

class Channel;

class Epoll
{
private:
    int epollfd_=1;
    static const int MaxEvents=100;
    epoll_event events_[MaxEvents];
public:
    Epoll();
    ~Epoll();

    // 用epoll_ctl添加监听的fd和事件
    // 把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件
    void updatechannel(Channel* ch); 

    // 从红黑树中删除channel
    void removechannel(Channel* ch);

    // 等待epoll_wait返回
    std::vector<Channel*> loop(int timeout);
};