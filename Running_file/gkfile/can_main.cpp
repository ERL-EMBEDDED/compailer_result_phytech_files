// can_main.cpp
#include "can.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <memory>

int main() {
 
   // Initialize CAN interface
    CanInterface can("can0");
    can.initCan("can0");


    // Run the threads for CAN and TCP/IP communication
    std::thread canThread(&CanInterface::run, &can); // Pass member function pointer and object instance

    // Join the threads (will never reach here in this example)
    canThread.join();

    return 0;
}
