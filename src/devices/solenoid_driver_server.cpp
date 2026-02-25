#include "gn10_can/devices/solenoid_driver_server.hpp"

#include "gn10_can/utils/can_converter.hpp"

namespace gn10_can {
namespace devices {

SolenoidDriverServer::SolenoidDriverServer(CANBus& bus, uint8_t dev_id)
    : CANDevice(bus, id::DeviceType::SolenoidDriver, dev_id) {}

void SolenoidDriverServer::send_feedback(bool feedback_val) {
    std::array<uint8_t, 1> payload{};
    converter::pack(payload, 0, feedback_val);
    send(id::MsgTypeSolenoidDriver::Feedback, payload);
}

bool SolenoidDriverServer::get_new_target(bool& target) {
    if (target_.has_value()) {
        target = target_.value();
        target_.reset();

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
        bool value;
        if (converter::unpack(frame.data.data(), frame.dlc, 0, value)) {
            target_ = value;
        }
    }
}

}  // namespace devices
}  // namespace gn10_can
