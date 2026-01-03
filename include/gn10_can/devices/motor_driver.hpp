#pragma once

#include "gn10_can/core/can_device.hpp"
#include "gn10_can/devices/motor_driver_types.hpp"

namespace gn10_can {
namespace devices {

class MotorDriver : public CANDevice {
  public:
    MotorDriver(CANManager& manager, uint8_t dev_id);

    void send_init(const MotorConfig& config);

    void send_target(float target);

    void send_gain(devices::GainType type, float value);

    void send_feedback(float current_val, uint8_t limit_switch_state);

    void send_status(float load_current, int8_t temperature);

    void on_receive(const CANFrame& frame) override;

    float get_feedback_value() const;
    uint8_t get_limit_switch_state() const;
    float get_load_current() const;
    int8_t get_temperature() const;

  private:
    float feedback_val_{0.0f};
    uint8_t limit_sw_state_{0};
    float load_current_{0.0f};
    int8_t temperature_{0};
};
}  // namespace devices
}  // namespace gn10_can