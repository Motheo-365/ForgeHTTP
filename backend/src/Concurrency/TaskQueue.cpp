#include "Concurrency/TaskQueue.h"

#include <utility>

TaskQueue::TaskQueue(std::size_t capacity)
    : capacity(capacity) {}

bool TaskQueue::push(std::function<void()> task) {
    std::unique_lock<std::mutex> lock(mtx);
    if (stopping || tasks.size() >= capacity) {
        return false;
    }
    tasks.push_back(std::move(task));
    cv.notify_one();
    return true;
}

std::function<void()> TaskQueue::pop()  {
    std::unique_lock<std::mutex> lock(mtx); // Acquire lock
    
    // Wait until queue is not empty
    cv.wait(lock,
        [this]() { return !tasks.empty() || stopping; }
    );

    if (tasks.empty()) return nullptr;

    // Retrieve item
    std::function<void()> task = tasks.front();
    tasks.pop_front();

    return task;
}

bool TaskQueue::empty() {
    std::unique_lock<std::mutex> lock(mtx);
    return tasks.empty();
}

void TaskQueue::shutdown() {
    std::unique_lock<std::mutex> lock(mtx);
    stopping = true;
    cv.notify_all();
}