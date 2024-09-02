#include"TheadPool.h"
TheadPool::TheadPool(size_t threadnum,const std::string& theadtype):stop_(false),theadtype_(theadtype)
{
    for(int i=1;i<=threadnum;++i)
    {
        threads_.emplace_back([this]{

            printf("creat %s thread(%d)\n",theadtype_.c_str(),syscall(SYS_gettid));   // 获取线程id

            while (stop_==false)
            {
                std::function<void()> task;     // 存放出队元素

                {   // 锁的作用域开始
                    std::unique_lock<std::mutex> lock(this->mutex_);

                    // 等待生产者唤醒信号
                    this->condition_.wait(lock,[this]{
                        // 带谓词的wait函数常用于避免虚假唤醒。只有被唤醒且当谓词返回true时，wait函数才会结束等待。
                        return (this->stop_==true || !this->taskqueue_.empty());
                    });

                    // 队列为空且需要退出，直接返回
                    if(this->stop_==true && this->taskqueue_.empty()) return;

                    // 出队一个元素
                    task=std::move(this->taskqueue_.front());
                    this->taskqueue_.pop();
                }   // 锁的作用域结束

                printf("%s thread is(%d)\n",theadtype_.c_str(),syscall(SYS_gettid));   // 获取线程id
                task();     // 执行任务
            }
            
        });
    }
}

TheadPool::~TheadPool()
{
    stop_=true;
    condition_.notify_all();    // 唤醒所有线程

    for(auto &i:threads_)
        i.join();
}
void TheadPool::addtask(std::function<void()> task)
{
    {   // 锁的作用域开始
        std::unique_lock<std::mutex> lock(mutex_);
        taskqueue_.push(task);
        //printf("已添加\n");
    }   // 锁的作用域结束

    condition_.notify_one();    // 唤醒一个线程
}
int TheadPool::size()
{
    return threads_.size();
}
void TheadPool::stop()
{
    if(stop_==true) return;     // 判断是否已经退出
    stop_=true;
    condition_.notify_all();    // 唤醒所有线程

    for(auto &i:threads_)
        i.join();

}
// 线程池的测试代码
/* class AA
{
public:
    void show(int no, const std::string &name){
        printf("小哥哥们好，我是第%d号超级女生%s。\n",no,name.c_str());
        sleep(1);   // 模拟任务耗时
    }
    ~AA(){printf("调用了析构函数\n");}
};

void test(){
    printf("我有一只小小鸟。\n");
    sleep(1);
}
void func(std::shared_ptr<AA> aa)
{
    sleep(5);
    aa->show(01,"珍珍");
} */
/* int main(){
    TheadPool threadpool(3,"TEST");sleep(1);
    AA aa;
    
    std::string name="西施";
    threadpool.addtask(std::bind(&AA::show,aa,8,name));
    threadpool.addtask(std::bind(test));
    threadpool.addtask(std::bind([]{ printf("我是一只傻傻鸟。\n"); sleep(1);}));
    // 下面三行代码执行不到是因为先析构了，sleep(2);可以解决
    threadpool.addtask(std::bind([]{ printf("我是1。\n"); sleep(1);}));
    threadpool.addtask(std::bind([]{ printf("我是2。\n"); sleep(1);}));
    threadpool.addtask(std::bind([]{ printf("我是3。\n"); sleep(1);}));
    sleep(2);
    return 0;
} */
/* int main(){
    TheadPool threadpool(3,"TEST");sleep(1);
    {
        std::shared_ptr<AA> aa(new AA);
        threadpool.addtask(std::bind(func,aa));
        //delete aa;
    }
    sleep(10);
} */
// g++ -g -o demo TheadPool.cpp -lpthread