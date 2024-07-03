// can.h
#ifndef CAN_H
#define CAN_H

#include "globals.h"
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <cstdio>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <mutex>
#include <cmath>

class CanInterface {
public:
    CanInterface(const char* ifname);
    ~CanInterface();
    bool initCan(const char* ifname);
    void run();
    bool sendFrame(struct can_frame* frame);
    bool receiveFrame(struct can_frame* frame);
    void sendEncodedData();
    void decodeAndSend();
    int receiveCanData();

private:
    int socket_;
    struct sockaddr_can addr_;
};

#endif // CAN_H


