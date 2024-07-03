#include "server.h"
#include <arpa/inet.h>
#include"global.h"
ServerTcpip::ServerTcpip() {
    serverSocket = -1;
    clientSocket = -1;
}

ServerTcpip::~ServerTcpip() {
    disconnect();
}

bool ServerTcpip::startServer() {
    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Error creating socket\n";
        return false;
    }

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Bind socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Binding failed\n";
        return false;
    }

    // Listen
    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Listening failed\n";
        return false;
    }

    return true;
}

void ServerTcpip::receiveVehicleSpeed() {
      // Receive encoded message from server
    recv(clientSocket, receiveTcpipFrame, TCPIP_FRAME_SIZE, 0);

    // Decode received message and store it in expectedVehicleSpeed
    memcpy(expectedVehicleSpeed.get(), &receiveTcpipFrame[1], sizeof(*expectedVehicleSpeed));
    std::cout << "Received Expectedspeed: " << *expectedVehicleSpeed << std::endl;

}

void ServerTcpip::sendVehicleSpeed() {
    // Send vehicle speed
    sendTcpipFrame[0] = 0x01; // Assuming CAN message ID
    memcpy(&sendTcpipFrame[1], actualVehicleSpeed.get(), sizeof(*actualVehicleSpeed));
    send(clientSocket, sendTcpipFrame, TCPIP_FRAME_SIZE, 0);
}

bool ServerTcpip::acceptClient() {
    socklen_t clientLen = sizeof(clientAddr);
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientLen);
    if (clientSocket < 0) {
        std::cerr << "Error accepting client connection\n";
        return false;
    }
    return true;
}

void ServerTcpip::run() {
    if (!startServer()) {
        std::cerr << "Server initialization failed\n";
        return;
    }

    while (true) {
        if (!acceptClient()) {
            std::cerr << "Error accepting client\n";
            continue;
        }
        std::cout << "Client connected\n";
        sendVehicleSpeed();
        receiveVehicleSpeed();
    }
}

void ServerTcpip::disconnect() {
    if (clientSocket != -1) {
        close(clientSocket);
        clientSocket = -1;
    }
    if (serverSocket != -1) {
        close(serverSocket);
        serverSocket = -1;
    }
}

