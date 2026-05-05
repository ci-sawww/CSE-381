// Copyright 2025 <Maria Kunigk-Bakalar>
#include <dirent.h>  // for opendir, readdir, closedir
#include <sys/stat.h>  // for stat() to get metadata
#include <filesystem>
#include <algorithm>  // for std::sort()
#include <functional>  // for the lmbda functions
#include <iomanip>  // for std::setw formatting
#include <iostream>  // for the input and output
#include <string>  // for std::string
#include <vector>  // for std::vector
#include <sstream>
#include <map>  // for std::map

// stores the name and size of a single file
struct FileInfo {
    std::string name;   // File name
    off_t size;         // File size in bytes
};

// extracts the type of file
std::string getType(const std::string& filename) {
    size_t dot = filename.rfind('.');  // finds the last dot in the filename
    if (dot == std::string::npos) return "";
    // returns empty if no extension is found
    return filename.substr(dot);  // returns the extension and includes the dot
}

// reads files, populates file vector
bool readDirectory(const std::string& path, std::vector<FileInfo>& files) {
    auto direct = opendir(path.c_str());  // opens the directory using c_str
    if (direct == nullptr) {  // can we open the directory or is it null
        std::cerr << "Error: Unable to open directory "
        // previous line tells user that the directory couldn't open
        << path << std::endl;  // makes a new line
        return false;  // since it wasn't able to open, return false
    }
    struct dirent* entry;  // creates struct for each directory entry input
    while ((entry = readdir(direct)) != nullptr) {
        // iterate through directory entries
        std::string name = entry->d_name;  // name is teh current directory name

        // Skip the current and parent directory entries
        if (name == "." || name == "..") continue;

        // Build full path for stat call
        std::string fullPath = path + "/" + name;
        struct stat info = {};
        // if stat fails, skip this entry
        if (stat(fullPath.c_str(), &info) != 0) continue;

        // Only process regular files, not subdirectories or special files
        if (S_ISREG(info.st_mode)) {
            files.push_back({name, info.st_size});
        }
    }
    closedir(direct);  // close directory stream
    return true;
}

// sorts the files by size descending, then by name also descending
void sortFs(std::vector<FileInfo>& files) {
    std::sort(files.begin(), files.end(), [](const FileInfo& a,
        const FileInfo& b) {
        if (a.size != b.size) return a.size > b.size;  // larger first
        return a.name > b.name;  // lexicographically
    });
}

// map for type and count for each type
std::map<std::string, int> countType(const std::vector<FileInfo>& files) {
    std::map<std::string, int> countT;
    for (const auto& file : files) {
        countT[getType(file.name)]++;  // increment count for the extension
    }
    return countT;  // returns the count for the extension
}

// printing out formatting
void printFormatting(const std::string& user_input, const
    std::vector<FileInfo>& files, const std::map<std::string, int>& countT) {
        std::cout << "File Report for Directory: " << user_input << std::endl;
    std::cout << "--------------------------------------------------"
    << std::endl;  // 50 dashes as divider
    std::cout << "Files (sorted descending by size, then by name):"
    << std::endl;
    for (const auto& file : files) {  // print each file with its size
        // using const because we're not going to change that part
        std::cout << "  " << file.name << "  -  "
        << file.size << " bytes" << std::endl;
    }
}

// sorts and prints the histogram of file types
void sortHg(const std::map<std::string, int>& countT) {
    // create a pair vector so we can use sort() like before
    std::vector<std::pair<std::string, int> > A;

    // converts the map we already have with the info to vector we made
    for (auto& it : countT) {
        A.push_back(it);
    }

    // sorts by count descending, then by extension also descending
    std::sort(A.begin(), A.end(), [](std::pair<std::string, int>& a,
        std::pair<std::string, int>& b) {
        if (a.second != b.second) return a.second > b.second;  // try this
        return a.first > b.first;  // if that doesn't work than sort by name
    });

    // print the histogram
    for (auto& it : A) {
        // if its more than one file, print 'file(s)' instead of just file
        std::string label = it.second == 1 ? " file" : " file(s)";
        // formatting specific to the test, 14 spaces before the arrow
        // also make sure its right aligned so the formatting si centered around
        // the arrows
        std::cout << std::setw(14) <<std::right << it.first
        << " -> " << it.second << label << std::endl;
    }
}

// prints the summary section with the sorted histogram
void printSummary(const std::string& user_input, const
    std::vector<FileInfo>& files, const std::map<std::string, int>& countT) {
        // this is just the heading
        std::cout << "\nFile Type Summary (sorted by histogram value):\n";
        // this is where the meat is
        sortHg(countT);
}

// the main function that calls almost all the other functions
int main(int argc, char* argv[]) {
    // we are expecting exactly one argument, the directory path
    if (argc != 2) {  // if its any more arguments than 1
        std::cerr << "Usage: homework01 <directory>\n";  // print this
        return 0;  // and return to stop executing main
    }

    // string from user input
    std::string user_input = argv[1];
    // create vector what will have the files
    std::vector<FileInfo> files;

    // try reading from the directory
    // return and stop executing main if program is unable to
    if (!readDirectory(user_input, files)) {
        return 0;
    }

    // sort the files with call to previous function
    sortFs(files);
    // compute the histogram of the extensions
    std::map<std::string, int> countT = countType(files);
    // print the report, not including the histogram
    printFormatting(user_input, files, countT);
    // print the summary/histogram (already sorted within function)
    printSummary(user_input, files, countT);


    // end ! all done ! yay !
    // please give me A+ :)
    return 0;
}

