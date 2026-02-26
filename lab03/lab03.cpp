// Copyright [2026] <Maria Kunigk-Bakalar>

#include <iostream>
#include <fstream>

int wordCount(std::fstream& fs) {
    std::string word;
    int count = 0;
    while (fs >> word) {
        count++;
    }
    return count;
}

int main() {
    while  (true) {
        std::string filename = "";
        std::cout << "Which file do you want to read: ";
        std::cin >> filename;
        std::fstream inputFile(filename);
        if (!inputFile.is_open()) {
            std::cerr << "Cannot open file. Try again." << std::endl;
        } else {
            std::cout << "\nThere are " << wordCount(inputFile) <<
        " words in the file." << std::endl;
        inputFile.close();
        return 0;
        }
    }
}
