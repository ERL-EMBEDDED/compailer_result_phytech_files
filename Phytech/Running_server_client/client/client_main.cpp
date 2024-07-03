// client_main.cpp
#include "client.h"

int main() {
    epsClient client;
    client.initSocket();
    if (client.connectToServer()) {
        while(true){
        client.communicateWithServer();
        }
    }
   // client.closeConnection();
    return 0;
}

