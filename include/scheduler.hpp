#ifndef SCHEDULER_H
#define SCHEDULER_H

#define MAX_PRODUCERS 2
#define MAX_CONSUMERS 8
#define ALL_THREADS (MAX_PRODUCERS + MAX_CONSUMERS)
#define MAX_JOBS 32

#include <iostream>
#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <semaphore>
#include <memory>
#include <unistd.h>
#include <cstdlib>
#include <chrono>

#include "job.hpp"

class Scheduler;

void produce(Scheduler* scheduler);

template <typename T>
void consume(Scheduler* scheduler);

class Scheduler {
public:
    std::chrono::_V2::system_clock::time_point start_time;

    // queue of jobs, needs to be accessed with a mutex to ensure thread safety
    std::unique_ptr<Job> queue[MAX_JOBS];
    long q_in, q_out, jobs;

    // producer and consumer threads
    std::thread producers[MAX_PRODUCERS];
    std::thread consumers[MAX_CONSUMERS];

    // mutex for thread safety
    std::mutex mut;
    std::counting_semaphore<ALL_THREADS> queueing{0};
    std::counting_semaphore<ALL_THREADS> empty{ALL_THREADS};

    Scheduler();
    ~Scheduler();

    // start the scheduler, launching producer and consumer threads
    void start();
    // stop the scheduler, joining producer and consumer threads
    void stop();

    // job queue operations, need to be in mutex because 
    // there could be race conditions on the in and out variables
    void pushJob(std::unique_ptr<Job> job);
    std::unique_ptr<Job> popJob();
    bool jobQueueEmpty() const;
    bool jobQueueFull() const;
};

#endif