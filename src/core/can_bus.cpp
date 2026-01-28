#include "gn10_can/core/can_bus.hpp"
#include "gn10_can/core/can_device.hpp"
#include "gn10_can/core/can_frame.hpp"

namespace gn10_can {

CANBus::CANBus(drivers::DriverInterface& driver) : driver_(driver) {}

void CANBus::update() {
    CANFrame frame;
    while (driver_.receive(frame)) {
        auto it = devices_.find(frame.id);
        if (it != devices_.end()) {
            it->second->on_receive(frame);
        }
    }
}

bool CANBus::send_raw(const CANFrame& frame) {
    return driver_.send(frame);
}

void CANBus::attach(CANDevice* device, uint32_t rx_id) {
    devices_[rx_id] = device;
}

void CANBus::detach(uint32_t rx_id) {
    devices_.erase(rx_id);
}

} // namespace gn10_can
