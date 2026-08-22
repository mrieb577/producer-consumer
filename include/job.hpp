#ifndef JOBS_H
#define JOBS_H

#include <iostream>
#include <unistd.h>

// virtual class for Job so polymorphism
class Job {
public:
    virtual void operator()() const = 0;
    virtual ~Job() = default;
};

// functor
class Fibbonacci : public Job {
private:
    int n;
    
public:
    Fibbonacci(int n) : n(n) {};
    void operator()() const;
};

class RandWait : public Job {
private:
    const long max_rand = 1000000L;
    double upper = 5, lower = 0;

public:
    RandWait() {};
    void operator()() const;
};

#endif // JOBS_H