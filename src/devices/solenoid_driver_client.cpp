#include "solenoid_driver_client.hpp"

#include "gn10_can/utils/can_converter.hpp"

namespace gn10_can {
namespace devices {

SolenoidDriverClient::SolenoidDriverClient(CANBus& bus, uint8_t dev_id)
    : CANDevice(bus, id::DeviceType::SolenoidDriver, dev_id) {}

void SolenoidDriverClient::set_init() {}

void SolenoidDriverClient::set_target(bool target) {
    std::array<uint8_t, 1> payload{};
    converter::pack(payload, 0, target);
    send(id::MsgTypeSolenoidDriver::Target, payload);
}

}  // namespace devices
}  // namespace gn10_can
