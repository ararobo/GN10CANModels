/**
 * @file solenoid_driver_server.hpp
 * @author Koichiro Watanabe (watanabe-koichiro)
 * @brief ソレノイドドライバ用デバイスクラスのヘッダーファイル
 * @version 0.1
 * @date 2026-02-18
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include <array>
#include <optional>

#include "gn10_can/core/can_device.hpp"

namespace gn10_can {
namespace devices {

class SolenoidDriverServer : public CANDevice {
  public:
    /**
     * @brief ソレノイド用サーバークラスのコンストラクタ
     *
     * @param bus CANBusクラスの参照
     * @param dev_id デバイスID
     */
    SolenoidDriverServer(CANBus& bus, uint8_t dev_id);

    /**
     * @brief 新しい目標値があれば更新する
     *
     * @param target ソレノイドの目標値
     * @return true 新しい目標値があり更新した
     * @return false 新しい目標値はなく,更新しなかった
     */
    bool get_new_target(std::array<bool, 8>& target);

    /**
     * @brief CANパケット受信時の呼び出し関数の実装
     *
     * @param frame 受信したCANパケット
     */
    void on_receive(const CANFrame& frame) override;

  private:
    std::optional<bool> target_[8];
};

}  // namespace devices
}  // namespace gn10_can
