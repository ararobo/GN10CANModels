/**
 * @file motor_driver_client.hpp
 * @author Gento Aiba (aiba-gento)
 * @brief モータードライバー用デバイスクラスのヘッダーファイル
 * @version 0.1.0
 * @date 2026-02-16
 *
 * @copyright Copyright (c) 2026 Gento Aiba
 * SPDX-License-Identifier: GPL-3.0
 */
#pragma once

#include <optional>

#include "gn10_can/core/can_device.hpp"
#include "gn10_can/devices/motor_driver_types.hpp"

namespace gn10_can {
namespace devices {

/**
 * @brief モータードライバー用デバイスクラス
 *
 */
class MotorDriverClient : public CANDevice {
  public:
    /**
     * @brief モータードライバー用デバイスクラスのコンストラクタ
     *
     * @param bus CANBusクラスの参照
     * @param dev_id デバイスID
     */
    MotorDriverClient(CANBus& bus, uint8_t dev_id);

    /**
     * @brief モータードライバー初期化コマンド送信関数
     *
     * @param config モータードライバー設定データ
     */
    void set_init(const MotorConfig& config);

    /**
     * @brief モータードライバー目標値コマンド送信関数
     *
     * @param target 目標値（速度制御の場合は速度、位置制御の場合は位置）
     */
    void set_target(float target);

    /**
     * @brief モータードライバーゲイン設定コマンド送信関数
     *
     * @param type ゲインの種類
     * @param value ゲイン値
     */
    void set_gain(devices::GainType type, float value);

    /**
     * @brief CANパケット受信時の呼び出し関数の実装
     *
     * @param frame 受信したCANパケット
     */
    void on_receive(const CANFrame& frame) override;

    /**
     * @brief 最新のフィードバック値を取得する
     *
     * @return float フィードバック値
     */
    float feedback_value() const;

    /**
     * @brief 最新のリミットスイッチ状態を取得する
     *
     * @return uint8_t リミットスイッチ状態（ビットマップ形式）
     */
    uint8_t limit_switches() const;

    /**
     * @brief 最新の負荷電流を取得する
     *
     * @return float 負荷電流
     */
    float load_current() const;
    /**
     * @brief 最新の温度を取得する
     *
     * @return int8_t 温度
     */
    int8_t temperature() const;

  private:
    float feedback_value_{0.0f};
    uint8_t limit_switches_{0};
    float load_current_{0.0f};
    int8_t temperature_{0};
};
}  // namespace devices
}  // namespace gn10_can
