#include <stdio.h>
#include <stdlib.h>

int main() {
    const char* canInterface = "can0";
    const int bitrate = 250000;
    const char* canMessage = "123#0190"; // Replace with your CAN message

    // Set down the can interface
    int result = system("ip link set can0 down");
    if (result != 0) {
        fprintf(stderr, "Error setting down can0 interface.\n");
        return EXIT_FAILURE;
    }

    // Set can interface type and bitrate
    char command[100];
    snprintf(command, sizeof(command), "ip link set %s type can bitrate %d", canInterface, bitrate);
    result = system(command);
    if (result != 0) {
        fprintf(stderr, "Error setting up can0 interface.\n");
        return EXIT_FAILURE;
    }

    // Set up the can interface
    result = system("ip link set can0 up");
    if (result != 0) {
        fprintf(stderr, "Error setting up can0 interface.\n");
        return EXIT_FAILURE;
    }

    // Send the CAN message
    snprintf(command, sizeof(command), "cansend %s %s", canInterface, canMessage);
    result = system(command);
    if (result != 0) {
        fprintf(stderr, "Error sending CAN message.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

