#include "can_frame_creator.h"
#include "global.h"
#include <iostream>

// Constants for bit positions and lengths
constexpr uint8_t CRC_UPPER_START_BIT = 1;
constexpr uint8_t CRC_UPPER_LENGTH = 6;
constexpr uint8_t CRC_LOWER_BIT_0 = 0;
constexpr uint8_t CRC_LOWER_BIT_1 = 63;

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

uint16_t CANMessage::extract(uint8_t start, uint8_t length) const {
    return (data >> start) & ((1ULL << length) - 1);
}

CANMessage CANMessage::createCANMessage(float Expected_wheelSpeedFR, float Expected_wheelSpeedFL, float Expected_vehicleSpeed) {
    CANMessage msg;

    // Example values for demonstration
    constexpr uint8_t vehicleSpeedCRC = 0x12;
    constexpr uint8_t vehicleSpeedClock = 0x34;

    uint16_t wheelSpeedFR = static_cast<uint16_t>(Expected_wheelSpeedFR / _config_wheelSpeedFR_Scale);
    uint16_t wheelSpeedFL = static_cast<uint16_t>(Expected_wheelSpeedFL / _config_wheelSpeedFL_Scale);
    uint16_t vehicleSpeed = static_cast<uint16_t>(Expected_vehicleSpeed / _config_vehicleSpeed_Scale);

    // Set bits in the CAN message
    msg.setBits(wheelSpeedFR, _config_wheelSpeedFR_start_bit, _config_wheelSpeedFR_length);
    msg.setBits(wheelSpeedFL, _config_wheelSpeedFL_start_bit, _config_wheelSpeedFL_length);
    msg.setBits(vehicleSpeed, _config_vehicleSpeed_start_bit, _config_vehicleSpeed_length);
    msg.setBits(vehicleSpeedClock, _config_vehicleSpeedClock_start_bit, _config_vehicleSpeedClock_length);

    // Set Vehicle Speed CRC
    msg.setBits((vehicleSpeedCRC >> 2) & 0x3F, CRC_UPPER_START_BIT, CRC_UPPER_LENGTH);
    msg.setBit(vehicleSpeedCRC & 0x01, CRC_LOWER_BIT_0);
    msg.setBit((vehicleSpeedCRC >> 1) & 0x01, CRC_LOWER_BIT_1);

    return msg;
}

DecodedCANMessage decodeCANMessage(const CANMessage& msg) {
    DecodedCANMessage decoded;

    // Extract values from the CAN message using the extract method
    decoded.wheelSpeedFR = static_cast<float>(msg.extract(_config_wheelSpeedFR_start_bit, _config_wheelSpeedFR_length)) * _config_wheelSpeedFR_Scale;
    decoded.wheelSpeedFL = static_cast<float>(msg.extract(_config_wheelSpeedFL_start_bit, _config_wheelSpeedFL_length)) * _config_wheelSpeedFL_Scale;
    decoded.vehicleSpeed = static_cast<float>(msg.extract(_config_vehicleSpeed_start_bit, _config_vehicleSpeed_length)) * _config_vehicleSpeed_Scale;
    decoded.vehicleSpeedClock = static_cast<uint8_t>(msg.extract(_config_vehicleSpeedClock_start_bit, _config_vehicleSpeedClock_length));

    // Extract Vehicle Speed CRC
    decoded.vehicleSpeedCRC = static_cast<uint8_t>(msg.extract(CRC_UPPER_START_BIT, CRC_UPPER_LENGTH) << 2);
    decoded.vehicleSpeedCRC |= (msg.extract(CRC_LOWER_BIT_0, 1) & 0x01);
    decoded.vehicleSpeedCRC |= (msg.extract(CRC_LOWER_BIT_1, 1) & 0x01) << 1;

    return decoded;
}

std::ostream& operator<<(std::ostream& os, const CANMessage& msg) {
    os << "CAN Message: " << std::hex << msg.data << std::dec;
    return os;
}
