#include "can.h"
#include <iostream>
#include <thread>
#include <chrono>

void receiveData(CanInterface* canInterface) {
    // Buffer for received CAN frame
    struct can_frame frame;

    while (true) {
        // Receive a CAN frame
        if (canInterface->receiveFrame(&frame)) {
            std::cout << "Received CAN frame with ID: 0x" << std::hex << frame.can_id << std::dec << "\n";
            std::cout << "Data Length: " << static_cast<int>(frame.can_dlc) << "\n";
            std::cout << "Data: ";
            for (int i = 0; i < frame.can_dlc; ++i) {
                std::cout << std::hex << static_cast<int>(frame.data[i]) << " ";
            }
            std::cout << std::dec << "\n";
        } else {
            std::cerr << "Failed to receive CAN frame\n";
            // You might want to add error handling here or take appropriate action
        }

        // Sleep for a short duration to avoid busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    // Create a CAN interface object
    CanInterface canInterface("can0");

    // Example CAN frame for sending
    struct can_frame frame;
    frame.can_id = 0x123; // Example CAN ID
    frame.can_dlc = 3;    // Example data length
    frame.data[0] = 0xAA; // Example data byte 1
    frame.data[1] = 0xBB; // Example data byte 2
    frame.data[2] = 0xCC; // Example data byte 3

    // Send the CAN frame
    if (canInterface.sendFrame(&frame)) {
        std::cout << "CAN frame sent successfully\n";
    } else {
        std::cerr << "Failed to send CAN frame\n";
        return 1;
    }

    // Create a thread for receiving CAN data
    std::thread receiveThread(receiveData, &canInterface);

    // Main thread can continue doing other tasks or just wait
    receiveThread.join();

    return 0;
}

