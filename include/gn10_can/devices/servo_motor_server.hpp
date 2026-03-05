#pragma once

#include <optional>

#include "gn10_can/core/can_bus.hpp"
#include "gn10_can/core/can_device.hpp"
#include "gn10_can/core/can_frame.hpp"

namespace gn10_can {
namespace devices {
class ServoMotorServer : public CANDevice
{
public:
    ServoMotorServer(CANBus& bus, uint8_t device_id);

    bool get_new_init(uint16_t& min_us, uint16_t& max_us);
    bool get_new_angle_rad(float& angle_rad);
    void on_receive(const CANFrame& frame) override;

private:
    /// Init受信データをまとめる構造体
    struct InitConfig
    {
        uint16_t min_us;
        uint16_t max_us;
    };

    std::optional<InitConfig> init_;
    std::optional<float>      angle_rad_;
};

}  // namespace devices
}  // namespace gn10_can