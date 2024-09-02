#pragma once
#include <string>
#include<iostream>
#include<cstring>

class Buffer
{
private:
    std::string buf_;
    const uint16_t sep_;// 报文的分隔符:0-无分隔符(固定长度、视频会议);1-四字节的报头;2-"\r\n\rn"分隔符(http协议)
public:
    Buffer(uint16_t sep=1);
    ~Buffer();

    void append(const char* data,size_t size);          // 把数据追加到buf中
    void appendwithsep(const char* data,size_t size);   // 把数据按分隔符追加到buf中
    size_t size();                                      // 返回buf 的大小
    const char* data();                                 // 返回buf 的首地址
    void clear();                                       // 清空buf
    void erase(int pos,int len);                        // 清空前len的长度
    bool pickmessage(std::string &ss);                  // 从buf 中拆分出一个报文，存放在ss中，如果buf 中没有报文，返回false。
};


