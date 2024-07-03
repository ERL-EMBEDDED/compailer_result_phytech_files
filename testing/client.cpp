#include <iostream>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // Add this header for inet_addr and htons
#include <thread> // For thread
#include "can.h"

void clientThread() {
    const int PORT = 8800;
    const int BUFFER_SIZE = 1024;

    // Create a socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating socket\n";
        return;
    }

    // Connect to the server
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("192.168.3.12"); // Server IP address (change to your server's IP)
    serverAddr.sin_port = htons(PORT);

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Connection failed\n";
        close(clientSocket);
        return;
    }

    int receivedNumbers[BUFFER_SIZE];
    int bytesReceived;

    while (true) {
        // Receive numbers from the server
        bytesReceived = recv(clientSocket, receivedNumbers, sizeof(receivedNumbers), 0);
        if (bytesReceived > 0) {
            // Assign the received value to the global atomic variable 'value' (assumed declared in can.h)
            value.store(receivedNumbers[0]);
            std::cout << "Received value from server: " << value.load() << std::endl;
        } else if (bytesReceived == 0) {
            std::cout << "Server disconnected\n";
            break;
        } else {
            perror("Receive error");
            break;
        }
    }

    // Close the socket
    close(clientSocket);
}

