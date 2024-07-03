#ifndef CAN_COMMUNICATION_H
#define CAN_COMMUNICATION_H

struct ThreadArgs {
    const char *canInterface;
    int bitrate;
    const char *originalCanMessage;
};

int Can_communication(const char *canInterface, const int bitrate, const char *originalCanMessage);
void *CanThread(void *args);

#endif // CAN_COMMUNICATION_H

