#include "gn10_can/devices/servo_driver_client.hpp"

#include "gn10_can/utils/can_converter.hpp"

namespace gn10_can {

ServoDriverClient::ServoDriverClient(CANBus& bus, uint8_t device_id)
    : CANDevice(bus, id::DeviceType::ServoDriver, device_id)
{
}

void ServoDriverClient::set_init(float frequency_hz)
{
    std::array<uint8_t, 4> payload{};
    converter::pack(payload, 0, frequency_hz);
    send(id::MsgTypeServoDriver::Init, payload);
}

void ServoDriverClient::set_duty_cycle(uint16_t duty_cycle)
{
    std::array<uint8_t, 2> payload{};
    converter::pack(payload, 0, duty_cycle);
    send(id::MsgTypeServoDriver::Target, payload);
}

void ServoDriverClient::on_receive(const CANFrame& frame) {}

}  // namespace gn10_can
