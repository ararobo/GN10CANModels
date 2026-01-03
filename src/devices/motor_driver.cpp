#include "gn10_can/devices/motor_driver.hpp"

#include "gn10_can/utils/can_converter.hpp"

namespace gn10_can {
namespace devices {

MotorDriver::MotorDriver(CANManager& manager, uint8_t dev_id)
    : CANDevice(manager, id::DeviceType::MotorDriver, dev_id) {}

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

}  // namespace devices
}  // namespace gn10_can
