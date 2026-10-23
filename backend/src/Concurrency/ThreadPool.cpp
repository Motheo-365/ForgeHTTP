# include "Concurrency/ThreadPool.h"

#include <utility>

ThreadPool::ThreadPool(int numThreads) : stopping(false) {
    workers.reserve(numThreads);

    for (int i = 0; i < numThreads; i++) {
        workers.emplace_back(i, queue);
        workers.back().start();
    }
}

ThreadPool::~ThreadPool() {
    shutdown();
}

bool ThreadPool::enqueue(std::function<void()> task) {
    if (stopping) return false;
    return queue.push(std::move(task));
}

void ThreadPool::shutdown() {
    if (stopping.exchange(true)) return;

    queue.shutdown();

    for (Worker& worker: workers) {
        worker.join();
    }
}