# include "Concurrency/ThreadPool.h"

ThreadPool::ThreadPool(int numThreads) : stopping(false) {
    workers.reserve(numThreads);

    for (int i = 0; i < numThreads; i++) {
        workers.emplace_back(i, queue);
        workers.back().start();
    }
}

void ThreadPool::enqueue(std::function<void()> task) {
    if (stopping) return;
    queue.push(task);
}

void ThreadPool::shutdown() {
    if (stopping) return;

    stopping = true;
    queue.shutdown();

    for (Worker& worker: workers) {
        worker.join();
    }
}