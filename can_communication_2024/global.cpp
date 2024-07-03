// global.cpp
#include "global.h"

// Define std::shared_ptr for actualVehicleSpeed and expectedVehicleSpeed
std::shared_ptr<unsigned int> actualVehicleSpeed = std::make_shared<unsigned int>(0);
std::shared_ptr<unsigned int> expectedVehicleSpeed = std::make_shared<unsigned int>(0);

// Mutexes for synchronization
std::mutex actualSpeedMutex;
std::mutex expectedSpeedMutex;
