#ifndef JOBS_H
#define JOBS_H

#include <iostream>

class Job{};

class Fibbonacci : public Job {
private:
    int n;
    
public:
    Fibbonacci(int n) : n(n) {};
    void operator()() const;
};

#endif // JOBS_H