// 网络通讯的客户端程序。
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage:./client ip port\n");
        printf("example:./client 192.168.247.128 5085\n\n");
        return -1;
    }
    
    int sockfd;
    struct sockaddr_in servaddr;
    char buf[1024];

    if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0) { printf("socket( failed.\n"); return -1; }

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(atoi(argv[2]));
    servaddr.sin_addr.s_addr=inet_addr(argv[1]);

    if (connect(sockfd, (struct sockaddr *)&servaddr,sizeof(servaddr))!= 0)
    {
        printf("connect(%s:%s) failed\n",argv[1],argv[2]); close(sockfd); return -1;
    }
    else printf("connect ok.\n");

    printf("开始时间:%d\n",time(0));

    for (int i = 0; i < 1000000; i++)
    {
        memset(buf,0,sizeof(buf));

        char tmp[1024];
        memset(tmp,0,sizeof(tmp));
        sprintf(tmp,"第%d个报文\n",i);

        int len=strlen(tmp);
        memcpy(buf,&len,4);
        memcpy(buf+4,tmp,len);
        
        
        if(send(sockfd,buf,len+4,0)<=0)
        {
            printf("write failed\n"); close(sockfd); return -1;
        }
        //printf("发送:%s\n",buf);
    
    
        recv(sockfd,&len,4,0);
        memset(buf,0,sizeof(buf));
        recv(sockfd,buf,len,0);
        //printf("recv:%s\n",buf);
    }
    printf("结束时间:%d\n",time(0));

    //
    /* for (int i = 0; i < 5; i++)
    {
        memset(buf,0,sizeof(buf));

        char tmp[1024];
        memset(tmp,0,sizeof(tmp));
        sprintf(tmp,"第%d个报文\n",i);

        memcpy(buf,tmp,sizeof tmp);
        
        
        if(send(sockfd,buf,sizeof buf,0)<=0)
        {
            printf("write failed\n"); close(sockfd); return -1;
        }
        printf("发送第%d个报文\n",i);
    
        memset(buf,0,sizeof(buf));
        recv(sockfd,buf,sizeof(buf),0);
        printf("recv:%s\n",buf);

        sleep(1);
    } */
}
