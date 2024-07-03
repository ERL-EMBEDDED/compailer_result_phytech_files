#ifndef CLIENT_H
#define CLIENT_H

#include "globals.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define SERVER_IP "192.168.3.12"
#define SERVER_PORT 9999
#define TCPIP_FRAME_SIZE 5

class ClientTcpip {
private:
    int clientSocket;
    struct sockaddr_in serverAddr;
    char sendTcpipFrame[TCPIP_FRAME_SIZE];
    char receiveTcpipFrame[TCPIP_FRAME_SIZE];

public:
    ClientTcpip();
    ~ClientTcpip();
    bool connectServer();
    void sendVehicleSpeed();
    void receiveVehicleSpeed();
    bool isConnected() const;
    void disconnect();
};

#endif // CLIENT_H

