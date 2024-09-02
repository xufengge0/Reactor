#include"InetAddress.h"

InetAdress::InetAdress(const char* ip,const char* port)
{
    addr_.sin_family = AF_INET;             // IPv4网络协议的套接字类型
    addr_.sin_addr.s_addr = inet_addr(ip);  // 服务端用于监听的ip地址
    addr_.sin_port = htons(atoi(port));
}
InetAdress::InetAdress(sockaddr_in addr):addr_(addr)
{

}
InetAdress::~InetAdress()
{

}
const char* InetAdress::ip() const
{
    return inet_ntoa(addr_.sin_addr);
}
uint16_t InetAdress::port() const
{
    return ntohs(addr_.sin_port);
}
sockaddr* InetAdress::addr() const
{
    return (sockaddr*)&addr_;
}
void InetAdress::setaddr(sockaddr_in peeraddr)
{
    addr_=peeraddr;
}