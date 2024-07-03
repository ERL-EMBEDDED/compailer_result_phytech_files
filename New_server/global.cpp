#include "global.h"

std::shared_ptr<int> actualVehicleSpeed = std::make_shared<int>(0);
std::shared_ptr<int> expectedVehicleSpeed = std::make_shared<int>(0);

float _config_wheelSpeedFR_Min;
float _config_wheelSpeedFR_Max;
float _config_wheelSpeedFR_Scale;
int _config_wheelSpeedFR_start_bit;
int _config_wheelSpeedFR_length;

float _config_wheelSpeedFL_Min;
float _config_wheelSpeedFL_Max;
float _config_wheelSpeedFL_Scale;
int _config_wheelSpeedFL_start_bit;
int _config_wheelSpeedFL_length;

float _config_vehicleSpeed_Min;
float _config_vehicleSpeed_Max;
float _config_vehicleSpeed_Scale;
int _config_vehicleSpeed_start_bit;
int _config_vehicleSpeed_length;

int _config_vehicleSpeedCRC_start_bit;
int _config_vehicleSpeedCRC_length;
float _config_vehicleSpeedCRC_Scale;
int _config_vehicleSpeedCRC_Min;
int _config_vehicleSpeedCRC_Max;

int _config_vehicleSpeedClock_start_bit;
int _config_vehicleSpeedClock_length;
int _config_vehicleSpeedClock_Min;
int _config_vehicleSpeedClock_Max;
float _config_vehicleSpeedClock_Scale;
