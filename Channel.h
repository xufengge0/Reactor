#pragma once
#include<sys/epoll.h>
#include"Epoll.h"
#include"Socket.h"
#include<functional>
#include "EventLoop.h"
#include"Connection.h"
#include <memory>

// 前向声明
class EventLoop;

class Channel
{
private:
    int fd_=-1;             // Channel拥有的fd，Channel和fd是一对一的关系。
    //Epoll *ep_=nullptr;     // Channel对应的红黑树，Channel与Epoll是多对一的关系，一个Channel只对应一个Epoll。
    bool inepoll_=false;    // Channel是否已添加到epoll树上，如果未添加，调用epoll_ctl()的时候用EPOLL_CTL_ADD，否则用EPOLL_CTL_MOD
    uint32_t events_=0;     // fd需要监视的事件。listenfd和clientfd需要监视EPOLLIN，clientfd还可能需要监视EPOLLOUT。
    uint32_t revents_=0;    // fd已发生的事件。
    bool islisten_=false;   // listenfd为true，客户端连接的fd为false


    std::function<void()> readcallback_;  // fd读事件的回调函数。
    std::function<void()> closecallback_; // TCP连接关闭(断开)的回调函数
    std::function<void()> errorcallback_; // TCP连接错误的回调函数
    std::function<void()> writecallback_; // 写事件的回调函数
    EventLoop* el_;

public:
    Channel(EventLoop* el,int fd);
    ~Channel();

    // 返回fd 成员。
    int fd();
    // 采用边缘触发。                       
    void useet();

    // 让epoll_ctl监视fd的读事件。
    void enablereading();
    void disablereading();  // 取消读事件。
    void enablewriteing();  // 注册写事件。
    void disablewriteing(); // 取消写事件。

    void disableall();  // 取消全部的事件
    void remove();      // 从事件循环中删除channel

    //把inepoll成员的值设置为true。
    void setinepoll();
    // 设置revents成员的值为参数ev。              
    void setrevents(uint32_t ev);
    // 返回inepol成员。
    bool inpoll();     
    // 返回events成员。
    uint32_t events();           
    //返回revents成员。
    uint32_t revents();    

    // epoll_wait返回时，处理事件的函数
    void handleevent();         

    //void onmessage();   // 处理对端发送过来的消息。

    void setreadcallback(std::function<void()> fn); // 设置fd读事件回调函数
    void setclosecallback(std::function<void()> fn); // 设置TCP连接关闭(断开)的回调函数
    void seterrorcallback(std::function<void()> fn); // 设置TCP连接错误的回调函数
    void setwritecallback(std::function<void()> fn); // 设置写事件的回调函数
};


