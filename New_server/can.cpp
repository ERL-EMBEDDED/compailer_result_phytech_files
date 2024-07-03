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
#include "can_frame_creator.h"

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
void CanInterface::received_can_data() {
    // Receive a CAN frame
    struct can_frame frame;
    if (!receiveFrame(&frame)) {
        std::cerr << "Failed to receive CAN frame." << std::endl;
        return;
    }
    std::cout <<"--->>>"<<getCurrentDateTimeseconds() << " Recevive CAN id " << std::hex <<frame.id << std::endl;
    std::cout <<"--->>>"<<getCurrentDateTimeseconds() << " Recevive CAN data[0] " << std::hex << frame.data[0]<< std::endl;
    std::cout <<"--->>>"<<getCurrentDateTimeseconds() << " Recevive CAN data[1] " << std::hex <<frame.data[1] << std::endl;

    float Expected_wheelSpeedFR =76.894532;
    float Expected_wheelSpeedFL =45.437832;
    float Expected_vehicleSpeed =65.346578;
    _hold_ = true; 
    server.Tcpip_sendMessage(Expected_wheelSpeedFR  ,Expected_wheelSpeedFL , Expected_vehicleSpeed); 
    _hold_ = false; 

}
void CanInterface::send_can_data(float Expected_wheelSpeedFR,float Expected_wheelSpeedFL,float Expected_vehicleSpeed) {

    CANMessage createCANMessage(Expected_wheelSpeedFR,Expected_wheelSpeedFL, Expected_vehicleSpeed);
    
    std::cout <<<"can_message_create" <<< msg << std::endl;
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
        received_can_data();
	usleep(1);
    }    
}
