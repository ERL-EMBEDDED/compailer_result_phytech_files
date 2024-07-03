// main.cpp
#include "can.h"

int main() {

    // Initialize CAN interface
    CanInterface can("can0");
    can.initCan("can0");
    can.run();

    return 0;
}

