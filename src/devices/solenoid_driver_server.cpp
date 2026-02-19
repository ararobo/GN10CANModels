#include "solenoid_driver_server.hpp"

#include "gn10_can/utils/can_converter.hpp"

namespace gn10_can {
namespace devices {

SolenoidDriverServer::SolenoidDriverServer(CANBus& bus, uint8_t dev_id)
    : CANDevice(bus, id::DeviceType::SolenoidDriver, dev_id) {}

bool SolenoidDriverServer::get_new_target(bool& target) {
    if (is_target_updated_) {
        target             = target_;
        is_target_updated_ = false;

        return true;
    }

    return false;
}

void SolenoidDriverServer::on_receive(const CANFrame& frame) {
    auto id_fields = id::unpack(frame.id);

    if (id_fields.type != device_type_ || id_fields.dev_id != device_id_) {
        return;
    }

    if (id_fields.is_command(id::MsgTypeSolenoidDriver::Target)) {
        converter::unpack(frame.data.data(), frame.dlc, 0, target_);
    }

    is_target_updated_ = true;
}

}  // namespace devices
}  // namespace gn10_can
