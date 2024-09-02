#include"Channel.h"



Channel::Channel(EventLoop* el,int fd):fd_(fd),el_(el)
{
    
}
Channel::~Channel()
{
    // 在析构函数中，不要销毁ep_，也不能关闭fd_,，因为这两个东西不属于Channel类，Channel类只是需要它们，使用它们而已.
}
int Channel::fd()
{
    return fd_;
}
void Channel::useet()
{
    events_=events_|EPOLLET;
}
void Channel::enablereading()
{
    events_|=EPOLLIN;
    el_->ep()->updatechannel(this);
}
void Channel::disablereading()
{
    events_&=~EPOLLIN;
    el_->ep()->updatechannel(this);
}
void Channel::enablewriteing()
{
    events_|=EPOLLOUT;
    el_->ep()->updatechannel(this);
}
void Channel::disablewriteing()
{
    events_&=~EPOLLOUT;   
    el_->ep()->updatechannel(this);
}
void Channel::disableall()
{
    events_=0;
    el_->ep()->updatechannel(this);
}
void Channel::remove()
{
    el_->removechannel(this);
} 
void Channel::setinepoll()
{
    inepoll_=true;
}
void Channel::setrevents(uint32_t ev)
{
    revents_=ev;
}
bool Channel::inpoll()
{
    return inepoll_;
}           
uint32_t Channel::events()
{
    return events_;
}      
uint32_t Channel::revents()
{
    return revents_;
}
void Channel::handleevent()
{
    // 连接的对端关闭了连接，有的系统检测不到，可以删除
    if(revents()& EPOLLRDHUP)
    {
        //remove();
        closecallback_();
    }
    // 有数据可以读
    else if(revents()& (EPOLLIN|EPOLLPRI))  //(EPOLLPRI事件，及时得知套接字上有优先级较高的数据需要处理，例如紧急消息或控制信息）
    {
       readcallback_(); // 回调函数使用时加"()"
    }
    // 有数据可以写
    else if(revents()& EPOLLOUT)
    {
        writecallback_();
    }
    // 其他事件视为错误
    else
    {
        //remove();
        errorcallback_();
    }
}

void Channel::setreadcallback(std::function<void()> fn)
{
    readcallback_=fn;
}
void Channel::setclosecallback(std::function<void()> fn)
{
    closecallback_=fn;
}
void Channel::seterrorcallback(std::function<void()> fn)
{
    errorcallback_=fn;
}
void Channel::setwritecallback(std::function<void()> fn)
{
    writecallback_=fn;
}