#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <iostream>
#include <functional>
#include <vector>
#include <thread>

#include "jobs.hpp"

class Scheduler;

void produce(Scheduler* scheduler);

template <typename T>
void consume(Scheduler* scheduler, T* job);

class Scheduler {
private:
    // mutex for thread safety
    std::mutex mut;

    // available threads circular buffer
    std::thread threads[8];
    long t_in, t_out;

    // queue of jobs
    std::vector<Job> queue;

    // producer and consumer threads
    std::thread producer;
    std::thread consumer;
public:
    Scheduler();
    ~Scheduler();


};

#endif