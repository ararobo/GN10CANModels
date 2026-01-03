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
    std::array<uint8_t, 4> raw_data{};
    converter::pack(raw_data, 0, target);
    send(id::MsgTypeMotorDriver::Target, raw_data);
}
}  // namespace devices
}  // namespace gn10_can
