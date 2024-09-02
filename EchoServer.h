#pragma once
#include"TcpServer.h"
/* 
    回显服务器的类
 */
class EchoServer
{
private:
    TcpServer tcpserver;
    TheadPool *theadpool_;              // 线程池
public:
    EchoServer(const char* ip,const char* port,int subtheadnum=3,int worktheadnum=5);
    ~EchoServer();

    void Start();                                 // 开始事件循环
    void Stop();                                  // 结束事件循环
    void HandleNewConnection(spConnection conn);   // 处理新客户端连接请求
    void onmessage(spConnection conn,std::string& message);  // 处理业务函数
    
    void HandleClose(spConnection conn);           // TCP连接关闭(断开)的回调函数，供TcpServer回调
    void HandleError(spConnection conn);           // TCP连接错误的回调函数，供TcpServer回调
    void HandleMessage(spConnection conn,std::string& message);//处理客户端的请求报文，供TcpServer回调
    void HandleSendComplete(spConnection conn);    // 数据发送完成后，在TcpServer类中回调此函数。
    //void HandleEpollTimeout(EventLoop* loop);     // epoll_wait()超时，在TcpServer类中回调此函数。
};


