#ifndef BLOCKING_QUEUE_H
#define BLOCKING_QUEUE_H

#include <mutex>
#include <condition_variable>
#include <queue>

namespace logging {

template<class T>
class BlockingQueue {
public:
    BlockingQueue() : queue_(), mutex_(), cond_() {}

    void push(T new_value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(new_value));
        cond_.notify_one();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this]{ return !queue_.empty(); });
        T value = std::move(queue_.front());
        queue_.pop();
        return value;
    }

private:
    BlockingQueue(const BlockingQueue& other) = delete;
    BlockingQueue& operator=(const BlockingQueue& other) = delete;
    BlockingQueue(BlockingQueue&& other) = delete;


private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cond_;
};



} //namespace

#endif // BLOCKING_QUEUE_H

