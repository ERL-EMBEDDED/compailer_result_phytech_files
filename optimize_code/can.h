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
#include <boost/endian/conversion.hpp>
#include <bitset>
#include <iostream>
#include <iomanip>

typedef union {
    uint64_t value;
    uint8_t values[8];
} _conversion;

typedef union {
    uint16_t value;
    uint8_t values[2];
} _byte_convo;

extern bool endian_flag;
extern _conversion convo;
extern _byte_convo _by_convo;

class CanInterface {
public:
    CanInterface();
    ~CanInterface();
    bool initCan();
    void run();
    bool sendFrame(struct can_frame* frame);
    bool receiveFrame(struct can_frame* frame);
    void send_can_data(float Expected_wheelSpeedFR,float Expected_wheelSpeedFL,float Expected_vehicleSpeed);
    void received_can_data();

private:
    int socket_;
    struct sockaddr_can addr_;
};

#endif // CAN_H
