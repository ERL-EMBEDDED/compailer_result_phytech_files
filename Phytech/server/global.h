#ifndef GLOBAL_H
#define GLOBAL_H

#include <memory>
#include "server.h"
#include "can.h"
extern bool _hold_;

typedef enum{
	INIT,
	WAIT,
	RUNNNING
}TCP_IP_t;

extern TCP_IP_t SERVER_STATES;

extern epsServer server;
extern CanInterface can;
extern void sendEncodedData(int v_speed);
extern int sendMessage(float speed); 

extern std::shared_ptr<int> actualVehicleSpeed;
extern std::shared_ptr<int> expectedVehicleSpeed;

#endif // GLOBAL_H

