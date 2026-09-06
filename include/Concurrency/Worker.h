#ifndef WORKER_H
#define WORKER_H

#include "TaskQueue.h"

// A single thread that repeatedly pulls from the share TaskQueue and executes tasks.
class Worker {
    public:
        Worker(int id, TaskQueue& q);
        void run();

    private:
        int id;
        std::thread thread;
};

#endif