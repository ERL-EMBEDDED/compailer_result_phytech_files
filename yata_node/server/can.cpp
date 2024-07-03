#include "can.h"
#include "global.h"
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <sstream>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <iomanip>

bool endian_flag = false;
_conversion convo;
_byte_convo _by_convo;

int CRC = 0;

CANMessage::CANMessage() : data(0) {}

CanInterface::CanInterface() {
    if (!initCan()) {
        std::cerr << "Initialization of CAN interface failed." << std::endl;
        exit(1);
    }
}

CanInterface::~CanInterface() {
    close(socket_);
}
static std::string getCurrentDateTimeseconds() {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_tm = *std::localtime(&now_time_t);

    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
    auto micro = std::chrono::duration_cast<std::chrono::microseconds>(duration).count() % 1000;
    auto nano = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() % 1000;

    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%Y-%m-%dT%H:%M:%S") << '.' << std::setw(3) << std::setfill('0') << millis
        << std::setw(3) << std::setfill('0') << micro << std::setw(3) << std::setfill('0') << nano;

        return oss.str();
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

std::ostream& operator<<(std::ostream& os, const CANMessage& msg) {
    os << "CAN Message (hex): 0x" << std::hex << std::setw(16) << std::setfill('0') << msg.data;
    return os;
}


bool CanInterface::initCan() {
    // Set down the CAN interface
    const char* ifname = "can0";
    if (system("ip link set can0 down") != 0) {
        perror("Error setting down can0 interface");
        return false;
    }

    // Set CAN interface type and bitrate
    char command[100];
    snprintf(command, sizeof(command), "ip link set %s type can bitrate 250000", ifname);
    if (system(command) != 0) {
        perror("Error setting up can0 interface");
        return false;
    }

    // Set up the CAN interface
    if (system("ip link set can0 up") != 0) {
        perror("Error setting up can0 interface");
        return false;
    }

    // Create a socket
    if ((socket_ = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Socket creation failed");
        return false;
    }

    // Get interface index
    struct ifreq ifr;
    strcpy(ifr.ifr_name, ifname);
    if (ioctl(socket_, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl failed");
        close(socket_);
        return false;
    }

    // Bind the socket to the CAN interface
    addr_.can_family = AF_CAN;
    addr_.can_ifindex = ifr.ifr_ifindex;
    if (bind(socket_, (struct sockaddr *)&addr_, sizeof(addr_)) < 0) {
        perror("Binding failed");
        close(socket_);
        return false;
    }

    return true;
}

bool CanInterface::sendFrame(struct can_frame* frame) {
    // Send the CAN frame
    if (write(socket_, frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("Write failed");
        return false;
    }
    return true;
}

bool CanInterface::receiveFrame(struct can_frame* frame) {
    // Receive a CAN frame
    if (read(socket_, frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("Read failed");
        return false;
    }
    return true;
}


void CanInterface::ReceivedData() {
    // Receive a CAN frame
    struct can_frame frame;
    if (!receiveFrame(&frame)) {
        std::cerr << "Failed to receive CAN frame." << std::endl;
        return;
    }

    // Reverse endian and reconstruct the data from frame
    uint64_t big_endian = 0;
    std::memcpy(&big_endian, frame.data, sizeof(big_endian));
    uint64_t little_endian = boost::endian::endian_reverse(big_endian);
    uint8_t byteArray[8];
     std::memcpy(byteArray, &little_endian, sizeof(little_endian));
    std::cout <<"--->>>"<<getCurrentDateTimeseconds() << " Recevive CAN data " << std::hex << little_endian << std::endl;
  /*  for(int i =0; i < 8; ++i)
    {
	    std::cout << "byteArray" << i << " : " << static_cast<int>(byteArray[i]) << std::endl;

    }
    */
    // Extract values directly from frame.data
    uint16_t can_id = frame.can_id;
    // uint16_t vehicle_speed =byteArray[1];
    float vehicle_speed = (byteArray[2] << 8) | byteArray[1];
    // Print all the extracted values for debugging
    std::cout << "Received Values: " << std::endl;
    std::cout << "can id : " << can_id << std::endl;
    std::cout << "Vehicle Speed: " << vehicle_speed << std::endl;

   _hold_ = true; 
    server.sendMessage(vehicle_speed); 
   _hold_ = false; 

}

void CanInterface::send_can_data(float Expected_wheelSpeedFR,float Expected_wheelSpeedFL,float Expected_vehicleSpeed) {
     
    std::cout << "Expected_wheelSpeedFR: " << Expected_wheelSpeedFR<< std::endl;
    std::cout << "Expected_wheelSpeedFL: " << Expected_wheelSpeedFL << std::endl;
    std::cout << "Expected_vehicleSpeed: " << Expected_vehicleSpeed << std::endl;

    CANMessage msg;

    uint8_t vehicleSpeedCRC = _config_vehicleSpeedCRC_Max;  // Example value
    uint8_t vehicleSpeedClock = _config_vehicleSpeedClock_Max;

    uint16_t wheelSpeedFR = static_cast<uint16_t>(Expected_wheelSpeedFR / _config_wheelSpeedFR_Scale);
    uint16_t wheelSpeedFL = static_cast<uint16_t>(Expected_wheelSpeedFL / _config_wheelSpeedFL_Scale);
    uint16_t vehicleSpeed = static_cast<uint16_t>(Expected_vehicleSpeed/ _config_vehicleSpeed_Scale);

    // Set bits in the CAN message
    msg.setBits(wheelSpeedFR, _config_wheelSpeedFR_start_bit, _config_wheelSpeedFR_length);       // Wheel Speed FR: Bits 7 to 22
    msg.setBits(wheelSpeedFL, _config_wheelSpeedFL_start_bit, _config_wheelSpeedFL_length);      // Wheel Speed FL: Bits 23 to 38
    msg.setBits(vehicleSpeed, _config_vehicleSpeed_start_bit, _config_vehicleSpeed_length);      // Vehicle Speed: Bits 39 to 54
    msg.setBits(vehicleSpeedClock,_config_vehicleSpeedClock_start_bit, _config_vehicleSpeedClock_length);  // Vehicle Speed Clock: Bits 51 to 54

    // Set Vehicle Speed CRC, splitting into two parts
    msg.setBits((vehicleSpeedCRC >> 2) & 0x3F, 1, 6);  // Upper 6 bits of CRC: Bits 1 to 6
    msg.setBit(vehicleSpeedCRC & 0x01, 0);             // Lower bit 0 of CRC: Bit 0
    msg.setBit((vehicleSpeedCRC >> 1) & 0x01, 63);     // Lower bit 1 of CRC: Bit 63

    msg.display();
    std::cout << msg << std::endl;

    struct can_frame frame;
    frame.can_id = 0x123;  // Example CAN ID
    frame.can_dlc = 8;  // CAN data length
    std::memcpy(frame.data, &msg.data, sizeof(msg.data));

    // Send the CAN frame
    if (sendFrame(&frame)) {
        std::cout << "--->>>"<<getCurrentDateTimeseconds() << " CAN frame sent successfully." << std::endl;
    } else {
        std::cerr << "Failed to send CAN frame." << std::endl;
    }
}

void CanInterface::run() {
    while(true)
    {
        std::cout << "CAN Thread is running" << std::endl;
        ReceivedData();
	usleep(1);
    }    
}
