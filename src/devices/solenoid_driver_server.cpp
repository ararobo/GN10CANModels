#include "gn10_can/devices/solenoid_driver_server.hpp"

#include "gn10_can/utils/can_converter.hpp"

namespace gn10_can {
namespace devices {

SolenoidDriverServer::SolenoidDriverServer(CANBus& bus, uint8_t dev_id)
    : CANDevice(bus, id::DeviceType::SolenoidDriver, dev_id) {}

bool SolenoidDriverServer::get_new_target(std::array<bool, 8>& target) {
    for (int i = 0; i < 8; i++) {
        if (target_[i].has_value()) {
            target[i] = target_[i].value();
            target_[i].reset();

            return true;
        }
    }

    return false;
}

void SolenoidDriverServer::on_receive(const CANFrame& frame) {
    auto id_fields = id::unpack(frame.id);

    if (id_fields.type != device_type_ || id_fields.dev_id != device_id_) {
        return;
    }

    if (id_fields.is_command(id::MsgTypeSolenoidDriver::Target)) {
        std::array<bool, 8> value;
        if (converter::unpack(frame.data.data(), frame.dlc, 0, value)) {
            for (int i = 0; i < 8; i++) {
                target_[i] = value[i];
            }
        }
    }
}

}  // namespace devices
}  // namespace gn10_can
