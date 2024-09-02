#pragma once
#include "EventLoop.h"


class Acceptor
{
private:
    EventLoop* el_;
    Socket acceptsocket_;
    Channel acceptchannel_;
    std::function<void(std::unique_ptr<Socket>)> newconnection_;

public:
    Acceptor(EventLoop* el_,const char* ip,const char* port);
    ~Acceptor();

    void newconnection();   // 处理新客户端连接请求.
    
    void setnewconnection(std::function<void(std::unique_ptr<Socket>)> fn);
};
