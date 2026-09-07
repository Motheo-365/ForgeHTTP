#ifndef WORKER_H
#define WORKER_H

#include "TaskQueue.h"
#include <thread>

// A single thread that repeatedly pulls from the share TaskQueue and executes tasks.
class Worker {
    public:
        /*
            Stores its id (for logging/debugging) and a reference to the shared queue. Does not start the thread itself — ThreadPool owns that.
        */
        Worker(int id, TaskQueue& q);

        /*
            The thread's main loop: repeatedly calls queue.pop() and executes the returned task, until told to stop.
        */
        void run();
        void start();
        void join();

    private:
        int id;
        std::thread thread;
        TaskQueue& q;
};

#endif