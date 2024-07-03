// server.cpp
#include "server.h"
#include <cstring>    // For memset
#include <string>
#include <iostream>   // For std::cout, std::cerr
#include "global.h"

epsServer::epsServer() {
    serverSocket = -1; 
    clientSocket = -1;
    addrLen = sizeof(clientAddr);
    memset(&serverAddr, 0, sizeof(serverAddr));
    memset(&clientAddr, 0, sizeof(clientAddr));
    if(initSocket()){

    }
    else{
	    printf("EPS init Exit \n");
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

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Bind failed\n" << std::endl;
        close(serverSocket);
        serverSocket = -1;
        return false;
    }

    if (listen(serverSocket, BACKLOG) < 0) {
        std::cerr << "Listen failed\n" << std::endl;
        close(serverSocket);
        serverSocket = -1;
        return false;
    }

    std::cout << "Server initialized and listening on " << SERVER_IP << ":" << SERVER_PORT << std::endl;
    return true;
    while (true) {
        if (!acceptClient()) {
            std::cerr << "Failed to accept client, continuing.\n" << std::endl;
            continue;
        }
}

}

int epsServer::receiveMessage() {
    const int bufferSize = 1024; // Adjust buffer size as needed
    char buffer[bufferSize];
    ssize_t bytesRead = recv(clientSocket, buffer, bufferSize - 1, 0);
    if (bytesRead < 0) {
        std::cerr << "Failed to receive message from client\n" << std::endl;
        return -1;
    } else if (bytesRead == 0) {
        std::cerr << "Client disconnected\n" << std::endl;
        acceptClient();
        return 0;
    }

    buffer[bytesRead] = '\0'; // Ensure null-terminated string
    std::string receivedMessage(buffer);
    std::cout << "Received from client: " << receivedMessage << std::endl;

    std::string key = "\"Speed\":";
    size_t startPos = receivedMessage.find(key);
    if (startPos != std::string::npos) {
        startPos += key.length();
        size_t endPos = receivedMessage.find_first_of(",}", startPos);
        if (endPos != std::string::npos) {
            std::string speedStr = receivedMessage.substr(startPos, endPos - startPos);
            try {
                int Speed = std::stoi(speedStr);
                std::cout << "Speed: " << Speed<< std::endl;
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid speed value\n" << std::endl;
            } catch (const std::out_of_range& e) {
                std::cerr << "Speed value out of range\n" << std::endl;
            }
        } else {
            std::cerr << "Unable to parse speed value\n" << std::endl;
        }
    } else {
        std::cerr << "Key \"Speed\" not found in JSON string\n" << std::endl;
    }

    return bytesRead;
}

int epsServer::sendMessage() {
    std::string sendMessage = "{\"Speed\":" + std::to_string(*actualVehicleSpeed) + "}";

    ssize_t bytesSent = send(clientSocket, sendMessage.c_str(), sendMessage.size(), 0);
    if (bytesSent < 0) {
        std::cerr << "Failed to send acknowledgment to client\n" << std::endl;
        return -1;
    }

    std::cout << "Message sent successfully: " << sendMessage << std::endl;
    return bytesSent;
}

void epsServer::run() {

        while (true) {
                receiveMessage();
                sendMessage();
        }
}

void epsServer::closeClientSocket() {
    if (clientSocket != -1) {
        close(clientSocket);
        clientSocket = -1;
    }
}

bool epsServer::isServerSocketValid() const {
    return serverSocket != -1;
}

