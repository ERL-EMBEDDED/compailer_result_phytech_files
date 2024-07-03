#include <iostream>
#include <thread>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <chrono>

int value;

// Function to initialize the CAN interface
int can_init(const char* ifname, int* s) {
    // Set down the CAN interface
    if (system("ip link set can0 down") != 0) {
        perror("Error setting down can0 interface");
        return 1;
    }

    // Set CAN interface type and bitrate
    char command[100];
    snprintf(command, sizeof(command), "ip link set %s type can bitrate 250000", ifname);
    if (system(command) != 0) {
        perror("Error setting up can0 interface");
        return 1;
    }

    // Set up the CAN interface
    if (system("ip link set can0 up") != 0) {
        perror("Error setting up can0 interface");
        return 1;
    }

    // Create a socket
    if ((*s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Get interface index
    struct ifreq ifr;
    strcpy(ifr.ifr_name, ifname);
    if (ioctl(*s, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl failed");
        close(*s);
        return 1;
    }

    // Bind the socket to the CAN interface
    struct sockaddr_can addr;
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(*s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Binding failed");
        close(*s);
        return 1;
    }

    return 0; // Success
}

// Function to send CAN data
int can_send(int s, uint32_t can_id, uint8_t can_dlc, int value) {

    // Scale the value to fit within 16 bits (0 to 65535)
    uint16_t scaledValue = static_cast<uint16_t>((value / 500.0) * 65535);

    // Prepare CAN frame data
    uint8_t data[2];
    data[0] = scaledValue & 0xFF;         // LSB
    data[1] = (scaledValue >> 8) & 0xFF;  // MSB

    struct can_frame frame;
    frame.can_id = can_id;
    frame.can_dlc = can_dlc;
    memcpy(frame.data, data, can_dlc);

    // Send the CAN frame
    if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("Write failed");
        return 1;
    }

    std::cout << "CAN frame sent\n";
    return 0; // Success
}

void thread_function(int interval) {
    int s;
    const char *ifname = "can0"; // Change this to your CAN interface name

    // Initialize the CAN interface
    if (can_init(ifname, &s) != 0) {
        std::cerr << "Initialization failed\n";
        return;
    }

    uint32_t can_id = 0x123; // Change this to your desired CAN ID
    uint8_t can_dlc = 2;     // Data length for 16-bit value

    while (true) {
        auto start = std::chrono::high_resolution_clock::now();

        value = 100;

        // Send CAN data
        if (can_send(s, can_id, can_dlc, value) != 0) {
            std::cerr << "Sending failed\n";
            close(s);
            return;
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        // Adjust sleep time based on interval and duration of operation
        int sleep_time = interval - duration.count();
        if (sleep_time > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
    }

    // Close the socket
    close(s);
}

int main() {
    int interval =1; // milliseconds
    std::thread t(thread_function, interval);
    t.join();

    return 0;
}

