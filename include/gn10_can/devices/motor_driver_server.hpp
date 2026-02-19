/**
 * @file motor_driver_server.hpp
 * @author Gento Aiba (aiba-gento)
 * @brief モータードライバー用デバイスクラスのヘッダーファイル
 * @version 0.1.0
 * @date 2026-02-16
 *
 * @copyright Copyright (c) 2026 Gento Aiba
 * SPDX-License-Identifier: GPL-3.0
 */
#pragma once

#include "gn10_can/core/can_device.hpp"
#include "gn10_can/devices/motor_driver_types.hpp"

namespace gn10_can {
namespace devices {

/**
 * @brief モータードライバー用デバイスクラス
 *
 */
class MotorDriverServer : public CANDevice {
  public:
    /**
     * @brief モータードライバー用デバイスクラスのコンストラクタ
     *
     * @param bus CANBusクラスの参照
     * @param dev_id デバイスID
     */
    MotorDriverServer(CANBus& bus, uint8_t dev_id);

    /**
     * @brief モータードライバーフィードバック送信関数
     *
     * @param feedback_val 現在値（速度制御の場合は速度、位置制御の場合は位置）
     * @param limit_switch_state リミットスイッチ状態（ビットマップ形式）
     */
    void send_feedback(float feedback_val, uint8_t limit_switch_state);

    /**
     * @brief モータードライバー状態送信関数
     *
     * @param load_current 電流
     * @param temperature 温度
     */
    void send_hardware_status(float load_current, int8_t temperature);

    /**
     * @brief 新しい設定があれば更新する
     *
     * @param config 設定
     * @return true 新しい設定が有り更新した
     * @return false 新しい設定はなく、更新しなかった
     */
    bool get_new_config(MotorConfig& config);

    /**
     * @brief 新しい目標値があれば更新する
     *
     * @param target モーター制御の目標値
     * @return true 新しい目標値があり更新した
     * @return false 新しい目標値はなく、更新しなかった
     */
    bool get_new_target(float& target);

    /**
     * @brief 新しいゲインがあれば更新する
     *
     * @param type ゲインの種類
     * @param value ゲインの値
     * @return true 新しいゲインがあり更新した
     * @return false 新しいゲインはなく、更新しなかった
     */
    bool get_new_gain(GainType type, float& value);

    /**
     * @brief CANパケット受信時の呼び出し関数の実装
     *
     * @param frame 受信したCANパケット
     */
    void on_receive(const CANFrame& frame) override;

  private:
    static constexpr std::size_t kGainTypeCount = static_cast<std::size_t>(GainType::Count);

    MotorConfig config_;
    bool is_config_updated_{false};
    float target_{0.0f};
    bool is_target_updated_{false};
    float gains_[kGainTypeCount]{0.0f};
    bool is_gains_updated_[kGainTypeCount]{false};
};
}  // namespace devices
}  // namespace gn10_can
