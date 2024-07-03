#include "json_parser.h"
#include "global.h"

std::string JsonParser::parseAndGenerateResponse(const std::string& receivedMessage) {
    try {
        auto jsonObj = json::parse(receivedMessage);
        std::string configValue = jsonObj.at("config").get<std::string>();

        if (configValue == "yes") {
            auto signals = jsonObj.at("data")[0].at("signals");

            // Assuming configuration variables are global or accessible through extern
            // Modify as per your actual global variable names
            // Example usage:
            for (const auto& signal : signals) {
                std::string name = signal.at("name").get<std::string>();

                if (name == "WheelSpeedFR") {
                    // Assign configuration values
                    _config_wheelSpeedFR_Min = signal.at("minimum").get<float>();
                    _config_wheelSpeedFR_Max = signal.at("maximum").get<float>();
                    _config_wheelSpeedFR_Scale = signal.at("scale").get<float>();
                    _config_wheelSpeedFR_start_bit = signal.at("start_bit").get<int>();
                    _config_wheelSpeedFR_length = signal.at("bit_length").get<int>();
                } else if (name == "WheelSpeedFL") {
                    // Assign configuration values
                    _config_wheelSpeedFL_Min = signal.at("minimum").get<float>();
                    _config_wheelSpeedFL_Max = signal.at("maximum").get<float>();
                    _config_wheelSpeedFL_Scale = signal.at("scale").get<float>();
                    _config_wheelSpeedFL_start_bit = signal.at("start_bit").get<int>();
                    _config_wheelSpeedFL_length = signal.at("bit_length").get<int>();
                } else if (name == "VehicleSpeed") {
                    // Assign configuration values
                    _config_vehicleSpeed_Min = signal.at("minimum").get<float>();
                    _config_vehicleSpeed_Max = signal.at("maximum").get<float>();
                    _config_vehicleSpeed_Scale = signal.at("scale").get<float>();
                    _config_vehicleSpeed_start_bit = signal.at("start_bit").get<int>();
                    _config_vehicleSpeed_length = signal.at("bit_length").get<int>();
                } else if (name == "VehicleSpeedCRC") {
                    // Assign configuration values
                    _config_vehicleSpeedCRC_Min = signal.at("minimum").get<int>();
                    _config_vehicleSpeedCRC_Max = signal.at("maximum").get<int>();
                    _config_vehicleSpeedCRC_start_bit = signal.at("start_bit").get<int>();
                    _config_vehicleSpeedCRC_length = signal.at("bit_length").get<int>();
                    _config_vehicleSpeedCRC_Scale = signal.at("scale").get<float>();
                } else if (name == "VehicleSpeedClock") {
                    // Assign configuration values
                    _config_vehicleSpeedClock_start_bit = signal.at("start_bit").get<int>();
                    _config_vehicleSpeedClock_length = signal.at("bit_length").get<int>();
                    _config_vehicleSpeedClock_Scale = signal.at("scale").get<float>();
                    _config_vehicleSpeedClock_Max = signal.at("maximum").get<int>();
                    _config_vehicleSpeedClock_Min = signal.at("minimum").get<int>();
                }
            }

            // Generate response message
            json response;
            response["config"] = "acknowledge"; // Modify response as needed
            return response.dump(); // Convert JSON to string
        } else if (configValue == "no") {
            // Handle 'no' configuration case
            // Example: Extract expected values and perform operations
            float Expected_wheelSpeedFR = jsonObj.at("WheelSpeedFR").get<float>();
            float Expected_wheelSpeedFL = jsonObj.at("WheelSpeedFL").get<float>();
            float Expected_vehicleSpeed = jsonObj.at("VehicleSpeed").get<float>();

            // Example: send_can_data(Expected_wheelSpeedFR, Expected_wheelSpeedFL, Expected_vehicleSpeed);

            // Generate response message
            json response;
            response["config"] = "reject"; // Modify response as needed
            return response.dump(); // Convert JSON to string
        }
    } catch (const std::exception& e) {
        // Handle JSON parsing exceptions
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
    }

    return ""; // Default return, handle appropriately in actual implementation
}

