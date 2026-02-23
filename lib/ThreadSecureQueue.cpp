#include "ThreadSecureQueue.h"

ThreadSafeQueue::~ThreadSafeQueue(){
    stop();
}


ThreadSecureQueue::void push(T new_value){
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(new_value));
    }
    cv_.notify_one();
}


ThreadSecureQueue::T pop(){
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return !queue_.empty() || stopped_; });

    if (stopped_ && queue_.empty())
            throw std::runtime_error("Queue stopped");

    T value = std::move(queue_.front());
    queue_.pop();
    return value;
}


ThreadSafeQueue::void stop() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stopped_ = true;
    }
    cv_.notify_all();
}