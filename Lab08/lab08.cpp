// Copyright 2025 <YMaria Kunigk-Bakalar>
#include <sys/stat.h>
#include <iostream>
#include <sstream>

struct stat fileStat;

std::string GetPermissions(const struct stat& fileInfo) {
    std::string permissions = "";
    permissions += (fileInfo.st_mode & S_IRUSR) ? "r" : "-";
    permissions += (fileInfo.st_mode & S_IWUSR) ? "w" : "-";
    permissions += (fileInfo.st_mode & S_IXUSR) ? "x" : "-";
    permissions += (fileInfo.st_mode & S_IRGRP) ? "r" : "-";
    permissions += (fileInfo.st_mode & S_IWGRP) ? "w" : "-";
    permissions += (fileInfo.st_mode & S_IXGRP) ? "x" : "-";
    permissions += (fileInfo.st_mode & S_IROTH) ? "r" : "-";
    permissions += (fileInfo.st_mode & S_IWOTH) ? "w" : "-";
    permissions += (fileInfo.st_mode & S_IXOTH) ? "x" : "-";
    return permissions;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: lab08 filename" << std::endl;
        return 0;
    }

    std::string filename = argv[1];

    if (stat(filename.c_str(), &fileStat) != 0) {
        std::cerr << "Error: Unable to stat '" << filename << "'" << std::endl;
        return 0;
    }

    std::cout << filename << " permissions: "
    << GetPermissions(fileStat) <<std::endl;

    return 0;
}
