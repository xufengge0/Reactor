#include "TcpServer.h"

TcpServer::TcpServer(const char* ip,const char* port,int theadpoolnum):theadpoolnum_(theadpoolnum),mainloop_(new EventLoop(true)),
    acceptor_(mainloop_.get(),ip,port),theadpool_(theadpoolnum_,"IO")
{
    //mainloop_ = new EventLoop;
    mainloop_->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout,this,std::placeholders::_1));
    
    //acceptor_ = new Acceptor(mainloop_,ip,port);
    acceptor_.setnewconnection(std::bind(&TcpServer::newconnection,this,std::placeholders::_1));

    //theadpool_ = new TheadPool(theadpoolnum_,"IO");  // 创建线程池

    for(int i=0;i<theadpoolnum_;++i)    // 创建从事件循环
    {
        subloops_.emplace_back(new EventLoop(false,5,10));
        subloops_[i]->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout,this,std::placeholders::_1));
        theadpool_.addtask(std::bind(&EventLoop::run,subloops_[i].get()));   // 在线程池中运行从事件循环
        subloops_[i]->settimercallback(std::bind(&TcpServer::removeconn,this,std::placeholders::_1));   // 设置超时的回调函数
    }
}
TcpServer::~TcpServer()
{
    //delete mainloop_;
    //delete acceptor_;
}
void TcpServer::start()
{
    mainloop_->run();
}
void TcpServer::stop()
{
    // 停止主事件循环。
    mainloop_->stop();
    printf("主事件循环停止\n");
    // 停止从事件循环。
    for(auto &i:subloops_)
        i->stop();
    printf("从事件循环停止\n");
    // 停止IO线程。
    theadpool_.stop();
    printf("IO线程停止\n");
}
void TcpServer::newconnection(std::unique_ptr<Socket> clientfd)
{
    spConnection conn (new Connection(subloops_[clientfd->fd() % theadpoolnum_].get(),std::move(clientfd)));
    conn->setclosecallback(std::bind(&TcpServer::closecallback,this,std::placeholders::_1));
    conn->seterrorcallback(std::bind(&TcpServer::errorcallback,this,std::placeholders::_1));
    conn->setonmessagecallback(std::bind(&TcpServer::onmessage,this,std::placeholders::_1,std::placeholders::_2));
    conn->setsendcompletecallback_(std::bind(&TcpServer::sendcomplete,this,std::placeholders::_1));
    
    //printf ("accept client(fd=%d,ip=%s, port=%d) ok\n",conn->fd(),conn->ip(),conn->port());

    {
        std::lock_guard<std::mutex> gd(mutex_);
        conns_[conn->fd()]=conn;    // 将conn加入TcpServer的map中
    }
    
    subloops_[conn->fd() % theadpoolnum_]->newconnection(conn); // 将conn加入EventLoop的map中

    newconntioncb_(conn);
}
void TcpServer::closecallback(spConnection conn)
{
    closeconntioncb_(conn);

    {
        std::lock_guard<std::mutex> gd(mutex_);
        conns_.erase(conn->fd());
    }
    
}
void TcpServer::errorcallback(spConnection conn)
{
    errorconntioncb_(conn);

    {
        std::lock_guard<std::mutex> gd(mutex_);
        conns_.erase(conn->fd());
    }
}

void TcpServer::onmessage(spConnection conn,std::string& message)
{
    onmessagecb_(conn,message);
    
}
void TcpServer::sendcomplete(spConnection conn)
{
    // 其他业务代码
    sendcompletecb_(conn);
}
void TcpServer::epolltimeout(EventLoop* loop)
{
    // 其他业务代码
    if(timeoutcb_) timeoutcb_(loop);
}
void TcpServer::setnewconntioncb(std::function<void(spConnection)> fn)
{
    newconntioncb_=fn;
}
void TcpServer::setcloseconntioncb(std::function<void(spConnection)> fn)
{
    closeconntioncb_=fn;
}
void TcpServer::seterrorconntioncb(std::function<void(spConnection)> fn)
{
    errorconntioncb_=fn;
}
void TcpServer::setonmessagecb(std::function<void(spConnection,std::string &message)> fn)
{
    onmessagecb_=fn;
}
void TcpServer::setsendcompletecb(std::function<void(spConnection)> fn)
{
    sendcompletecb_=fn;
}
void TcpServer::settimeoutcb(std::function<void(EventLoop*)> fn)
{
    timeoutcb_=fn;
}
void TcpServer::removeconn(int fd)
{
    {
        std::lock_guard<std::mutex> gd(mutex_);
        conns_.erase(fd);
    }
    
}