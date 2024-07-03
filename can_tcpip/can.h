#ifndef CAN_H
#define CAN_H

#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <cstdint>

class CanInterface {
public:
    CanInterface(const char* ifname);
    ~CanInterface();
    bool sendFrame(struct can_frame* frame);

private:
    int socket_;
    struct sockaddr_can addr_;
};

extern CanInterface canInterface;

void sendValue(uint16_t value, CanInterface* canInterface);

#endif // CAN_H

