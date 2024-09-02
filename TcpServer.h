#pragma once
#include "EventLoop.h"
#include"Acceptor.h"
#include<map>
#include"TheadPool.h"
#include <memory>
#include<mutex>

class TcpServer
{
private:
    // 事件循环类
    std::unique_ptr<EventLoop> mainloop_;               // 主事件循环
    std::vector<std::unique_ptr<EventLoop>> subloops_;  // 从事件循环
    int theadpoolnum_;                 // 线程池大小(与构造函数初始化列表中的先后顺序保持一致！)
    TheadPool theadpool_;              // 线程池
    std::mutex mutex_;                 //保护conns的互斥锁
    
    Acceptor acceptor_;
    std::map<int,spConnection> conns_;

    std::function<void(spConnection)> newconntioncb_;    //回调EchoServer::HandleNewConnection().
    std::function<void(spConnection)> closeconntioncb_;  //回调EchoServer::HandleClose()
    std::function<void(spConnection)> errorconntioncb_;  //回调EchoServer::HandleError()
    std::function<void(spConnection,std::string &message)>onmessagecb_;//回调EchoServer:HandleMessage().
    std::function<void(spConnection)> sendcompletecb_;   //回调EchoServer::HandleSendComplete()
    std::function<void(EventLoop*)> timeoutcb_;         //回调EchoServer::HandleTimeOut()

public:
    TcpServer(const char* ip,const char* port,int theadpoolnum=3);
    ~TcpServer();

    void start();                           // 开始事件循环
    void stop();                            // 停止IO线程和事件循环
    void newconnection(std::unique_ptr<Socket> clientfd);   // 处理新客户端连接请求.
    
    void closecallback(spConnection conn);   // TCP连接关闭(断开)的回调函数，供Connection回调
    void errorcallback(spConnection conn);   // TCP连接错误的回调函数，供Connection回调
    void onmessage(spConnection conn,std::string& message);//处理客户端的请求报文，供Connection回调
    void sendcomplete(spConnection conn);    // 数据发送完成后，在Connection类中回调此函数。
    void epolltimeout(EventLoop* loop);      // epoll_wait()超时，在EventLoop类中回调此函数。

    void setnewconntioncb(std::function<void(spConnection)> fn);
    void setcloseconntioncb(std::function<void(spConnection)> fn);
    void seterrorconntioncb(std::function<void(spConnection)> fn);
    void setonmessagecb(std::function<void(spConnection,std::string &message)> fn);
    void setsendcompletecb(std::function<void(spConnection)> fn);
    void settimeoutcb(std::function<void(EventLoop*)> fn);
    void removeconn(int fd);    //删除conns 中的Connection对象，在EventLoop::handletimer()中将回调此函数。
    
};