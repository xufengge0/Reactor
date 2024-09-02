/* 
    性能测试：QPS每秒8万（本地虚拟机）
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>    // TCP NODELAY需要包含这个头文件。
#include"InetAddress.h"
#include"Socket.h"
#include"Epoll.h"
#include"Channel.h"
#include"EventLoop.h"
#include "TcpServer.h"
#include"EchoServer.h"
#include<signal.h>

EchoServer *echoserver;
// 信号处理函数
void Stop(int sig)
{
    printf("sig=%d\n",sig);

    // 停止服务
    echoserver->Stop();

    printf("服务已退出\n");
    delete echoserver;
    printf("delete echoserver\n");
    exit(0);
}
int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("usage: ./tcpepoll ip port\n");
        printf("example: ./tcpepoll 192.168.247.128 5085\n\n");
        return -1;
    }
    // 设置信号2和15的信号处理函数
    signal(SIGINT,Stop);signal(SIGTERM,Stop);

    echoserver=new EchoServer(argv[1],argv[2],3,0);

    echoserver->Start();
    
    return 0;
}