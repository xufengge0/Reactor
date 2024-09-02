#pragma once
#include"Epoll.h"
#include "Channel.h"
#include <memory>
#include <unistd.h>
#include <sys/syscall.h>
#include <mutex>
#include <queue>
#include<sys/eventfd.h>
#include<sys/timerfd.h>
#include<map>
#include"Connection.h"
#include<atomic>
class Epoll;
class Channel;
class Connection;
using spConnection = std::shared_ptr<Connection>;

// 事件循环类
class EventLoop
{
private:
    std::unique_ptr<Epoll> ep_;     // 使用栈内存会发生头文件和前置声明的报错！
    std::function<void(EventLoop*)> epolltimeoutcallback_;
    pid_t threadid_;                // 所在线程的id
    std::mutex mutex_;              // 任务队列同步的互斥锁。
    std::queue<std::function<void()>> taskqueue_; // 事件循环线程被eventfd唤醒后执行的任务队列。
    int wakeupfd_;                  // 用于唤醒事件循环线程的eventfd
    Channel* wakeupchannel_;        // eventfd的channel
    int timerfd_;                   // 定时器fd
    std::unique_ptr<Channel> timerchannel_; // 定时器fd的channel
    bool mainloop_;                 // 是否是主事件循环
    std::map<int,spConnection> conns_;
    std::function<void(int)> timercallback_;//删除TcpServer中超时的Connection对象，将被设置为TcpServer:removeconn
    std::mutex mmutex_;             //保护conns的互斥锁
    int timeval_;                    //闹钟时间间隔，单位:秒。
    int timeout_;                    //Connection对象超时的时间，单位:秒.
    std::atomic_bool stop_;         // 事件结束的标志位

public:
    EventLoop(bool mainloop,int timeval=30,int timeout=80); // 在构造函数中创建Epoll对象ep_
    ~EventLoop();// 在析构函数中销毁Epoll对象ep_

    void run();
    void stop();    // 停止事件循环的函数
    Epoll* ep();
    void removechannel(Channel* ch);    // 从红黑树上删除channel
    void setepolltimeoutcallback(std::function<void(EventLoop*)> fn);
    bool isinloopthread();              // 判断当前线程是否为事件循环线程
    void queueinloop(std::function<void()> fn); // 添加任务到队列中
    void wakeup();                      // 唤醒事件
    void handlewakeup();                // 唤醒后执行事件的函数
    void handletimer();                 // 定时器事件的函数
    void newconnection(spConnection conn);  //把Connection对象保存在conns_中
    void settimercallback(std::function<void(int)> fn);
};


