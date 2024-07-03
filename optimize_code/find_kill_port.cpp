#include "find_kill_port.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>

int findAndKillPortProcess(int port) {
    // Command to find PID using fuser
    std::ostringstream fuserCommand;
    fuserCommand << "fuser " << port << "/tcp 2>&1";

    // Open a pipe to execute the command
    FILE* pipe = popen(fuserCommand.str().c_str(), "r");
    if (!pipe) {
        std::cerr << "Error executing fuser command." << std::endl;
        return 1;
    }

    // Read the PID from the output
    char buffer[128];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }
    int status = pclose(pipe);

    if (status == -1) {
        std::cerr << "Error closing pipe after fuser command." << std::endl;
        return 1;
    }

    if (result.empty()) {
        std::cerr << "No process found using port " << port << "." << std::endl;
        return 1;
    }

    // Extract PID from result
    int pid;
    try {
        pid = std::stoi(result);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid PID received from fuser command: " << result << std::endl;
        return 1;
    } catch (const std::out_of_range& e) {
        std::cerr << "PID out of range: " << result << std::endl;
        return 1;
    }

    // Command to kill process
    std::ostringstream killCommand;
    killCommand << "kill -9 " << pid;

    // Execute the kill command
    int killResult = system(killCommand.str().c_str());
    if (killResult == -1) {
        std::cerr << "Error executing kill command." << std::endl;
        return 1;
    }

    return 0;
}
