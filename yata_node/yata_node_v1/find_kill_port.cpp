#include "find_kill_port.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>

int findAndKillPortProcess(int port) {
    // Command to find PID using fuser
    std::string fuserCommand = "fuser ";
    fuserCommand += std::to_string(port);
    fuserCommand += "/tcp";

    // Open a pipe to execute the command
    FILE* pipe = popen(fuserCommand.c_str(), "r");
    if (!pipe) {
        std::cerr << "Error executing fuser command." << std::endl;
        return 1;
    }

    // Read the PID from the output
    char buffer[128];
    std::string result = "";
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);

    // Extract PID from result
    int pid = std::stoi(result);

    // Command to kill process
    std::string killCommand = "kill -9 ";
    killCommand += std::to_string(pid);

    // Execute the kill command
    int killResult = system(killCommand.c_str());
    if (killResult == -1) {
        std::cerr << "Error executing kill command." << std::endl;
        return 1;
    }

    return 0;
}

