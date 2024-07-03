#include "client.h"
#include "can.h"
#include <iostream>
#include "globals.h"

// Function to send a 16-bit integer value within the range of 0 to 500
void CanInterface::sendEncodedData(CanInterface* canInterface) {
    // Scale the value to fit within 16 bits (0 to 65535)
    uint16_t scaledValue = static_cast<uint16_t>((expectedVehicleSpeed / 500.0) * 65535);

    // Prepare CAN frame
    struct can_frame frame;
    frame.can_id = 0x123;                      // Arbitration ID
    frame.can_dlc = sizeof(scaledValue);       // Data length for 16-bit value
    frame.data[0] = scaledValue & 0xFF;        // LSB
    frame.data[1] = (scaledValue >> 8) & 0xFF; // MSB

    // Send the CAN frame
    if (canInterface->sendFrame(&frame)) {
        std::cout << "CAN frame sent successfully." << std::endl;
    } else {
        std::cerr << "Failed to send CAN frame." << std::endl;
    }
}

// Function to receive a CAN frame and extract a 16-bit integer value
uint16_t receiveCanData(CanInterface* canInterface) {
    struct can_frame frame;

    // Receive a CAN frame
    if (!canInterface->receiveFrame(&frame)) {
        std::cerr << "Failed to receive CAN frame." << std::endl;
        return 0;
    }

    // Extract the 16-bit value from the received CAN frame
    uint16_t receivedValue = static_cast<uint16_t>((frame.data[1] << 8) | frame.data[0]);

    // Return the received value
    return receivedValue;
}

// Function to decode the received CAN data
void CanInterface::decodeReceiveData() {
    CanInterface canInterface("can0");
    uint16_t receivedValue = receiveCanData(&canInterface);
    std::cout << "Received CAN data: " << receivedValue << std::endl;
    // Scale the received value back to the range of 0 to 500
    actualVehicleSpeed = (receivedValue / 65535.0) * 500;
    std::cout << "Decoded CAN data: " << actualVehicleSpeed << std::endl;
}

int main() {
    ClientTcpip tcpip;
    tcpip.connectServer();
    
    CanInterface can("can0");
    can.initCan("can0");

    bool running = true;
    while (running) {
        // Receive vehicle speed from TCP/IP
        tcpip.receiveVehicleSpeed();
        
        // Send vehicle speed over CAN
        can.sendEncodedData(&can);
        
        // Receive encoded data from CAN and decode it
        can.decodeReceiveData();
        
        // Send vehicle speed to TCP/IP
        tcpip.sendVehicleSpeed();
    }

    // Disconnect TCP/IP client
    tcpip.disconnect();

    return 0;
}

