# 基于reactor的高并发服务器

主要工作：

    1、从0开始以epoll原型为基础，通过增加socket类、channel类、eventloop类、acceptor类、connection类、tcpserver类、buffer类和回调函数实现单线程reactor模型；
    2、引入线程池，利用多核资源，实现one loop per thread的主从事件循环模型（非阻塞IO线程）
    3、根据业务的要求可以再添加工作线程专门用于业务处理、Eventfd用于事件通知、添加定时器清理空闲TCP连接，性能测试QPS可达到10w/秒（本地虚拟机）。

项目难点：

    share_ptr管理connection资源、回调函数的逻辑处理、线程间race condition、buffer设计。
