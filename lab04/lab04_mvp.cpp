// Copyright 2025 <Maria Kunigk-Bakalar>
// lab04.cpp
// Starter code for Lab 04: Containers and Algorithms
// Usage: ./lab04 <filename> <k>
//
// Complete TODOs in three layers:
//   - TODO(MVP)   : file I/O + total count + empty-file behavior (4 pts)
//   - TODO(NEXT)  : summary stats with <algorithm> (4 pts)
//   - TODO(FINAL) : frequency map + sorted top-k (2 pts)
//
// Constraints: no global variables; lowerCamelCase; concise functions.

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// ---------------------------------------------------------------------
// Declarations for lambda-free helpers students will implement later.
// ---------------------------------------------------------------------

// TODO(NEXT): return true if x is divisible by 2.
// Helper predicate for count_if (lambda-free).
bool isEven(int x);


// TODO(FINAL): implement so that higher frequency comes first; on ties,
//              smaller numeric value comes first.
// Helper comparator for sort (lambda-free).
bool compareByFreqThenValue(const std::pair<int, std::size_t>& lhs,
                            const std::pair<int, std::size_t>& rhs);


// ---------------------------------------------------------------------
// MVP: parsing helpers and file loading
// ---------------------------------------------------------------------

// Reads a single line, extracts all integers, and appends them to `out`.
// Lines beginning with '#' are comments. Non-integer tokens are skipped.
void parseIntsFromLine(const std::string& line, std::vector<int>& out) {
  // TODO(MVP): Implement integer extraction.
  // Hints:
  //   - if line starts with '#', return immediately
  //   - use std::istringstream iss(line);
  //   - loop:
  //      int v; if (iss >> v) { out.push_back(v); continue; }
  //       iss.clear(); std::string junk;
  //       if (iss >> junk) continue; else break;
  if (line[0] == '#') {
    return;
  }
  // std::cout << "line = " << line << std::endl;
  std::istringstream iss(line);
  while (true) {
    int v;
    if (iss >> v) {
      out.push_back(v);
      continue;
    }
    iss.clear();
    std::string junk;
    if (iss >> junk) {
      continue;
    } else {
      break;
    }
  }

  // (void)line;  // remove after implementing
  // (void)out;   // remove after implementing
}

// Read all integers from a file into a vector<int>. Throw on open failure.
std::vector<int> loadNumbers(const std::string& path) {
  // TODO(MVP): std::ifstream in(path); if (!in) throw runtime_error(...).
  // TODO(MVP): std::string line; while (getline) call parseIntsFromLine.
  std::vector<int> out;
  std::ifstream in(path);
  std::string line;
  if (!in) {
    throw std::runtime_error("Failed to open: " + path);
  }
  while (std::getline(in, line)) {
    // std::cout << line << std::endl;
    parseIntsFromLine(line, out);
  }
  // (void)path;  // remove after implementing
  return out;
}

// ---------------------------------------------------------------------
// NEXT: summary statistics with <algorithm>
// ---------------------------------------------------------------------

// Return the count of unique values using sort + unique + erase.
std::size_t computeUniqueCount(const std::vector<int>& a) {
  // TODO(NEXT): copy `a` to a working vector, sort it,
  //             unique+erase, then return the size.
  (void)a;  // remove after implementing
  return 0;
}

// Return the average as double using accumulate (64-bit sum). Empty -> 0.0.
double computeAverage(const std::vector<int>& a) {
  // TODO(NEXT): use std::accumulate with int64_t initial value; divide by size.
  (void)a;  // remove after implementing
  return 0.0;
}

// Return how many elements are even using std::count_if and isEven().
std::size_t countEvens(const std::vector<int>& a) {
  // TODO(NEXT): implement with std::count_if(a.begin(), a.end(), isEven).
  (void)a;  // remove after implementing
  return 0;
}

// ---------------------------------------------------------------------
// FINAL: frequency table + sorted top-k
// ---------------------------------------------------------------------

// Build a frequency table: value -> count using std::unordered_map.
std::unordered_map<int, std::size_t> buildFreq(const std::vector<int>& a) {
  // TODO(FINAL): iterate over `a`; for each x, increment the count in the map.
  (void)a;  // remove after implementing
  return {};
}

// Convert the map to vector<pair<int,size_t>> and sort with comparator.
std::vector<std::pair<int, std::size_t>>
sortedItemsByFreq(const std::unordered_map<int, std::size_t>& f) {
  // TODO(FINAL):
  //   - build vector<pair<int,size_t>> items from `f`
  //   - std::sort(items.begin(), items.end(), compareByFreqThenValue);
  (void)f;  // remove after implementing
  return {};
}

// Print the top-k entries "value -> count". If fewer than k, print them all.
void printTopK(const std::vector<std::pair<int, std::size_t>>& items,
               std::size_t k) {
  // TODO(FINAL):
  //   std::size_t limit = (k < items.size()) ? k : items.size();
  //   std::cout << "Top-" << k << ":\n";
  //   loop i in [0, limit): print items[i].first << " -> " << items[i].second
  (void)items;  // remove after implementing
  (void)k;      // remove after implementing
}

// ---------------------------------------------------------------------
// Printing utilities / control flow
// ---------------------------------------------------------------------

void printUsage() {
  std::cerr << "Usage: ./lab04 <filename> <k>\n";
}

// Print the "empty file" summary (used in MVP and beyond).
void printEmptySummary(std::size_t k) {
  // TODO(MVP): print the zeroed stats block exactly like this:
  //   Total: 0
  std::cout << "Total: " << 0 << std::endl;
  //   Unique: 0
  std::cout << "Unique: " << 0 << std::endl;
  //   Average: 0
  std::cout << "Average: " << 0 << std::endl;
  //   Min: n/a
  std::cout << "Min: " << "n/a" << std::endl;
  //   Max: n/a
  std::cout << "Max: " << "n/a" << std::endl;
  //   Even numbers: 0
  std::cout << "Even numbers: " << 0 << std::endl;
  //   (blank line)
  std::cout << "" << std::endl;
  //   Top-<k>:
  std::cout << "Top-" << k << ":" << std::endl;
  // (void)k; remove after implementing
}

// Compute stats and print summary + (eventually) top-k list.
// Use: sort/unique, accumulate, min_element, max_element, count_if.
void printSummary(const std::vector<int>& nums, std::size_t k) {
  // TODO(MVP):
  //   - Print only "Total: <n>"
  std::cout << "Total: " << nums.size() << std::endl;
  //
  // TODO(NEXT):
  //   - Print Unique (computeUniqueCount)
  //   - Print Average (computeAverage)
  //   - Print Min (min_element) and Max (max_element)
  //   - Print Even numbers (countEvens)
  //   - Then print a blank line
  //
  // TODO(FINAL):
  //   - Build frequency map (buildFreq)
  //   - Transform + sort (sortedItemsByFreq)
  //   - Print top-k list (printTopK)
  (void)nums;  // remove after implementing
  (void)k;     // remove after implementing
}

// Orchestrate parsing, empty-input handling, and summary printing.
int run(int argc, char* argv[]) {
  if (argc != 3) {
    printUsage();
    return 0;
  }
  const std::string fileName = argv[1];
  const std::size_t k = static_cast<std::size_t>(std::stoull(argv[2]));

  // TODO(MVP):
  const std::vector<int> nums = loadNumbers(fileName);
  // std::cout << "read file\n";
  if (nums.empty()) {
    printEmptySummary(k);
    return 0;
  } else {
    printSummary(nums, k);
    return 0;
  }
  //  (void)fileName;  // remove after implementing
  // (void)k; remove after implementing
  return 0;
}

int main(int argc, char* argv[]) {
  try {
    return run(argc, argv);
  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << "\n";
    return 0;
  }
}
