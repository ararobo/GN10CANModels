#include "gn10_can/devices/motor_driver_server.hpp"

#include "gn10_can/utils/can_converter.hpp"

namespace gn10_can {
namespace devices {

MotorDriverServer::MotorDriverServer(CANBus& bus, uint8_t dev_id)
    : CANDevice(bus, id::DeviceType::MotorDriver, dev_id) {}

void MotorDriverServer::send_feedback(float feedback_val, uint8_t limit_switch_state) {
    std::array<uint8_t, 5> payload{};
    converter::pack(payload, 0, feedback_val);
    converter::pack(payload, 4, limit_switch_state);
    send(id::MsgTypeMotorDriver::Feedback, payload);
}

void MotorDriverServer::send_hardware_status(float load_current, int8_t temperature) {
    std::array<uint8_t, 5> payload{};
    converter::pack(payload, 0, load_current);
    converter::pack(payload, 4, temperature);
    send(id::MsgTypeMotorDriver::HardwareStatus, payload);
}

bool MotorDriverServer::get_new_init(MotorConfig& config) {
    if (config_.has_value()) {
        config = config_.value();
        config_.reset();
        return true;
    }
    return false;
}

bool MotorDriverServer::get_new_target(float& target) {
    if (target_.has_value()) {
        target = target_.value();
        target_.reset();
        return true;
    }
    return false;
}

bool MotorDriverServer::get_new_gain(GainType type, float& value) {
    auto index = static_cast<std::size_t>(type);
    if (index < kGainTypeCount && gains_[index].has_value()) {
        value = gains_[index].value();
        gains_[index].reset();
        return true;
    }
    return false;
}

void MotorDriverServer::on_receive(const CANFrame& frame) {
    auto id_fields = id::unpack(frame.id);

    if (id_fields.type != device_type_ || id_fields.dev_id != device_id_) {
        return;
    }

    if (id_fields.is_command(id::MsgTypeMotorDriver::Init)) {
        config_ = MotorConfig::from_bytes(frame.data);
    } else if (id_fields.is_command(id::MsgTypeMotorDriver::Target)) {
        float val;
        if (converter::unpack(frame.data.data(), frame.dlc, 0, val)) {
            target_ = val;
        }
    } else if (id_fields.is_command(id::MsgTypeMotorDriver::Gain)) {
        if (frame.dlc >= 5) {
            uint8_t type_val = frame.data[0];
            float gain_val;
            if (type_val < static_cast<uint8_t>(GainType::Count) &&
                converter::unpack(frame.data.data(), frame.dlc, 1, gain_val)) {
                gains_[type_val] = gain_val;
            }
        }
    }
}

}  // namespace devices
}  // namespace gn10_can
