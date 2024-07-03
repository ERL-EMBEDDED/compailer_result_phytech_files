#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "json_parser.h"
#include "find_kill_port.h"
#include "server.h"
#include "global.h"
#include "config_file.h"

static std::string getCurrentDateTimeseconds() {
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
    if(initSocket()){
    } else {
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
        close(serverSocket);
        findAndKillPortProcess(SERVER_PORT);
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
    std::cout << "--->>>" << getCurrentDateTimeseconds() << " Received from client: " << receivedMessage << std::endl;

    auto [status, Expected_wheelSpeedFR, Expected_wheelSpeedFL, Expected_vehicleSpeed] = parseJson_read(receivedMessage);
    if (status) {

        std::cout << "Can Frame Configuration is Succussfull " << std::endl;
        modifyFile(receivedMessage);
	std::string send_Message ="Can Frame Configuration is Succussfull ";
        ssize_t bytesSent = send(clientSocket, send_Message.c_str(), send_Message.size(), 0);

    } else {
      
       /*	    
        std::cout << "Parsed Values:\n";
        std::cout << "Expected_wheelSpeedFL: " << Expected_wheelSpeedFL << "\n";
        std::cout << "Expected_wheelSpeedFR: " << Expected_wheelSpeedFR << "\n";
        std::cout << "Expected_vehicleSpeed: " << Expected_vehicleSpeed << "\n";
        */

        can.send_can_data(Expected_wheelSpeedFR,Expected_wheelSpeedFL,Expected_vehicleSpeed);
    }
    return 1;
}

int epsServer::Tcpip_sendMessage(float Expected_wheelSpeedFR,float Expected_wheelSpeedFL,float Expected_vehicleSpeed){
      
       	std::cout << "Tcpip actualVehicleSpeed : " << Expected_vehicleSpeed << std::endl;

    /*
     	std::string send_Message = parseJson_create(Expected_wheelSpeedFR,Expected_wheelSpeedFL,Expected_vehicleSpeed);

      ssize_t bytesSent = send(clientSocket, send_Message.c_str(), send_Message.size(), 0);

    */
    
	std::stringstream speed_FR,speed_FL,speed_ACT;
        speed_FR << Expected_wheelSpeedFR;
        std::string FR_speed = speed_FR.str();
        speed_FL << Expected_wheelSpeedFL;
        std::string FL_speed = speed_FL.str();
        speed_ACT << Expected_vehicleSpeed;
        std::string ACT_speed = speed_ACT.str();
	std::cout << "String_in_Speed : " << ACT_speed<<std::endl;
     
         std::string send_Message = "{\"Expected_vehicleSpeed\":\"" + ACT_speed + "\","
                               "\"Expected_wheelSpeedFR\":\"" + FR_speed + "\","
                               "\"Expected_wheelSpeedFL\":\"" + FL_speed + "\"}";

	ssize_t bytesSent = send(clientSocket, send_Message.c_str(), send_Message.size(), 0);
	if (bytesSent < 0) {
		std::cerr << "Failed to send acknowledgment to client " << std::endl;
		return -1;
	}
         else{
         	std::cout<< "--->>>" << getCurrentDateTimeseconds() << "Tcpip Message Send Succesfully" << send_Message <<std::endl;

	 }

      return 0;
}

void epsServer::run() {
    while (true) {
        if (!_hold_ && SERVER_STATES == RUNNING) {
            receiveMessage();
        }
        usleep(1);
    }
}

void epsServer::closeClientSocket() {
    if (clientSocket != -1) {
        close(clientSocket);
        clientSocket = -1;
    }
}

void epsServer::closeServerSocket() {
    if (serverSocket != -1) {
        close(serverSocket);
        serverSocket = -1;
    }
}

bool epsServer::isServerSocketValid() const {
    return serverSocket != -1;
}

