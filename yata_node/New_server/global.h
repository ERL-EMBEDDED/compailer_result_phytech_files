#ifndef GLOBAL_H
#define GLOBAL_H

#include <memory>
#include "server.h"
#include "can.h"
#include "can_frame_creator.h"

extern epsServer server;
extern CanInterface can;
extern bool _hold_;

typedef enum{
	INIT,
	WAIT,
	RUNNNING
}TCP_IP_t;

extern TCP_IP_t SERVER_STATES;

extern void send_can_data(float Expected_wheelSpeedFR,float Expected_wheelSpeedFL,float Expected_vehicleSpeed);
extern int Tcpip_sendMessage(float Expected_wheelSpeedFR  ,float Expected_wheelSpeedFL ,float Expected_vehicleSpeed);
extern CANMessage createCANMessage(float Expected_wheelSpeedFR, float Expected_wheelSpeedFL, float Expected_vehicleSpeed);

extern float _config_wheelSpeedFR_Min;
extern float _config_wheelSpeedFR_Max;
extern float _config_wheelSpeedFL_Min;
extern float _config_wheelSpeedFL_Max;
extern float _config_vehicleSpeed_Min;
extern float _config_vehicleSpeed_Max;
extern int _config_vehicleSpeedCRC_Min;
extern int _config_vehicleSpeedCRC_Max;
extern int _config_vehicleSpeedClock_Min;
extern int _config_vehicleSpeedClock_Max;

extern float _config_wheelSpeedFR_Scale;
extern float _config_wheelSpeedFL_Scale;
extern float _config_vehicleSpeed_Scale;

extern float _config_vehicleSpeedCRC_Scale;
extern float _config_vehicleSpeedClock_Scale;

extern int _config_vehicleSpeedCRC;
extern int _config_vehicleSpeedClock;

extern int _config_wheelSpeedFR_start_bit;
extern int _config_wheelSpeedFL_start_bit;
extern int _config_vehicleSpeed_start_bit;
extern int _config_vehicleSpeedCRC_start_bit;
extern int _config_vehicleSpeedClock_start_bit;


extern int _config_wheelSpeedFR_length;
extern int _config_wheelSpeedFL_length;
extern int _config_vehicleSpeed_length;
extern int _config_vehicleSpeedCRC_length;
extern int _config_vehicleSpeedClock_length;

extern std::shared_ptr<int> actualVehicleSpeed;
extern std::shared_ptr<int> expectedVehicleSpeed;

#endif // GLOBAL_H

