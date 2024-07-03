#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <string>
#include "json.hpp"
#include <iostream>
using json = nlohmann::json;

class JsonParser {
public:
    static std::string parseAndGenerateResponse(const std::string& receivedMessage);
};

#endif // JSON_PARSER_H

