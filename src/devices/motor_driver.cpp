#include "gn10_can/devices/motor_driver.hpp"

#include "gn10_can/utils/can_converter.hpp"

namespace gn10_can {
namespace devices {

MotorDriver::MotorDriver(CANBus& bus, uint8_t dev_id)
    : CANDevice(bus, id::DeviceType::MotorDriver, dev_id) {}

void MotorDriver::send_init(const MotorConfig& config) {
    send(id::MsgTypeMotorDriver::Init, config.to_bytes());
}

void MotorDriver::send_target(float target) {
    std::array<uint8_t, 4> payload{};
    converter::pack(payload, 0, target);
    send(id::MsgTypeMotorDriver::Target, payload);
}

void MotorDriver::send_gain(devices::GainType type, float value) {
    std::array<uint8_t, 5> payload{};
    payload[0] = static_cast<uint8_t>(type);
    converter::pack(payload, 1, value);
    send(id::MsgTypeMotorDriver::Gain, payload);
}

void MotorDriver::send_feedback(float current_val, uint8_t limit_switch_state) {
    std::array<uint8_t, 5> payload{};
    converter::pack(payload, 0, current_val);
    converter::pack(payload, 4, limit_switch_state);
    send(id::MsgTypeMotorDriver::Feedback, payload);
}

void MotorDriver::send_status(float load_current, int8_t temperature) {
    std::array<uint8_t, 5> payload{};
    converter::pack(payload, 0, load_current);
    converter::pack(payload, 4, temperature);
    send(id::MsgTypeMotorDriver::Status, payload);
}

void MotorDriver::on_receive(const CANFrame& frame) {
    auto id_fields = id::unpack(frame.id);

    if (id_fields.type != device_type_ || id_fields.dev_id != device_id_) {
        return;
    }

    if (id_fields.is_command(id::MsgTypeMotorDriver::Feedback)) {
        converter::unpack(frame.data.data(), frame.dlc, 0, feedback_val_);
        converter::unpack(frame.data.data(), frame.dlc, 4, limit_sw_state_);
    } else if (id_fields.is_command(id::MsgTypeMotorDriver::Status)) {
        converter::unpack(frame.data.data(), frame.dlc, 0, load_current_);
        converter::unpack(frame.data.data(), frame.dlc, 4, temperature_);
    }
}

float MotorDriver::get_feedback_value() const {
    return feedback_val_;
}

uint8_t MotorDriver::get_limit_switch_state() const {
    return limit_sw_state_;
}

float MotorDriver::get_load_current() const {
    return load_current_;
}

int8_t MotorDriver::get_temperature() const {
    return temperature_;
}

}  // namespace devices
}  // namespace gn10_can
