#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <string>
#include <tuple>
#include "json.hpp" // Assuming json.hpp is in the same directory or adjust the path accordingly
#include "global.h"

using json = nlohmann::json;

bool parseConfigMessage(const std::string &message);
std::tuple<bool, float, float, float> parseJson_read(const std::string &message);
std::string parseJson_create(float Expected_wheelSpeedFR, float Expected_wheelSpeedFL, float Expected_vehicleSpeed);

#endif // JSON_PARSER_H
