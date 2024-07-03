#include "can_communication.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int Can_communication(const char *canInterface, const int bitrate, const char *originalCanMessage) {
    const char* verificationMessage = "123#000400";
    const char* expectedVerificationResponse = "can0  00000123   [3]  00 02 00";
    char receivedMessage[100];

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

    // Send verification CAN message
    snprintf(command, sizeof(command), "cansend %s %s", canInterface, verificationMessage);
    result = system(command);
    if (result != 0) {
        fprintf(stderr, "Error sending verification CAN message.\n");
        return EXIT_FAILURE;
    }

    // Read the verification response
    snprintf(command, sizeof(command), "candump %s", canInterface);
    printf("Waiting for verification response...\n");

    // Capture the output of the candump command
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        perror("Error opening pipe");
        return EXIT_FAILURE;
    }

    // Read the verification response from the pipe
    if (fgets(receivedMessage, sizeof(receivedMessage), fp) == NULL) {
        perror("Error reading verification response");
        pclose(fp);
        return EXIT_FAILURE;
    }

    // Print the received CAN message
    printf("Received CAN message: %s\n", receivedMessage);

    // Compare the received verification response
    if (strstr(receivedMessage, expectedVerificationResponse) != NULL) {
        printf("Verification Success \n");

        // Verification response matched, send the original CAN message
        snprintf(command, sizeof(command), "cansend %s %s", canInterface, originalCanMessage);
        result = system(command);
        if (result != 0) {
            fprintf(stderr, "Error sending original CAN message.\n");
            return EXIT_FAILURE;
        }
    } else {
        fprintf(stderr, "Verification response did not match expected response.\n");
        return EXIT_FAILURE;
    }

    pclose(fp);
    return EXIT_SUCCESS;
}

void *CanThread(void *args) {
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    Can_communication(threadArgs->canInterface, threadArgs->bitrate, threadArgs->originalCanMessage);
    return NULL;
}

