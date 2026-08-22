#include "job.hpp"

void Fibbonacci::operator()() const {
    long a = 0, b = 1, c;
    std::cout << "Fibonacci number " << n << ": ";
    for (int i = 0; i < n; ++i) {
        c = a + b;
        a = b;
        b = c;
    }
    std::cout << a << std::endl;
}

void RandWait::operator()() const {
    double len = lower + (upper - lower) * (random() % max_rand) / max_rand;
    std::cout << "Sleeping for " << len << "ms" << std::endl;
    sleep(len);
}
