#include "../Threadpool.h"

#include <spdlog/spdlog.h>

void Threadpool::start() {
    m_running = true;
    spdlog::debug("Starting {} threads", m_threadCount);
    for (size_t i = 0; i < m_threadCount; ++i)
        m_threads.emplace_back(&Threadpool::runInThread, this);
}

void Threadpool::stop() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_running = false;
        m_condition.notify_all();
    }
    for (auto& thread : m_threads) thread.join();
}

void Threadpool::addTask(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_tasks.emplace(std::move(task));
    }
    m_condition.notify_one();
}

void Threadpool::setThreadCount(size_t threadCount) {
    m_threadCount = threadCount;
}

size_t Threadpool::getThreadCount() const { return m_threadCount; }

size_t Threadpool::getTaskCount() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_tasks.size();
}

size_t Threadpool::getQueueSize() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_tasks.size();
}

bool Threadpool::isRunning() const { return m_running; }

void Threadpool::runInThread() {
    while (m_running) {
        std::function<void()> task = take();
        if (task) task();
    }
}

std::function<void()> Threadpool::take() {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_condition.wait(lock, [this] { return !m_running || !m_tasks.empty(); });
    std::function<void()> task;
    if (!m_tasks.empty()) {
        task = std::move(m_tasks.front());
        m_tasks.pop();
    }
    return task;
}