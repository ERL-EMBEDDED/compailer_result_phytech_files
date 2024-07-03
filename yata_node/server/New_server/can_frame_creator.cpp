#include <iostream>
#include "can_frame_creator.h"
#include "global.h"
CANMessage::CANMessage() : data(0) {}

void CANMessage::setBits(uint16_t value, uint8_t start_bit, uint8_t length) {
    uint64_t mask = ((1ULL << length) - 1) << start_bit;
    data = (data & ~mask) | ((static_cast<uint64_t>(value) << start_bit) & mask);
}

void CANMessage::setBit(uint8_t value, uint8_t bit) {
    if (value)
        data |= (1ULL << bit);
    else
        data &= ~(1ULL << bit);
}

void CANMessage::display() const {
    std::cout << "CAN Message: " << std::hex << data << std::dec << std::endl;
}

std::ostream& operator<<(std::ostream& os, const CANMessage& msg) {
    os << std::hex << msg.data << std::dec;
    return os;
}

CANMessage createCANMessage(float Expected_wheelSpeedFR, float Expected_wheelSpeedFL, float Expected_vehicleSpeed) {
    std::cout << "Expected_wheelSpeedFR: " << Expected_wheelSpeedFR << std::endl;
    std::cout << "Expected_wheelSpeedFL: " << Expected_wheelSpeedFL << std::endl;
    std::cout << "Expected_vehicleSpeed: " << Expected_vehicleSpeed << std::endl;

    CANMessage msg;

    uint8_t vehicleSpeedCRC = _config_vehicleSpeedCRC_Max;  // Example value
    uint8_t vehicleSpeedClock = _config_vehicleSpeedClock_Max;

    uint16_t wheelSpeedFR = static_cast<uint16_t>(Expected_wheelSpeedFR / _config_wheelSpeedFR_Scale);
    uint16_t wheelSpeedFL = static_cast<uint16_t>(Expected_wheelSpeedFL / _config_wheelSpeedFL_Scale);
    uint16_t vehicleSpeed = static_cast<uint16_t>(Expected_vehicleSpeed / _config_vehicleSpeed_Scale);

    // Set bits in the CAN message
    msg.setBits(wheelSpeedFR, _config_wheelSpeedFR_start_bit, _config_wheelSpeedFR_length);       // Wheel Speed FR: Bits 7 to 22
    msg.setBits(wheelSpeedFL, _config_wheelSpeedFL_start_bit, _config_wheelSpeedFL_length);      // Wheel Speed FL: Bits 23 to 38
    msg.setBits(vehicleSpeed, _config_vehicleSpeed_start_bit, _config_vehicleSpeed_length);      // Vehicle Speed: Bits 39 to 54
    msg.setBits(vehicleSpeedClock, _config_vehicleSpeedClock_start_bit, _config_vehicleSpeedClock_length);  // Vehicle Speed Clock: Bits 51 to 54

    // Set Vehicle Speed CRC, splitting into two parts
    msg.setBits((vehicleSpeedCRC >> 2) & 0x3F, 1, 6);  // Upper 6 bits of CRC: Bits 1 to 6
    msg.setBit(vehicleSpeedCRC & 0x01, 0);             // Lower bit 0 of CRC: Bit 0
    msg.setBit((vehicleSpeedCRC >> 1) & 0x01, 63);     // Lower bit 1 of CRC: Bit 63

    return msg;
}

