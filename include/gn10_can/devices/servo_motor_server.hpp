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
    /**
     * @brief 受け取ったパルス幅の最大値と最小値の設定
     *
     * @param min_us　最大のパルス幅値
     * @param max_us　最小のパルス幅値
     * @return true
     * @return false
     */
    bool get_new_init(uint16_t& min_us, uint16_t& max_us);
    /**
     * @brief 受け取ったサーボモータの角度の設定
     *
     * @param angle_rad　サーボモータの角度
     * @return true
     * @return false
     */
    bool get_new_angle_rad(float& angle_rad);
    void on_receive(const CANFrame& frame) override;

private:
    struct PulseSet {
        uint16_t min_us;
        uint16_t max_us;
    };
    std::optional<PulseSet> pulse_set_;
    std::optional<float> angle_rad_;
};

}  // namespace devices
}  // namespace gn10_can