#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cctype>

// Token types
enum class TokenType {
    LBRACE, RBRACE, LBRACKET, RBRACKET, COLON, COMMA,
    STRING, NUMBER, TRUE, FALSE, NULL_,
    END, // End of input
    UNKNOWN // Unknown token type
};

// Token structure
struct Token {
    TokenType type;
    std::string value;

    Token(TokenType type = TokenType::UNKNOWN, const std::string& value = "")
        : type(type), value(value) {}
};

// JSON Parser class
class JsonParser {
public:
    JsonParser(const std::string& json) : input(json), currentPos(0) {}

    // Parse entry point
    void parse() {
        getToken();
        parseValue();
    }

private:
    std::string input;
    size_t currentPos;
    Token currentToken;

    // Tokenizer (lexer)
    void getToken() {
        // Skip whitespace characters
        while (currentPos < input.size() && std::isspace(input[currentPos])) {
            currentPos++;
        }

        if (currentPos >= input.size()) {
            currentToken = { TokenType::END, "" };
            return;
        }

        char currentChar = input[currentPos];
        switch (currentChar) {
            case '{': currentToken = { TokenType::LBRACE, "{" }; break;
            case '}': currentToken = { TokenType::RBRACE, "}" }; break;
            case '[': currentToken = { TokenType::LBRACKET, "[" }; break;
            case ']': currentToken = { TokenType::RBRACKET, "]" }; break;
            case ':': currentToken = { TokenType::COLON, ":" }; break;
            case ',': currentToken = { TokenType::COMMA, "," }; break;
            case '"': currentToken = parseString(); break;
            default:
                if (isdigit(currentChar) || currentChar == '-') {
                    currentToken = parseNumber();
                } else if (currentChar == 't' || currentChar == 'f') {
                    currentToken = parseBoolean();
                } else if (currentChar == 'n') {
                    currentToken = parseNull();
                } else {
                    throw std::runtime_error("Unexpected character: " + std::string(1, currentChar));
                }
        }
        currentPos++;
    }

    // Parse string token
    Token parseString() {
        std::string value;
        size_t startPos = currentPos + 1; // Skip opening quote

        while (currentPos < input.size()) {
            char currentChar = input[currentPos];
            if (currentChar == '"') {
                value = input.substr(startPos, currentPos - startPos);
                break;
            }
            currentPos++;
        }

        currentPos++; // Move past closing quote
        return { TokenType::STRING, value };
    }

    // Parse number token
    Token parseNumber() {
        size_t startPos = currentPos;
        while (currentPos < input.size() && (isdigit(input[currentPos]) || input[currentPos] == '.')) {
            currentPos++;
        }
        return { TokenType::NUMBER, input.substr(startPos, currentPos - startPos) };
    }

    // Parse boolean token
    Token parseBoolean() {
        std::string value = input.substr(currentPos, 4); // "true" or "false"
        currentPos += (value == "true") ? 4 : 5; // Move past "true" or "false"
        return (value == "true") ? Token(TokenType::TRUE, "true") : Token(TokenType::FALSE, "false");
    }

    // Parse null token
    Token parseNull() {
        std::string value = input.substr(currentPos, 4); // "null"
        currentPos += 4; // Move past "null"
        return Token(TokenType::NULL_, "null");
    }

    // Parse JSON value
    void parseValue() {
        switch (currentToken.type) {
            case TokenType::LBRACE: parseObject(); break;
            case TokenType::LBRACKET: parseArray(); break;
            case TokenType::STRING: std::cout << "String value: " << currentToken.value << std::endl; break;
            case TokenType::NUMBER: std::cout << "Number value: " << currentToken.value << std::endl; break;
            case TokenType::TRUE: std::cout << "Boolean value: true" << std::endl; break;
            case TokenType::FALSE: std::cout << "Boolean value: false" << std::endl; break;
            case TokenType::NULL_: std::cout << "Null value" << std::endl; break;
            default:
                throw std::runtime_error("Unexpected token");
        }

        getToken();
    }

    // Parse JSON object
    void parseObject() {
        std::cout << "Parsing object..." << std::endl;
        getToken(); // Move past '{'
        while (currentToken.type != TokenType::RBRACE) {
            if (currentToken.type == TokenType::STRING) {
                // Parse key
                std::cout << "Key: " << currentToken.value << ", ";
                getToken(); // Move past key

                if (currentToken.type != TokenType::COLON) {
                    throw std::runtime_error("Expected ':'");
                }
                getToken(); // Move past ':'

                // Parse value
                parseValue();

                if (currentToken.type == TokenType::COMMA) {
                    getToken(); // Move past ','
                } else if (currentToken.type != TokenType::RBRACE) {
                    throw std::runtime_error("Expected ',' or '}'");
                }
            } else {
                throw std::runtime_error("Expected string key");
            }
        }
        getToken(); // Move past '}'
    }

    // Parse JSON array
    void parseArray() {
        std::cout << "Parsing array..." << std::endl;
        getToken(); // Move past '['
        while (currentToken.type != TokenType::RBRACKET) {
            parseValue();
            if (currentToken.type == TokenType::COMMA) {
                getToken(); // Move past ','
            } else if (currentToken.type != TokenType::RBRACKET) {
                throw std::runtime_error("Expected ',' or ']'");
            }
        }
        getToken(); // Move past ']'
    }
};

int main() {
    std::string json = R"({
        "name": "John",
        "age": 30,
        "city": "New York",
        "isStudent": true,
        "courses": ["Math", "Science"],
        "address": {
            "street": "123 Main St",
            "city": "Anytown"
        },
        "grades": [90, 85, 95]
    })";

    try {
        JsonParser parser(json);
        parser.parse();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

