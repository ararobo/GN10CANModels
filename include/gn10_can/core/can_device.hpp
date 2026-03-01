/**
 * @file can_device.hpp
 * @author Gento Aiba (aiba-gento)
 * @brief デバイス（MotorDriverやServoDriverなど）の抽象化クラスのヘッダーファイル
 * @version 0.1.0
 * @date 2026-01-10
 *
 * @copyright Copyright (c) 2026 Gento Aiba
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <array>
#include <cstdint>

#include "gn10_can/core/can_bus.hpp"
#include "gn10_can/core/can_frame.hpp"
#include "gn10_can/core/can_id.hpp"

namespace gn10_can {

/**
 * @brief デバイス（MotorDriverやServoDriverなど）の抽象化クラス
 * @note 各デバイス毎に具体化クラスを継承して定義してください。
 *
 */
class CANDevice
{
public:
    /**
     * @brief デバイス抽象化クラスのコンストラクタ
     *
     * @param bus
     * CANパケットを送信する為にCANBusクラスのインスタンスポインタを渡す
     * @param device_type デバイスの種類
     * @param device_id
     * デバイスのID（同じデバイスの種類のデバイスが複数あることを配慮して、0,1,2,..）
     */
    CANDevice(CANBus& bus, id::DeviceType device_type, uint8_t device_id)
        : bus_(bus), device_type_(device_type), device_id_(device_id)
    {
        bus_.attach(this);
    }

    virtual ~CANDevice()
    {
        bus_.detach(this);
    }

    // コピーとムーブを禁止 (RAIIによるデバイス登録の一意性を保つため)
    CANDevice(const CANDevice&)            = delete;
    CANDevice& operator=(const CANDevice&) = delete;
    CANDevice(CANDevice&&)                 = delete;
    CANDevice& operator=(CANDevice&&)      = delete;

    /**
     * @brief CANパケット受信時の呼び出し関数
     *
     * @param frame 受信したCANパケット
     */
    virtual void on_receive(const CANFrame& frame) = 0;

    /**
     * @brief ルーティングIDを取得
     *
     * コマンド部を除いた、デバイス特定用の上位ビット列を返します。
     *
     * @return uint32_t Routing ID (Type + DeviceID)
     */
    uint32_t get_routing_id() const
    {
        // CANFrame::get_routing_id() と同じ形式 (Type << BIT_WIDTH_DEV_ID) | DeviceID を返す
        return (static_cast<uint32_t>(device_type_) << id::BIT_WIDTH_DEV_ID) |
               static_cast<uint32_t>(device_id_);
    }

protected:
    /**
     * @brief コマンド・データ・データ長からCANフレームを作成しCANManagerを使用して送信
     *
     * @tparam CmdEnum コマンドのEnum Class
     * @param command
     * コマンド（データの種類を示す、CAN通信時のデータは指令として見れるためコマンドとして見なす）
     * @param data 送信データ
     * @param len 送信データ長（MAX:8）
     * @return true 送信成功（CANDriverの継承後クラスによって定義）
     * @return false 送信失敗（CANDriverの継承後クラスによって定義）
     */
    template <typename CmdEnum>
    bool send(CmdEnum command, const uint8_t* data = nullptr, std::size_t len = 0)
    {
        auto frame = CANFrame::make(device_type_, device_id_, command, data, len);
        return bus_.send_frame(frame);
    }

    /**
     * @brief コマンド・データ(array)からCANフレームを作成しCANManagerを使用して送信
     *
     * @tparam CmdEnum コマンドのEnum Class
     * @tparam N 送信データ長：1~8
     * @param command
     * コマンド（データの種類を示す、CAN通信時のデータは指令として見れるためコマンドとして見なす）
     * @param data 送信データ（要素数1~8のarray配列）
     * @return true 送信成功（CANDriverの継承後クラスによって定義）
     * @return false 送信失敗（CANDriverの継承後クラスによって定義）
     */
    template <typename CmdEnum, std::size_t N>
    bool send(CmdEnum command, const std::array<uint8_t, N>& data)
    {
        return send(command, data.data(), static_cast<uint8_t>(data.size()));
    }

    CANBus& bus_;                 // CAN通信を統括するクラスの参照
    id::DeviceType device_type_;  // デバイスの種類
    uint8_t device_id_;           // デバイスID
};
}  // namespace gn10_can
