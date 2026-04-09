#pragma once

#include <optional>

#include "gn10_can/core/fdcan_bus.hpp"
#include "gn10_can/core/fdcan_device.hpp"
#include "gn10_can/core/fdcan_frame.hpp"
#include "gn10_can/devices/esc_hub_config.hpp"

namespace gn10_can {
namespace devices {

class ESCHubServer : public FDCANDevice
{
public:
    /**
     * @brief ESCHubServerのコンストラクタ
     * @details CANbusの登録とdevice_idの割り振りを行う
     */
    ESCHubServer(FDCANBus& bus, uint8_t device_id);

    /**
     * @brief ゲインを読み取る関数。init
     *
     * @param motor_num ゲインを読み取りたいモーター(0~3)
     * @param all_gain ゲインを格納している配列 kp ki kd ffの順で格納されています
     */
    bool get_gain(ESCHubConfig& esc_hub_config);

    /**
     * @brief 受け取った角速度にアクセスする関数（すべてできる）
     *
     * @param angular_velocities ４つ分のモーターの角速度の配列
     * @return true すべての角速度を受け取ることができた
     * @return false すべての角速度を受け取ることができなかった。
     */
    bool get_angular_velocities(float angular_velocities[4]);

    /**
     * @brief データをprivate関数に格納してあげる関数
     */
    void on_receive(const FDCANFrame& frame) override;

private:
    // 角速度格納用構造体
    struct AngularVelocities {
        float angular_velocity[4];
    };

    std::optional<AngularVelocities> angular_velocity_;
    std::optional<ESCHubConfig> motor_gain_;
};

}  // namespace devices
}  // namespace gn10_can