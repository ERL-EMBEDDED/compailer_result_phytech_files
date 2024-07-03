#include "client.h"
#include "can.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>

std::atomic<int> receivedNumber{-1};

void printTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::chrono::duration<double> seconds = now.time_since_epoch();
    std::chrono::milliseconds milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(seconds);
    std::chrono::nanoseconds nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(seconds) % std::chrono::seconds(1);
    std::cout << std::put_time(std::localtime(&time), "%F %T") << "." << std::setfill('0') << std::setw(3) << milliseconds.count() % 1000 << " " << std::setw(9) << nanoseconds.count() << ": ";
}

void receiveDataFromServer(int clientSocket) {
    while (true) {
        int number;
        int bytesReceived = recv(clientSocket, &number, sizeof(number), 0);
        if (bytesReceived <= 0) {
            std::cerr << "Failed to receive data from server" << std::endl;
            break;
        }

        receivedNumber.store(number);
        printTime();
        std::cout << "Received number: " << number << std::endl;
        sendValue(number, &canInterface);
    }
}

int connectToServer(const char* ipAddress) {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, ipAddress, &serverAddr.sin_addr);

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) != 0) {
        std::cerr << "Connection to server failed" << std::endl;
        close(clientSocket);
        return -1;
    }

    std::cout << "Connected to server" << std::endl;

    std::thread receiverThread(receiveDataFromServer, clientSocket);
    receiverThread.detach(); // Detach the thread so it runs independently

    return clientSocket;
}

