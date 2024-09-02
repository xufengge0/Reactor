#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <mutex>
#include <unistd.h>
#include <thread>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>
#include <sys/syscall.h>

class TheadPool
{
private:
    std::vector<std::thread> threads_;              // 线程池中的线程
    std::queue<std::function<void()>> taskqueue_;   // 任务队列。
    std::mutex mutex_;                              // 任务队列同步的互斥锁。
    std::condition_variable condition_;             // 任务队列同步的条件变量:
    std::atomic_bool stop_;                         // 在析构函数中，把stop的值设置为true，全部的线程将退出。
    
public:
    std::string theadtype_;                         // 线程类型：IO、WORK
    
    //在构造函数中将启动threadnum个线程
    TheadPool(size_t threadnum,const std::string& theadtype);

    // 在析构函数中将停止线程。
    ~TheadPool();

    //把任务添加到队列中
    void addtask(std::function<void()> task);

    // 获取线程数量
    int size();

    // 结束线程池
    void stop();
};

