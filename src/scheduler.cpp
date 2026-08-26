#include "scheduler.hpp"

void produce(Scheduler* scheduler) {
    for(int i = 0; i < 100; i++){
        auto job = std::make_unique<RandWait>();
        scheduler->empty.acquire(); // remove one from the empty job slot count

        // puts scope control on the lock, will unlock when destroyed at end of iteration
        std::lock_guard<std::mutex> lock(scheduler->mut);

        scheduler->pushJob(std::move(job));
        scheduler->produced_jobs++;
        scheduler->queueing.release(); // add one to the count of queueing jobs
        //std::cout << "Current jobs in queue: " << scheduler->q_size << std::endl;
    }
}

void consume(Scheduler* scheduler) {
    while(scheduler->consumed_jobs < scheduler->produced_jobs){
        scheduler->queueing.acquire(); // remove one from the count of queueing jobs

        // use a unique lock for scope control, but allows early unlocking
        std::unique_lock<std::mutex> lock(scheduler->mut);

        std::unique_ptr<Job> job = scheduler->popJob();
        scheduler->consumed_jobs++;
        scheduler->empty.release(); // add one to the list of empty job slots
        // make sure the task is run outside of the mutex so it can be freed
        lock.unlock();

        (*job)(); // run the job
    }
}

Scheduler::Scheduler() : q_in(0), q_out(0), q_size(0), produced_jobs(0), consumed_jobs(0) {}
Scheduler::~Scheduler() {}

void Scheduler::start() {
    start_time = std::chrono::high_resolution_clock::now();

    for(size_t i = 0; i < MAX_PRODUCERS; i++){
        producers[i] = std::thread(produce, this);
        std::cout << "Started producer thread " << producers[i].get_id() << std::endl;
    }

    for(size_t i = 0; i < MAX_CONSUMERS; i++){
        consumers[i] = std::thread(consume, this);
        std::cout << "Started consumer thread " << consumers[i].get_id() << std::endl;
    }
}

void Scheduler::stop() {
    // wait for the threads to finish
    for(size_t i = 0; i < MAX_PRODUCERS; i++){
        producers[i].join();
    }

    for(size_t i = 0; i < MAX_CONSUMERS; i++){
        consumers[i].join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> dur = end_time - start_time;
    std::cout << "Completed " << consumed_jobs << " jobs in " << dur.count() << "ms" << std::endl;
}

void Scheduler::pushJob(std::unique_ptr<Job> job){
    queue[q_in] = std::move(job);
    q_in = (q_in + 1) % MAX_JOBS;
    q_size++;
}

std::unique_ptr<Job> Scheduler::popJob(){
    std::unique_ptr<Job> job = std::move(queue[q_out]);
    q_out = (q_out + 1) % MAX_JOBS;
    q_size--;
    return job;
}

bool Scheduler::jobQueueEmpty() const {
    return q_in == q_out;
}

bool Scheduler::jobQueueFull() const {
    return (q_in + 1) % MAX_JOBS == q_out;
}
