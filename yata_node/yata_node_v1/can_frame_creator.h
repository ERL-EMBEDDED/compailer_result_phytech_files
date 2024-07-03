#ifndef CAN_FRAME_CREATOR_H
#define CAN_FRAME_CREATOR_H

#include "can.h"

// Function declarations
void createCANMessage(CANMessage& msg, float Expected_wheelSpeedFR, float Expected_wheelSpeedFL, float Expected_vehicleSpeed);

#endif // CAN_FRAME_CREATOR_H

