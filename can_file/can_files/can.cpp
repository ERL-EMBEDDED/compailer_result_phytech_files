// can.cpp
#include "can.h"

CanInterface::CanInterface(const char* ifname) {
    if (!initCan(ifname)) {
        std::cerr << "Initialization of CAN interface failed." << std::endl;
        exit(1);
    }
}

CanInterface::~CanInterface() {
    close(socket_);
}

bool CanInterface::initCan(const char* ifname) {
    // Set down the CAN interface
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

int CanInterface::receiveCanData() {
    struct can_frame frame;
    int can_Receive_hexData;
    // Receive a CAN frame
    if (!receiveFrame(&frame)) {
        std::cerr << "Failed to receive CAN frame." << std::endl;
        return 0;
    }

    // Strip leading zeros from CAN frame ID
    uint16_t frameID = frame.can_id;
    while (frameID > 0 && frameID % 10 == 0) {
        frameID /= 10;
    }

    // Print the received CAN frame ID without leading zeros
    std::cout << "Received CAN frame ID: " << std::hex << frameID << std::dec << std::endl;

    // Print each byte of the received CAN frame data separately
    std::cout << "Data: ";
    can_Receive_hexData = 0;
    for (int i = 0; i < frame.can_dlc; ++i) {
        can_Receive_hexData = (can_Receive_hexData << 8) | frame.data[i];
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(frame.data[i]) << " ";
    }
    std::cout << std::endl;
    

    return can_Receive_hexData;
}

void CanInterface::decodeAndSend() {
 {
    std::lock_guard<std::mutex> lock(actualSpeedMutex);
    int receiveValue = receiveCanData();
     // Divide receiveValue by 65535 and multiply by 500
    actualVehicleSpeed = (static_cast<double>(receiveValue) / 65535.0) * 500.0;
    std::cout << "actualVehilespeed " << actualVehicleSpeed << std::endl;
   }

}

void CanInterface::sendEncodedData() {
    {
        std::lock_guard<std::mutex> lock(expectedSpeedMutex);
        // Scale the value to fit within 16 bits (0 to 65535)
        uint16_t scaledValue = static_cast<uint16_t>((expectedVehicleSpeed / 500.0) * 65535);

        // Prepare CAN frame
        struct can_frame frame;
        frame.can_id = 0x123;                      // Arbitration ID
        frame.can_dlc = sizeof(scaledValue);       // Data length for 16-bit value
        frame.data[0] = scaledValue & 0xFF;        // LSB
        frame.data[1] = (scaledValue >> 8) & 0xFF; // MSB

        // Send the CAN frame
        if (sendFrame(&frame)) {
            std::cout << "CAN frame sent successfully." << std::endl;
        } else {
            std::cerr << "Failed to send CAN frame." << std::endl;
        }
    }
}

void CanInterface::run() {
    while(true) {
        sendEncodedData();
        decodeAndSend();
        // Add a delay to prevent excessive resource usage
        usleep(100000); // 100ms delay
    }
}


