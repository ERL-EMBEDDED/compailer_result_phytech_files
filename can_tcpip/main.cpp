#include "can.h"
#include "client.h"

int main() {
    const char* serverIP = "192.168.3.12"; // Server IP address
    const char* canInterfaceName = "can0"; // CAN interface name

    // Create a CAN interface object
    CanInterface canInterface(canInterfaceName);

    // Connect to the server
    int clientSocket = connectToServer(serverIP);
    if (clientSocket == -1) {
        return 1; // Connection failed
    }

    // Main loop to keep the program running
    while (true) {
        // Add any main program logic here
        // For example, handling user inputs or other tasks
    }

    // Close the client socket (not reached in this example)
    close(clientSocket);

    return 0;
}

