/**
 * @file motor_driver.hpp
 * @description モータードライバー用デバイスクラス (Proposal 5 Adapter)
 * @copyright Copyright (c) 2026 Gento Aiba
 * SPDX-License-Identifier: GPL-3.0
 */
#pragma once

#include "gn10_can/core/can_device.hpp"
#include "gn10_can/core/can_bus.hpp"
#include "gn10_can/devices/motor_driver_types.hpp"

namespace gn10_can {
namespace devices {

/**
 * @brief モータードライバー用デバイスクラス
 *
 */
class MotorDriver : public CANDevice {
  public:
    /**
     * @brief モータードライバー用デバイスクラスのコンストラクタ
     *
     * @param bus CANBusクラスの参照
     * @param id 受信CAN-ID
     */
    MotorDriver(CANBus& bus, uint32_t id);

    /**
     * @brief モータードライバー初期化コマンド送信関数
     *
     * @param config モータードライバー設定データ
     */
    void send_init(const MotorConfig& config);

    /**
     * @brief モータードライバー目標値コマンド送信関数
     *
     * @param target 目標値（速度制御の場合は速度、位置制御の場合は位置）
     */
    void send_target(float target);

    /**
     * @brief モータードライバーゲイン設定コマンド送信関数
     *
     * @param type ゲインの種類
     * @param value ゲインの値
     */
    void send_gain(devices::GainType type, float value);

    /**
     * @brief モータードライバーフィードバック送信関数
     *
     * @param current_val 現在値
     * @param limit_switch_state リミットスイッチ状態
     */
    void send_feedback(float current_val, uint8_t limit_switch_state);

    /**
     * @brief モータードライバー状態送信関数
     *
     * @param load_current 負荷電流
     * @param temperature 温度
     */
    void send_status(float load_current, int8_t temperature);

    /**
     * @brief CANパケット受信時の呼び出し関数
     *
     * @param frame 受信したCANパケット
     */
    void on_receive(const CANFrame& frame) override;

    float get_feedback_value() const { return feedback_val_; }
    uint8_t get_limit_switch_state() const { return limit_sw_state_; }
    float get_load_current() const { return load_current_; }
    int8_t get_temperature() const { return temperature_; }

  private:
    float feedback_val_ = 0.0f;
    uint8_t limit_sw_state_ = 0;
    float load_current_ = 0.0f;
    int8_t temperature_ = 0;
};

}  // namespace devices
}  // namespace gn10_can
