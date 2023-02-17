#include <queue>
#include <mutex>

template<typename T>
struct concurrent_queue {
public:

    void push(T const& x) {
        std::unique_lock<std::mutex> l(lock);
        q.push(x);
        l.unlock();
    }

    bool empty() {
        std::lock_guard<std::mutex> l(lock);
        return q.empty();
    }

    bool try_pop(T& result) {
        std::unique_lock<std::mutex> l(lock);
        if (q.empty()) {
            return false;
        }
        result = q.front();
        q.pop();
        return true;
    }
private:
    std::queue<T> q;
    std::mutex lock;
};
