#ifndef GLOBALS_H
#define GLOBALS_H

#include <mutex>
extern unsigned int actualVehicleSpeed; // Global variable for actual vehicle speed
extern unsigned int expectedVehicleSpeed; // Global variable for expected vehicle speed

extern std::mutex actualSpeedMutex;
extern std::mutex expectedSpeedMutex;

#endif // GLOBALS_H

