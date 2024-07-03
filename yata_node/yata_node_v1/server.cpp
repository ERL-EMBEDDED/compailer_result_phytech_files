#include "server.h"
#include "global.h"
#include "json_parser.h"
#include <cstring>    // For memset
#include <iostream>   // For std::cout, std::cerr
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "find_kill_port.h"
           
// Static function to get current date time with milliseconds
static std::string getCurrentDateTimeMilliseconds() {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_tm = *std::localtime(&now_time_t);

    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
    auto micro = std::chrono::duration_cast<std::chrono::microseconds>(duration).count() % 1000;
    auto nano = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() % 1000;

    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%Y-%m-%dT%H:%M:%S") << '.' << std::setw(3) << std::setfill('0') << millis
        << std::setw(3) << std::setfill('0') << micro << std::setw(3) << std::setfill('0') << nano;

    return oss.str();
}

epsServer::epsServer() {
    serverSocket = -1; 
    clientSocket = -1;
    addrLen = sizeof(clientAddr);
    memset(&serverAddr, 0, sizeof(serverAddr));
    memset(&clientAddr, 0, sizeof(clientAddr));
    if (initSocket()) {

    } else {
        std::cerr << "EPS init Exit \n";
    }
}

epsServer::~epsServer() {
    if (clientSocket != -1) {
        close(clientSocket);
    }
    if (serverSocket != -1) {
        close(serverSocket);
    }
}

bool epsServer::acceptClient() {
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
    if (clientSocket < 0) {
        std::cerr << "Failed to accept client\n" << std::endl;
        return false;
    }
    std::cout << "Client connected: " << inet_ntoa(clientAddr.sin_addr) << std::endl;
    return true;
}

bool epsServer::initSocket() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket\n" << std::endl;
        return false;
    }

    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
         std::cerr << "Set socket options failed\n" << std::endl;
         close(serverSocket);
         return false;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Bind failed\n" << std::endl;
        findAndKillPortProcess(SERVER_PORT);
        close(serverSocket);
        serverSocket = -1;
        return false;
    }

    if (listen(serverSocket, BACKLOG) < 0) {
        std::cerr << "Listen failed" << std::endl;
        close(serverSocket);
        serverSocket = -1;
        return false;
    }

    std::cout << "Server initialized and listening on " << SERVER_IP << ":" << SERVER_PORT << std::endl;
    acceptClient();
    SERVER_STATES = RUNNING;
    return true;
}

int epsServer::receiveMessage() {
    const int bufferSize = 4024; // Adjust buffer size as needed
    char buffer[bufferSize];
    ssize_t bytesRead = recv(clientSocket, buffer, bufferSize - 1, 0);
    if (bytesRead < 0) {
        std::cerr << "Failed to receive message from client" << std::endl;
        return -1;
    } else if (bytesRead == 0) {
        std::cerr << "Client disconnected" << std::endl;
        acceptClient();
        return 0;
    }

    buffer[bytesRead] = '\0'; // Ensure null-terminated string
    std::string receivedMessage(buffer);
    std::cout << "--->>>" << getCurrentDateTimeMilliseconds() << " Received from client: " << receivedMessage << std::endl;

    // Process received message using JSON parser
    std::string response = JsonParser::parseAndGenerateResponse(receivedMessage);

    // Send response to client
    ssize_t bytesSent = send(clientSocket, response.c_str(), response.size(), 0);
    if (bytesSent < 0) {
        std::cerr << "Failed to send response to client" << std::endl;
        return -1;
    }

    std::cout << "<<<---" << getCurrentDateTimeMilliseconds() << " Sent to client: " << response << std::endl;
    return bytesSent;
}
int epsServer::sendMessage(float speed) {
    // Example implementation
    std::cout << "Sending message with speed: " << speed << std::endl;
    // Perform actual sending operation here
    
    return 0; // Return appropriate result or error code
}
void epsServer::run() {
    while (_hold_) {
        if (receiveMessage() < 0) {
            std::cerr << "Error receiving or sending message. Stopping server..." << std::endl;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Adjust sleep duration as needed
    }
}

