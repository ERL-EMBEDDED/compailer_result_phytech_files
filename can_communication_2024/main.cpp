// main.cpp
#include "server.h"
#include "can.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <memory>

int main() {
     // Connect to TCP/IP server
    ServerTcpip server;
    server.startServer();
 
    // Initialize CAN interface
    CanInterface can("can0");
    can.initCan("can0");


    // Run the threads for CAN and TCP/IP communication
    std::thread tcpipThread(&ServerTcpip::run, &server); // Pass member function pointer and object instance
    std::thread canThread(&CanInterface::run, &can); // Pass member function pointer and object instance

    // Join the threads (will never reach here in this example)
    tcpipThread.join();
    canThread.join();

    return 0;
}
