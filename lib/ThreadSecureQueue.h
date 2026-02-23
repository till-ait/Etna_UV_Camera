#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadSecureQueue {
public:
    ThreadSecureQueue() = default;
    // Désactive la copie
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    ~ThreadSafeQueue();

    void push(T new_element);
    T pop();
    void stop();

private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    condition_variable cv_;
    bool stopped_ = false;
};