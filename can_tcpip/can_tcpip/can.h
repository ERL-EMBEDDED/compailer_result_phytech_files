#ifndef CAN_H
#define CAN_H

#include "globals.h"
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

class CanInterface {
private:
    int socket_;
    struct sockaddr_can addr_;

public:
    CanInterface(const char*);  // Constructor
    void sendEncodedData(CanInterface* canInterface); // Declaration
    void decodeReceiveData(); 
    ~CanInterface();
    bool sendFrame(struct can_frame* frame);
    bool receiveFrame(struct can_frame* frame);
    bool initCan(const char* ifname);
};

#endif // CAN_H

