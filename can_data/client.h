#ifndef CLIENT_H
#define CLIENT_H

#include "globals.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <mutex>

#define SERVER_IP "192.168.3.12"
#define SERVER_PORT 9090
#define TCPIP_FRAME_SIZE 5

class ClientTcpip {

private:
    int clientSocket;
    struct sockaddr_in serverAddr;
    char sendTcpipFrame[TCPIP_FRAME_SIZE]; // Declaration of sendTcpipFrame
    char receiveTcpipFrame[TCPIP_FRAME_SIZE]; // Declaration of receiveTcpipFrame


public:
    ClientTcpip();
    ~ClientTcpip();
    bool connectServer();
    void sendVehicleSpeed();
    void receiveVehicleSpeed();
    bool isConnected() const;
    void disconnect();
    void run();
};

#endif // CLIENT_H

