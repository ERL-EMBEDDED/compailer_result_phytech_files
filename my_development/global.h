// global.h
#ifndef GLOBAL_H
#define GLOBAL_H

#include <memory>
#include <mutex>

// Declare std::shared_ptr for actualVehicleSpeed and expectedVehicleSpeed
extern std::shared_ptr<unsigned int> actualVehicleSpeed;
extern std::shared_ptr<unsigned int> expectedVehicleSpeed;

extern std::mutex actualSpeedMutex;
extern std::mutex expectedSpeedMutex;

#endif // GLOBAL_H
