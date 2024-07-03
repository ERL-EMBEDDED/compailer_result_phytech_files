#include "server.h"
#include <iostream> 
#include "find_kill_port.h"
#include "global.h"
#include "json_parser.h"

static std::string getCurrentDateTimeseconds() {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_tm = *std::localtime(&now_time_t);

    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
    auto micro = std::chrono::duration_cast<std::chrono::microseconds>(duration).count() % 1000;
    auto nano = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() % 1000;

    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%Y-%m-%dT%H:%M:%S") << '.' << std::setw(3) << std::setfill('0') << millis
        << std::setw(3) << std::setfill('0') << micro << std::setw(3) << std::setfill('0') << nano;

        return oss.str();
}
epsServer::epsServer() {
	serverSocket = -1; 
	clientSocket = -1;
	addrLen = sizeof(clientAddr);
	memset(&serverAddr, 0, sizeof(serverAddr));
	memset(&clientAddr, 0, sizeof(clientAddr));
	if(initSocket()){

	}
	else{
		printf("EPS init Exit \n");
	}
}

epsServer::~epsServer() {
	if (clientSocket != -1) {
		close(clientSocket);
	}
	if (serverSocket != -1) {
		close(serverSocket);
	}
}


bool epsServer::acceptClient() {
	clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
	if (clientSocket < 0) {
		std::cerr << "Failed to accept client\n" << std::endl;
		return false;
	}
	std::cout << "Client connected: " << inet_ntoa(clientAddr.sin_addr) << std::endl;
	return true;
}

bool epsServer::initSocket() {	
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		std::cerr << "Failed to create socket\n" << std::endl;
		return false;
	}
	int opt = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		 std::cerr << "Set socket options failed\n" << std::endl;
                 close(serverSocket);
		 return false;
		         }


	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
		std::cerr << "Bind failed\n" << std::endl;
		findAndKillPortProcess(SERVER_PORT);
		close(serverSocket);
		serverSocket = -1;
		return false;
	}

	if (listen(serverSocket, BACKLOG) < 0) {
		std::cerr << "Listen failed" << std::endl;
		close(serverSocket);
		serverSocket = -1;
		return false;
	}
    std::cout << "Server initialized and listening on " << SERVER_IP << ":" << SERVER_PORT << std::endl;
	acceptClient();
	SERVER_STATES = RUNNNING;
	return true;
}
std::string epsServer::receiveMessage() {
    const int bufferSize = 4024;
	char buffer[bufferSize];
	ssize_t bytesRead = recv(clientSocket, buffer, bufferSize - 1, 0);
	if (bytesRead < 0) {
		std::cerr << "Failed to receive message from client" << std::endl;
		return -1;
	} else if (bytesRead == 0) {
		std::cerr << "Client disconnected" << std::endl;
		acceptClient();
		return 0;
	}

	buffer[bytesRead] = '\0'; 
	std::string receivedMessage(buffer);
	std::cout <<"--->>>"<<getCurrentDateTimeseconds() << "Received from client: " << receivedMessage << std::endl; 
    return receivedMessage;
        
}
int epsServer::sendMessage(std::string send_Message){
    ssize_t bytesSent = send(clientSocket, send_Message.c_str(), send_Message.size(), 0);
	if (bytesSent < 0) {
		std::cerr << "Failed to send acknowledgment to client " << std::endl;
		return -1;
	}

	std::cout <<"--->>>"<<getCurrentDateTimeseconds() <<" Message sent to client: " << send_Message << std::endl;
	return bytesSent;

}

int Tcpip_receivedMessage(){

   auto [status, Expected_wheelSpeedFL, Expected_wheelSpeedFR,Expected_vehicleSpeed] parseJson_read(receiveMessage());
   if(status == 1){
        can.send_can_data(Expected_wheelSpeedFR ,Expected_wheelSpeedFL ,Expected_vehicleSpeed);
   }
   else if(status == 0){
    std::cout<<"CAN Frame Confiqure Scucess " <<std::endl;
   }
    return 0;
}

int Tcpip_sendMessage(float Expected_wheelSpeedFR  ,float Expected_wheelSpeedFL ,float Expected_vehicleSpeed){

    sendMessage(parseJson_create(float Expected_wheelSpeedFR  ,float Expected_wheelSpeedFL ,float Expected_vehicleSpeed));

    return 0;
}

void epsServer::run(){

    while (true) {
		if(!_hold_ && SERVER_STATES == RUNNNING){
			Tcpip_receivedMessage();
		}
		usleep(1);
	}

}
void epsServer::closeClientSocket() {
	if (clientSocket != -1) {
		close(clientSocket);
		clientSocket = -1;
	}
}
void epsServer::closeServerSocket() {
	if (serverSocket != -1) {
	       close(serverSocket);
	       serverSocket = -1;
	       }
}
bool epsServer::isServerSocketValid() const {
	return serverSocket != -1;
}

