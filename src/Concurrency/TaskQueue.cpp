#include "Concurrency/TaskQueue.h"

#include <iostream>
#include <queue>

void TaskQueue::push(std::function<void()> task) {
    std::unique_lock<std::mutex> lock(mtx);
    tasks.push_back(task);
    cv.notify_one();
}

std::function<void()> TaskQueue::pop()  {
    std::unique_lock<std::mutex> lock(mtx); // Acquire lock
    
    // Wait until queue is not empty
    cv.wait(lock,
        [this]() { return !tasks.empty(); }
    );

    // Retrieve item
    std::function<void()> task = tasks.front();
    tasks.pop_front();

    return task;
}

bool TaskQueue::empty() {
    std::unique_lock<std::mutex> lock(mtx);
    return tasks.empty();
}