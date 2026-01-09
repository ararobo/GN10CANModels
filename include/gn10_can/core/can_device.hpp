/**
 * @file can_device.hpp
 * @author Gento Aiba (aiba-gento)
 * @brief デバイス（MotorDriverやServoDriverなど）の抽象化クラスのヘッダーファイル
 * @version 0.1
 * @date 2026-01-10
 *
 * @copyright Copyright (c) 2026 Gento Aiba
 * SPDX-License-Identifier: GPL-3.0
 */
#pragma once

#include <array>
#include <cstdint>
#include <initializer_list>

#include "gn10_can/core/can_frame.hpp"
#include "gn10_can/core/can_manager.hpp"


namespace gn10_can {

/**
 * @brief デバイス（MotorDriverやServoDriverなど）の抽象化クラス
 * @note 各デバイス毎に具体化クラスを継承して定義してください。
 *
 */
class CANDevice {
  public:
    /**
     * @brief デバイス抽象化クラスのコンストラクタ
     *
     * @param manager
     * CANパケットを送信する為にCANManagerクラスのインスタンスポインタを渡す
     * @param device_type デバイスの種類
     * @param device_id
     * デバイスのID（同じデバイスの種類のデバイスが複数あることを配慮して、0,1,2,..）
     */
    CANDevice(CANManager& manager, id::DeviceType device_type, uint8_t device_id);
    virtual ~CANDevice() = default;

    /**
     * @brief CANパケット受信時の呼び出し関数
     *
     * @param frame 受信したCANパケット
     */
    virtual void on_receive(const CANFrame& frame) = 0;

    /**
     * @brief on_receive関数と同じ周期で呼ばれる関数
     *
     */
    virtual void update() {}

    /**
     * @brief デバイスの種類を取得
     *
     * @return id::DeviceType デバイスの種類
     */
    id::DeviceType get_device_type() const { return device_type_; }

    uint8_t get_device_id() const { return device_id_; }

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
    bool send(CmdEnum command, const uint8_t* data = nullptr, uint8_t len = 0) {
        auto frame = CANFrame::make(device_type_, device_id_, command, data, len);
        return manager_.send_frame(frame);
    }

    /**
     * @brief コマンド・データ(initializer list)からCANフレームを作成しCANManagerを使用して送信
     *
     * @tparam CmdEnum コマンドのEnum Class
     * @param command
     * コマンド（データの種類を示す、CAN通信時のデータは指令として見れるためコマンドとして見なす）
     * @param data 送信データ（{データ配列}の様に関数呼び出し時に作成可能）
     * @return true 送信成功（CANDriverの継承後クラスによって定義）
     * @return false 送信失敗（CANDriverの継承後クラスによって定義）
     * @note send(id::MsgTypeMotorDriver::Gain, {10,0,255,20});の様に使用可
     */
    template <typename CmdEnum>
    bool send(CmdEnum command, std::initializer_list<uint8_t> data) {
        return send(command, data.begin(), data.size());
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
    bool send(CmdEnum command, const std::array<uint8_t, N>& data) {
        return send(command, data.data(), static_cast<uint8_t>(data.size()));
    }

    CANManager& manager_;  // CAN通信をすべて統括するクラスのポインタを保持して送信時に使用する。
    id::DeviceType device_type_;  // 具体化後クラスのデバイスの種類
    uint8_t device_id_;           // コンストラクタで設定する、このデバイスのID。送信時に使用する。
};

}  // namespace gn10_can