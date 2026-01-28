#include "gn10_can/core/can_manager.hpp"
#include "gn10_can/core/can_device.hpp"

namespace gn10_can {

CANManager::CANManager(drivers::DriverInterface& driver) : driver_(driver) {}

void CANManager::register_device(CANDevice* device, uint32_t rx_id) {
    subscribers_[rx_id] = device;
}

void CANManager::update() {
    CANFrame frame;
    while (driver_.receive(frame)) {
        auto it = subscribers_.find(frame.id);
        if (it != subscribers_.end()) {
            it->second->on_receive(frame);
        }
    }
}

bool CANManager::send_frame(const CANFrame& frame) {
    return driver_.send(frame);
}

}  // namespace gn10_can
