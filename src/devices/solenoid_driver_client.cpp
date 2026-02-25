#include "gn10_can/devices/solenoid_driver_client.hpp"

#include "gn10_can/utils/can_converter.hpp"

namespace gn10_can {
namespace devices {

SolenoidDriverClient::SolenoidDriverClient(CANBus& bus, uint8_t dev_id)
    : CANDevice(bus, id::DeviceType::SolenoidDriver, dev_id) {}

void SolenoidDriverClient::set_target(std::array<bool, 8> target) {
    std::array<uint8_t, 8> payload{};
    converter::pack(payload, 0, target);
    send(id::MsgTypeSolenoidDriver::Target, payload);
}

void SolenoidDriverClient::on_receive(const CANFrame& frame) {}

}  // namespace devices
}  // namespace gn10_can
