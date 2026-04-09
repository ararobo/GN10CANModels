#include "gn10_can/devices/esc_hub_server.hpp"

#include "gn10_can/utils/can_converter.hpp"
namespace gn10_can {
namespace devices {
ESCHubServer::ESCHubServer(FDCANBus& bus, uint8_t device_id)
    : FDCANDevice(bus, id::DeviceType::ESCHUB, device_id)
{
}

bool ESCHubServer::get_gain(ESCHubConfig& esc_hub_config)
{
    if (motor_gain_.has_value()) {
        esc_hub_config = *motor_gain_;
        motor_gain_.reset();
        return true;
    }
    return false;
}

bool ESCHubServer::get_angular_velocities(float angular_velocities[4])
{
    if (angular_velocity_.has_value()) {
        for (int i = 0; i < 4; i++) {
            angular_velocities[i] = angular_velocity_->angular_velocity[i];
        }
        angular_velocity_.reset();
        return true;
    }
    return false;
}

void ESCHubServer::on_receive(const FDCANFrame& frame)
{
    auto id_fields = id::unpack(frame.id);

    if (id_fields.is_command(id::MsgTypeESCHub::Gain)) {
        ESCHubConfig config;
        if (converter::unpack(frame.data.data(), frame.dlc, 0, config)) {
            motor_gain_ = config;
        }
    } else if (id_fields.is_command(id::MsgTypeESCHub::AngularVelocities)) {
        AngularVelocities config;
        if (converter::unpack(frame.data, 0, config)) {
            angular_velocity_ = config;
        }
    }
}
}  // namespace devices
}  // namespace gn10_can