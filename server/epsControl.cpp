#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "epsControl.h"

	struct timespec next_time_;


epsControl::epsControl()
{
	clock_gettime(CLOCK_MONOTONIC, &next_time_);
}

epsControl::~epsControl()
{

}

void epsControl::initSocketConnection()
{
    etherNERIServer_socket = socket(AF_INET, SOCK_STREAM, 0);
    

    // Specify the IP address and port

    //Send data path Expected speed reception

    etherNERIServer_address.sin_family = AF_INET;
    etherNERIServer_address.sin_addr.s_addr = inet_addr(host);
    etherNERIServer_address.sin_port = htons(etherNERIPort);

    // Bind to the IP address and port
    if (bind(etherNERIServer_socket, (struct sockaddr *)&etherNERIServer_address, sizeof(etherNERIServer_address)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(etherNERIServer_socket, 5) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("etherNERI Path Server Ready\n");

    while (true)
    {
        socklen_t etherNERIClient_address_len = sizeof(etherNERIClient_address);
        if ((etherNERIClient_socket = accept(etherNERIServer_socket, (struct sockaddr *)&etherNERIClient_address, &etherNERIClient_address_len)) == -1) {
            perror("Accept failed");
	    usleep(10000);
            continue;
        }
        printf("etherNERI Send Path : Got connection from %s\n", inet_ntoa(etherNERIClient_address.sin_addr));
	break;
    }


}

void epsControl::closeSocketConnection()
{
	close(etherNERIClient_socket);
	close(etherNERIServer_socket);
}

void epsControl::messageEncoding()
{
	memset(sendCANFrame, 0, 5);
	sendCANFrame[0] = CANMsgId;
        vehicleSpeed = expectedVehicleSpeed;

        intDataptr = &vehicleSpeed;
        charDataptr = (unsigned char*)intDataptr;
        for(int i=0;i<4;i++)
	{
		sendCANFrame[i+1] = *(charDataptr+i);
                printf("charDataptr[%d]:[%x] \n",i, *(charDataptr+i));
	}
        for(int i=0; i < 5 ; i++)
	{
 		printf("sendCAN[%d] : [%x]\n",i,sendCANFrame[i]);
	}	

}

void epsControl::sendVehicleSpeed()
{

	clock_gettime(CLOCK_MONOTONIC, &next_time_);
    printf("Before sernd timespec.tv_nsec:%jd.%09ld\n",next_time_.tv_sec, next_time_.tv_nsec); 
      int bytesSent = send(etherNERIClient_socket, sendCANFrame, CAN_FRAME_SIZE, 0);
    printf("After Send timespec.tv_nsec:%jd.%09ld\n",next_time_.tv_sec, next_time_.tv_nsec); 
       // Check if data is sent successfully
    if (bytesSent == CAN_FRAME_SIZE) {
        // Data sent successfully
        std::cout << "Vehicle speed data sent successfully." << std::endl;
    } else if (bytesSent == -1) {
        // Error occurred during sending
        std::cerr << "Error in sending data." << std::endl;
    } else {
        // Partial data sent or unexpected behavior
        std::cerr << "Unexpected behavior in sending data. Sent bytes: " << bytesSent << std::endl;
    }
    printf("Raw timespec.tv_nsec:%jd.%09ld\n",next_time_.tv_sec, next_time_.tv_nsec); 
}


void epsControl::messageDecoding()
{
    actualVehicleSpeed = 0x0;
    vehicleSpeed = 0x00;
    for(int i=0 ; i < CAN_DATA_SIZE; i++)
    {
	    receiveCANData[i] = receiveCANFrame[i+1];
	    printf("[%c]", receiveCANFrame[i]);
    }
    printf("\n");
    
    vehicleSpeed = vehicleSpeed | ((0xFFFF00 | (receiveCANData[3])) << 24);
    vehicleSpeed = vehicleSpeed | ((0xFFFF00 | (receiveCANData[2])) << 16);
    vehicleSpeed = vehicleSpeed | ((0xFF0000 | (receiveCANData[1])) << 8);
    vehicleSpeed = vehicleSpeed | ((0x000000 | (receiveCANData[0])));

    actualVehicleSpeed = vehicleSpeed;
    printf("actualVehiclespeed [%d]\n", actualVehicleSpeed);
		    
//    memcpy((*(receiveCANData)), (*(receiveCANFrame+1)),CAN_DATA_SIZE);
}


void epsControl::receiveVehicleSpeed() {
    // Receive TCP/IP frame from client
   int bytesReceived = recv(etherNERIClient_socket, receiveCANFrame, CAN_FRAME_SIZE, 0);
    if(bytesReceived > 0) {
        // Data received successfully
        // Decode received frame and extract vehicle speed
        int actualVehicleSpeed;
        memcpy(&actualVehicleSpeed, &receiveCANFrame[1], sizeof(actualVehicleSpeed));
        std::cout << "Received Vehicle Speed: " << actualVehicleSpeed << std::endl;
    } else if (bytesReceived == 0) {
        // No data received
        std::cout << "No data received within "  << std::endl;
    } else {
        // Error occurred during receiving
        std::cerr << "Error in receiving data." << std::endl;
    }
}



int main()
{
	epsControl eps;
	eps.initSocketConnection();
	expectedVehicleSpeed =100;
	while (true)
	{
		//printf("Enter ExpectedSpeed\n");
		//scanf("%d", &expectedVehicleSpeed);
		expectedVehicleSpeed ++;
		if(expectedVehicleSpeed ==500)
		{
			expectedVehicleSpeed=100;
		}

		eps.messageEncoding();
	clock_gettime(CLOCK_MONOTONIC, &next_time_);
    printf("After encoding going to send timespec.tv_nsec:%jd.%09ld\n",next_time_.tv_sec, next_time_.tv_nsec); 
		eps.sendVehicleSpeed();
	clock_gettime(CLOCK_MONOTONIC, &next_time_);
    printf("post reciveing timespec.tv_nsec:%jd.%09ld\n",next_time_.tv_sec, next_time_.tv_nsec); 
		eps.receiveVehicleSpeed();
		//eps.messageDecoding();
        }
	eps.closeSocketConnection();
	return 0;
}
