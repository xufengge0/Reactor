#pragma once
#include "EventLoop.h"
#include"Channel.h"
#include"Buffer.h"
#include<memory>
#include<atomic>
#include"Timestamp.h"

class Connection;   // 前置声明
using spConnection = std::shared_ptr<Connection>;

class Channel;
class EventLoop;


class Connection:public std::enable_shared_from_this<Connection> // 继承一个模板类，this指针要替换为shared_from_this()
{
private:
    EventLoop* el_;
    std::unique_ptr<Socket> clientsocket_;
    std::unique_ptr<Channel> clientchannel_;    // Connection存在多个使用智能指针而不是使用栈内存
    std::function<void(spConnection)> closecallback_;
    std::function<void(spConnection)> errorcallback_;
    std::function<void(spConnection,std::string&)> onmessagecallback_;
    std::function<void(spConnection)> sendcompletecallback_;
    Buffer inputbuffer_;    // 接收缓冲区
    Buffer outputbuffer_;   // 发送缓冲区
    std::atomic_bool disconnect_;    // 客户端连接是否已经断开，断开设置为true
    Timestamp lastatime_;   // 时间戳，创建Connection对象时为当前时间，每接收到一个报文，把时间戳更新为当前时间。
    
public:
    Connection(EventLoop* el,std::unique_ptr<Socket> clientsocket);
    ~Connection();

    const int fd();
    const char* ip();
    const uint16_t port();

    void closecallback();   //TCP连接关闭(断开)的回调函数，供Channel回调
    void errorcallback();   //TCP连接错误的回调函数，供Channel回调
    void writecallback();   // 处理写事件的回调函数，供Channel回调

    void setclosecallback(std::function<void(spConnection)> fn);
    void seterrorcallback(std::function<void(spConnection)> fn);
    void setonmessagecallback(std::function<void(spConnection,std::string&)> fn);
    void setsendcompletecallback_(std::function<void(spConnection)> fn);

    void onmessage();   // 处理对端发送过来的消息。

    //发送数据，不管在任何线程中，都是调用此函数发送数据
    void send(const char* data,size_t size);
    //发送数据，如果当前线程是IO线程，直接调用此函数，如果是工作线程，将把此函数传给IO线程
    void sendinloop(const char* data,size_t size); 
    bool timeout(time_t now,int val);     // 判断连接是否超时
};