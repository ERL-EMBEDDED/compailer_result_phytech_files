#include "json_parser.h"
#include <iostream>

std::tuple<int, float, float, float> parseJson_read(const std::string& parser_readMessage) {

    std::cout << "Parser Input String: " << parser_readMessage << std::endl;

    try {
        auto jsonObj = json::parse(parser_readMessage);
        std::string configValue = jsonObj.at("config").get<std::string>();

        if (configValue == "yes") {
            auto signals = jsonObj.at("data")[0].at("signals");

            for (const auto& signal : signals) {
                std::string name = signal.at("name").get<std::string>();

                if (name == "WheelSpeedFR") {
                    _config_wheelSpeedFR_Min = signal.at("minimum").get<float>();
                    _config_wheelSpeedFR_Max = signal.at("maximum").get<float>();
                    _config_wheelSpeedFR_Scale = signal.at("scale").get<float>();
                    _config_wheelSpeedFR_start_bit = signal.at("start_bit").get<int>();
                    _config_wheelSpeedFR_length = signal.at("bit_length").get<int>();
                } else if (name == "WheelSpeedFL") {
                    _config_wheelSpeedFL_Min = signal.at("minimum").get<float>();
                    _config_wheelSpeedFL_Max = signal.at("maximum").get<float>();
                    _config_wheelSpeedFL_Scale = signal.at("scale").get<float>();
                    _config_wheelSpeedFL_start_bit = signal.at("start_bit").get<int>();
                    _config_wheelSpeedFL_length = signal.at("bit_length").get<int>();
                } else if (name == "VehicleSpeed") {
                    _config_vehicleSpeed_Min = signal.at("minimum").get<float>();
                    _config_vehicleSpeed_Max = signal.at("maximum").get<float>();
                    _config_vehicleSpeed_Scale = signal.at("scale").get<float>();
                    _config_vehicleSpeed_start_bit = signal.at("start_bit").get<int>();
                    _config_vehicleSpeed_length = signal.at("bit_length").get<int>();
                } else if (name == "VehicleSpeedCRC") {
                    _config_vehicleSpeedCRC_start_bit = signal.at("start_bit").get<int>();
                    _config_vehicleSpeedCRC_length = signal.at("bit_length").get<int>();
                    _config_vehicleSpeedCRC_Scale = signal.at("scale").get<int>();
                } else if (name == "VehicleSpeedClock") {
                    _config_vehicleSpeedClock_start_bit = signal.at("start_bit").get<int>();
                    _config_vehicleSpeedClock_length = signal.at("bit_length").get<int>();
                }
            }

            // Print values for verification
            std::cout << "Config Yes:\n";
            std::cout << "WheelSpeedFR: Min=" << _config_wheelSpeedFR_Min << ", Max=" << _config_wheelSpeedFR_Max << ", Scale=" << _config_wheelSpeedFR_Scale << ", start_bit=" << _config_wheelSpeedFR_start_bit << "\n";
            std::cout << "WheelSpeedFL: Min=" << _config_wheelSpeedFL_Min << ", Max=" << _config_wheelSpeedFL_Max << ", Scale=" << _config_wheelSpeedFL_Scale << "\n";
            std::cout << "VehicleSpeed: Min=" << _config_vehicleSpeed_Min << ", Max=" << _config_vehicleSpeed_Max << ", Scale=" << _config_vehicleSpeed_Scale << "\n";
            std::cout << "VehicleSpeedCRC start bit=" << _config_vehicleSpeedCRC_start_bit << ", length=" << _config_vehicleSpeedCRC_length << ", scale=" << _config_vehicleSpeedCRC_Scale << "\n";
            std::cout << "VehicleSpeedClock start bit=" <<_config_vehicleSpeedClock_start_bit << ", length=" <<_config_vehicleSpeedClock_length << "\n";

             int status = 0; 
            return std::make_tuple(status ,_config_wheelSpeedFR_Max ,_config_wheelSpeedFL_Max ,_config_vehicleSpeed_Max );

        } else if (configValue == "no") {
            float Expected_wheelSpeedFR = jsonObj.at("WheelSpeedFR").get<float>();
            float Expected_wheelSpeedFL = jsonObj.at("WheelSpeedFL").get<float>();
            float Expected_vehicleSpeed = jsonObj.at("VehicleSpeed").get<float>();

            // Print values for verification
            std::cout << "Config No:\n";
            std::cout << "Expected_wheelSpeedFR: " << Expected_wheelSpeedFR << "\n";
            std::cout << "Expected_wheelSpeedFL: " << Expected_wheelSpeedFL << "\n";
            std::cout << "Expected_vehicleSpeed: " << Expected_vehicleSpeed << "\n";
            int status = 0; 
            return std::make_tuple(status ,Expected_wheelSpeedFR ,Expected_wheelSpeedFL ,Expected_vehicleSpeed );

        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
    }
    return std::make_tuple(-1, 0.0f, 0.0f, 0.0f); // Return error status
}
std::string parseJson_create(float Expected_wheelSpeedFR  ,float Expected_wheelSpeedFL ,float Expected_vehicleSpeed){

    json jsonobj;

    jsonobj["Expected_wheelSpeedFR"] =Expected_wheelSpeedFR;
    jsonobj["Expected_wheelSpeedFL"] =Expected_wheelSpeedFL;
    jsonobj["Expected_vehicleSpeed"] =Expected_vehicleSpeed;

    std::string Json_create_string =jsonobj.dump();
    std::cout<< "Create Jsaon String  : " <<Json_create_string<<std::endl;

    return Json_create_string;
}

