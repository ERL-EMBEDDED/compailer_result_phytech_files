#include <iostream>
#include <linux/can.h>

int main() {
    struct can_frame frame; // Assuming 'frame' is a structure defined in linux/can.h
    std::cout << "Size of frame: " << sizeof(frame) << " bytes" << std::endl;
    return 0;
}

