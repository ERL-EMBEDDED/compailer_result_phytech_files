// globals.cpp
#include "globals.h"

int actualVehicleSpeed = 0;
int expectedVehicleSpeed = 0;

// Mutexes for synchronization
std::mutex actualSpeedMutex;
std::mutex expectedSpeedMutex;

