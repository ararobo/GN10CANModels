/**
 * @file driver_interface.hpp
 * @author Gento Aiba (aiba-gento)
 * @brief CAN通信ハードウェアインターフェースの抽象化クラスのヘッダーファイル
 * @version 0.1.0
 * @date 2026-01-10
 *
 * @copyright Copyright (c) 2026 Gento Aiba
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "gn10_can/core/can_frame.hpp"

namespace gn10_can {
namespace drivers {

/**
 * @brief CAN通信ハードウェアインターフェースの抽象化クラス
 *
 */
class ICanDriver
{
public:
    virtual ~ICanDriver() = default;

    /**
     * @brief CANフレーム送信関数
     *
     * @param frame 送信するCANフレーム
     * @return true 送信成功
     * @return false 送信失敗
     */
    virtual bool send(const CANFrame& frame) = 0;

    /**
     * @brief CANフレーム受信関数
     *
     * @param out_frame 受信したCANフレームの格納先
     * @return true 受信成功
     * @return false 受信失敗（受信データなしなど）
     */
    virtual bool receive(CANFrame& out_frame) = 0;
};
}  // namespace drivers
}  // namespace gn10_can
