// globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

#include <mutex>

extern int actualVehicleSpeed;
extern int expectedVehicleSpeed;

extern std::mutex actualSpeedMutex;
extern std::mutex expectedSpeedMutex;

#endif // GLOBALS_H

