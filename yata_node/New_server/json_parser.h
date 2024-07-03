#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include<iostream>
#include<string>
#include "global.h"
#include <tuple>
#include "json.hpp" 
using json = nlohmann::json;

std::tuple<int, float, float, float>parseJson_read(const std::string& parser_readMessage);
std::string parseJson_create(float Expected_wheelSpeedFR  ,float Expected_wheelSpeedFL ,float Expected_vehicleSpeed);

#endif // JSON_PARSER_H