#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <iomanip>
#include <cstring>
#include <cerrno>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

extern std::atomic<int> receivedNumber;

void printTime();
void receiveDataFromServer(int clientSocket);
int connectToServer(const char* ipAddress);

#endif // CLIENT_H

