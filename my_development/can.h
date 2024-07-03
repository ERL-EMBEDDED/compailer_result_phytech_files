#ifndef CAN_H
#define CAN_H

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <cmath>
#include <iostream>

class CanInterface {
public:
    CanInterface(const char* ifname);
    ~CanInterface();
    bool initCan(const char* ifname);
    void run(); // Added for running the CanInterface
    bool sendFrame(struct can_frame* frame);
    bool receiveFrame(struct can_frame* frame);
    void sendEncodedData(); // Modified to remove the parameter
    void decodeAndSend(); // Added to decode received data and send
    uint16_t receiveCanData(); // Added to receive CAN data
    bool isConnected() const; // Declaration of isConnected function

private:
    int socket_;
    struct sockaddr_can addr_;
};

#endif // CAN_H

