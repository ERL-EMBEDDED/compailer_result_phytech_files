#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include "global.h"

#define SERVER_PORT 8080
#define TCPIP_FRAME_SIZE 5

class ServerTcpip {
private:
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    char receiveTcpipFrame[TCPIP_FRAME_SIZE];

public:
    ServerTcpip();
    ~ServerTcpip();
    bool startServer();
    void receiveVehicleSpeed();
    void sendVehicleSpeed();
    bool acceptClient();
    void run();
    void disconnect();
};

#endif // SERVER_H

