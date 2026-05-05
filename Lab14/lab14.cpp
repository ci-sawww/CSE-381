// Copyright Maria Kunigk-Bakalar 2026

#include <future>
#include <mutex>
#include <string>
#include <vector>
#include <iostream>

// Function to swap "fuzzy duck" with "ducky fuzz".  It keeps track
// of the number of times the string was "fuzzy duck" and returns
// that value at the end
int fuzzy(std::string& current, int numIter, std::mutex& mtx) {
    // Initialize the count of times current was equal to "fuzzy duck"
    int fuzzyCount = 0;

    // Iterate numIter times
    for (int j = 0; j < numIter; j++) {
        mtx.lock();
        // If the current value of the string is "fuzzy duck", increment the
        // count and then swap the value to "ducky fuzz"
        if (current == "fuzzy duck") {
            fuzzyCount++;
            current = "ducky fuzz";
        // If the current value of the string is "ducky fuzz", swap the value
        // to "fuzzy duck"
        } else if (current == "ducky fuzz") {
            current = "fuzzy duck";
        // We should never get here, but in case something goes wrong, give an
        // error message and exit
        } else {
            mtx.unlock();
            std::cerr << "An error has occurred!\n";
            exit(0);
        }
        mtx.unlock();
    }
    // Return the count of the number of times the string was "fuzzy duck"
    return fuzzyCount;
}

int main(int argc, char* argv[]) {
    // Check that exactly one command line argument was provided
    if (argc != 2) {
        std::cerr << "Usage: lab12 count\n";
        return 0; }

    // int count = 0;
    std::string arg = argv[1];

    bool ok = true;
    for (char c : arg) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            ok = false;
            break;}}

    if (!ok) {
        std::cerr << "Error: arguments must be integers\n";
        return 0;}

    int count = std::stoi(arg);

    // Number of times to iterate the runs of fuzzy.  In the multithreaded
    // version, this will be the number of asynchronous threads to run
    // int count = std::stoi(argv[1]);
    int numThreads = count;

    // Initialize the current value of the string to be "fuzzy duck"
    std::string current = "fuzzy duck";

    // Keep track of the number of times "fuzzy duck" was observed
    int totalFuzzy = 0;

    std::mutex mtx;

    // Run the fuzzy function count times, incrementing totalFuzzy with the
    // count of times "fuzzy duck" was observed
    std::vector<std::future<int>> futures;
    for (int j = 0; j < numThreads; j++) {
        futures.push_back(std::async(std::launch::async, fuzzy,
            std::ref(current), 117 + 10 * j, std::ref(mtx))); }

    for (auto& f : futures) {
        totalFuzzy += f.get(); }

    // Write out the total times that "fuzzy duck" was observed
    std::cout << "totalFuzzy = " << totalFuzzy << std::endl;
}
