// Copyright 2026 <Maria Kunigk-Bakalar

#include <iostream>
#include <string>

bool check(auto num1, auto num2) {
    try {
        std::stoi(num1);
        std::stoi(num2);
    } catch (const std::exception& e) {
        std::cout << "Error: arguments must be integers" << std::endl;
        return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: lab01 number1 number2" << std::endl;
        return 0;
    }
    if (check(argv[1], argv[2])) {
        std::cout << "Welcome to the Number Analyzer!" << std::endl;
        std::cout << "First number is " << argv[1] << std::endl;
        std::cout << "Second number is " << std::stoi(argv[2]) << std::endl;
        std::cout << "Sum is " <<
        std::stoi(argv[1]) + std::stoi(argv[2]) << std::endl;
    }
    return 0;
}

