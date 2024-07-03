 #include "client.h"

ClientTcpip::ClientTcpip() {
    clientSocket = -1; // Initialize socket to an invalid value
}

ClientTcpip::~ClientTcpip() {
    if (clientSocket != -1)
        close(clientSocket);
}

bool ClientTcpip::connectServer() {
    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cerr << "Error creating socket\n";
        return false;
    }

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed\n";
        close(clientSocket);
        clientSocket = -1;
        return false;
    }
    return true;
}

void ClientTcpip::sendVehicleSpeed() {
		
    // Send vehicle speed  
    sendTcpipFrame[0] = 0x133; // Assuming CAN message ID
    memcpy(&sendTcpipFrame[1], actualVehicleSpeed.get(), sizeof(*actualVehicleSpeed));
    send(clientSocket, sendTcpipFrame, TCPIP_FRAME_SIZE, 0);

}

void ClientTcpip::receiveVehicleSpeed() {
	
    // Receive encoded message from server
    recv(clientSocket, receiveTcpipFrame, TCPIP_FRAME_SIZE, 0);

    // Decode received message and store it in expectedVehicleSpeed
    memcpy(expectedVehicleSpeed.get(), &receiveTcpipFrame[1], sizeof(*expectedVehicleSpeed));
    std::cout << "Received Expectedspeed: " << *expectedVehicleSpeed << std::endl;
    std::cout << "Can Id: " << &receiveTcpipFrame[0] << std::endl;

}

bool ClientTcpip::isConnected() const {
    return clientSocket != -1;
}

void ClientTcpip::disconnect() {
    // Close the client socket
    if (clientSocket != -1) {
        close(clientSocket);
        clientSocket = -1;
    }
}

void ClientTcpip::run()
{
    while (true)
    {
	std::cout << "Client Thread is running" << std::endl;
        sendVehicleSpeed();	
        receiveVehicleSpeed();
    }
}

