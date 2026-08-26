#ifndef SCHEDULER_H
#define SCHEDULER_H

#define MAX_PRODUCERS 16
#define MAX_CONSUMERS 16
#define MAX_JOBS 128

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

class Scheduler {
public:
    std::chrono::high_resolution_clock::time_point start_time;

    // queue of jobs, needs to be accessed with a mutex to ensure thread safety
    // pointer for polymorphism, unique_ptr for safety
    std::unique_ptr<Job> queue[MAX_JOBS];
    int q_in, q_out, q_size;
    int produced_jobs, consumed_jobs;

    // producer and consumer threads
    std::thread producers[MAX_PRODUCERS];
    std::thread consumers[MAX_CONSUMERS];

    // mutex for thread safety
    std::mutex mut;
    // makes the queue a blocking queue
    std::counting_semaphore<MAX_JOBS> queueing{0}; // amount of queueing jobs
    std::counting_semaphore<MAX_JOBS> empty{MAX_JOBS}; // amount of empty jobs slots

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

void produce(Scheduler* scheduler);
void consume(Scheduler* scheduler);

#endif