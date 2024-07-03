// server.h
#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>    // For memset
#include <string>
#include <iostream>   // For std::cout, std::cerr
#include "global.h"
#include <thread>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <chrono>
#include "find_kill_port.h"
#include <tuple>
#include "json_parser.h"

#define SERVER_IP "192.168.3.11"
#define SERVER_PORT 9080
#define BACKLOG 5



class epsServer {
private:
    int serverSocket;
    int clientSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    socklen_t addrLen;

public:
    epsServer();
    ~epsServer();
    bool initSocket();
    bool acceptClient();
    std::string receiveMessage();
    int sendMessage(std::string send_Message);
    int Tcpip_sendMessage(float Expected_wheelSpeedFR,float Expected_wheelSpeedFL,float Expected_vehicleSpeed);
    int Tcpip_receivedMessage();
    void run();
    void closeClientSocket();
    void closeServerSocket();
    bool isServerSocketValid() const;

};

#endif // SERVER_H
