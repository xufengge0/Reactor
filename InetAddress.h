#ifndef INETADDRESS_H
#define INETADDRESS_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

class InetAdress
{
private:
    sockaddr_in addr_;

public:
    InetAdress(){};
    InetAdress(const char* ip,const char* port);
    InetAdress(sockaddr_in addr);
    ~InetAdress();

    const char* ip() const;
    uint16_t port() const;
    sockaddr* addr() const;
    void setaddr(sockaddr_in peeraddr);

};



#endif