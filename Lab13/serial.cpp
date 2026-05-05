// Copyright 2026 Alan M. Ferrenberg
#include <unistd.h>
#include <iostream>
#include <vector>
#include <random>

// A few defines here:  The number of different job types
// and a scaling factor to speed up time.  When a sleep happens
// it will not be for the number of seconds specified, rather
// 0.002 times that amount
#define TYPES 10
#define SLEEPBASE 2000

// Some aliases to simplify the code
using workElement = std::pair<std::string, int>;
using workVec = std::vector<workElement>;
using std::cout, std::endl;

// Define the different types of work
void defineWorkTypes(workVec& workTypes) {
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

// Do the "work".  For this, the work is just sleeping for a specified
// amount of time
void doWork(workElement& elem, int& elapsedTime) {
    // If PROD is defined, we skip the output
    #ifndef PROD
    cout << "Working on job " << elem.first << endl;
#endif
    // This is the actual "work"
    usleep(SLEEPBASE*elem.second);

    // Now that we're done, increment the elapsedTime variable
    elapsedTime += elem.second;
}

int main(int argc, char* argv[]) {
    // Create the different types of jobs to work on.
    // workVec is a vector of type workElement which is
    // std::pair{std::string, int}
    workVec workTypes;

    // How many different jobs will be in this run?
    const int workToAssign = 1000;

    // Define the work types:  Job name and duration
    defineWorkTypes(workTypes);

    // This is the total time that all of the jobs take
    int elapsedTime = 0;

    // Initialize the random number generator (RNG)
    std::mt19937 rng(87654321);
    // This is the normalization for the RNG so we can
    // generate numbers between 0 and 1
    double norm = 1.0/static_cast<double>(rng.max());

    // Loop over the work to be done
    for (size_t i = 0; i < workToAssign; i++) {
        // Select a work type at random.  It will be an int between
        // 0 and TYPES-1
        int workType = TYPES*rng()*norm;

        // myWork is the randomly selected work element
        workElement myWork = workTypes[workType];

        // Do the work.  This consists of sleeping for the correct period
        // of time and then incrementing elapsedTime
        doWork(myWork, elapsedTime);
    }

    // Print out the elapsed time for all jobs
    cout << "Elapsed time = " << elapsedTime << endl;
    return 0;
}
