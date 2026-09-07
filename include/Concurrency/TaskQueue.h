#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <function>
// Fixed-size worker pool pulling tasks off a thread-safe queue, instead of spawning a thread per connection.
//
// Thread-safe queue of pending work, guarded by a mutex and condition variable.
class TaskQueue {
    public:
        /*
            Locks the internal mutex, appends the task, then notifies one waiting worker via the condition variable.
        */
        void push(std::function task);

        /*
            Blocks on the condition variable until a task is available (or the pool is shutting down), then removes and returns the front desk.
        */
        function<void()> pop();

        /*
            Returns whether the queue currently holds no pending tasks. Mainly useful for tests/metrics, not for the pop loop itself (which should block, not poll).
        */
        bool empty();

    private:
        deque<function<void()>> tasks;
        mutex mtx;
        condition_variable cv;
};

#endif