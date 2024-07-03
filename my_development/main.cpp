#include "client.h"
#include "can.h"
#include <thread>

int main() {
    // Initialize and run client thread
    ClientTcpip client;
    std::thread clientThread([&]() {
        client.run();
    });

    // Initialize and run CAN thread
    CanInterface can("can0");
    std::thread canThread([&]() {
        can.run();
    });

    // Join both threads to wait for their completion
    clientThread.join();
    canThread.join();

    return 0;
}

