#include"Socket.h"

int creatnonblocking()
{
    int listenfd=socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK,IPPROTO_TCP);
    if(listenfd<0)
    {
        perror("socket() failed"); exit(-1);
        printf("%s:%s:%d listen socket create error:%d\n",__FILE__,__FUNCTION__,__LINE__, errno); exit(-1);
    }
    return listenfd;
}
const int Socket::fd()
{
    return fd_;
}
const char* Socket::ip()
{
    return ip_;
}
const uint16_t Socket::port()
{
    return port_;
}
void Socket::setSO_REUSEADDR(bool on)
{
    if(on)
    {
        int opt = 1;
        setsockopt(fd_,SOL_SOCKET,SO_REUSEADDR,&opt,static_cast<socklen_t>(sizeof opt));// 必须的。允许重用本地地址和端口
    }
}
void Socket::setTCP_NODELAY(bool on)
{
    if(on)
    {
        int opt = 1;
        setsockopt(fd_,SOL_SOCKET,TCP_NODELAY ,&opt,static_cast<socklen_t>(sizeof opt));// 必须的。禁用Nagle算法（立即发送小数据包）
    }
}
void Socket::setSO_REUSEPORT(bool on)
{
    if(on)
    {
        int opt = 1;
        setsockopt(fd_,SOL_SOCKET,SO_REUSEPORT,&opt,static_cast<socklen_t>(sizeof opt));// 允许多个套接字绑定到同一个端口，从而实现负载均衡和高并发处理,但在reactor中意义不大
    }
}
void Socket::setSO_KEEPALIVE(bool on)
{
    if(on)
    {
        int opt = 1;
        setsockopt(fd_,SOL_SOCKET,SO_KEEPALIVE,&opt,static_cast<socklen_t>(sizeof opt));// 保持连接活跃。建议自己做心跳
    }
}
void Socket::bind(const InetAdress& serveraddr)
{
    if(::bind(fd_,serveraddr.addr(),sizeof(sockaddr))< 0 )
    {
        perror("bind() failed"); close(fd_); exit(-1);
    }

    ip_=serveraddr.ip();
    port_=serveraddr.port();
}
void Socket::listen(int nn)
{
    if (::listen(fd_,nn)!=0) // 在高并发的网络服务器中，第二个参数要大一些
    {
        perror("listen() failed"); close(fd_); exit(-1);
    }
}
int Socket::accept(InetAdress& clientaddr)
{
    sockaddr_in peeraddr;
    socklen_t len = sizeof(peeraddr);
    clientaddr.setaddr(peeraddr);
    int clientfd = accept4(fd_,clientaddr.addr(),&len,SOCK_NONBLOCK); // 最后一个参数将socket设置为非阻塞

    return clientfd;
}
void Socket::setipport(const char* ip,uint16_t port)
{
    ip_=ip; port_=port;
}