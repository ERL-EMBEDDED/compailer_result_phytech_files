#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#include "global.h"

#define SERVER_IP "192.168.3.11"
#define SERVER_PORT 8888
#define TCPIP_FRAME_SIZE 5
#define MAX_CONNECTIONS 5 

class ServerTcpip {
private:
    int serverSocket;
    int clientSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen;
    char sendTcpipFrame[TCPIP_FRAME_SIZE];
    char receiveTcpipFrame[TCPIP_FRAME_SIZE];

public:
    ServerTcpip();
    ~ServerTcpip();
    bool startServer();
    bool acceptClient();
    void sendVehicleSpeed();
    void receiveVehicleSpeed();
    void run();
    bool isClientConnected() const;
    void disconnectClient();
    void stopServer();
};

#endif // SERVER_H
