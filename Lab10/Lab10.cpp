// Copyright 2026 Maria Kunigk-Bakalar, Kolbey Lewis, Leah Johnston
#include <iostream>
#include <vector>
#include <thread>
#include <map>
#include <iomanip>

#define NUMTHREADS 5

using std::string, std::vector, std::map;
using std::thread, std::ref;
using std::cout, std::cerr, std::endl, std::setw, std::left;

void workerValue(int myArg) {
    std::cout << "workerValue myArg = " << myArg << std::endl;
}

void workerReference(int& myArg) {
    myArg++;
    std::cout << "workerReference myArg = " << myArg << std::endl;
}

void function1() {
    vector<thread> workers;
    for (int i = 0; i < NUMTHREADS; i++) {
        workers.push_back(thread(workerValue, i));
    }
    for (auto& t : workers) {
    t.join();
    }
}

void function2() {
    vector<thread> workers;
    vector<int> values(NUMTHREADS);
    for (int i = 0; i < NUMTHREADS; i++) {
        values[i] = i;
        workers.push_back(thread(workerReference, ref(values[i])));
    }

    for (auto& t : workers) {
        t.join();
    }
}

void function3() {
    vector<thread> workers;
    const int offset = NUMTHREADS;
    vector<int> values(NUMTHREADS);
    for (int i = 0; i < NUMTHREADS; i++) {
        values[i] = i + offset;
        workers.push_back(thread(workerReference, ref(values[i])));
    }

    for (auto& t : workers) {
        t.join();
    }
}


int main(int argc, char* argv[]) {
    // Call the first function
    function1();
    // Print out a blank line, then call the second function
    std::cout << std::endl;
    function2();
    // Print out a blank line, then call the third function
    std::cout << std::endl;
    function3();
}
