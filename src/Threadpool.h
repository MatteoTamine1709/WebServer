#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>

class Threadpool {
   public:
    Threadpool() = default;
    Threadpool(const Threadpool&) = delete;
    Threadpool(Threadpool&&) = delete;
    Threadpool& operator=(const Threadpool&) = delete;
    Threadpool& operator=(Threadpool&&) = delete;
    ~Threadpool() = default;

    void start();
    void stop();
    void addTask(std::function<void()> task);
    void setThreadCount(size_t threadCount);
    size_t getThreadCount() const;
    size_t getTaskCount();
    size_t getQueueSize();
    bool isRunning() const;

   private:
    void runInThread();
    std::function<void()> take();

    std::vector<std::thread> m_threads;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::queue<std::function<void()>> m_tasks;
    size_t m_threadCount = 0;
    bool m_running = false;
};

#endif