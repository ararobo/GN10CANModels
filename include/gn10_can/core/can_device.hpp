/**
 * @file can_device.hpp
 * @author Gento Aiba (aiba-gento)
 * @brief デバイス（MotorDriverやServoDriverなど）の抽象化クラス (Proposal 5: RAII & Modern API)
 * @version 0.2
 * @date 2026-01-28
 *
 * @copyright Copyright (c) 2026 Gento Aiba
 * SPDX-License-Identifier: GPL-3.0
 */
#pragma once

#include <vector>
#include <array>
#include <cstdint>
#include <initializer_list>
#include <cstring> /* for memcpy */

#include "gn10_can/core/can_bus.hpp"
#include "gn10_can/core/can_frame.hpp"

namespace gn10_can {

class CANDevice {
  protected:
    CANBus& bus_;
    uint32_t my_id_;

  public:
    /**
     * @brief デバイス抽象化クラスのコンストラクタ (RAII)
     * コンストラクタで自動的にバスに登録されます。
     * 
     * @param bus CANバスへの参照
     * @param id 受信ID (このIDのメッセージを受信します)
     */
    CANDevice(CANBus& bus, uint32_t id) : bus_(bus), my_id_(id) {
        bus_.attach(this, my_id_);
    }

    /**
     * @brief デストラクタ (RAII)
     * デストラクタで自動的にバスから登録解除されます。
     */
    virtual ~CANDevice() {
        bus_.detach(my_id_);
    }

    /**
     * @brief CANパケット受信時の呼び出し関数
     * @param frame 受信したCANパケット
     */
    virtual void on_receive(const CANFrame& frame) = 0;

    // --- Proposal 5 Modern APIs ---

    /**
     * @brief データ送信 (簡易API)
     * 自身のIDを使ってデータを送信します。
     * @param data 送信データ
     */
    bool send(const std::vector<uint8_t>& data) {
        CANFrame frame;
        frame.id = my_id_;
        frame.dlc = static_cast<uint8_t>(data.size());
        if(frame.dlc > 8) frame.dlc = 8;
        if(frame.dlc > 0) std::memcpy(frame.data.data(), data.data(), frame.dlc);
        return bus_.send_raw(frame);
    }

    /**
     * @brief コマンド付き送信 (簡易API)
     * 第1バイトをコマンドとして送信します。
     * @param cmd コマンド
     * @param data データペイロード
     */
    bool send(uint8_t cmd, const std::vector<uint8_t>& data) {
        CANFrame frame;
        frame.id = my_id_;
        // 1バイト目コマンド + データ
        frame.dlc = static_cast<uint8_t>(data.size() + 1);
        if(frame.dlc > 8) frame.dlc = 8;
        
        frame.data[0] = cmd;
        if (frame.dlc > 1) {
            std::memcpy(&frame.data[1], data.data(), frame.dlc - 1);
        }
        return bus_.send_raw(frame);
    }

    // --- Legacy / Template Support (Updated to use bus_) ---

    template <typename CmdEnum>
    bool send(CmdEnum command, const uint8_t* data = nullptr, uint8_t len = 0) {
        std::vector<uint8_t> payload;
        if (data && len > 0) {
            payload.assign(data, data + len);
        }
        return send(static_cast<uint8_t>(command), payload);
    }
    
    template <typename CmdEnum>
    bool send(CmdEnum command, std::initializer_list<uint8_t> data) {
        return send(command, data.begin(), data.size());
    }

    template <typename CmdEnum, std::size_t N>
    bool send(CmdEnum command, const std::array<uint8_t, N>& data) {
        return send(command, data.data(), static_cast<uint8_t>(data.size()));
    }
};

}  // namespace gn10_can
