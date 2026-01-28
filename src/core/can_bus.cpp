#include "gn10_can/core/can_manager.hpp"

#include "gn10_can/core/can_device.hpp"

namespace gn10_can {

CANManager::CANManager(drivers::DriverInterface& driver) : driver_(driver) {}

bool CANManager::register_device(CANDevice* device) {
    if (device_count_ >= MAX_DEVICES) {
        return false;
    }

    devices_[device_count_] = device;
    device_count_++;
    return true;
}

void CANManager::update() {
    CANFrame rx_frame;
    while (driver_.receive(rx_frame)) {
        for (std::size_t i = 0; i < device_count_; ++i) {
            devices_[i]->on_receive(rx_frame);
        }
    }
}

bool CANManager::send_frame(const CANFrame& frame) {
    return driver_.send(frame);
}

}  // namespace gn10_can
