#include"Buffer.h"


Buffer::Buffer(uint16_t sep):sep_(sep)
{
}

Buffer::~Buffer()
{
}
void Buffer::append(const char* data,size_t size)
{
    buf_.append(data,size);
}
void Buffer::appendwithsep(const char* data,size_t size)
{
    if(sep_==0)
    {
        buf_.append(data,size);
    }
    else if(sep_==1)
    {
        buf_.append((char*)&size,4);
        buf_.append(data,size);
    }
    else if(sep_==2)
    {
        
        buf_.append(data,size);
        
    }
}
size_t Buffer::size()
{
    return buf_.size();
}
const char* Buffer::data()
{
    return buf_.data();
}
void Buffer::clear()
{
    buf_.clear();
}
void Buffer::erase(int pos,int len)
{
    buf_.erase(pos,len);
}
bool Buffer::pickmessage(std::string &ss)
{
    if(buf_.size()==0) return false;

    if(sep_==0)
    {
        ss=buf_;
        buf_.clear();
    }
    else if(sep_==1)
    {
        int len;
        memcpy(&len,buf_.data(),4);
        if(buf_.size()<len+4) return false;

        ss=buf_.substr(4,len);
        buf_.erase(0,len+4);
    }
    else if(sep_==2)
    {
        
    }
    return true;
}

/* int main(){
    std::string s1="aaaaaaaaaaaaab";
    Buffer buf(1);
    buf.appendwithsep(s1.data(),s1.size());
    std::string s2;
    buf.pickmessage(s2);
    printf("s2=%s\n",s2.c_str());
} */

//g++ -g -o test Buffer.cpp