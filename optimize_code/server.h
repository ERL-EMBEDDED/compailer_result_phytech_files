// server.h
#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

#define SERVER_IP "192.168.4.11"
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
    int receiveMessage();
    int Tcpip_sendMessage(float Expected_wheelSpeedFR, float Expected_wheelSpeedFL, float Expected_vehicleSpeed);
    void run();
    void closeClientSocket();
    void closeServerSocket();
    bool isServerSocketValid() const;

};

#endif // SERVER_H
