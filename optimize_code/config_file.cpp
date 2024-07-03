#include "json_parser.h"
#include <iostream>
#include <fstream>
#include "config_file.h"

// Function to read JSON content from a file
std::string readJsonFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        return content;
    } else {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return "";
    }
}

// Function to write JSON content to a file
bool writeJsonToFile(const std::string& filename, const std::string& jsonMessage) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << jsonMessage;
        file.close();
        return true;
    } else {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }
}

// Function to read config file data and parse it
int config_file_data() {
    std::string jsonMessage = readJsonFromFile("config_data.json");

    if (jsonMessage.empty()) {
        std::cerr << "Failed to read JSON data from file." << std::endl;
        return 1;
    }

    parseConfigMessage(jsonMessage);

    return 0;
}

// Function to modify and overwrite JSON data in a file
int modifyFile(const std::string& jsonMessage) {
    if (!writeJsonToFile("config_data.json", jsonMessage)) {
        std::cerr << "Failed to write JSON data to file." << std::endl;
        return 1;
    } else {
        std::cout << "File Overwritten Successfully" << std::endl;
    }

    return 0;
}
