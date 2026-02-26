// Copyright 2025 <Maria Kunigk-Bakalar>
#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using std::cout, std::left, std::right, std::setw;

// A struct to hold movie information
struct Movie {
    std::string title;
    int year;
    double rating;
};

// Dataset for the lab: DO NOT CHANGE (CODE relies on it).
std::vector<Movie> BuildDataset() {
    return {
        {"The Matrix", 1999, 8.7},
        {"Memento", 2000, 8.4},
        {"The Dark Knight", 2008, 9.0},
        {"Inception", 2010, 8.8},
        {"The Room", 2003, 3.7},
        {"Spirited Away", 2001, 8.6},
        {"The Godfather", 1972, 9.2},
        {"Mad Max: Fury Road", 2015, 8.1},
        {"Interstellar", 2014, 8.7},
        {"Parasite", 2019, 8.5},
        {"Avengers: Endgame", 2019, 8.4},
        {"Up", 2009, 8.3}
    };
}

void PrintHeader(const std::string& label) {
    cout << "== " << label << " ==" << "\n";
    cout << left
              << setw(22) << "Title"
              << right
              << setw(6) << "Year"
              << setw(9) << "Rating"
              << "\n";
    cout << std::string(37, '-') << "\n";
}

void PrintMovies(const std::vector<Movie>& movies) {
    for (const Movie& m : movies) {
        cout << left
                  << setw(22) << m.title
                  << right
                  << setw(6) << m.year
                  << setw(9) << std::fixed << std::setprecision(1)
                  << m.rating
                  << "\n";
    }
}

std::vector<Movie> Filter(const std::vector<Movie>& input,
                          const std::function<bool(const Movie&)>& keep) {
    std::vector<Movie> out;
    out.reserve(input.size());

    // NOTE: keep is a callable to be implemented via a lambda expression.

    // TODO(MVP): Use std::copy_if and 'keep' to produce 'out'.
    // not sure if this is right
    std::copy_if(input.begin(), input.end(), std::back_inserter(out), keep);

    return out;
}

int main() {
    const std::vector<Movie> all = BuildDataset();

    // ------------------------------------------------------------
    // MVP
    // Filter movies released in 2000 or later.
    //
    // Requirements:
    //   - Use Filter(...) above
    //   - Pass a lambda predicate (no named predicate function)
    //   - Preserve original dataset order (copy_if does)
    // ------------------------------------------------------------

    // TODO(MVP): Create a lambda predicate: year >= 2000
    // TODO(MVP): Call Filter(all, <your lambda>) to produce mvp
    // not sure if this is right
    std::vector<Movie> mvp = Filter(all, [](const Movie& m) {
        return m.year >= 2000;
    });

    PrintHeader("MVP (year >= 2000)");
    PrintMovies(mvp);
    cout << "\n";

    // ------------------------------------------------------------
    // NEXT
    // Filter movies released in 2000 or later AND rating >= 8.0
    //
    // Requirements:
    //   - Use Filter(...) above
    //   - Pass a lambda predicate that checks BOTH conditions
    //   - Preserve original dataset order
    // ------------------------------------------------------------

    // TODO(NEXT): Create a lambda predicate: year >= 2000 && rating >= 8.0
    // TODO(NEXT): Call Filter(all, <your lambda>) to produce next
    std::vector<Movie> next = Filter(all, [](const Movie& m) {
        return m.year >= 2000 && m.rating >= 8.0;
    });

    PrintHeader("NEXT (year >= 2000, rating >= 8.0)");
    PrintMovies(next);
    cout << "\n";

    // ------------------------------------------------------------
    // COMPLETE
    // Starting from the NEXT result:
    // Sort by:
    //   1) rating descending
    //   2) title ascending (tie-breaker)
    //
    // Requirements:
    //   - Use std::sort
    //   - Use a lambda comparator (no named comparator function)
    //   - Deterministic order via tie-break rule
    // ------------------------------------------------------------

    // TODO(COMPLETE): Sort 'next' in-place using std::sort and a lambda
    // function as the comparator.

    std::sort(next.begin(), next.end(), [] (const Movie& m1, const Movie& m2) {
        if (m1.rating > m2.rating) {
            return true;
        } else if (m1.rating < m2.rating) {
            return false;
        } else { return (m1.title < m2.title); }
    });

    PrintHeader("COMPLETE (sorted: rating desc, title asc)");
    PrintMovies(next);

    return 0;
}
