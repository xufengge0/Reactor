#include"Acceptor.h"

Acceptor::Acceptor(EventLoop* el,const char* ip,const char* port):el_(el),
    acceptsocket_(creatnonblocking()),acceptchannel_(el_,acceptsocket_.fd())
{
    // 创建监听的socket，并设置为非阻塞
    //acceptsocket_ = new Socket(creatnonblocking());
    InetAdress servaddr(ip,port);

    // 设置listenfd的属性，如果对这些属性不熟悉，百度之。
    acceptsocket_.setSO_KEEPALIVE(true);
    acceptsocket_.setSO_REUSEADDR(true);
    acceptsocket_.setSO_REUSEPORT(true);
    acceptsocket_.setTCP_NODELAY(true);
    acceptsocket_.bind(servaddr);
    acceptsocket_.listen(128);

    // 创建channel对象,绑定listenfd、epollfd
    //acceptchannel_ = new Channel(el_,acceptsocket_.fd());
    acceptchannel_.setreadcallback(std::bind(&Acceptor::newconnection,this));
    // 将要监听的读事件添加到红黑树
    acceptchannel_.enablereading();
    
}

Acceptor::~Acceptor()
{
    //delete acceptsocket_;
    //delete acceptchannel_;
}

#include"Connection.h"
void Acceptor::newconnection()
{
    InetAdress clientaddr;

    // 
    std::unique_ptr<Socket> clientfd(new Socket(acceptsocket_.accept(clientaddr)));
    clientfd->setipport(clientaddr.ip(),clientaddr.port());
    
    newconnection_(std::move(clientfd));

}
void Acceptor::setnewconnection(std::function<void(std::unique_ptr<Socket> )> fn)
{
    newconnection_=fn;
}