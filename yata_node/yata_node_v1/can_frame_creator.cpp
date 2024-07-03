#include "can_frame_creator.h"
#include "global.h"

void createCANMessage(CANMessage& msg, float Expected_wheelSpeedFR, float Expected_wheelSpeedFL, float Expected_vehicleSpeed) {
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
}

void CANMessage::setBits(uint64_t value, int startBit, int bitLength) {
    uint64_t mask = ((1ULL << bitLength) - 1) << startBit;
    data = (data & ~mask) | ((value << startBit) & mask);
}

void CANMessage::setBit(uint64_t value, int bitPosition) {
    if (value)
        data |= (1ULL << bitPosition);
    else
        data &= ~(1ULL << bitPosition);
}

void CANMessage::display() const {
    std::bitset<64> bits(data);
    std::cout << "CAN Message (binary): " << bits << std::endl;
}

uint64_t CANMessage::getData() const {
    return data;
}

