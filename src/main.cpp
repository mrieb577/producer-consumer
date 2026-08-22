#include <iostream>

#include "scheduler.hpp"


int main(int argc, char* argv[]) {
    std::cout << "Attempting to run the scheduler!" << std::endl;
    Scheduler s = Scheduler();
    s.start();
    s.stop();
    return 0;
}