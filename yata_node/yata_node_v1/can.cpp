#include "can.h"
#include "can_frame_creator.h"
#include "global.h"

bool endian_flag = false;

int CRC = 0;

CANMessage::CANMessage() : data(0) {}

CanInterface::CanInterface() {
    if (!initCan()) {
        std::cerr << "Initialization of CAN interface failed." << std::endl;
        exit(1);
    }
}

CanInterface::~CanInterface() {
    close(socket_);
}

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

bool CanInterface::initCan() {
    const char* ifname = "can0";
    if (system("ip link set can0 down") != 0) {
        perror("Error setting down can0 interface");
        return false;
    }

    char command[100];
    snprintf(command, sizeof(command), "ip link set %s type can bitrate 250000", ifname);
    if (system(command) != 0) {
        perror("Error setting up can0 interface");
        return false;
    }

    if (system("ip link set can0 up") != 0) {
        perror("Error setting up can0 interface");
        return false;
    }

    if ((socket_ = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Socket creation failed");
        return false;
    }

    struct ifreq ifr;
    strcpy(ifr.ifr_name, ifname);
    if (ioctl(socket_, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl failed");
        close(socket_);
        return false;
    }

    addr_.can_family = AF_CAN;
    addr_.can_ifindex = ifr.ifr_ifindex;
    if (bind(socket_, (struct sockaddr *)&addr_, sizeof(addr_)) < 0) {
        perror("Binding failed");
        close(socket_);
        return false;
    }

    return true;
}

bool CanInterface::sendFrame(struct can_frame* frame) {
    if (write(socket_, frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("Write failed");
        return false;
    }
    return true;
}

bool CanInterface::receiveFrame(struct can_frame* frame) {
    if (read(socket_, frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("Read failed");
        return false;
    }
    return true;
}

void CanInterface::ReceivedData() {
    struct can_frame frame;
    if (!receiveFrame(&frame)) {
        std::cerr << "Failed to receive CAN frame." << std::endl;
        return;
    }

    uint64_t big_endian = 0;
    std::memcpy(&big_endian, frame.data, sizeof(big_endian));
    uint64_t little_endian = boost::endian::endian_reverse(big_endian);
    uint8_t byteArray[8];
    std::memcpy(byteArray, &little_endian, sizeof(little_endian));
    std::cout << "--->>>" << getCurrentDateTimeseconds() << " Recevive CAN data " << std::hex << little_endian << std::endl;

    uint16_t can_id = frame.can_id;
    float vehicle_speed = (byteArray[2] << 8) | byteArray[1];

    std::cout << "Received Values: " << std::endl;
    std::cout << "can id : " << can_id << std::endl;
    std::cout << "Vehicle Speed: " << vehicle_speed << std::endl;

    _hold_ = true;
    server.sendMessage(vehicle_speed);
    _hold_ = false;
}

void CanInterface::send_can_data(float Expected_wheelSpeedFR, float Expected_wheelSpeedFL, float Expected_vehicleSpeed) {
    std::cout << "Expected_wheelSpeedFR: " << Expected_wheelSpeedFR << std::endl;
    std::cout << "Expected_wheelSpeedFL: " << Expected_wheelSpeedFL << std::endl;
    std::cout << "Expected_vehicleSpeed: " << Expected_vehicleSpeed << std::endl;

    CANMessage msg;
    createCANMessage(msg, Expected_wheelSpeedFR, Expected_wheelSpeedFL, Expected_vehicleSpeed);

    msg.display();
    std::cout << msg << std::endl;

    struct can_frame frame;
    frame.can_id = 0x123;
    frame.can_dlc = 8;
    uint64_t msgData = msg.getData();
    std::memcpy(frame.data, &msgData, sizeof(msgData));

    if (sendFrame(&frame)) {
        std::cout << "--->>>" << getCurrentDateTimeseconds() << " CAN frame sent successfully." << std::endl;
    } else {
        std::cerr << "Failed to send CAN frame." << std::endl;
    }
}

void CanInterface::run() {
    while (true) {
        std::cout << "CAN Thread is running" << std::endl;
        ReceivedData();
        usleep(1);
    }
}
std::ostream& operator<<(std::ostream& os, const CANMessage& msg) {
    os << std::bitset<64>(msg.getData());
    return os;
}
