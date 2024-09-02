#include"Epoll.h"

Epoll::Epoll()
{
    epollfd_=epoll_create(1);
}
Epoll::~Epoll()
{
    close(epollfd_);
}

void Epoll::updatechannel(Channel* ch)
{
    epoll_event ev;
    ev.data.ptr=ch;
    ev.events=ch->events();

    // 是否已经添加到红黑树上，未添加则添加
    if(ch->inpoll()==false)
    {
        if(epoll_ctl(epollfd_,EPOLL_CTL_ADD,ch->fd(),&ev)==-1)
        {
            perror("epoll ctl() failed.\n"); exit(-1);
        }
        ch->setinepoll();
    }
    else // 已添加，则修改
    {
        if(epoll_ctl(epollfd_,EPOLL_CTL_MOD,ch->fd(),&ev)==-1)
        {
            perror("epoll ctl() failed.\n"); exit(-1);
        }
    }
}    
void Epoll::removechannel(Channel* ch)
{
    if(ch->inpoll())
    {
        if(epoll_ctl(epollfd_,EPOLL_CTL_DEL,ch->fd(),0)==-1)
        {
            perror("epoll ctl() failed.\n"); exit(-1);
        }
    }
}
std::vector<Channel*> Epoll::loop(int timeout)
{
    std::vector<Channel*> channels;

    bzero(events_,sizeof(events_));
    int infds=epoll_wait(epollfd_,events_,MaxEvents,timeout);

    if(infds<0) // 返回失败
    {
        // EBADF:epfd不是一个有效的描述符。
        // EFAULT :参数events指向的内存区域不可写。
        // EINVAL:epfd不是一个epoll文件描述符，或者参数maxevents小于等于0.
        // EINTR :阻塞过程中被信号中断，epoll pwait()可以避免，或者错误处理中，解析error后重新调用epoll wait()。
        // 在Reactor模型中，不建议使用信号，因为信号处理起来很麻烦，没有必要。-----陈硕
        perror("epoll wait() failed"); exit(-1);
    }
    if(infds==0) // 超时
    {
        //printf("epoll wait() timeout.\n");
        return channels; // 返回的channels为空
    }

    for(int ii=0;ii<infds;ii++)
    {
        Channel* ch = (Channel*)events_[ii].data.ptr;
        ch->setrevents(events_[ii].events);
        channels.push_back(ch);
    }
    return channels;
}