// Copyright 2026 <Maria Kunigk-Bakalar
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <sstream>

using StrVec = std::vector<std::string>;

// Right here you should implement a function called myExec that will invoke
// an appropriate syscall from the exec family.  Use the sample code from
// Session 12 as a guide for how to do this.
int myExec(StrVec argList) {
    std::vector<char*> args;
    for (auto& s : argList) {
        args.push_back(&s[0]);
    }
    args.push_back(nullptr);
    execvp(args[0], &args[0]);
    return -1;
}


// Invoke a fork syscall and, if I am the child process, call the myExec
// function (which you wrote above) that will execute the command
// passed in via argList
pid_t forkNexec(StrVec& argList) {
    // Fork and save the pid of the child process
    pid_t childPid = fork();
    // Call the myExec helper method in the child
    if (childPid == 0) {
        // We are in the child process.  If myExec returns -1 we know that
        // the command failed and we should notify the caller by setting the
      // pid to -1
        int retVal = myExec(argList);
        if (retVal == -1) {
            childPid = -1;
        }
    }
    // Control drops here only in the parent process!
    return childPid;
}

int main() {
    while (true) {
        // Display a prompt to the user (e.g., "$ ")
        std::cout << "$ ";

        // Read the user's input and store it in a std::string
        // variable (e.g., user_input)
        std::string user_input;
        std::getline(std::cin, user_input);


        // Check if the user wants to exit (e.g., by typing "exit")
        // If yes, break out of the loop.
        if (user_input == "exit") {
            break;
        }


        // Implement the logic to fork a child process and execute
        // the user's input using some flavor of exec.  If the command
        // the user wants to run doesn't exist, the program should continue
        // and return to the command prompt just as would happen in a
        // bash terminal.

        // Use waitpid() to wait for the child process to finish.

        StrVec argList;
        std::stringstream strs(user_input);
        std::string word;
        while (strs >> word) {
            argList.push_back(word);
        }
        pid_t pid = forkNexec(argList);
        if (pid == 0) {
            return 0;
        }
        if (pid == -1) {
            std::cerr << user_input << " command not found..." << std::endl;
            return 0;
        }
        waitpid(pid, nullptr, 0);
    }
    return 0;
}
