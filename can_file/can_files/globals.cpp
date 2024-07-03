// globals.cpp
#include "globals.h"
#include <mutex>

double actualVehicleSpeed = 0;
double expectedVehicleSpeed = 0;

// Mutexes for synchronization
std::mutex actualSpeedMutex;
std::mutex expectedSpeedMutex;

