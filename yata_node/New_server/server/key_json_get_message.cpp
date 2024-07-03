#include <iostream>
#include <string>
#include "json.hpp"

using json = nlohmann::json;

// Function to simulate getting the current date and time
std::string getCurrentDateTimeseconds() {
    // Placeholder for actual date-time function
    return "2024-06-13 12:00:00 ";
}

// Function to simulate sending data over CAN (dummy implementation)
class CanSimulator {
public:
    void sendEncodedData(float speed, float speed2, float speed3) {
        // Placeholder for actual CAN send implementation
        std::cout << "Sending encoded data: " << speed << ", " << speed2 << ", " << speed3 << std::endl;
    }
};

int main() {

      std::string buffer = R"({"config":"yes","data":[{"name":"VDC_A9:","id_hex":650,"length":0,"signals":[{"name":"WheelSpeedFR","start_bit":7,"bit_length":16,"is_big_endian":true,"is_signed":false,"scale":0.041667,"offset":0,"minimum":0,"maximum":2730.58,"unit":"rpm"},{"name":"WheelSpeedFL","start_bit":23,"bit_length":16,"is_big_endian":true,"is_signed":false,"scale":0.041667,"offset":0,"minimum":0,"maximum":2730.58,"unit":"rpm"},{"name":"VehicleSpeedCRC","start_bit":63,"bit_length":8,"is_big_endian":true,"is_signed":false,"scale":1,"offset":0,"minimum":0,"maximum":255,"unit":""},{"name":"VehicleSpeedClock","start_bit":51,"bit_length":4,"is_big_endian":true,"is_signed":false,"scale":1,"offset":0,"minimum":0,"maximum":15,"unit":""},{"name":"VehicleSpeed","start_bit":39,"bit_length":16,"is_big_endian":true,"is_signed":false,"scale":0.01,"offset":0,"minimum":0,"maximum":655.34,"unit":"km/h"}]}]})";


    //std::string buffer = R"({"config":"no","WheelSpeedFR":76.767754,"WheelSpeedFL":98.654321,"VehicleSpeed":54.875423})";

    // Variable declarations
    float Expected_wheelSpeedFR_Min;
    float Expected_wheelSpeedFR_Max;
    float Expected_wheelSpeedFL_Min;
    float Expected_wheelSpeedFL_Max;
    float Expected_vehicleSpeed_Min;
    float Expected_vehicleSpeed_Max;
    float _config_wheelSpeedFR_Scale;
    float _config_wheelSpeedFL_Scale;
    float _config_vehicleSpeed_Scale;

    int _config_vehicleSpeedCRC;
    int _config_vehicleSpeedClock;

    float _config_vehicleSpeedCRC_scale;
    float _config_vehicleSpeedClock_scale;

    int _config_wheelSpeedFR_start_bit;
    int _config_wheelSpeedFL_start_bit;
    int _config_vehicleSpeed_start_bit;
    int _config_vehicleSpeedCRC_start_bit;
    int _config_SpeedClock_start_bit;

    int _config_wheelSpeedFR_length;
    int _config_wheelSpeedFL_length;
    int _config_vehicleSpeed_length;
    int _config_vehicleSpeedCRC_length;
    int _config_vehicleSpeedClock_length;

    float Expected_wheelSpeedFR;
    float Expected_wheelSpeedFL;
    float Expected_vehicleSpeed;

    std::string receivedMessage(buffer);
    std::cout << "--->>>" << getCurrentDateTimeseconds() << "Received from client: " << receivedMessage << std::endl;

    try {
        auto jsonObj = json::parse(receivedMessage);
        std::string configValue = jsonObj.at("config").get<std::string>();

        if (configValue == "yes") {
            auto signals = jsonObj.at("data")[0].at("signals");

            for (const auto& signal : signals) {
                std::string name = signal.at("name").get<std::string>();

                if (name == "WheelSpeedFR") {
                    Expected_wheelSpeedFR_Min = signal.at("minimum").get<float>();
                    Expected_wheelSpeedFR_Max = signal.at("maximum").get<float>();
                    _config_wheelSpeedFR_Scale = signal.at("scale").get<float>();
                    _config_wheelSpeedFR_start_bit = signal.at("start_bit").get<int>();
                    _config_wheelSpeedFR_length = signal.at("bit_length").get<int>();
                } else if (name == "WheelSpeedFL") {
                    Expected_wheelSpeedFL_Min = signal.at("minimum").get<float>();
                    Expected_wheelSpeedFL_Max = signal.at("maximum").get<float>();
                    _config_wheelSpeedFL_Scale = signal.at("scale").get<float>();
                    _config_wheelSpeedFL_start_bit = signal.at("start_bit").get<int>();
                    _config_wheelSpeedFL_length = signal.at("bit_length").get<int>();
                } else if (name == "VehicleSpeed") {
                    Expected_vehicleSpeed_Min = signal.at("minimum").get<float>();
                    Expected_vehicleSpeed_Max = signal.at("maximum").get<float>();
                    _config_vehicleSpeed_Scale = signal.at("scale").get<float>();
                    _config_vehicleSpeed_start_bit = signal.at("start_bit").get<int>();
                    _config_vehicleSpeed_length = signal.at("bit_length").get<int>();
                } else if (name == "VehicleSpeedCRC") {
                    _config_vehicleSpeedCRC_start_bit = signal.at("start_bit").get<int>();
                    _config_vehicleSpeedCRC_length = signal.at("bit_length").get<int>();
                    _config_vehicleSpeedCRC_scale = signal.at("scale").get<int>();
                } else if (name == "VehicleSpeedClock") {
                    _config_SpeedClock_start_bit = signal.at("start_bit").get<int>();
                    _config_vehicleSpeedClock_length = signal.at("bit_length").get<int>();
                }
            }

            // Print values for verification
            std::cout << "Config Yes:\n";
            std::cout << "WheelSpeedFR: Min=" << Expected_wheelSpeedFR_Min << ", Max=" << Expected_wheelSpeedFR_Max << ", Scale=" << _config_wheelSpeedFR_Scale << ", start_bit=" << _config_wheelSpeedFR_start_bit << "\n";
            std::cout << "WheelSpeedFL: Min=" << Expected_wheelSpeedFL_Min << ", Max=" << Expected_wheelSpeedFL_Max << ", Scale=" << _config_wheelSpeedFL_Scale << "\n";
            std::cout << "VehicleSpeed: Min=" << Expected_vehicleSpeed_Min << ", Max=" << Expected_vehicleSpeed_Max << ", Scale=" << _config_vehicleSpeed_Scale << "\n";
            std::cout << "VehicleSpeedCRC start bit=" << _config_vehicleSpeedCRC_start_bit << ", length=" << _config_vehicleSpeedCRC_length << ", scale=" << _config_vehicleSpeedCRC_scale << "\n";
            std::cout << "VehicleSpeedClock start bit=" << _config_SpeedClock_start_bit << ", length=" << _config_vehicleSpeedClock_length << "\n";

        } else if (configValue == "no") {
            Expected_wheelSpeedFR = jsonObj.at("WheelSpeedFR").get<float>();
            Expected_wheelSpeedFL = jsonObj.at("WheelSpeedFL").get<float>();
            Expected_vehicleSpeed = jsonObj.at("VehicleSpeed").get<float>();

            // Print values for verification
            std::cout << "Config No:\n";
            std::cout << "Expected_wheelSpeedFR: " << Expected_wheelSpeedFR << "\n";
            std::cout << "Expected_wheelSpeedFL: " << Expected_wheelSpeedFL << "\n";
            std::cout << "Expected_vehicleSpeed: " << Expected_vehicleSpeed << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
    }

    return 0;
}

