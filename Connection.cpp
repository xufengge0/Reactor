#include"Connection.h"

Connection::Connection(EventLoop* el,std::unique_ptr<Socket> clientsocket):clientsocket_(std::move(clientsocket)),
    el_(el),disconnect_(false),clientchannel_(new Channel(el_,clientsocket_->fd()))
{
    
    //为新客户端连接准备读事件，并添加到epoll中。
    //clientchannel_ = new Channel(el_,clientsocket_->fd());
    
    clientchannel_->setreadcallback(std::bind(&Connection::onmessage,this));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback,this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback,this));
    clientchannel_->setwritecallback(std::bind(&Connection::writecallback,this));

    //clientchannel_->useet(); // 边缘触发
    clientchannel_->enablereading();
}

Connection::~Connection()
{
    //printf("conn已析构\n");
    //delete clientsocket_;
    //delete clientchannel_;
}
const int Connection::fd()
{
    return clientsocket_->fd();
}
const char* Connection::ip()
{
    return clientsocket_->ip();
}
const uint16_t Connection::port()
{
    return clientsocket_->port();
}
void Connection::closecallback()
{
    disconnect_=true;
    clientchannel_->remove();   // tcp连接断开删除channel
    closecallback_(shared_from_this());
}
void Connection::errorcallback()
{
    disconnect_=true;
    clientchannel_->remove();   // tcp连接断开删除channel
    errorcallback_(shared_from_this());
}
void Connection::setclosecallback(std::function<void(spConnection)> fn)
{
    closecallback_=fn;
}
void Connection::seterrorcallback(std::function<void(spConnection)> fn)
{
    errorcallback_=fn;
}
void Connection::setonmessagecallback(std::function<void(spConnection,std::string&)> fn)
{
    onmessagecallback_=fn;
}
void Connection::setsendcompletecallback_(std::function<void(spConnection)> fn)
{
    sendcompletecallback_=fn;
}
void Connection::onmessage()
{
    char buffer[1024];
    bzero(buffer,sizeof(buffer)); // 用于将指定内存区域的内容全部设置为零。已弃用

    // 用循环读取数据
    while (1)
    {
        int nread = recv(fd(),buffer,sizeof(buffer),0);

        if(nread>0) // 读取到数据
        {
            inputbuffer_.append(buffer,nread);
        }
        else if(nread==-1 && errno==EINTR) // 读取数据时被信号中断，继续读取
        {
            continue;
        }
        else if(nread==-1 && ((errno==EAGAIN)||(errno==EWOULDBLOCK))) // 全部数据已读取完毕（表示当前没有数据可读，但不会阻塞程序。）
        {
            std::string message;
            while (1)
            {   
                
                //////////////////////////////////////////////////////////
                // 从接收缓冲区中读取数据并赋给message
                if(inputbuffer_.pickmessage(message)==false) break;
                ////////////////////////////////////////////////////////////
                
                lastatime_=Timestamp::now();

                onmessagecallback_(shared_from_this(),message);
                //std::cout<<"lastatime="<<lastatime_.tostring()<<std::endl;
            }
            break;
            
        }
        else if(nread==0) // 连接已断开
        {
            closecallback();
            break;
        }
    }
}
void Connection::send(const char* data,size_t size)
{
    if(disconnect_==true) {printf("客户端连接断开，send返回\n"); return;}

    // 判断当前线程是否是事件线程（IO线程）
    if(el_->isinloopthread()==true)
    {
        // 如果是IO线程，直接发送数据
        sendinloop(data,size);
        //printf("send() 在事件循环的线程中。\n");
    }
    else
    {
        // 如果不是IO线程，交给IO线程执行发送数据操作
        //printf("send() 不在事件循环的线程中。\n");
        
        // 将sendinloop丢到任务队列中
        el_->queueinloop(std::bind(&Connection::sendinloop,this,data,size));
    }
    
}
void Connection::writecallback()
{
    int writen=::send(clientsocket_->fd(),outputbuffer_.data(),outputbuffer_.size(),0);

    if(writen>0) outputbuffer_.erase(0,writen); // 删除outputbuffer_中已发送的数据

    if(outputbuffer_.size()==0) // 数据全部发送完
    {
        clientchannel_->disablewriteing();  // 关闭写事件

        sendcompletecallback_(shared_from_this());    // 回调TCPserver的业务函数
    }
}
void Connection::sendinloop(const char* data,size_t size)
{
    // 把要发送的数据放入发送缓冲区
    outputbuffer_.appendwithsep(data,size);

    // 注册写事件
    clientchannel_->enablewriteing();
}
bool Connection::timeout(time_t now,int val)
{
    return now-lastatime_.toint() > val;
}