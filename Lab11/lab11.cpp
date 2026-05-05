// Copyright 2026 Maria Kunigk-Bakalar
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

struct Params {
    int width;
    int height;
    int maxIter;
    double xmin = -1.5;
    double xmax = 1.5;
    double ymin = -1.0;
    double ymax = 1.0;
    double cReal = 0.355534;
    double cImag = -0.337292;
};

// -------------------------------------------------------
// --- Color mapping (provided -- DO NOT CHANGE THESE) ---
// -- this includes:  hueToRgb, colorOf and writePixel ---
// -------------------------------------------------------
void hueToRgb(double hue, double& r1, double& g1, double& b1) {
    double h = std::fmod(hue, 360.0);
    if (h < 0.0) {
        h += 360.0;
    }
    double s = h / 60.0;
    int seg = static_cast<int>(s);
    double f = s - static_cast<double>(seg);
    double x = 1.0 - std::fabs(2.0 * f - 1.0);
    seg = seg % 6;

    const double rp[6] = {1.0, x, 0.0, 0.0, x, 1.0};
    const double gp[6] = {x, 1.0, 1.0, x, 0.0, 0.0};
    const double bp[6] = {0.0, 0.0, x, 1.0, 1.0, x};

    r1 = rp[seg];
    g1 = gp[seg];
    b1 = bp[seg];
}

void colorOf(int it, int maxIter,
             uint8_t& r, uint8_t& g, uint8_t& b) {
    if (it == maxIter) {
        r = 0;
        g = 0;
        b = 0;
        return;
    }

    double t = static_cast<double>(it) /
               static_cast<double>(maxIter);
    t = std::sqrt(t);

    const double hue = 360.0 * t;
    double r1 = 0.0;
    double g1 = 0.0;
    double b1 = 0.0;
    hueToRgb(hue, r1, g1, b1);

    r = static_cast<uint8_t>(255.0 * r1);
    g = static_cast<uint8_t>(255.0 * g1);
    b = static_cast<uint8_t>(255.0 * b1);
}

void writePixel(std::vector<uint8_t>& px, int w, int x, int y,
                uint8_t r, uint8_t g, uint8_t b) {
    const size_t idx = (static_cast<size_t>(y) * w + x) * 3ULL;
    px[idx + 0] = r;
    px[idx + 1] = g;
    px[idx + 2] = b;
}

// --------------------------------------------------------------------------
// -------------------- Julia set core (DO NOT CHANGE!) --------------------
// --------- this includes:  toComplex, escapeIters and renderRow ----------
// --------------------------------------------------------------------------
void toComplex(const Params& p, int px, int py,
               double& zr, double& zi) {
    const double u =
        static_cast<double>(px) / static_cast<double>(p.width - 1);
    const double v =
        static_cast<double>(py) / static_cast<double>(p.height - 1);
    zr = p.xmin + u * (p.xmax - p.xmin);
    zi = p.ymax - v * (p.ymax - p.ymin);
}

int escapeIters(double zr, double zi,
                double cr, double ci, int maxIter) {
    int it = 0;
    while (it < maxIter) {
        const double zr2 = zr * zr;
        const double zi2 = zi * zi;
        if (zr2 + zi2 > 4.0) {
            break;
        }
        const double twoZrZi = 2.0 * zr * zi;
        zr = zr2 - zi2 + cr;
        zi = twoZrZi + ci;
        ++it;
    }
    return it;
}

void renderRow(const Params& prm, int y,
               std::vector<uint8_t>& pixels) {
    for (int x = 0; x < prm.width; ++x) {
        double zr = 0.0;
        double zi = 0.0;
        toComplex(prm, x, y, zr, zi);

        const int it =
            escapeIters(zr, zi,
                        prm.cReal, prm.cImag,
                        prm.maxIter);

        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;

        colorOf(it, prm.maxIter, r, g, b);
        writePixel(pixels, prm.width, x, y, r, g, b);
    }
}


// -------------------- Students implement these --------------------

// MVP(Implement printUsage)
// printUsage should be written to pass the first two tests in CODE
void printUsage() {
    // Your code here.
    std::cout << "Usage:\n" << "  lab11 serial WIDTH HEIGHT MAX_ITER OUT.ppm\n"
              << "  lab11 mt WIDTH HEIGHT MAX_ITER THREADS OUT.ppm\n";
}

// NEXT(Implement renderSerial)
// Loop y = 0..height-1 and call renderRow(prm, y, pixels).
// No threading, no timing, just fill pixels.
void renderSerial(const Params& prm, std::vector<uint8_t>& pixels) {
    // Your code here.
    for (int y = 0; y < prm.height; ++y) {
        renderRow(prm, y, pixels);
    }
}

// A provided worker for cyclic rows (you should use this and NOT
// modify it).
void renderCyclicWorker(const Params& prm, int t, int threads,
                        std::vector<uint8_t>& pixels) {
    for (int y = t; y < prm.height; y += threads) {
        renderRow(prm, y, pixels);
    }
}

// FINAL(Implement renderMt with *cyclic rows*)
// Requirements:
//   - Ensure that all rows are processed and that no two threads ever
//     update the same row
//   - There should be NO race condition (as tested with -fsanitize=thread)
//     and you should NOT use a mutex.
//   - Create std::vector<std::thread> ths;
//   - For t in [0..threads-1], push_back a std::thread that calls
//     renderCyclicWorker(std::cref(prm), t, threads, std::ref(pixels)).
//     (No lambdas, use std::ref/std::cref.)
//   - Join all threads before returning.
//   - No synchronization primitives; threads write disjoint rows.
void renderMt(const Params& prm, int threads, std::vector<uint8_t>& pixels) {
    // Your code here.
    std::vector<std::thread> ths;
    for (int t = 0; t < threads; ++t) {
        ths.push_back(std::thread(renderCyclicWorker,
            std::cref(prm), t, threads, std::ref(pixels)));
    }
    for (auto& th : ths) {
        th.join();
    }
}

// --------------------------------------------------------------------
// ------------------ PPM writer (DO NOT CHANGE) ----------------------
// --------------------------------------------------------------------
bool writePpm(const std::string& path, const Params& prm,
              const std::vector<uint8_t>& pixels) {
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        return false;
    }
    out << "P6\n" << prm.width << ' ' << prm.height << "\n255\n";
    const size_t nbytes =
        static_cast<size_t>(prm.width) *
        static_cast<size_t>(prm.height) * 3ULL;
    out.write(reinterpret_cast<const char*>(pixels.data()),
              static_cast<std::streamsize>(nbytes));
    return static_cast<bool>(out);
}

// ------------------------------------------------------------
// --------------- CLI parsing (DO NOT CHANGE) ----------------
// ------------ includes parseInt and parseCommon -------------
// ------------------------------------------------------------
bool parseInt(const std::string& s, int& out) {
    try {
        out = std::stoi(s);
        return true;
    } catch (...) {
        return false;
    }
}

// Parses mode + numeric args common to both modes.
// On error, prints a message and returns false.
bool parseCommon(int argc, const std::vector<std::string>& args,
                 Params& prm, std::string& mode) {
    if (argc < 6) {
        printUsage();
        return false;
    }
    mode = args[1];

    if (!parseInt(args[2], prm.width) ||
        !parseInt(args[3], prm.height) ||
        !parseInt(args[4], prm.maxIter)) {
        std::cerr << "Invalid numeric argument.\n";
        return false;
    }
    if (prm.width <= 0 || prm.height <= 0 || prm.maxIter <= 0) {
        std::cerr << "WIDTH, HEIGHT, MAX_ITER must be positive.\n";
        return false;
    }
    return true;
}

// MVP(Implement parseSerialArgs)
// Validate arg count for serial mode: argc must be 6.
// If not, an error message that passes the third test in CODE should be
// displayed.
// On success, set out = args[5] and return true; else return false.
bool parseSerialArgs(int argc, const std::vector<std::string>& args,
                     std::string& out) {
    // Your code here.
    if (argc != 6) {
        std::cerr << "serial requires WIDTH HEIGHT MAX_ITER OUT.ppm\n";
        return false;
    }
    out = args[5];
    return true;
}

// MVP(Implement parseMtArgs)
// Validate arg count for mt mode: argc must be 7.
// If not, an error message that passes the fourth test in CODE should be
// displayed.:
// If threads < 2, an error message that passes the fifth test in CODE should
// be displayed
// On success, set out = args[6], return true; else return false.
bool parseMtArgs(int argc, const std::vector<std::string>& args,
                 int& threads, std::string& out) {
    // Your code here.
    if (argc != 7) {
        std::cerr << "mt requires WIDTH HEIGHT MAX_ITER THREADS OUT.ppm\n";
        return false;
    }
    parseInt(args[5], threads);
    if (threads < 2) {
        std::cerr << "THREADS must be > 1.\n";
        return false;
    }
    out = args[6];
    return true;
}

// MVP(Implement parseArgs)
// Steps:
//   1) Keep the first if-block as-is (calls parseCommon).
//   2) If mode == "serial": set threads=1; call parseSerialArgs(...).
//   3) Else if mode == "mt": call parseMtArgs(...).
//   4) Else: an error message that passes the sixth test in CODE should
//      be displayed, then return false.
bool parseArgs(int argc, const std::vector<std::string>& args,
               Params& prm, std::string& mode, int& threads,
               std::string& out) {
    if (!parseCommon(argc, args, prm, mode)) {
        return false;
    }
    // Your code here.
    if (mode == "serial") {
        threads = 1;
        return parseSerialArgs(argc, args, out);
    } else if (mode == "mt") {
        return parseMtArgs(argc, args, threads, out);
    } else {
        std::cerr << "Mode must be 'serial' or 'mt'.\n";
        return false;
    }
    return false;
}

// -----------------------------------------------------------------
// ------------ Program runner & main (DO NOT CHANGE) --------------
// -----------------------------------------------------------------
int runProgram(const Params& prm, const std::string& mode,
               int threads, const std::string& out) {
    std::vector<uint8_t> pixels(
        static_cast<size_t>(prm.width) *
        static_cast<size_t>(prm.height) * 3ULL,
        0);

    if (mode == "serial") {
        renderSerial(prm, pixels);
    } else {
        renderMt(prm, threads, pixels);
    }

    if (!writePpm(out, prm, pixels)) {
        std::cerr << "Failed to write " << out << "\n";
    }
    return 0;
}

int main(int argc, char* argv[]) {
    std::vector<std::string> args;
    args.reserve(static_cast<size_t>(argc));
    for (int i = 0; i < argc; ++i) {
        args.push_back(std::string(argv[i]));
    }

    Params prm{};
    std::string mode;
    std::string out;
    int threads = 1;

    if (!parseArgs(argc, args, prm, mode, threads, out)) {
        return 0;
    }
    return runProgram(prm, mode, threads, out);
}
