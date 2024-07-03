#ifndef _DECLARE_H_
#define _DECLARE_H_

#include <pthread.h>
#include <sys/mman.h>  // necessary for mlockall
#include <cstring>
#include <ctime>  // For timespec
#include <stdexcept>
#include <string>
#include<stdio.h>
#include<iostream>
#include <stdint.h>
#include <cstdlib>
#include <unistd.h>
#include <mutex>
#include <sstream>


void RunSocket();
void RunCAN();
void RunSocketThreadFunc()
{
	RunSocket();
	pthread_exit(NULL);
}

void RunCANThreadFunc()
{
	RunCAN();
	pthread_exit(NULL);
}

typedef enum{
	INIT,
	KEY_ON,
	ENGINE_ON,
	MOVING_STATE,
	OVER_SPEEDING,
	TORQUE_OVERLOAD,
	DRIFTING,
	CRASH,
	ENGINE_OFF	
}ENGINE_STATES;

typedef enum{
	INTERLOCKS,
	IGNITION,
	BRAKES,
	UNDERSTEERING,
	OVERSTEERING,
	CRASH_EVE,
	OFF_ROAD,
	DRIFTING_EVE
}ENGINE_EVENTS;




class Thread {
	int priority_;
	int policy_;
	char threadID;

	int64_t         period_ns_;
	struct timespec next_wakeup_time_;

	pthread_t thread_;

	static void* RunSocket(void* data) {
		Thread* thread = static_cast<Thread*>(data);
		RunSocketThreadFunc();    
		return NULL;
	}

	static void* RunCAN(void* data) {
		Thread* thread = static_cast<Thread*>(data);
		RunCANThreadFunc();
		return NULL;
	}

	public:
	Thread(char ID, int priority, int policy, int64_t period_ns = 1'000'000'00)
		: threadID(ID), priority_(priority), policy_(policy), period_ns_(period_ns) {}
	virtual ~Thread() = default;


	void Start() {
		pthread_attr_t attr;

		int ret = pthread_attr_init(&attr);
		if (ret) {
			throw std::runtime_error(std::string("error in pthread_attr_init: ") + std::strerror(ret));
		}

		ret = pthread_attr_setschedpolicy(&attr, policy_);
		if (ret) {
			throw std::runtime_error(std::string("error in pthread_attr_setschedpolicy: ") + std::strerror(ret));
		}

		struct sched_param param;
		param.sched_priority = priority_;
		ret = pthread_attr_setschedparam(&attr, &param);
		if (ret) {
			throw std::runtime_error(std::string("error in pthread_attr_setschedparam: ") + std::strerror(ret));
		}

		ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
		if (ret) {
			throw std::runtime_error(std::string("error in pthread_attr_setinheritsched: ") + std::strerror(ret));
		}

		cpu_set_t cpuset;
		CPU_ZERO(&cpuset);
		// Set the CPU affinity for each thread based on threadID
		switch (threadID) {
			case 1:
				CPU_SET(0, &cpuset); // Assigning to core 0
				break;
			case 2:
				CPU_SET(1, &cpuset); // Assigning to core 1
				break;
			default:
				// Handle other cases if needed
				break;
		}

		ret = pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);
		if (ret) {
			throw std::runtime_error(std::string("error in pthread_attr_setaffinity_np: ") + std::strerror(ret));
		}


		if (threadID == 1)
		{	   
			printf("pthread_create(1) \n"); 
			ret = pthread_create(&thread_, &attr, &Thread::RunSocket, this);
			if (ret) {
				throw std::runtime_error(std::string("error in pthread_create: ") + std::strerror(ret));
			}
		}
		else if (threadID == 2)
		{
			printf("pthread_Create(2) \n");
			ret = pthread_create(&thread_, &attr, &Thread::RunCAN, this);
			if (ret) {
				throw std::runtime_error(std::string("error in pthread_create: ") + std::strerror(ret));
			}

		}
	}

	int Join() {
		return pthread_join(thread_, NULL);
	}

	virtual void Run() noexcept {
		clock_gettime(CLOCK_MONOTONIC, &next_wakeup_time_);

		while (true) {
			Loop();
			next_wakeup_time_ = AddTimespecByNs(next_wakeup_time_, period_ns_);
			clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_wakeup_time_, NULL);
		}
	}

	virtual void Loop() noexcept {

		printf("Raw timespec.tv_nsec:%jd.%09ld\n",next_wakeup_time_.tv_sec, next_wakeup_time_.tv_nsec); 
		printf("Loop get called\n");
	}

	struct timespec AddTimespecByNs(struct timespec ts, int64_t ns) {
		ts.tv_nsec += ns;

		while (ts.tv_nsec >= 1'000'000'000) {
			++ts.tv_sec;
			ts.tv_nsec -= 1'000'000'000;
		}

		while (ts.tv_nsec < 0) {
			--ts.tv_sec;
			ts.tv_nsec += 1'000'000'000;
		}

		return ts;
	}
};

#endif
