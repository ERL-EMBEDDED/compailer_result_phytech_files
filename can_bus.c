#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
int main() {


   // Create a socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

   // Specify the server's IP address and port
      const char *host = "192.168.3.12";
      int port = 1234;

        struct sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port);
        inet_pton(AF_INET, host, &server_address.sin_addr);

        // Connect to the server
     if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
          perror("Connection failed");
          close(client_socket);
     }

    // Receive data from the server
    char buffer[1024];
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received == -1) {
         perror("Error receiving data");
         close(client_socket);
      }
    buffer[bytes_received] = '\0';
    printf("Received message: %s\n", buffer);

    // Close the connection
    close(client_socket);    
    
    const char* canInterface = "can0";
    const int bitrate = 250000;
    const char* canMessage = "123#1122334455667788"; // Replace with your CAN message

    // Set down the can interface
    int result = system("ip link set can0 down");
    if (result != 0) {
        fprintf(stderr, "Error setting down can0 interface.\n");
        return EXIT_FAILURE;
    }

    // Set can interface type and bitrate
    char command[100];
    snprintf(command, sizeof(command), "ip link set %s type can bitrate %d", canInterface, bitrate);
    result = system(command);
    if (result != 0) {
        fprintf(stderr, "Error setting up can0 interface.\n");
        return EXIT_FAILURE;
    }

    // Set up the can interface
    result = system("ip link set can0 up");
    if (result != 0) {
        fprintf(stderr, "Error setting up can0 interface.\n");
        return EXIT_FAILURE;
    }

    // Send the CAN message
    snprintf(command, sizeof(command), "cansend %s %s", canInterface, canMessage);
    result = system(command);
    if (result != 0) {
        fprintf(stderr, "Error sending CAN message.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

