#pragma once

#include "gn10_can/core/can_device.hpp"

namespace gn10_can {
namespace devices {

class MotorDriver : public CANDevice {
  public:
    MotorDriver(CANManager& manager, uint8_t dev_id);

    void send_init();

    void send_target(float target);

  private:
};
}  // namespace devices
}  // namespace gn10_can