// client.cpp
#include "client.h"
#include <cstring>    // For memset
#include <string>

epsClient::epsClient() {
    clientSocket = -1;
    memset(&serverAddr, 0, sizeof(serverAddr));
}

epsClient::~epsClient() {
    if (clientSocket != -1) {
        close(clientSocket);
    }
}

void epsClient::initSocket() {
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
}

bool epsClient::connectToServer() {
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Failed to connect to server" << std::endl;
        return false;
    }
    std::cout << "Connected to server" << std::endl;
    return true;
}

void epsClient::communicateWithServer() {
    int speed = 100; // Example speed value
    std::string sendMessage = "{\"Speed\":" + std::to_string(speed) + "}";

    // Send JSON string to server
    ssize_t bytesSent = send(clientSocket, sendMessage.c_str(), sendMessage.size(), 0);
    if (bytesSent < 0) {
        std::cerr << "Failed to send message to server" << std::endl;
        return;
    }
    
    // Receive JSON string from client
    const int bufferSize = 1024; // Adjust buffer size as needed
    char buffer[bufferSize];
    ssize_t bytesRead = recv(clientSocket, buffer, bufferSize, 0);
    
    // Process received JSON string (in this case, just print it)
    std::string receivedMessage(buffer, bytesRead);
    std::cout << "Received from client: " << receivedMessage << std::endl;  


    if (bytesRead < 0) {
        std::cerr << "Failed to receive acknowledgment from server" << std::endl;
        return;
    } else if (bytesRead == 0) {
        std::cerr << "Server disconnected" << std::endl;
        return;
    }

}

void epsClient::closeConnection() {
    if (clientSocket != -1) {
        close(clientSocket);
        clientSocket = -1;
    }
}
