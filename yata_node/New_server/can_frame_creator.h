#ifndef CAN_FRAME_CREATOR_H
#define CAN_FRAME_CREATOR_H

#include <cstdint>
#include <iostream>
class CANMessage {
public:
    uint64_t data;

    CANMessage();

    void setBits(uint16_t value, uint8_t start_bit, uint8_t length);
    void setBit(uint8_t value, uint8_t bit);
    void display() const;
    CANMessage createCANMessage(float Expected_wheelSpeedFR, float Expected_wheelSpeedFL, float Expected_vehicleSpeed);

    friend std::ostream& operator<<(std::ostream& os, const CANMessage& msg);
};

#endif // CAN_FRAME_CREATOR_H
