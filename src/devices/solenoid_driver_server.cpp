#include "gn10_can/devices/solenoid_driver_server.hpp"

#include "gn10_can/utils/can_converter.hpp"

namespace gn10_can {
namespace devices {

SolenoidDriverServer::SolenoidDriverServer(CANBus& bus, uint8_t dev_id)
    : CANDevice(bus, id::DeviceType::SolenoidDriver, dev_id)
{
}

bool SolenoidDriverServer::get_new_init()
{
    if (init_.has_value()) {
        return false;
    }
    return true;
}
bool SolenoidDriverServer::get_new_target(uint8_t& target)
{
    if (!target_.has_value()) {
        return false;
    }
    target = target_.value();
    target_.reset();
    return true;
}

bool SolenoidDriverServer::get_new_target(std::array<bool, 8>& target)
{
    uint8_t data;
    if (!get_new_target(data)) {
        return false;
    }
    for (int i = 0; i < 8; i++) {
        target[i] = (data >> i) & 1;
    }
    return true;
}

void SolenoidDriverServer::on_receive(const CANFrame& frame)
{
    auto id_fields = id::unpack(frame.id);

    if (id_fields.is_command(id::MsgTypeSolenoidDriver::Init)) {
        uint8_t value;
        if (converter::unpack(frame.data.data(), frame.dlc, 0, value)) {
            init_ = value;
        }
    } else if (id_fields.is_command(id::MsgTypeSolenoidDriver::Target)) {
        uint8_t value;
        if (converter::unpack(frame.data.data(), frame.dlc, 0, value)) {
            target_ = value;
        }
    }
}

}  // namespace devices
}  // namespace gn10_can
