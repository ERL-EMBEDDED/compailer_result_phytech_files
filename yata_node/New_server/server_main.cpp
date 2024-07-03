// server_main.cpp
#include "server.h"
#include "can.h"
#include "global.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <memory>
#include <csignal>
#include <unistd.h>
#include "find_kill_port.h"

bool _hold_ = false;

epsServer server;
CanInterface can;
TCP_IP_t SERVER_STATES;
TCP_IP_t PREV_SERVER_STATES;

void signalHandler(int signum) {
    if (signum == SIGINT) {
        findAndKillPortProcess(SERVER_PORT);
        exit(signum);
    }
}

int main() {
 
    signal(SIGINT, signalHandler);

    // Run the threads for CAN and TCP/IP communication
    std::thread tcpipThread(&epsServer::run, &server); // Pass member function pointer and object instance
    std::thread canThread(&CanInterface::run, &can); // Pass member function pointer and object instance

    while(1){
	    switch(SERVER_STATES){
		    case INIT:
			    server.initSocket();
			    can.initCan();
			    break;
		    case WAIT:
			    server.initSocket();
			    server.acceptClient();
			    break;
		    case RUNNNING:
			    break;
	    }
	    PREV_SERVER_STATES = SERVER_STATES;
    }

    // Join the threads (will never reach here in this example)
    tcpipThread.join();
    canThread.join();
}
