#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H


std::string readJsonFromFile(const std::string& filename);
int config_file_data();
int modifyFile(const std::string& jsonMessage);
bool writeJsonToFile(const std::string& filename, const std::string& jsonMessage);

#endif
