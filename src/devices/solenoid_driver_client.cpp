#include "gn10_can/devices/solenoid_driver_client.hpp"

#include "gn10_can/utils/can_converter.hpp"

namespace gn10_can {
namespace devices {

SolenoidDriverClient::SolenoidDriverClient(CANBus& bus, uint8_t dev_id)
    : CANDevice(bus, id::DeviceType::SolenoidDriver, dev_id)
{
}

void SolenoidDriverClient::set_init()
{
    std::array<uint8_t, 1> init{0};
    send(id::MsgTypeSolenoidDriver::Init, init);
}

void SolenoidDriverClient::set_target(const uint8_t& target)
{
    std::array<uint8_t, 1> payload{};
    converter::pack(payload, 0, target);
    send(id::MsgTypeSolenoidDriver::Target, payload);
}

void SolenoidDriverClient::set_target(const std::array<bool, 8>& target)
{
    uint8_t data = 0;
    for (int i = 0; i < 8; i++) {
        data |= static_cast<uint8_t>(target[i]) << i;
    }
    set_target(data);
}

void SolenoidDriverClient::on_receive(const CANFrame& frame) {}

}  // namespace devices
}  // namespace gn10_can
