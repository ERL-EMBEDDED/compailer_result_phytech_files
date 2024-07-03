#include <iostream>
#include <thread>
#include "can.h"
#include "client.h"
#include <unistd.h> // Include for 'close' function

void canThread() {
    const char *ifname = "can0"; // Change this to your CAN interface name
    int s;

    // Initialize the CAN interface
    if (can_init(ifname, &s) != 0) {
        std::cerr << "Initialization failed\n";
        return;
    }

    uint32_t can_id = 0x123; // Change this to your desired CAN ID
    uint8_t can_dlc = 2;     // Data length for 16-bit value

    // Example value to send via CAN
    int value = 1000;

    // Continuously send CAN data
    while (true) {
        // Send CAN data
        if (can_send(s, can_id, can_dlc, value) != 0) {
            std::cerr << "Sending failed\n";
            break;
        }

        // Sleep for 1 second before sending again
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Close the CAN socket when done
    ::close(s); // Use '::' to refer to the global 'close' function
}

int main() {
    // Start the CAN thread
    std::thread can(canThread);

    // Start the client thread
    std::thread client(clientThread);

    // Wait for both threads to finish
    can.join();
    client.join();

    return 0;
}

