#include"EventLoop.h"
int createtimerfd(int sec)
{
    int tfd=timerfd_create(CLOCK_MONOTONIC,TFD_CLOEXEC|TFD_NONBLOCK); // 创建timerfd.
    struct itimerspec timeout;  //定时时间的数据结构
    memset(&timeout,0,sizeof(struct itimerspec));
    timeout.it_value.tv_sec = sec; // 定时时间为
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(tfd,0,&timeout,0);
    return tfd;
}
EventLoop::EventLoop(bool mainloop,int timeval,int timeout):ep_(new Epoll),mainloop_(mainloop),timeval_(timeval),timeout_(timeout),
    wakeupfd_(eventfd(0,EFD_NONBLOCK)),wakeupchannel_(new Channel(this,wakeupfd_)),
    timerfd_(createtimerfd(timeout)),timerchannel_(new Channel(this,timerfd_)),stop_(false)
{
    wakeupchannel_->setreadcallback(std::bind(&EventLoop::handlewakeup,this));
    wakeupchannel_->enablereading();

    timerchannel_->setreadcallback(std::bind(&EventLoop::handletimer,this));
    timerchannel_->enablereading();
    
}

EventLoop::~EventLoop()
{
    //delete ep_;
}

void EventLoop::run()
{
    threadid_=syscall(SYS_gettid);  // 获取所在事件循环的线程id

    while (stop_==false)
    {
        // 等待epoll_wait返回
        std::vector<Channel*> evs = ep_->loop(10*1000);  // 设置超时时间10秒

        //如果channels为空，表示超时，回调TcpServer:epolltimeout()
        if(evs.size()==0) 
        {
            epolltimeoutcallback_(this);
        }
        else
        {
            for(int ii=0;ii<evs.size();ii++)
            {
                // 处理epoll_wait返回的事件
                evs[ii]->handleevent();
            }
        }
        
    }
}
void EventLoop::stop()
{
    stop_=true;
    wakeup(); // 唤醒事件循环，使其停止
}
Epoll* EventLoop::ep()
{
    return ep_.get();
}
void EventLoop::removechannel(Channel* ch)
{
    ep_->removechannel(ch);
}
void EventLoop::setepolltimeoutcallback(std::function<void(EventLoop*)> fn)
{
    epolltimeoutcallback_=fn;
}
bool EventLoop::isinloopthread()
{
    return threadid_==syscall(SYS_gettid);
}
void EventLoop::queueinloop(std::function<void()> fn)
{
    {
        std::lock_guard<std::mutex> gd(mutex_);
        taskqueue_.push(fn);
    }

    wakeup();   // 唤醒事件
}
void EventLoop::wakeup()
{
    uint64_t val=1;
    write(wakeupfd_,&val,sizeof(uint64_t));
}
void EventLoop::handlewakeup()
{
    printf("handlewakeup thread is(%d)\n",syscall(SYS_gettid));

    uint64_t val;
    read(wakeupfd_,&val,sizeof(uint64_t));  //从eventfd中读取出数据，如果不读取，eventfd的读事件会一直触发

    std::lock_guard<std::mutex> gd(mutex_);

    while (taskqueue_.size()>0) // 执行队列中的所有任务
    {
        auto task=std::move(taskqueue_.front());
        taskqueue_.pop();
        task();
    }   
}
void EventLoop::handletimer()
{
    // 重新计时。
    // 定时时间的数据结构。
    struct itimerspec timeout;
    memset(&timeout,0,sizeof(struct itimerspec));
    timeout.it_value.tv_sec = timeval_;    //定时时间，固定为5，方便测试。
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(timerfd_,0,&timeout,0);
    
    if(mainloop_)
    {
        //printf("主事件闹钟时间到了。\n");
    }
    else
    {
        time_t now=time(0);
        //printf("从事件闹钟时间到了。\n");
        for(auto &i:conns_)
        {
            //printf("fd=%d",i.first);
            if(i.second->timeout(now,timeout_))
            {
                {
                    std::lock_guard<std::mutex> gd(mmutex_);
                    conns_.erase(i.first);      //从EventLoop的map中删除超时的conn。
                }
                
                timercallback_(i.first);    //从TcpServer的map中删除超时的conn.
            }
        }
        //printf("\n");
    }
    
}
void EventLoop::newconnection(spConnection conn)
{
    std::lock_guard<std::mutex> gd(mmutex_);
    conns_[conn->fd()]=conn;
}
void EventLoop::settimercallback(std::function<void(int)> fn)
{
    timercallback_=fn;
}