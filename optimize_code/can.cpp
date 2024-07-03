#include "can.h"
#include "global.h"
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <cstring>
#include "can_frame_creator.h"
#include "config_file.h"

using namespace std::chrono_literals;

bool endian_flag = false;
_conversion convo;
_byte_convo _by_convo;

int CRC = 0;

CanInterface::CanInterface() {
    if (!initCan()) {
        std::cerr << "Initialization of CAN interface failed." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

CanInterface::~CanInterface() {
    close(socket_);
}

static std::string getCurrentDateTime() {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_tm = *std::localtime(&now_time_t);

    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration) % 1000;
    auto micros = std::chrono::duration_cast<std::chrono::microseconds>(duration) % 1000;
    auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(duration) % 1000;

    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%Y-%m-%dT%H:%M:%S")
        << '.' << std::setw(3) << std::setfill('0') << millis.count()
        << std::setw(3) << std::setfill('0') << micros.count()
        << std::setw(3) << std::setfill('0') << nanos.count();

    return oss.str();
}

bool CanInterface::initCan() {
    const char* ifname = "can0";
    if (system("ip link set can0 down") != 0) {
        perror("Error setting down can0 interface");
        return false;
    }

    char command[100];
    snprintf(command, sizeof(command), "ip link set %s type can bitrate 250000", ifname);
    if (system(command) != 0) {
        perror("Error setting up can0 interface");
        return false;
    }

    if (system("ip link set can0 up") != 0) {
        perror("Error setting up can0 interface");
        return false;
    }

    if ((socket_ = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Socket creation failed");
        return false;
    }

    struct ifreq ifr;
    strcpy(ifr.ifr_name, ifname);
    if (ioctl(socket_, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl failed");
        close(socket_);
        return false;
    }

    addr_.can_family = AF_CAN;
    addr_.can_ifindex = ifr.ifr_ifindex;
    if (bind(socket_, (struct sockaddr *)&addr_, sizeof(addr_)) < 0) {
        perror("Binding failed");
        close(socket_);
        return false;
    }

    if (CONFIG != "yes") {
        config_file_data();
    }

    return true;
}

void CanInterface::run() {
    while (true) {
        std::cout << "CAN Thread is running" << std::endl;
        received_can_data();
        std::this_thread::sleep_for(1ms);
    }
}

bool CanInterface::sendFrame(struct can_frame* frame) {
    if (write(socket_, frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("Write failed");
        return false;
    }
    return true;
}

bool CanInterface::receiveFrame(struct can_frame* frame) {
    if (read(socket_, frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("Read failed");
        return false;
    }
    return true;
}

void CanInterface::received_can_data() {
    struct can_frame frame;
    if (!receiveFrame(&frame)) {
        std::cerr << "Failed to receive CAN frame." << std::endl;
        return;
    }

    uint64_t receive_msg = 0;
    std::memcpy(&receive_msg, frame.data, sizeof(receive_msg));

    std::cout << "--->>>" << getCurrentDateTime() << " Receive CAN data " << std::hex << receive_msg << std::endl;
    DecodedCANMessage decoded = decodeCANMessage(*reinterpret_cast<CANMessage*>(&receive_msg));

    server.Tcpip_sendMessage(decoded.wheelSpeedFR, decoded.wheelSpeedFL, decoded.vehicleSpeed);
}

void CanInterface::send_can_data(float Expected_wheelSpeedFR, float Expected_wheelSpeedFL, float Expected_vehicleSpeed) {
    CANMessage msg = CANMessage::createCANMessage(Expected_wheelSpeedFR, Expected_wheelSpeedFL, Expected_vehicleSpeed);
    
    DecodedCANMessage decoded = decodeCANMessage(msg);

    struct can_frame frame;
    frame.can_id = 0x123;  // Example CAN ID
    frame.can_dlc = 8;     // CAN data length
    std::memcpy(frame.data, &msg.data, sizeof(msg.data));

    if (sendFrame(&frame)) {
        std::cout << "--->>>" << getCurrentDateTime() << " CAN frame sent successfully." << std::endl;
    } else {
        std::cerr << "Failed to send CAN frame." << std::endl;
    }
}
