#include "json_parser.h"
#include <iostream>
#include <string>
#include <unordered_map>

bool parseConfigMessage(const std::string &message) {
    try {
        auto jsonObj = json::parse(message);
        if (jsonObj.at("mode").get<std::string>() == "config") {
            CONFIG = "yes";
            const auto& signals = jsonObj.at("data")[0].at("signals");

            static const std::unordered_map<std::string, std::function<void(const json&)>> configMap = {
                {"WheelSpeedFR", [](const json& signal) {
                    _config_wheelSpeedFR_Min = signal.at("minimum").get<float>();
                    _config_wheelSpeedFR_Max = signal.at("maximum").get<float>();
                    _config_wheelSpeedFR_Scale = signal.at("scale").get<float>();
                    _config_wheelSpeedFR_start_bit = signal.at("start_bit").get<int>();
                    _config_wheelSpeedFR_length = signal.at("bit_length").get<int>();
                }},
                {"WheelSpeedFL", [](const json& signal) {
                    _config_wheelSpeedFL_Min = signal.at("minimum").get<float>();
                    _config_wheelSpeedFL_Max = signal.at("maximum").get<float>();
                    _config_wheelSpeedFL_Scale = signal.at("scale").get<float>();
                    _config_wheelSpeedFL_start_bit = signal.at("start_bit").get<int>();
                    _config_wheelSpeedFL_length = signal.at("bit_length").get<int>();
                }},
                {"VehicleSpeed", [](const json& signal) {
                    _config_vehicleSpeed_Min = signal.at("minimum").get<float>();
                    _config_vehicleSpeed_Max = signal.at("maximum").get<float>();
                    _config_vehicleSpeed_Scale = signal.at("scale").get<float>();
                    _config_vehicleSpeed_start_bit = signal.at("start_bit").get<int>();
                    _config_vehicleSpeed_length = signal.at("bit_length").get<int>();
                }},
                {"VehicleSpeedCRC", [](const json& signal) {
                    _config_vehicleSpeedCRC_Min = signal.at("minimum").get<int>();
                    _config_vehicleSpeedCRC_Max = signal.at("maximum").get<int>();
                    _config_vehicleSpeedCRC_start_bit = signal.at("start_bit").get<int>();
                    _config_vehicleSpeedCRC_length = signal.at("bit_length").get<int>();
                    _config_vehicleSpeedCRC_Scale = signal.at("scale").get<float>();
                }},
                {"VehicleSpeedClock", [](const json& signal) {
                    _config_vehicleSpeedClock_start_bit = signal.at("start_bit").get<int>();
                    _config_vehicleSpeedClock_length = signal.at("bit_length").get<int>();
                    _config_vehicleSpeedClock_Scale = signal.at("scale").get<float>();
                    _config_vehicleSpeedClock_Max = signal.at("maximum").get<int>();
                    _config_vehicleSpeedClock_Min = signal.at("minimum").get<int>();
                }}
            };

            for (const auto& signal : signals) {
                std::string name = signal.at("name").get<std::string>();
                if (configMap.find(name) != configMap.end()) {
                    configMap.at(name)(signal);
                }
            }

            // Print values for verification
            std::cout << "Config Yes:\n";
            std::cout << "WheelSpeedFR: Scale=" << _config_wheelSpeedFR_Scale << ", start_bit=" << _config_wheelSpeedFR_start_bit << ", Length=" << _config_wheelSpeedFR_length << "\n";
            std::cout << "WheelSpeedFL: Scale=" << _config_wheelSpeedFL_Scale << ", start_bit=" << _config_wheelSpeedFL_start_bit << ", Length=" << _config_wheelSpeedFL_length << "\n";
            std::cout << "VehicleSpeed: Scale=" << _config_vehicleSpeed_Scale << ", start_bit=" << _config_vehicleSpeed_start_bit << ", Length=" << _config_vehicleSpeed_length << "\n";

            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON in parseConfigMessage: " << e.what() << std::endl;
        return false;
    }
    return false;
}

std::tuple<bool, float, float, float> parseJson_read(const std::string &message) {
    try {
        auto jsonObj = json::parse(message);
        std::string configValue = jsonObj.at("mode").get<std::string>();

        if (configValue == "run") {
            float expectedWheelSpeedFR = jsonObj.at("WheelSpeedFR").get<float>();
            float expectedWheelSpeedFL = jsonObj.at("WheelSpeedFL").get<float>();
            float expectedVehicleSpeed = jsonObj.at("VehicleSpeed").get<float>();
            return {false, expectedWheelSpeedFR, expectedWheelSpeedFL, expectedVehicleSpeed};
        } else if (configValue == "config") {
            parseConfigMessage(message);
            return {true, 0.0, 0.0, 0.0};
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON in parseJson_read: " << e.what() << std::endl;
    }
    return {false, 0.0, 0.0, 0.0};
}

std::string parseJson_create(float expectedWheelSpeedFR, float expectedWheelSpeedFL, float expectedVehicleSpeed) {
    json jsonObj;
    
    std::cout << "Actual vehicle speed: " << expectedVehicleSpeed << std::endl;

    jsonObj["Expected_wheelSpeedFR"] = expectedWheelSpeedFR;
    jsonObj["Expected_wheelSpeedFL"] = expectedWheelSpeedFL;
    jsonObj["Expected_vehicleSpeed"] = expectedVehicleSpeed;

    return jsonObj.dump();
}
