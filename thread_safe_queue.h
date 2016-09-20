#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>
namespace cppevent{

template<class T>
class ThreadSafeQueue
{
    typedef T value_type;
    typedef std::shared_ptr<value_type> value_ptr;

public:
    ThreadSafeQueue():queue_(), mutex_(), cond_(){}

    void push(value_type new_value){
        std::shared_ptr<T> data_ptr(std::make_shared<T>(std::move(new_value)));
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(data_ptr);
        cond_.notify_one();
    }

    void pop(){
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this]{return !queue_.empty();});
        queue_.pop();
    }

    //我这里写错了,不应该把front和pop分开.以后再改
    std::shared_ptr<T> front() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this]{return !queue_.empty();});
        return queue_.front();
    }

    bool empty() const{
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

private:
    std::queue<value_ptr> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cond_;
};

}
#endif // THREADSAFEQUEUE_H
