// Copyright 2026 Maria Kunigk-Bakalar
#include <unistd.h>
#include <iostream>
#include <vector>
#include <random>
#include <mutex>
#include <thread>
#include <queue>
#include <atomic>

// A few defines here:  The number of different job types
// and a scaling factor to speed up time.  When a sleep happens
// it will not be for the number of seconds specified, rather
// 0.002 times that amount
#define TYPES 10
#define SLEEPBASE 2000

// Some aliases to simplify the code
using workElement = std::pair<std::string, int>;
using workMap = std::vector<workElement>;
using std::cout, std::endl, std::ref;

// A namespace to hold the work queue
namespace pc {
    std::queue<workElement> queue;
    std::mutex mtx;
    const int MAXQUEUESIZE = 5;
    std::atomic<bool> allDone;
}

// Define the different types of work
void defineWorkTypes(workMap& workTypes) {
    // This is using simple names for the job types
    // The number after "Job-" is the time in seconds
    // the job will take
    workTypes.push_back({"Job-01", 1});
    workTypes.push_back({"Job-02", 2});
    workTypes.push_back({"Job-03", 3});
    workTypes.push_back({"Job-04", 4});
    workTypes.push_back({"Job-05", 5});
    workTypes.push_back({"Job-06", 6});
    workTypes.push_back({"Job-07", 7});
    workTypes.push_back({"Job-08", 8});
    workTypes.push_back({"Job-09", 9});
    workTypes.push_back({"Job-10", 10});
}

// consumer replaces the doWork function in the serial code.  It implements
// the consumer part of the producer/consumer busy-wait approach
void consumer(int &elapsedTime, std::mutex& workMutex) {
    // The consumer will work until the queue is empty AND allDone is true.
    // consumers don't know how much work they will get, so they have to be
    // prepared for an infinite amount
    while (true) {
        // elem is the work that will be read from the queue
        workElement elem;

        // As with the producer, OK is false until we have successfully
        // retrieved new work from the queue
        bool OK = false;

        // Implement busy-wait here.  This will be a loop
        // that continuously checks if the queue is empty.  If it is empty,
        // check whether the entire job is complete (via the allDone variable).
        // If the queue is empty and we are allDone, return.  If the queue is
        // empty, but allDone is false, just continue the while loop.
        // If the queue is not empty, retrieve a value from it and then
        // process it

        // While OK is false, check whether there's something in the queue
        // for me
        while (!OK) {
            std::lock_guard<std::mutex> lock(pc::mtx);
            // TO DO:  Check if the queue is empty.  If it is not, set OK
            // to TRUE and then retrieve a piece of work from the queue.
            // If the queue is empty, check if allDone is true.  If it is,
            // return to main.  If the queue is empty but allDone is false,
            // remain in the while loop.
            if (!pc::queue.empty()) {
                OK = true;
                elem = pc::queue.front();
                pc::queue.pop();
            } else {
                if (pc::allDone) {
                    return;
                }
            }
        }

        // If PROD is not defined, we display diagnostic information
#ifndef PROD
        cout << "Consumer processing " << elem.first << " from the queue.\n";
#endif
        // TO DO:  If we get here, we have retrieved a piece of work from the
        // queue.  This is where the replacement for doWork comes in.  Sleep
        // for the appropriate amount of time.  This is NOT part of the
        // critical section
        usleep(SLEEPBASE*elem.second);

        // TO DO:  After sleeping for the right amount of time, update the
        // shared variable elapsedTime.  This is a critical section.
        std::lock_guard<std::mutex> lock(workMutex);
        elapsedTime += elem.second;
    }
}

// Implement the producer.  Its job is to generate work and put it in a queue
// for consumers to process
void producer(size_t workToAssign) {
    // For ease of testing, we will initialize the RNG with a known seed
    std::mt19937 rng(87654321);

    // Normalization for the RNG so we can get numbers between 0 and 1
    double norm = 1.0/static_cast<double>(rng.max());

    // Define the work types:  Job name and duration
    workMap workTypes;
    defineWorkTypes(workTypes);

    // workType is the type of work we will randomly generate
    int workType = 0;

    // a workElement is a std::pair<std::string, int>.  The first element
    // is the name of the piece of work and the second is how long it
    // will take to complete (in seconds)
    workElement newWork;

    // This is false until all the work has been assigned to the queue
    pc::allDone = false;

    // Loop over all of the work to assign
    for (size_t i = 0; i < workToAssign; i++) {
        // Implement busy-wait here.  This will be a loop
        // that continuously checks if the queue is full.  If it is,
        // the loop continues.  When there is room in the queue, generate
        // a work item and then add it to the queue.

        // HINT:  You will not get the correct answer unless you are adding
        // the same work to the queue as the serial program does.  Check
        // that the work the producer is creating is the same as the serial
        // program by comparing the "producer" output below to the output
        // of the serial program.  The Job-XX entries should have the same
        // sequence.  Perhaps using grep would be useful
        bool OK = false;
        while (!OK) {
            // TO DO:  Check if the queue is full.  If it is not, set OK
            // to TRUE and then generate a random work element and add
            // it to the queue
            std::lock_guard<std::mutex> lock(pc::mtx);
            if (pc::queue.size() != pc::MAXQUEUESIZE) {
                OK = true;
                workType = TYPES*rng()*norm;
                newWork = workTypes[workType];
                pc::queue.push(newWork);
            }
        }
        // If PROD is not defined, we display diagnostic information
#ifndef PROD
        cout << "Producer added " << newWork.first << " to the queue.\n";
#endif
    }
    // TO DO:  When all of the work is assigned, notify the consumers via
    // the allDone variable
    pc::allDone = true;
}

int main(int argc, char* argv[]) {
    // Shared variable that holds the elapsed time for completing all jobs
    int elapsedTime = 0;

    // Because there is a shared variable, we need a shared mutex to protect it
    std::mutex workMutex;

    // How many different jobs will be in this run?
    const int workToAssign = 1000;

    // How many consumers will we have.  The number of threads will be
    // this + 1 (since there will be a single producer)
    const int consumers = 3;

    // Create a vector of threads to hold the producer and consumers
    std::vector<std::thread> threads;

    // Add the producer to the vector of threads
    threads.push_back(std::thread(producer, workToAssign));

    // Create a thread for each of the consumers
    for (size_t i = 0; i < consumers; i++) {
        threads.push_back(std::thread(consumer,
                          ref(elapsedTime),
                          ref(workMutex)));
    }

    // Wait for the producer and consumer threads to complete
    for (auto& t : threads) {
        t.join();
    }

    // Print out the elapsed time for all jobs
    cout << "Elapsed time = " << elapsedTime << endl;
    return 0;
}
