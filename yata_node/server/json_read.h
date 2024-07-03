#ifndef JSON_READ_H
#define JSON_READ_H

#include <string>
#include <vector>

struct Signal {
    std::string name;
    int start_bit;
    int bit_length;
    bool is_big_endian;
    bool is_signed;
    double scale;
    double offset;
    double minimum;
    double maximum;
    std::string unit;
};

struct Message {
    std::string name;
    int id_hex;
    std::vector<Signal> signals;
};

class JsonReader {
public:
    JsonReader(const std::string& filename);
    bool parse();
    const std::vector<Message>& getMessages() const;

private:
    std::string filename_;
    std::vector<Message> messages_;
};

#endif // JSON_READ_H

