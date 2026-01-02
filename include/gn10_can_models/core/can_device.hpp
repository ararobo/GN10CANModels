#pragma once

#include <cstdint>

#include "can_frame.hpp"

namespace gn10_can {

class CANManager;

class CANDevice {
 public:
    CANDevice(CANManager& manager, uint8_t device_type, uint8_t device_id);
    virtual ~CANDevice() = default;

    virtual void on_receive(const CANFrame& frame) = 0;

    uint8_t get_device_type() const { return device_type_; }

    uint8_t get_device_id() const { return device_id_; }

 protected:
    bool send(const uint8_t* data, uint8_t len, uint8_t command);

    CANManager& manager_;
    uint8_t device_type_;
    uint8_t device_id_;
};

}  // namespace gn10_can