#include"EchoServer.h"

EchoServer::EchoServer(const char* ip,const char* port,int subtheadnum,int worktheadnum):tcpserver(ip,port,subtheadnum)
{
    theadpool_ = new TheadPool(worktheadnum,"WORK");

    tcpserver.setnewconntioncb(std::bind(&EchoServer::HandleNewConnection,this,std::placeholders::_1));
    tcpserver.setcloseconntioncb(std::bind(&EchoServer::HandleClose,this,std::placeholders::_1));
    tcpserver.seterrorconntioncb(std::bind(&EchoServer::HandleError,this,std::placeholders::_1));
    tcpserver.setonmessagecb(std::bind(&EchoServer::HandleMessage,this,std::placeholders::_1,std::placeholders::_2));
    tcpserver.setsendcompletecb(std::bind(&EchoServer::HandleSendComplete,this,std::placeholders::_1));
    //tcpserver.settimeoutcb(std::bind(&EchoServer::HandleEpollTimeout,this,std::placeholders::_1));
}

EchoServer::~EchoServer()
{
}
void EchoServer::Start()
{
    tcpserver.start();
}                                 
void EchoServer::Stop()
{
    // 停止工作线程
    theadpool_->stop();
    printf("工作线程已停止\n");
    // 停止IO线程
    tcpserver.stop();
} 
void EchoServer::HandleNewConnection(spConnection conn)
{
    //printf("HandleNewConnection thread is(%d)\n",syscall(SYS_gettid));
    printf ("%s accept client(fd=%d,ip=%s, port=%d) ok\n",Timestamp::now().tostring().c_str(),conn->fd(),conn->ip(),conn->port());

    // 根据业务的需求，在这里可以增加其它的代码。
}   
void EchoServer::onmessage(spConnection conn,std::string& message)
{
    //printf("message(eventfd=%d):%s\n",conn->fd(),message.c_str());

    message= "reply:" + message;
    
    conn->send(message.data(),message.size());
}
void EchoServer::HandleClose(spConnection conn)
{
    printf ("%s close client(fd=%d,ip=%s, port=%d) ok\n",Timestamp::now().tostring().c_str(),conn->fd(),conn->ip(),conn->port());

    std::cout << "EchoServer conn closed." << std::endl;
    
    //根据业务的需求，在这里可以增加其它的代码,
}           
void EchoServer::HandleError(spConnection conn)
{
    std::cout << "EchoServer conn error." << std::endl;
    
    //根据业务的需求，在这里可以增加其它的代码,
}           
void EchoServer::HandleMessage(spConnection conn,std::string& message)
{
    //printf("HandleMessage thread is(%d)\n",syscall(SYS_gettid));

    // 是否存在工作线程
    if(theadpool_->size()==0)
    {   
        // 如果没有工作线程，直接在IO线程中计算
        onmessage(conn,message);
    }
    else
    {
        // 把计算任务添加到工作线程池
        theadpool_->addtask(std::bind(&EchoServer::onmessage,this,conn,message));
    }
    
}
void EchoServer::HandleSendComplete(spConnection conn)
{
    //std::cout <<"Message send complete." << std::endl;
    
    // 根据业务的需求，在这里可以增加其它的代码,
}
/* void EchoServer::HandleEpollTimeout(EventLoop* loop)
{
    std::cout <<"EchoServer timeout." << std::endl;
    
    // 根据业务的需求，在这里可以增加其它的代码,
}   */  