/**
 * @file can_bus.hpp
 * @description CAN通信のバスを管理するクラス (Proposal 5: RAII & Modern API)
 * @copyright Copyright (c) 2026 Gento Aiba
 * SPDX-License-Identifier: GPL-3.0
 */
#pragma once

#include <map>
#include <cstdint>
#include "gn10_can/drivers/driver_interface.hpp"

namespace gn10_can {

class CANDevice;
class CANFrame;

/**
 * @brief CANバス管理クラス
 * 
 * デバイスの登録・登録解除はRAIIにより自動管理されるため、
 * ユーザーが直接 attach/detach を呼ぶ必要はありません。
 */
class CANBus {
  public:
    /**
     * @brief コンストラクタ
     * @param driver ハードウェアドライバーへの参照
     */
    explicit CANBus(drivers::DriverInterface& driver);

    /**
     * @brief バス更新処理
     * 受信データを読み込み、適切なデバイスへ配送します。
     */
    void update();

    /**
     * @brief 生フレーム送信
     * 通常はCANDevice経由で送信しますが、デバッグ等で必要な場合に利用します。
     */
    bool send_raw(const CANFrame& frame);

  private:
    // CANDevice からのみアクセス可能
    friend class CANDevice;
    
    /**
     * @brief デバイスをバスに接続する
     * @param device デバイスポインタ
     * @param rx_id 受信ID
     */
    void attach(CANDevice* device, uint32_t rx_id);

    /**
     * @brief デバイスをバスから切断する
     * @param rx_id 受信ID
     */
    void detach(uint32_t rx_id);

    drivers::DriverInterface& driver_;
    std::map<uint32_t, CANDevice*> devices_;
};
}  // namespace gn10_can
