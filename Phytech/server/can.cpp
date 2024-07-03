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

void CanInterface::sendEncodedData(int v_speed) {
    std::cout << "Can send speed : " << v_speed << std::endl;

    convo.values[7] = 111;
    uint16_t _by_convo_value = 1200;
    convo.values[6] = static_cast<uint8_t>(_by_convo_value >> 8);  // Wheel Speed FR (high byte)
    convo.values[5] = static_cast<uint8_t>(_by_convo_value);       // Wheel Speed FR (low byte)
    _by_convo_value = 1200;    // Wheel Speed FL
    convo.values[4] = static_cast<uint8_t>(_by_convo_value >> 8);  // Wheel Speed FL (high byte)
    convo.values[3] = static_cast<uint8_t>(_by_convo_value);       // Wheel Speed FL (low byte)
    _by_convo_value = v_speed;   // Vehicle Speed
    convo.values[2] = static_cast<uint8_t>(_by_convo_value >> 8);  // Vehicle Speed (high byte)
    convo.values[1] = static_cast<uint8_t>(_by_convo_value);       // Vehicle Speed (low byte)
    convo.values[0] = 10;    // Vehicle Speed Clock

  /*  for(int i =0; i <8; ++i)
    {
	std::cout << "convo data: " << static_cast<int>(convo.values[i]) << std::endl;
    }
    */
     uint64_t big_endian = 0;
    for (int i = 0; i < 8; ++i) {
        big_endian |= static_cast<uint64_t>(convo.values[i]) << ((7 - i) * 8);
    }
  
    std::cout << "Received CAN Frame : " << std::hex << big_endian << std::endl;

    // Reverse the endianness of the combined value
    uint64_t big_endian_conv = boost::endian::endian_reverse(big_endian);

    std::cout << "Big-endian value: " << std::hex << big_endian_conv << std::endl;
     uint8_t byteArray[8];
     std::memcpy(byteArray, &big_endian, sizeof(big_endian));
 /*    std::cout << "Bytes in the array:" << std::endl;
    for (int i = 0; i < 8; ++i) {
        std::cout << "Byte " << i << ": " << std::hex << static_cast<int>(byteArray[i]) << std::endl;
    }
      */

    struct can_frame frame;
    frame.can_id = 0x123;
    frame.can_dlc = 8;
   for (int i = 0; i < 8; ++i) {
	   frame.data[i] = byteArray[i];
   }

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
