#include "Concurrency/Worker.h"

Worker::Worker(int id, TaskQueue& q) : id(id), q(q) {}

void Worker::run() {
    // Now worker never stops. Later make it stop when its told to.
    while (true) {
        auto task = q.pop();
        if (!task) break;
        task();
    }
}

void Worker::start() {
    thread = std::thread(&Worker::run, this);
}

void Worker::join() {
    if (thread.joinable()) thread.join();
}