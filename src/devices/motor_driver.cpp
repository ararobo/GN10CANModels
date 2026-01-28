#include "gn10_can/devices/motor_driver.hpp"

#include "gn10_can/utils/can_converter.hpp"

namespace gn10_can {
namespace devices {

MotorDriver::MotorDriver(CANBus& bus, uint32_t id)
    : CANDevice(bus, id) {}

void MotorDriver::send_init(const MotorConfig& config) {
    // Assuming config.to_bytes() returns an array or vector compatible with send template
    send(id::MsgTypeMotorDriver::Init, config.to_bytes());
}

void MotorDriver::send_target(float target) {
    std::array<uint8_t, 4> payload{};
    converter::pack(payload, 0, target);
    send(id::MsgTypeMotorDriver::Target, payload);
}

void MotorDriver::send_gain(devices::GainType type, float value) {
    std::array<uint8_t, 5> payload{};
    payload[0] = static_cast<uint8_t>(type);
    converter::pack(payload, 1, value);
    send(id::MsgTypeMotorDriver::Gain, payload);
}

void MotorDriver::send_feedback(float current_val, uint8_t limit_switch_state) {
    std::array<uint8_t, 5> payload{};
    converter::pack(payload, 0, current_val);
    converter::pack(payload, 4, limit_switch_state);
    send(id::MsgTypeMotorDriver::Feedback, payload);
}

void MotorDriver::send_status(float load_current, int8_t temperature) {
    std::array<uint8_t, 5> payload{};
    converter::pack(payload, 0, load_current);
    converter::pack(payload, 4, temperature);
    send(id::MsgTypeMotorDriver::Status, payload);
}

void MotorDriver::on_receive(const CANFrame& frame) {
    // New Protocol: Command is in Data[0]
    if (frame.dlc < 1) return;
    
    uint8_t cmd = frame.data[0];
    
    // Check if command matches our expectations
    // Note: Previously checked Type and DeviceID from ID. 
    // Now filtering is done by CANBus dispatching to my_id_.
    // So we assume any message reaching here is for us.
    
    // Payload is from data[1] onwards.
    const uint8_t* payload = frame.data.data() + 1;
    // uint8_t payload_len = frame.dlc - 1; // Unused in unpack if we trust offsets?
    
    // Note: unpack takes raw buffer and offsets. 
    // Previous code: unpack(frame.data.data(), frame.dlc, 0, val);
    // Be careful with offsets. If payload is shifted by 1 byte (cmd), 
    // then value at payload[0] is actually frame.data[1].
    // If we use converter::unpack on `frame.data.data()`, we must adjust offset by +1.
    
    if (cmd == static_cast<uint8_t>(id::MsgTypeMotorDriver::Feedback)) {
        converter::unpack(frame.data.data(), frame.dlc, 1 + 0, feedback_val_);
        converter::unpack(frame.data.data(), frame.dlc, 1 + 4, limit_sw_state_);
    } else if (cmd == static_cast<uint8_t>(id::MsgTypeMotorDriver::Status)) {
        converter::unpack(frame.data.data(), frame.dlc, 1 + 0, load_current_);
        converter::unpack(frame.data.data(), frame.dlc, 1 + 4, temperature_);
    }
}

}  // namespace devices
}  // namespace gn10_can
