#include "declare.h"
#include "server.h"
#include "can.h"

ENGINE_STATES CURRENT_ENGINE_STATES;
ENGINE_STATES PREVIOUS_ENGINE_STATES;

int main() {
	Thread CANThread(1,40,SCHED_FIFO);
	Thread SOCKETThread(2,39,SCHED_FIFO);


	CANThread.Start();
	SOCKETThread.Start();
	CURRENT_ENGINE_STATES = KEY_ON;
	while(1){
		usleep(1000'000);
		if(PREVIOUS_ENGINE_STATES == CURRENT_ENGINE_STATES){
			usleep(500000);
			continue;
		}
		PREVIOUS_ENGINE_STATES = CURRENT_ENGINE_STATES;

		switch(CURRENT_ENGINE_STATES){
			case INIT:
				std::cout<<"INIT STATE" <<std::endl;
				CURRENT_ENGINE_STATES = KEY_ON;
				break;
			case KEY_ON:
				std::cout<<"Current_State ::"<<CURRENT_ENGINE_STATES <<std::endl;
				std::cout<<"KEY_ON STATE" <<std::endl;
				CURRENT_ENGINE_STATES = ENGINE_ON;
				std::cout<<"Current_State ::"<<CURRENT_ENGINE_STATES <<std::endl;
				break;
			case ENGINE_ON:
				std::cout<<"ENGINE_ON STATE" <<std::endl;
				CURRENT_ENGINE_STATES = MOVING_STATE;
				break;
			case MOVING_STATE:
				std::cout<<"MOVING STATE" <<std::endl;
				break;
			case OVER_SPEEDING:
				std::cout<<"OVER_SPEEDING STATE" <<std::endl;
				break;
			case TORQUE_OVERLOAD:
				std::cout<<"TORQUE_OVERLOAD STATE" <<std::endl;
				break;
			case DRIFTING:
				std::cout<<"DRIFITING STATE" <<std::endl;
				break;
			case CRASH:
				std::cout<<"CRASH STATE" <<std::endl;
				break;
			case ENGINE_OFF:
				std::cout<<"ENGINE_OFF STATE" <<std::endl;
				break;
		}
	}

	CANThread.Join();
	SOCKETThread.Join();

	return 0;
}

void RunSocket(){

       epsServer server;
       server.acceptClient();
       while (true) {
                printf("Send and Receive Function Runiing \n");
                server.run();
        }
		usleep(10000);
}

void RunCAN(){

	CanInterface can("can0");
        can.initCan("can0");
	 while(1){
	    can.run();	

	 }

}
