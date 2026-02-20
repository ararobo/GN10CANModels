/**
 * @file solenoid_driver_client.hpp
 * @author Koichiro Watanabe (watanabe-koichiro)
 * @brief ソレノイドドライバ用デバイスクラスのヘッダーファイル
 * @version 0.1
 * @date 2026-02-18
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "gn10_can/core/can_device.hpp"

namespace gn10_can {
namespace devices {

class SolenoidDriverClient : CANDevice {
  public:
    /**
     * @brief ソレノイド用クライアントクラスのコンストラクタ
     *
     * @param bus CANBusクラスの参照
     * @param dev_id デバイスID
     */
    SolenoidDriverClient(CANBus& bus, uint8_t dev_id);

    /**
     * @brief ソレノイドドライバー目標値コマンド送信関数
     *
     * @param target 目標値
     */
    void set_target(bool target);

  private:
    /* data */
};

}  // namespace devices
}  // namespace gn10_can
