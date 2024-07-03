// globals.cpp
#include "globals.h"

unsigned int actualVehicleSpeed =0;
unsigned int expectedVehicleSpeed = 0;


// Mutexes for synchronization
std::mutex actualSpeedMutex;
std::mutex expectedSpeedMutex;

