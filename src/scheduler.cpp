#include "scheduler.hpp"

void produce(Scheduler* scheduler) {
    for(int i = 0; i < 100;){
        auto job = std::make_unique<RandWait>();
        scheduler->empty.acquire();

        // puts scope control on the lock, will unlock when destroyed at end of iteration
        std::lock_guard<std::mutex> lock(scheduler->mut);

        scheduler->pushJob(std::move(job));
        scheduler->queueing.release();
        
        i++; // only advance the loop if an action was completed
    }
}

void consume(Scheduler* scheduler) {
    for(int i = 0; i < 100;){
        scheduler->queueing.acquire();

        // puts scope control on the lock, will unlock when destroyed at end of iteration
        std::lock_guard<std::mutex> lock(scheduler->mut);

        std::unique_ptr<Job> job = scheduler->popJob();
        scheduler->jobs++;
        scheduler->empty.release();

        (*job)(); // run the job
        i++; // only advance the loop if an action was completed
    }
}

Scheduler::Scheduler() : q_in(0), q_out(0), jobs(0) {}
Scheduler::~Scheduler() {}

void Scheduler::start() {
    start_time = std::chrono::high_resolution_clock::now();

    for(size_t i = 0; i < MAX_PRODUCERS; i++){
        producers[i] = std::thread(produce, this);
    }

    for(size_t i = 0; i < MAX_PRODUCERS; i++){
        consumers[i] = std::thread(consume, this);
    }
}

void Scheduler::stop() {
    // wait for the threads to finish
    for(size_t i = 0; i < MAX_PRODUCERS; i++){
        producers[i].join();
    }

    for(size_t i = 0; i < MAX_PRODUCERS; i++){
        consumers[i].join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> dur = end_time - start_time;
    std::cout << "Completed " << jobs << " jobs in " << dur.count() << "ms" << std::endl;
}

void Scheduler::pushJob(std::unique_ptr<Job> job){
    queue[q_in] = std::move(job);
    q_in = (q_in + 1) % MAX_JOBS;
}

std::unique_ptr<Job> Scheduler::popJob(){
    std::unique_ptr<Job> job = std::move(queue[q_out]);
    q_out = (q_out + 1) % MAX_JOBS;
    return job;
}

bool Scheduler::jobQueueEmpty() const {
    return q_in == q_out;
}

bool Scheduler::jobQueueFull() const {
    return (q_in + 1) % MAX_JOBS == q_out;
}
