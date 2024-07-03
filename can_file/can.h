// can.h
#ifndef CAN_H
#define CAN_H

#include <linux/can.h>

class CanInterface {
public:
    CanInterface(const char* ifname);
    ~CanInterface();

    bool sendFrame(struct can_frame* frame);
    bool receiveFrame(struct can_frame* frame);

private:
    int socket_;
    struct sockaddr_can addr_;
};

#endif // CAN_H

