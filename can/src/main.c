
#include <stdio.h>
#include <stdlib.h>
#include "can_communication.h"

int main() {
    int result = StartCanCommunicationThread("can0", 250000, "123#000400");

    if (result != EXIT_SUCCESS) {
        fprintf(stderr, "Error starting CAN communication thread\n");
        return EXIT_FAILURE;
    }

    return 0;
}
