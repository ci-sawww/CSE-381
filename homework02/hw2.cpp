// Copyright 2026 Maria Kunigk-Bakalar

// needed for std::array to store bigram counts and mutexes
#include <array>
// needed more isdigit, isalnum, tolower
// used when classifying and cleaning
#include <cctype>
// needed for int64_t
// used for ensuring consistent 64-bit integer size
#include <cstdint>
// needed for ifstream
// used for opening and reading input file
#include <fstream>
// needed for setw
// used for right-align bigram counts in output
#include <iomanip>
// needed for cout, cerr
// used for output and error messages
#include <iostream>
// needed for mutex and lock_guard
// used to protect from race conditions
#include <mutex>
// needed for string
// used throughout
#include <string>
// needed for thread
// used to create and manage threads
#include <thread>
// needed for vector
// used to store threads before joining them
#include <vector>

// defines number of valid symbols
// 10 digits + 26 lowercase letters
#define SYMBOLS 36
// defines total number of possible bigrams
// 36 x 36 = 1296
#define BIGRAMS 1296

namespace bg {
// to avoid race conditions during initialization

// fixed size array of 1296 64-bit integers
// all threads write counts into this
std::array<int64_t, BIGRAMS> counts{};
// fixed-size array of 1296 mutexes, one per bigram
// allows concurrent updates to different bigrams without waiting
std::array<std::mutex, BIGRAMS> locks{};
}

// takes a single character an dreturns its position
int charIndex(char ch) {
    // converts character to unsigned char
    // avoids undefined behavoir
    unsigned char uch = static_cast<unsigned char>(ch);
    // checks if character is a number
    if (std::isdigit(uch)) {
        // makes the char an int
        return ch - '0';
    }
    // makes char a number
    // a --> 10, b -> 11, etc
    return 10 + (ch - 'a');
}

// Combines two character indices into a single array index
// With 36 symbols, the index space is 36x36 = 1296
int bigramIndex(char first, char second) {
    // treats the indices with coordinates ('a' 'b' -> 10*36 + 11 = 371)
    return charIndex(first) * SYMBOLS + charIndex(second);
}

// Reads the file one character at a time into a string with no processing
// Sets opened to false if the file cannot be opened,
// isEmpty if it has no content
std::string readFile(const std::string& filename, bool& opened, bool& isEmpty) {
    std::ifstream file(filename);
    // check if the file opened
    opened = file.is_open();
    // if it didn't open
    if (!opened) {
        // return empty string
        return "";
    }

    std::string text;
    char ch;
    // read one character at a time
    while (file.get(ch)) {
        text += ch;
    }
    // check after reading to distinguish an empty file vs missing file
    isEmpty = text.empty();
    return text;
}

// Strips non-alphanumeric characters
// lowercases everything
// This gives us a clean string that only contains valid bigram characters
std::string cleanText(const std::string& rawText) {
    std::string cleaned;
    // reserve space upfront to avoid repeats as we build string
    cleaned.reserve(rawText.size());

    for (char ch : rawText) {
        // cast to unsigned char to avoid undef behavior
        unsigned char uch = static_cast<unsigned char>(ch);
        // skip spaces/punctuation
        // lowercase everything to bigrams are not case-sensitive
        if (std::isalnum(uch)) {
            cleaned += static_cast<char>(std::tolower(uch));
        }
    }
    // return cleaned string
    return cleaned;
}

// takes full cleaned text and a start/end range defining the chunk
// this thread is responsible for
void worker(const std::string& text, size_t start, size_t end) {
    // Step back one position so we catch the bigram that straddles the boundary
    // between this chunk and the one before it
    // skipped for first thread since there is no previous chunk
    if (start > 0) {
        start--;
    }

    // iterates through chunk one character at a time
    for (size_t i = start; i + 1 < end; i++) {
        // casts characters to unsigned char to avoid undefined behavior
        unsigned char c1 = static_cast<unsigned char>(text[i]);
        unsigned char c2 = static_cast<unsigned char>(text[i + 1]);

        // only processes pair if both characters are numbers or letters
        // (chunk boundries could land between characters that were adjacent)
        if (std::isalnum(c1) && std::isalnum(c2)) {
            // computes array index for this specific bigram
            int idx = bigramIndex(text[i], text[i + 1]);
            // Lock only the mutex for this specific bigram so other threads
            // can update different bigrams concurrently
            std::lock_guard<std::mutex> lock(bg::locks[idx]);
            // increases count for this bigram in chared array
            bg::counts[idx]++;
        }
    }
}

// divides the cleaned text into equal chunks and assigns each to a thread
// the last thread gets any remaining characters if the
// text doesn't divide evenly
void runThreads(const std::string& text, int numThreads) {
    // Nothing to do if there aren't at least two characters to form a bigram
    if (text.size() < 2) {
        return;
    }
    // divides text length evenly by number of threads
    size_t chunkSize = text.size() / static_cast<size_t>(numThreads);
    // if there are more threads than characters
    if (chunkSize == 0) {
        chunkSize = 1;
    }
    // vector to hold all threads before they're joined
    std::vector<std::thread> threads;
    // avoid repeat reallocations as threads are added
    threads.reserve(static_cast<size_t>(numThreads));

    for (int i = 0; i < numThreads; i++) {
        // finds where this thread's chunk begins
        size_t start = static_cast<size_t>(i) * chunkSize;
        // finds where it ends
        size_t end = static_cast<size_t>(i + 1) * chunkSize;

        // Give the last thread any leftover characters
        if (i == numThreads - 1) {
            end = text.size();
        }
        // new thread that runs worker function and then adds to vector
        threads.push_back(std::thread(worker, std::cref(text), start, end));
    }
    // waits for all threads to finish
    // joins
    for (auto& t : threads) {
        t.join();
    }
}

// Prints all 1296 bigram counts, 5 per line, with each value right-aligned
// in a field of width 7
void printCounts() {
    // tracks how many bigrams have been printed to know when to enter
    int col = 0;
    // iterates over all 36 possible first characters
    for (int i = 0; i < SYMBOLS; i++) {
        // converts loop index into character
        char first = (i < 10) ? static_cast<char>('0' + i)
                              : static_cast<char>('a' + i - 10);
        // iterates over all 36 possible second characters
        // both loops together cover all 1296 combos
        for (int j = 0; j < SYMBOLS; j++) {
            // converts loop index into character (like before)
            char second = (j < 10) ? static_cast<char>('0' + j)
                                   : static_cast<char>('a' + j - 10);
            // like bigramIndex, finds array index
            int idx = i * SYMBOLS + j;
            // formatting
            std::cout << "  " << first << second << ":"
                      << std::setw(7) << bg::counts[idx];
            col++;
            // every 5th bigram produce new row
            if (col % 5 == 0) {
                std::cout << "\n";
            }
        }
    }
    // 1296 is not divisible by 5, so the final line needs an explicit newline
    std::cout << "\n";
}

// Returns true and sets numThreads if the
// string parses as an int greater than 0
bool validThreads(const std::string& text, int& numThreads) {
    // std::stoi could throw error is string can't be converted to int
    try {
        size_t pos = 0;
        // parse string as an int, storing result in value
        // update pos to wehre it stopped
        int value = std::stoi(text, &pos);
        // Reject if there are trailing non-numeric characters
        if (pos != text.size()) {
            return false;
        }
        // reject if 0 or negative value
        if (value <= 0) {
            return false;
        }
        numThreads = value;
        return true;
    } catch (...) {
        return false;
    }
}

// Validates argument and thread count, printing appropriate error
// message and return false if either is invalid
bool parseArgs(int argc, char* argv[], int& numThreads) {
    // check that exactly 2 command line arguments were provided
    if (argc != 3) {
        std::cerr << "Usage: homework02 file numThreads" << std::endl;
        return false;
    }
    // makes sure second command line argument is positive int
    if (!validThreads(argv[2], numThreads)) {
        std::cerr << "Error: numThreads must be an int > 0" << std::endl;
        return false;
    }
    return true;
}

// Opens and reads the input file, printing an error and returning false
// if the file cannot be opened or is empty
bool loadFile(const std::string& filename, std::string& rawText) {
    bool opened = false;
    bool isEmpty = false;
    // readFile fills rawText with rile contents and sets opens and isEmpty
    rawText = readFile(filename, opened, isEmpty);
    // check if it couldn't open or if its empty
    if (!opened || isEmpty) {
        std::cerr << "Error reading file " << filename << std::endl;
        return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    // initialize thread count to 0 so tis always defined
    int numThreads = 0;
    // sets numThreads and validates command line arguments
    if (!parseArgs(argc, argv, numThreads)) {
        return 0;
    }
    // decalres string that will hold raw file contents
    std::string rawText;
    // tries to open and read into rawTezxt
    if (!loadFile(argv[1], rawText)) {
        return 0;
    }
    // strips non-alphanumeric characters and lowercases everything
    std::string text = cleanText(rawText);
    // divides cleaned text among threads and counts bigrams
    runThreads(text, numThreads);
    // prints all 1296 counts in right format after threads have finished
    printCounts();
    return 0;
}  // done!! i hope you enjoyed the code :) i made sure to add lots of comments
