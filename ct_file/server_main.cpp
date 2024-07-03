// server_main.cpp
#include "server.h"

int main() {
    epsServer server;
    server.acceptClient();
        while (true) {
               
               server.run();
        }
    return 0;
}
