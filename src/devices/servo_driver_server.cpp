#include "gn10_can/devices/servo_driver_server.hpp"

#include "gn10_can/utils/can_converter.hpp"

namespace gn10_can {
ServoDriverServer::ServoDriverServer(CANBus& bus, uint8_t device_id)
    : CANDevice(bus, id::DeviceType::ServoDriver, device_id)
{
}

bool ServoDriverServer::get_new_init(float& frequency_hz)
{
    if (init_frequency_hz_.has_value()) {
        frequency_hz = init_frequency_hz_.value();
        init_frequency_hz_.reset();
        return true;
    }
    return false;
}

bool ServoDriverServer::get_new_duty_cycle(uint16_t& duty_cycle)
{
    if (target_duty_cycle_.has_value()) {
        duty_cycle = target_duty_cycle_.value();
        target_duty_cycle_.reset();
        return true;
    }
    return false;
}

void ServoDriverServer::on_receive(const CANFrame& frame)
{
    auto id_fields = id::unpack(frame.id);

    if (id_fields.is_command(id::MsgTypeServoDriver::Init)) {
        float freq;
        if (converter::unpack(frame.data.data(), frame.dlc, 0, freq)) {
            init_frequency_hz_ = freq;
        }
    } else if (id_fields.is_command(id::MsgTypeServoDriver::Target)) {
        uint16_t duty;
        if (converter::unpack(frame.data.data(), frame.dlc, 0, duty)) {
            target_duty_cycle_ = duty;
        }
    }
}
}  // namespace gn10_can
