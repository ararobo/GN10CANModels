#include "gn10_can/devices/solenoid_driver_client.hpp"

#include "gn10_can/utils/can_converter.hpp"

namespace gn10_can {
namespace devices {

SolenoidDriverClient::SolenoidDriverClient(CANBus& bus, uint8_t dev_id)
    : CANDevice(bus, id::DeviceType::SolenoidDriver, dev_id) {}

void SolenoidDriverClient::set_target(bool target) {
    std::array<uint8_t, 1> payload{};
    converter::pack(payload, 0, target);
    send(id::MsgTypeSolenoidDriver::Target, payload);
}

void SolenoidDriverClient::on_receive(const CANFrame& frame) {
    auto id_fields = id::unpack(frame.id);

    if (id_fields.type != device_type_ || id_fields.dev_id != device_id_) {
        return;
    }

    if (id_fields.is_command(id::MsgTypeSolenoidDriver::Target)) {
        bool value;
        if (converter::unpack(frame.data.data(), frame.dlc, 0, value)) {
            feedback_value_ = value;
        }
    }
}

bool SolenoidDriverClient::feedback_value() const {
    return feedback_value_;
}

}  // namespace devices
}  // namespace gn10_can
