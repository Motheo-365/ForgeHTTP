#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "Worker.h"
#include "TaskQueue.h"

#include <atomic>
#include <functional>
#include <vector>

// owns a fixed set of Worker threads and the shared TaskQueue they pull from.
class ThreadPool {
    public:
        /*
            Constrcucts the TaskQueue, then constructs numThreads Worker objects, each backed by a real std::thread running WOrker::run();=.
        */
        ThreadPool(int numThreads);

        /*
            Pushes a unit of work (typically "handle this one connection") onto the shared TaskQueue for the next available worker to pick up.
        */
        void enqueue(std::function<void()> task);
        
        /*
            Sets the stopping flag, wakes every worker via the condition variable, and joins all worker threads so the process can exit cleanly with no leaked threads.
        */
        void shutdown();

    private:
        std::vector<Worker> workers;
        TaskQueue queue;
        std::atomic<bool> stopping;
};

#endif