#ifndef CAN_H
#define CAN_H

#include <atomic>

extern std::atomic<int> value;

int can_init(const char* ifname, int* s);
int can_send(int s, uint32_t can_id, uint8_t can_dlc, int value);

#endif // CAN_H

