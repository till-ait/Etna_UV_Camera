#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadSecureQueue {
public:
    ThreadSecureQueue() = default;
    // Désactive la copie
    ThreadSecureQueue(const ThreadSecureQueue&) = delete;
    ThreadSecureQueue& operator=(const ThreadSecureQueue&) = delete;

    ~ThreadSecureQueue(){
        stop();
    }


    void push(T new_value){
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(std::move(new_value));
        }
        cv_.notify_one();
    }


    T pop(){
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !queue_.empty() || stopped_; });

        if (stopped_ && queue_.empty())
                throw std::runtime_error("Queue stopped");

        T value = std::move(queue_.front());
        queue_.pop();
        return value;
    }


    void stop() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stopped_ = true;
        }
        cv_.notify_all();
    }

private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    bool stopped_ = false;
};