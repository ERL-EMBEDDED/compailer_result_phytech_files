#ifndef _EPS_CONTROL_
#define _EPS_CONTROL_

// Include Files here
#include<stdio.h>
#include<iostream>
//#include "../common.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>



// Declare functions here


// Define Macros here

#define CAN_FRAME_SIZE 5
#define CAN_DATA_SIZE 4


unsigned int expectedVehicleSpeed;
unsigned int actualVehicleSpeed;


//Class starts from here

class epsControl
{
private: 

	const char *host = "192.168.3.11";
    	int etherNERIPort = 8080;
    	int receivePort = 1111;
	struct sockaddr_in etherNERIServer_address;
	struct sockaddr_in receiveServer_address;
	int etherNERIClient_socket;
	int receiveClient_socket;
	int etherNERIServer_socket;
	int receiveServer_socket;
       
	unsigned int vehicleSpeed;
       	unsigned int *intDataptr;  // ptr conversion
        unsigned char *charDataptr; // ptr conversion
 
  


	struct sockaddr_in etherNERIClient_address;
	struct sockaddr_in receiveClient_address;
	unsigned char sendCANFrame[CAN_FRAME_SIZE];
	unsigned char receiveCANFrame[CAN_FRAME_SIZE];
	unsigned char sendCANData[CAN_DATA_SIZE];
	unsigned char receiveCANData[CAN_DATA_SIZE];
	unsigned char CANMsgId = 0x55;
	int bytes_received;


public:

	epsControl();
	~epsControl();
	void initSocketConnection();
	void closeSocketConnection();
	void sendVehicleSpeed();
	void messageEncoding();
	void messageDecoding();
	void receiveVehicleSpeed();
	void epsControlThreadfunc();

};

#endif
