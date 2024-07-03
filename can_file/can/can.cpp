#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

int Can_communication(const char *canInterface, const int bitrate, const char *originalCanMessage) {
    const char* verificationMessage = "123#000400";
    const char* expectedVerificationResponse = "can0  00000123   [3]  00 02 00";
    char receivedMessage[100];  // Increase the buffer size to accommodate the received CAN message

    // Set down the can interface
    int result = std::system("ip link set can0 down");
    if (result != 0) {
        std::cerr << "Error setting down can0 interface." << std::endl;
        return EXIT_FAILURE;
    }

    // Set can interface type and bitrate
    char command[100];
    std::snprintf(command, sizeof(command), "ip link set %s type can bitrate %d", canInterface, bitrate);
    result = std::system(command);
    if (result != 0) {
        std::cerr << "Error setting up can0 interface." << std::endl;
        return EXIT_FAILURE;
    }

    // Set up the can interface
    result = std::system("ip link set can0 up");
    if (result != 0) {
        std::cerr << "Error setting up can0 interface." << std::endl;
        return EXIT_FAILURE;
    }

    // Send verification CAN message
    std::snprintf(command, sizeof(command), "cansend %s %s", canInterface, verificationMessage);
    result = std::system(command);
    if (result != 0) {
        std::cerr << "Error sending verification CAN message." << std::endl;
        return EXIT_FAILURE;
    }

    // Read the verification response
    std::snprintf(command, sizeof(command), "candump %s", canInterface);
    std::cout << "Waiting for verification response..." << std::endl;

    // Capture the output of the candump command
    FILE *fp = popen(command, "r");
    if (fp == nullptr) {
        perror("Error opening pipe");
        return EXIT_FAILURE;
    }

    // Read the verification response from the pipe
    if (std::fgets(receivedMessage, sizeof(receivedMessage), fp) == nullptr) {
        perror("Error reading verification response");
        pclose(fp);
        return EXIT_FAILURE;
    }
    // Print the received CAN message
    std::cout << "Received CAN message: " << receivedMessage << std::endl;

    // Compare the received verification response
    if (std::strstr(receivedMessage, expectedVerificationResponse) != nullptr) {
        std::cout << "Verification Success" << std::endl;
        // Verification response matched, send the original CAN message
        std::snprintf(command, sizeof(command), "cansend %s %s", canInterface, originalCanMessage);
        result = std::system(command);
        if (result != 0) {
            std::cerr << "Error sending original CAN message." << std::endl;
            return EXIT_FAILURE;
        }
    } else {
        std::cerr << "Verification response did not match expected response." << std::endl;
        return EXIT_FAILURE;
    }
    pclose(fp);
    return EXIT_SUCCESS;
}

int main() {
    Can_communication("can0", 250000, "123#000400");
    return 0;
}

