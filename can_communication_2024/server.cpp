#include "server.h"

ServerTcpip::ServerTcpip() {
    serverSocket = -1; // Initialize socket to an invalid value
    clientSocket = -1; // Initialize client socket to an invalid value
}

ServerTcpip::~ServerTcpip() {
    if (serverSocket != -1)
        close(serverSocket);
    if (clientSocket != -1)
        close(clientSocket);
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
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Listen on any interface

    // Bind socket to the address and port
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Binding failed\n";
        close(serverSocket);
        serverSocket = -1;
        return false;
    }

    // Start listening for incoming connections
    if (listen(serverSocket, MAX_CONNECTIONS) < 0) {
        std::cerr << "Listening failed\n";
        close(serverSocket);
        serverSocket = -1;
        return false;
    }

    return true;
}

bool ServerTcpip::acceptClient() {
    // Accept incoming connection
    clientAddrLen = sizeof(clientAddr); // Initialize client address length
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientSocket < 0) {
        std::cerr << "Error accepting client\n";
        return false;
    }

    return true;
}

void ServerTcpip::sendVehicleSpeed() {
    // Send vehicle speed
    sendTcpipFrame[0] = 0x01; // Assuming CAN message ID
    memcpy(&sendTcpipFrame[1], &actualVehicleSpeed, sizeof(actualVehicleSpeed));
    send(clientSocket, sendTcpipFrame, TCPIP_FRAME_SIZE, 0);
}

void ServerTcpip::receiveVehicleSpeed() {
    // Receive encoded message from client
    recv(clientSocket, receiveTcpipFrame, TCPIP_FRAME_SIZE, 0);

    // Decode received message and store it in expectedVehicleSpeed
    memcpy(&expectedVehicleSpeed, &receiveTcpipFrame[1], sizeof(expectedVehicleSpeed));
    std::cout << "Received Expected Speed: " << expectedVehicleSpeed << std::endl;
}

bool ServerTcpip::isClientConnected() const {
    return clientSocket != -1;
}

void ServerTcpip::disconnectClient() {
    // Close the client socket
    if (clientSocket != -1) {
        close(clientSocket);
        clientSocket = -1;
    }
}

void ServerTcpip::stopServer() {
    // Close the server socket
    if (serverSocket != -1) {
        close(serverSocket);
        serverSocket = -1;
    }
}

void ServerTcpip::run() {
    while (true) {
        if (acceptClient()) {
            receiveVehicleSpeed();
            sendVehicleSpeed();
        }
    }
}

