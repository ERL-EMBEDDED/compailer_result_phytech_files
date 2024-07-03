#include "can.h"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <cstdint>

CanInterface::CanInterface(const char* ifname) {
    // Set down the CAN interface
    if (system("ip link set can0 down") != 0) {
        perror("Error setting down can0 interface");
        exit(1);
    }

    // Set CAN interface type and bitrate
    char command[100];
    snprintf(command, sizeof(command), "ip link set %s type can bitrate 250000", ifname);
    if (system(command) != 0) {
        perror("Error setting up can0 interface");
        exit(1);
    }

    // Set up the CAN interface
    if (system("ip link set can0 up") != 0) {
        perror("Error setting up can0 interface");
        exit(1);
    }

    // Create a socket
    if ((socket_ = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Get interface index
    struct ifreq ifr;
    strcpy(ifr.ifr_name, ifname);
    if (ioctl(socket_, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl failed");
        close(socket_);
        exit(1);
    }

    // Bind the socket to the CAN interface
    addr_.can_family = AF_CAN;
    addr_.can_ifindex = ifr.ifr_ifindex;
    if (bind(socket_, (struct sockaddr *)&addr_, sizeof(addr_)) < 0) {
        perror("Binding failed");
        close(socket_);
        exit(1);
    }
}

CanInterface::~CanInterface() {
    close(socket_);
}

bool CanInterface::sendFrame(struct can_frame* frame) {
    // Send the CAN frame
    if (write(socket_, frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("Write failed");
        return false;
    }
    return true;
}

void sendValue(uint16_t value, CanInterface* canInterface) {
    // Scale the value to fit within 16 bits (0 to 65535)
    uint16_t scaledValue = static_cast<uint16_t>((value / 500.0) * 65535);

    // Prepare CAN frame
    struct can_frame frame;
    frame.can_id = 0x123; // Arbitration ID
    frame.can_dlc = 2;    // Data length for 16-bit value
    frame.data[0] = scaledValue & 0xFF;         // LSB
    frame.data[1] = (scaledValue >> 8) & 0xFF;  // MSB

    // Send the CAN frame
    if (canInterface->sendFrame(&frame)) {
        std::cout << "CAN frame sent\n";
    }
}

