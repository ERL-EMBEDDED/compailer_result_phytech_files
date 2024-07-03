#include "json_read.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

class SimpleJsonParser {
public:
    SimpleJsonParser(const std::string& json) : json_(json), pos_(0) {}

    bool parse() {
        skipWhitespace();
        return parseObject();
    }

    const std::vector<Message>& getMessages() const {
        return messages_;
    }

private:
    bool parseObject() {
        if (!match('{')) return false;

        while (true) {
            skipWhitespace();
            std::string key = parseString();
            skipWhitespace();

            if (!match(':')) return false;
            skipWhitespace();

            if (key == "messages") {
                if (!parseMessages()) return false;
            } else {
                skipValue();
            }

            skipWhitespace();
            if (peek() == ',') {
                ++pos_;
                continue;
            } else if (peek() == '}') {
                ++pos_;
                break;
            } else {
                return false;
            }
        }

        return true;
    }

    bool parseMessages() {
        if (!match('[')) return false;

        while (true) {
            skipWhitespace();
            if (peek() == ']') {
                ++pos_;
                break;
            }

            Message message;
            if (!parseMessage(message)) return false;
            messages_.push_back(message);

            skipWhitespace();
            if (peek() == ',') {
                ++pos_;
                continue;
            } else if (peek() == ']') {
                ++pos_;
                break;
            } else {
                return false;
            }
        }

        return true;
    }

    bool parseMessage(Message& message) {
        if (!match('{')) return false;

        while (true) {
            skipWhitespace();
            std::string key = parseString();
            skipWhitespace();

            if (!match(':')) return false;
            skipWhitespace();

            if (key == "name") {
                message.name = parseString();
            } else if (key == "id_hex") {
                message.id_hex = parseNumber();
            } else if (key == "signals") {
                if (!parseSignals(message.signals)) return false;
            } else {
                skipValue();
            }

            skipWhitespace();
            if (peek() == ',') {
                ++pos_;
                continue;
            } else if (peek() == '}') {
                ++pos_;
                break;
            } else {
                return false;
            }
        }

        return true;
    }

    bool parseSignals(std::vector<Signal>& signals) {
        if (!match('[')) return false;

        while (true) {
            skipWhitespace();
            if (peek() == ']') {
                ++pos_;
                break;
            }

            Signal signal;
            if (!parseSignal(signal)) return false;
            signals.push_back(signal);

            skipWhitespace();
            if (peek() == ',') {
                ++pos_;
                continue;
            } else if (peek() == ']') {
                ++pos_;
                break;
            } else {
                return false;
            }
        }

        return true;
    }

    bool parseSignal(Signal& signal) {
        if (!match('{')) return false;

        while (true) {
            skipWhitespace();
            std::string key = parseString();
            skipWhitespace();

            if (!match(':')) return false;
            skipWhitespace();

            if (key == "name") {
                signal.name = parseString();
            } else if (key == "start_bit") {
                signal.start_bit = parseNumber();
            } else if (key == "bit_length") {
                signal.bit_length = parseNumber();
            } else if (key == "is_big_endian") {
                signal.is_big_endian = parseBool();
            } else if (key == "is_signed") {
                signal.is_signed = parseBool();
            } else if (key == "scale") {
                signal.scale = parseDouble();
            } else if (key == "offset") {
                signal.offset = parseDouble();
            } else if (key == "minimum") {
                signal.minimum = parseDouble();
            } else if (key == "maximum") {
                signal.maximum = parseDouble();
            } else if (key == "unit") {
                signal.unit = parseString();
            } else {
                skipValue();
            }

            skipWhitespace();
            if (peek() == ',') {
                ++pos_;
                continue;
            } else if (peek() == '}') {
                ++pos_;
                break;
            } else {
                return false;
            }
        }

        return true;
    }

    std::string parseString() {
        std::string result;
        if (match('"')) {
            while (pos_ < json_.size() && json_[pos_] != '"') {
                if (json_[pos_] == '\\') {
                    if (pos_ + 1 < json_.size()) {
                        result += json_[pos_ + 1];
                        pos_ += 2;
                    } else {
                        break;
                    }
                } else {
                    result += json_[pos_++];
                }
            }
            ++pos_; // Skip closing quote
        }
        return result;
    }

    int parseNumber() {
        int result = 0;
        bool negative = false;
        if (json_[pos_] == '-') {
            negative = true;
            ++pos_;
        }
        while (pos_ < json_.size() && std::isdigit(json_[pos_])) {
            result = result * 10 + (json_[pos_] - '0');
            ++pos_;
        }
        return negative ? -result : result;
    }

    bool parseBool() {
        if (json_.compare(pos_, 4, "true") == 0) {
            pos_ += 4;
            return true;
        } else if (json_.compare(pos_, 5, "false") == 0) {
            pos_ += 5;
            return false;
        }
        return false;
    }

    double parseDouble() {
        std::stringstream ss;
        while (pos_ < json_.size() && std::isdigit(json_[pos_])) {
            ss << json_[pos_++];
        }
        if (pos_ < json_.size() && json_[pos_] == '.') {
            ss << json_[pos_++];
            while (pos_ < json_.size() && std::isdigit(json_[pos_])) {
                ss << json_[pos_++];
            }
        }
        double result;
        ss >> result;
        return result;
    }

    void skipWhitespace() {
        while (pos_ < json_.size() && std::isspace(json_[pos_])) {
            ++pos_;
        }
    }

    void skipValue() {
        while (pos_ < json_.size() && json_[pos_] != ',' && json_[pos_] != '}') {
            ++pos_;
        }
    }

    char peek() const {
        return pos_ < json_.size() ? json_[pos_] : '\0';
    }

    bool match(char expected) {
        if (pos_ < json_.size() && json_[pos_] == expected) {
            ++pos_;
            return true;
        }
        return false;
    }

private:
    std::string json_;
    size_t pos_;
    std::vector<Message> messages_;
};

JsonReader::JsonReader(const std::string& filename)
    : filename_(filename) {}

bool JsonReader::parse() {
    std::ifstream jsonFile(filename_);
    if (!jsonFile.is_open()) {
        std::cerr << "Error opening JSON file: " << filename_ << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << jsonFile.rdbuf();
    jsonFile.close();

    SimpleJsonParser parser(buffer.str());
    if (!parser.parse()) {
        std::cerr << "Failed to parse JSON." << std::endl;
        return false;
    }

    messages_ = parser.getMessages();
    return true;
}

const std::vector<Message>& JsonReader::getMessages() const {
    return messages_;
}

