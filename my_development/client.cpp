#include "client.h"

ClientTcpip::ClientTcpip() {
    clientSocket = -1; // Initialize socket to an invalid value
}

ClientTcpip::~ClientTcpip() {
    disconnect();
}

bool ClientTcpip::init() {
    if (!connectServer()) {
        std::cerr << "Error: Failed to connect to the server." << std::endl;
        return false;
    }
    std::cout << "Connected to server." << std::endl;
    return true;
}

bool ClientTcpip::connectServer() {
    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
        return false;
    }

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed: " << strerror(errno) << std::endl;
        disconnect();
        return false;
    }
    return true;
}

void ClientTcpip::sendVehicleSpeed() {
    if (!isConnected()) {
        std::cerr << "Error: Not connected to server." << std::endl;
        return;
    }

    // Send vehicle speed
    sendTcpipFrame[0] = 0x01; // Assuming CAN message ID
    std::lock_guard<std::mutex> lock(actualSpeedMutex);
    memcpy(&sendTcpipFrame[1], actualVehicleSpeed.get(), sizeof(*actualVehicleSpeed));
    if (send(clientSocket, sendTcpipFrame, TCPIP_FRAME_SIZE, 0) < 0) {
        std::cerr << "Error sending vehicle speed: " << strerror(errno) << std::endl;
    }
    std::lock_guard<std::mutex> unlock(actualSpeedMutex);
}

void ClientTcpip::receiveVehicleSpeed() {
    if (!isConnected()) {
        std::cerr << "Error: Not connected to server." << std::endl;
        return;
    }

    // Receive encoded message from server
    if (recv(clientSocket, receiveTcpipFrame, TCPIP_FRAME_SIZE, 0) < 0) {
        std::cerr << "Error receiving vehicle speed: " << strerror(errno) << std::endl;
        disconnect();
        return;
    }
    std::lock_guard<std::mutex> lock(expectedSpeedMutex);
    // Decode received message and store it in expectedVehicleSpeed
    memcpy(expectedVehicleSpeed.get(), &receiveTcpipFrame[1], sizeof(*expectedVehicleSpeed));
    std::cout << "Received Expected speed: " << *expectedVehicleSpeed << std::endl;
    std::lock_guard<std::mutex> unlock(expectedSpeedMutex);
}

bool ClientTcpip::isConnected() const {
    return clientSocket != -1;
}

void ClientTcpip::disconnect() {
    // Close the client socket
    if (clientSocket != -1) {
        close(clientSocket);
        clientSocket = -1;
        std::cout << "Disconnected from server." << std::endl;
    }
}

void ClientTcpip::run() {
    while (true) {
        std::cout << "Client Thread is running" << std::endl;   
        if (!isConnected()) {
            if (!init()) {
                // Retry initialization after a delay
                std::cerr << "Retrying initialization in 5 seconds..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(5));
                continue;
            }
        }
        receiveVehicleSpeed();
        sendVehicleSpeed();
    }
}

