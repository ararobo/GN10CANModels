#include "gn10_can/devices/motor_driver_client.hpp"

#include "gn10_can/utils/can_converter.hpp"

namespace gn10_can {
namespace devices {

MotorDriverClient::MotorDriverClient(CANBus& bus, uint8_t dev_id)
    : CANDevice(bus, id::DeviceType::MotorDriver, dev_id) {}

void MotorDriverClient::set_init(const MotorConfig& config) {
    send(id::MsgTypeMotorDriver::Init, config.to_bytes());
}

void MotorDriverClient::set_target(float target) {
    std::array<uint8_t, 4> payload{};
    converter::pack(payload, 0, target);
    send(id::MsgTypeMotorDriver::Target, payload);
}

void MotorDriverClient::set_gain(devices::GainType type, float value) {
    std::array<uint8_t, 5> payload{};
    payload[0] = static_cast<uint8_t>(type);
    converter::pack(payload, 1, value);
    send(id::MsgTypeMotorDriver::Gain, payload);
}

void MotorDriverClient::on_receive(const CANFrame& frame) {
    auto id_fields = id::unpack(frame.id);

    if (id_fields.is_command(id::MsgTypeMotorDriver::Feedback)) {
        float val;
        uint8_t sw;
        if (converter::unpack(frame.data.data(), frame.dlc, 0, val)) {
            feedback_value_ = val;
        }
        if (converter::unpack(frame.data.data(), frame.dlc, 4, sw)) {
            limit_switches_ = sw;
        }
    } else if (id_fields.is_command(id::MsgTypeMotorDriver::HardwareStatus)) {
        float curr;
        int8_t temp;
        if (converter::unpack(frame.data.data(), frame.dlc, 0, curr)) {
            load_current_ = curr;
        }
        if (converter::unpack(frame.data.data(), frame.dlc, 4, temp)) {
            temperature_ = temp;
        }
    }
}

float MotorDriverClient::feedback_value() const {
    return feedback_value_;
}

uint8_t MotorDriverClient::limit_switches() const {
    return limit_switches_;
}

float MotorDriverClient::load_current() const {
    return load_current_;
}

int8_t MotorDriverClient::temperature() const {
    return temperature_;
}

}  // namespace devices
}  // namespace gn10_can