#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <function>

// Thread-safe wqueue of pending work, guarded by a mutex and condition variable.
class TaskQueue {
    public:
        void push(std::function task);
        function<void()> pop();
        bool empty();

    private:
        deque<function<void()>> tasks;
        mutex mtx;
        condition_variable cv;
};

#endif