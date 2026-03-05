#include "gn10_can/devices/servo_motor_client.hpp"
#include "gn10_can/utils/can_converter.hpp"

namespace gn10_can {
namespace devices {

ServoMotorClient::ServoMotorClient(CANBus& bus, uint8_t device_id)
    : CANDevice(bus, id::DeviceType::ServoMotor, device_id)
{
}

void ServoMotorClient::set_init(uint16_t min_us, u_int16_t max_us)
{
    std::array<uint8_t, 4> payload{};
    converter::pack(payload, 0, min_us);
    converter::pack(payload, 2, max_us);
    send(id::MsgTypeServoMotor::Init, payload);
}

void ServoMotorClient::set_angle_rad(float angle_rad)
{
    std::array<uint8_t, 4> payload{};
    converter::pack(payload, 0, angle_rad);
    send(id::MsgTypeServoMotor::AngleRad, payload);
}

}  // namespace devices
}  // namespace gn10_can