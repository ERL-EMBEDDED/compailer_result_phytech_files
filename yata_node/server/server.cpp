// server.cpp
#include "server.h"
#include <cstring>    // For memset
#include <string>
#include <iostream>   // For std::cout, std::cerr
#include "global.h"
#include <thread>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <chrono>
#include "find_kill_port.h"

#include "json.hpp" // Assuming json.hpp is in the same directory or adjust the path accordingly
using json = nlohmann::json;


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

int epsServer::receiveMessage() {
        const int bufferSize = 4024; // Adjust buffer size as needed
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

	buffer[bytesRead] = '\0'; // Ensure null-terminated string
	std::string receivedMessage(buffer);
	std::cout <<"--->>>"<<getCurrentDateTimeseconds() << "Received from client: " << receivedMessage << std::endl; 
	try {
        auto jsonObj = json::parse(receivedMessage);
        std::string configValue = jsonObj.at("config").get<std::string>();

        if (configValue == "yes") {
            auto signals = jsonObj.at("data")[0].at("signals");

            for (const auto& signal : signals) {
                std::string name = signal.at("name").get<std::string>();

                if (name == "WheelSpeedFR") {
                    _config_wheelSpeedFR_Min = signal.at("minimum").get<float>();
                    _config_wheelSpeedFR_Max = signal.at("maximum").get<float>();
                    _config_wheelSpeedFR_Scale = signal.at("scale").get<float>();
                    _config_wheelSpeedFR_start_bit = signal.at("start_bit").get<int>();
                    _config_wheelSpeedFR_length = signal.at("bit_length").get<int>();
                } else if (name == "WheelSpeedFL") {
                    _config_wheelSpeedFL_Min = signal.at("minimum").get<float>();
                    _config_wheelSpeedFL_Max = signal.at("maximum").get<float>();
                    _config_wheelSpeedFL_Scale = signal.at("scale").get<float>();
                    _config_wheelSpeedFL_start_bit = signal.at("start_bit").get<int>();
                    _config_wheelSpeedFL_length = signal.at("bit_length").get<int>();
                } else if (name == "VehicleSpeed") {
                    _config_vehicleSpeed_Min = signal.at("minimum").get<float>();
                    _config_vehicleSpeed_Max = signal.at("maximum").get<float>();
                    _config_vehicleSpeed_Scale = signal.at("scale").get<float>();
                    _config_vehicleSpeed_start_bit = signal.at("start_bit").get<int>();
                    _config_vehicleSpeed_length = signal.at("bit_length").get<int>();
                } else if (name == "VehicleSpeedCRC") {
                    _config_vehicleSpeedCRC_Min = signal.at("minimum").get<int>();
	            _config_vehicleSpeedCRC_Max = signal.at("maximum").get<int>();
	            _config_vehicleSpeedCRC_start_bit = signal.at("start_bit").get<int>();
                    _config_vehicleSpeedCRC_length = signal.at("bit_length").get<int>();
                    _config_vehicleSpeedCRC_Scale = signal.at("scale").get<float>();
                } else if (name == "VehicleSpeedClock") {
                    _config_vehicleSpeedClock_start_bit = signal.at("start_bit").get<int>();
                    _config_vehicleSpeedClock_length = signal.at("bit_length").get<int>();
		    _config_vehicleSpeedClock_Scale = signal.at("scale").get<float>();
		    _config_vehicleSpeedClock_Max = signal.at("maximum").get<int>();
	            _config_vehicleSpeedClock_Min = signal.at("minimum").get<int>();
                }
            }

             // Print values for verification
            std::cout << "Config Yes:\n";
            std::cout << "WheelSpeedFR: Min=" << _config_wheelSpeedFR_Min << ", Max=" <<_config_wheelSpeedFR_Max << ", Scale=" << _config_wheelSpeedFR_Scale << ", start_bit=" << _config_wheelSpeedFR_start_bit << "\n";
            std::cout << "WheelSpeedFL: Min=" << _config_wheelSpeedFL_Min << ", Max=" << _config_wheelSpeedFL_Max << ", Scale=" << _config_wheelSpeedFL_Scale << "\n";
            std::cout << "VehicleSpeed: Min=" << _config_vehicleSpeed_Min << ", Max=" << _config_vehicleSpeed_Max << ", Scale=" << _config_vehicleSpeed_Scale << "\n";
            std::cout << "VehicleSpeedCRC start bit=" << _config_vehicleSpeedCRC_start_bit << ", length=" << _config_vehicleSpeedCRC_length << ", scale=" << _config_vehicleSpeedCRC_Scale << "\n";
            std::cout << "VehicleSpeedClock start bit=" << _config_vehicleSpeedClock_start_bit << ", length=" << _config_vehicleSpeedClock_length << "\n";
          std::string send_Message = "{\"config\"}";

        ssize_t bytesSent = send(clientSocket, send_Message.c_str(), send_Message.size(), 0);

        } else if (configValue == "no") {
           float Expected_wheelSpeedFR = jsonObj.at("WheelSpeedFR").get<float>();
           float Expected_wheelSpeedFL = jsonObj.at("WheelSpeedFL").get<float>();
           float Expected_vehicleSpeed = jsonObj.at("VehicleSpeed").get<float>();
              
      
            // Print values for verification
            std::cout << "Config No:\n";
            std::cout << "Expected_wheelSpeedFR: " << Expected_wheelSpeedFR << "\n";
            std::cout << "Expected_wheelSpeedFL: " << Expected_wheelSpeedFL << "\n";
            std::cout << "Expected_vehicleSpeed: " << Expected_vehicleSpeed << "\n";
	    can.send_can_data(Expected_wheelSpeedFR ,Expected_wheelSpeedFL ,Expected_vehicleSpeed );
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
    }
 return 1;	 
}

int epsServer::sendMessage(float speed) {

	std::cout << "Tcpip actualVehicleSpeed : " << speed<<std::endl;
	std::stringstream speed_a;
        speed_a << speed;
        std::string a_speed = speed_a.str();
	std::cout << "String_in_Speed : " << a_speed<<std::endl;

	std::string send_Message = "{\"Speed\":\"" + a_speed + "\"}";

	ssize_t bytesSent = send(clientSocket, send_Message.c_str(), send_Message.size(), 0);
	if (bytesSent < 0) {
		std::cerr << "Failed to send acknowledgment to client " << std::endl;
		return -1;
	}

	std::cout <<"--->>>"<<getCurrentDateTimeseconds() <<" Message sent to client: " << send_Message << std::endl;
	return bytesSent;
}

void epsServer::run() {

	while (true) {
		if(!_hold_ && SERVER_STATES == RUNNNING){
			receiveMessage();
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
