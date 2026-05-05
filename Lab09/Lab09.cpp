// Copyright 2026 <Maria Kunigk-Bakalar>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <thread>

using std::string, std::vector, std::map;
using std::cout, std::cerr, std::endl, std::setw, std::left;

// Count the number of times findWord appears in the vector text
// Use count to return the value to the caller. Note that this is
// an abstracted function since in the main program it will use
// elements of a map to provide findWord and count.
void countWord(const vector<string>& text, const string& findWord, int& count) {
    // Initialize count here in case it wasn't in the main program.
    count = 0;

    // Loop through each element of text
    for (const auto& word : text) {
        // If word and findWord are identical, compare will return 0
        if (word.compare(findWord) == 0) {
            // The strings are the same, so increment the counter
            count++;
        }
    }
}

// Open a file with text and read every individual word in the text into
// a std::vector
bool readText(vector<string>& text, const string& fileName) {
    // Create an ifstream based on the fileName provided
    std::ifstream textFile(fileName);

    // If the file is open, we read from it
    if (textFile.is_open()) {
        // Read until we reach the end of the file
        string temp;
        while (textFile >> temp) {
            // Add the word to the end of the vector
            text.push_back(temp);
        }

        // Close the file!
        textFile.close();
        return true;
    } else {
        // If we couldn't read the file, give an error message and exit
        // ADD CODE HERE
        cout << "File not found" << endl;
        return false;
    }
}

int main(int argc, char* argv[]) {
    // Make sure that a single command-line argument has been provided. Give
    // an error message and exit if not
    // ADD CODE HERE!
    if (argc != 2) {
        cerr << "Usage: lab09 inputFile" << endl;
        return 0;  // must return 0 for the tests
    }

    // Now, try to open the file and read its contents. If the file can't be
    // read, readText will exit with an error message

    // Create a vector to hold the text
    // ADD CODE HERE
    vector<string> text;

    // Be nice and convert the c-string to a std::string
    // ADD CODE HERE
    string fileName = argv[1];

    // Now create a vector that holds all of the words in the text. Do this
    // by passing in your empty vector that holds text and the filename the
    // user passed in.
    if (!readText(text, fileName)) {
        return 0;
    }

    // If we've made it here, we have enough to continue. We want to count
    // how many times specific words show up. A std::map is a good way to
    // do that. Here is a map of the 6 words we're looking for, all
    // initialized to zero
    map<string, int> common = { {"the", 0}, {"as", 0}, {"while", 0},
                                {"The", 0}, {"A", 0}, {"across", 0} };

    // Loop through the common words and for each one count the number of times
    // each word in the map shows up. You can use the countWord function that
    // is provided.
    // ADD CODE HERE. THIS WOULD BE A GOOD PLACE TO CREATE YOUR THREADS
    // IF YOU ARE DOING THE MULTITHREADED VERSION
    std::vector<std::thread> threads;
    for (auto& entry : common) {
        threads.emplace_back(countWord, std::cref(text), std::cref(entry.first),
                             std::ref(entry.second));
    }

    // IF YOU ARE DOING THE MULTITHREADED VERSION, THIS WOULD BE A GOOD PLACE
    // TO WAIT FOR YOUR THREADS TO COMPLETE
    for (auto& t : threads) {
        t.join();
    }

    // Print out the results: Give the word and the count of times it
    // was found in the text. You should left justify the output and the first
    // field should have a width of 10
    // We must print in this specific order: A, The, across, as, the, while
    std::vector<string> order = {"A", "The", "across", "as", "the", "while"};
    for (const auto& w : order) {
        cout << left << setw(11) << w << common[w] << endl;
    }

    // Exiting here!
    return 0;
}
