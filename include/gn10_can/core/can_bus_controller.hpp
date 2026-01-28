/**
 * @file can_manager.hpp
 * @author Gento Aiba (aiba-gento)
 * @brief CAN通信を統括するクラスのヘッダーファイル
 * @version 0.1
 * @date 2026-01-10
 *
 * @copyright Copyright (c) 2026 Gento Aiba
 * SPDX-License-Identifier: GPL-3.0
 */
#pragma once

#include <array>
#include <cstddef>

#include "gn10_can/drivers/driver_interface.hpp"

namespace gn10_can {

class CANDevice;

/**
 * @brief CAN通信を統括するクラス
 *
 */
class CANManager {
  public:
    static constexpr std::size_t MAX_DEVICES = 16;  // 最大登録デバイス数

    /**
     * @brief CANManagerクラスのコンストラクタ
     *
     * @param driver CANドライバーインターフェースの参照
     */
    explicit CANManager(drivers::DriverInterface& driver);

    /**
     * @brief デバイス登録関数
     *
     * @param device 登録するデバイスのポインタ
     * @return true 登録成功
     * @return false 登録失敗（登録可能数超過）
     */
    bool register_device(CANDevice* device);

    /**
     * @brief CANパケット受信処理関数
     *
     * 登録されているデバイスのon_receive関数を呼び出す
     */
    void update();

    /**
     * @brief CANフレーム送信関数
     *
     * @param frame 送信するCANフレーム
     * @return true 送信成功
     * @return false 送信失敗
     */
    bool send_frame(const CANFrame& frame);

  private:
    drivers::DriverInterface& driver_;               // CANドライバーインターフェースの参照を保持
    std::array<CANDevice*, MAX_DEVICES> devices_{};  // 登録されているデバイスの配列
    std::size_t device_count_ = 0;                   // 登録されているデバイス数
};
}  // namespace gn10_can
