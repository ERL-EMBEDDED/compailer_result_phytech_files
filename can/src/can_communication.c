#include "can_communication.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *CanThread(void *args) {
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    Can_communication(threadArgs->canInterface, threadArgs->bitrate, threadArgs->originalCanMessage);
    return NULL;
}

int StartCanCommunicationThread(const char *canInterface, int bitrate, const char *originalCanMessage) {
    pthread_t thread;
    struct ThreadArgs threadArgs = {canInterface, bitrate, originalCanMessage};

    int threadResult = pthread_create(&thread, NULL, CanThread, (void *)&threadArgs);
    if (threadResult != 0) {
        perror("Error creating thread");
        return EXIT_FAILURE;
    }

    pthread_join(thread, NULL);  // Wait for the thread to finish

    return EXIT_SUCCESS;
}

int Can_communication(const char *canInterface, int bitrate, const char *originalCanMessage) {
    // Your existing Can_communication function implementation...
    return EXIT_SUCCESS;
}
