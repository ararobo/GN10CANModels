#include "gn10_can/devices/servo_moter_client.hpp"

#include "gn10_can/utils/can_converter.hpp"

namespace gn10_can {
namespace devices {

ServoMoterClient::ServoMoterClient(CANBus& bus, uint8_t dev_id)
    : CANDevice(bus, id::DeviceType::ServoMoter, dev_id)
{
}

void ServoMoterClient::set_init(uint16_t min_us, u_int16_t max_us)
{
    std::array<uint8_t, 4> payload{};
    converter::pack(payload, 0, min_us);
    converter::pack(payload, 2, max_us);
    send(id::MsgTypeServoMoter::Init, payload);
}

void ServoMoterClient::set_angle_rad(float angle_rad)
{
    std::array<uint8_t, 4> payload{};
    converter::pack(payload, 0, angle_rad);
    send(id::MsgTypeServoMoter::AngleRad, payload);
}

}  // namespace devices
}  // namespace gn10_can