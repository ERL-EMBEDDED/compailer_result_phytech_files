#include "client.h"
#include "can.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <memory>

void threadFunction1(ClientTcpip& client, CanInterface& can) {
    while (true) {
        // Run client.sendVehicleSpeed() and can.sendEncodedData() continuously
        client.sendVehicleSpeed();
        can.sendEncodedData();
        std::this_thread::sleep_for(std::chrono::microseconds(500)); // Sleep for 0.5 milliseconds
    }
}

void threadFunction2(ClientTcpip& client, CanInterface& can) {
    while (true) {
        // Run can.decodeAndSend() and client.receiveVehicleSpeed() continuously
        can.decodeAndSend();
        client.receiveVehicleSpeed();
        std::this_thread::sleep_for(std::chrono::microseconds(500)); // Sleep for 0.5 milliseconds
    }
}

int main() {
    // Connect to TCP/IP server
    ClientTcpip client;
    client.connectServer();
 
    // Initialize CAN interface
    CanInterface can("can0");
    can.initCan("can0");

    // Define the threads
    std::thread thread1(threadFunction1, std::ref(client), std::ref(can));
    std::thread thread2(threadFunction2, std::ref(client), std::ref(can));

    // Join the threads to wait for them to finish (which they won't)
    thread1.join();
    thread2.join();

    return 0;
}

