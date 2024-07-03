#include <iostream>
#include <fstream>
#include "json_read.h"  // Assuming you have implemented these files correctly

int main() {
    // JSON input string (as provided)
    std::string buffer = R"({
        "messages": [
            {
                "name": "VDC_A9:",
                "id_hex": 650,
                "length": 0,
                "signals": [
                    {
                        "name": "WheelSpeedFR",
                        "start_bit": 7,
                        "bit_length": 16,
                        "is_big_endian": true,
                        "is_signed": false,
                        "scale": 0.041667,
                        "offset": 0,
                        "minimum": 0,
                        "maximum": 2730.58,
                        "unit": "rpm"
                    }
                ]
            }
        ]
    })";

   JsonReader reader(buffer);

    if (reader.parse()) {
        const std::vector<Message>& messages = reader.getMessages();

        for (const auto& msg : messages) {
            std::cout << "Message: " << msg.name << ", ID: " << msg.id_hex << std::endl;

            for (const auto& sig : msg.signals) {
                std::cout << "  Signal: " << sig.name << std::endl;
                std::cout << "    Start Bit: " << sig.start_bit << std::endl;
                std::cout << "    Bit Length: " << sig.bit_length << std::endl;
                std::cout << "    Scale: " << sig.scale << std::endl;
                std::cout << "    Offset: " << sig.offset << std::endl;
                std::cout << "    Minimum: " << sig.minimum << std::endl;
                std::cout << "    Maximum: " << sig.maximum << std::endl;
                std::cout << "    Unit: " << sig.unit << std::endl;
            }
        }
    } else {
        std::cerr << "Failed to parse JSON." << std::endl;
        return 1;
    }

    return 0;
}

