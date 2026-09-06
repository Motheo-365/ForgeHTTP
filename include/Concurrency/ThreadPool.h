#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <function>

// owns a fixed set of Worker threads and the shared TaskQueue they pull from.
class ThreadPool {
    public:
        ThreadPool(int numThreads);
        void enqueue(std::function<void()> task);
        void shutdown();

    private:
        vector<Worker> workers;
        TaskQueue queue;
        atomic<bool> stopping;
};

#endif