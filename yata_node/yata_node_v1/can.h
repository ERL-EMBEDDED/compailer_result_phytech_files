#ifndef CAN_H
#define CAN_H

#include <iostream>
#include <bitset>
#include <iomanip>
#include <chrono>
#include <string>
#include <cstring>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <boost/endian/conversion.hpp>
#include <unistd.h>

// CANMessage class declaration
class CANMessage {
public:
    CANMessage();
    void setBits(uint64_t value, int startBit, int bitLength);
    void setBit(uint64_t value, int bitPosition);
    void display() const;
    uint64_t getData() const; // Getter method for data
    friend std::ostream& operator<<(std::ostream& os, const CANMessage& msg);

private:
    uint64_t data;
};

std::ostream& operator<<(std::ostream& os, const CANMessage& msg);

// CanInterface class declaration
class CanInterface {
public:
    CanInterface();
    ~CanInterface();
    void ReceivedData();
    void send_can_data(float Expected_wheelSpeedFR, float Expected_wheelSpeedFL, float Expected_vehicleSpeed);
    void run();
    bool initCan();
    int socket_;
    struct sockaddr_can addr_;
private:
    bool sendFrame(struct can_frame* frame);
    bool receiveFrame(struct can_frame* frame);
};

#endif // CAN_H

