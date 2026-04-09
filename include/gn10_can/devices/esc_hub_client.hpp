/**
 * @file esc_hub_client.hpp
 * @author Ayu Kanai
 * @brief ESCHubのサーバーのヘッダーファイル。
 * @version 0.1
 * @date 2026-04-08
 *
 * @copyright Copyright (c) 2026 ararobo
 *
 */
#pragma once

#include <optional>

#include "gn10_can/core/fdcan_bus.hpp"
#include "gn10_can/core/fdcan_device.hpp"
#include "gn10_can/core/fdcan_frame.hpp"
#include "gn10_can/devices/esc_hub_config.hpp"

namespace gn10_can {
namespace devices {

class ESCHubClient : public FDCANDevice
{
public:
    /**
     * @brief ESCHubClientのコンストラクタ
     * @details CANbusの登録とdevice_idの割り振りを行う
     */
    ESCHubClient(FDCANBus& bus, uint8_t device_id);

    /**
     * @brief ゲインを格納する関数。
     *
     * @param motor_num ゲインを設定したいモーター (0〜3)
     * @param all_gain ゲインを格納する配列 kp ki kd ffの順で格納します
     */
    void set_gain_all(const ESCHubConfig& esc_hub_config);

    /**
     * @brief　角速度を設定する変数
     *
     * @param angular_velocities ４つ分のモーターの角速度の配列
     */
    void set_angular_velocities(float angular_velocities[4]);

    /**
     * @brief 角速度を受け取る関数
     *
     * @param angular_velocity_feedbacks フィードバックで受け取った角速度
     * @return true すべての角速度を受け取ることができた
     * @return false すべての角速度を受け取ることができなかった。
     */
    bool get_angular_velocity_feedbacks(float angular_velocity_feedbacks[4]);

    /**
     * @brief データをprivate関数に格納してあげる関数
     */
    void on_receive(const FDCANFrame& frame) override;

private:
    // 角速度格納用構造体
    struct AngularVelocityFeedbacks {
        float angular_velocity_feedback[4];
    };

    std::optional<AngularVelocityFeedbacks> angular_velocity_feedback_;
};

}  // namespace devices
}  // namespace gn10_can