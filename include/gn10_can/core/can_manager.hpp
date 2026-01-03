#pragma once

#include <array>
#include <cstddef>

#include "gn10_can/drivers/driver_interface.hpp"

namespace gn10_can {

class CANDevice;

class CANManager {
  public:
    static constexpr std::size_t MAX_DEVICES = 16;

    explicit CANManager(drivers::DriverInterface& driver);

    bool register_device(CANDevice* device);

    void update();

    bool send_frame(const CANFrame& frame);

  private:
    drivers::DriverInterface& driver_;
    std::array<CANDevice*, MAX_DEVICES> devices_{};
    std::size_t device_count_ = 0;
};
}  // namespace gn10_can