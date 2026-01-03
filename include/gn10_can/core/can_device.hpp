#pragma once

#include <cstdint>

#include "gn10_can/core/can_frame.hpp"
#include "gn10_can/core/can_manager.hpp"

namespace gn10_can {

class CANDevice {
  public:
    CANDevice(CANManager& manager, id::DeviceType device_type, uint8_t device_id);
    virtual ~CANDevice() = default;

    virtual void on_receive(const CANFrame& frame) = 0;

    virtual void update() {}

    id::DeviceType get_device_type() const { return device_type_; }

    uint8_t get_device_id() const { return device_id_; }

  protected:
    template <typename CmdEnum>
    bool send(CmdEnum command, const uint8_t* data = nullptr, uint8_t len = 0) {
        auto frame = CANFrame::make(device_type_, device_id_, command, data, len);
        return manager_.send_frame(frame);
    }

    template <typename CmdEnum>
    bool send(CmdEnum command, std::initializer_list<uint8_t> data) {
        return send(command, data.begin(), data.size());
    }

    CANManager& manager_;
    id::DeviceType device_type_;
    uint8_t device_id_;
};

}  // namespace gn10_can