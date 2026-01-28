/**
 * @file can_frame.hpp
 * @author Gento Aiba (aiba-gento)
 * @brief CANフレーム構造体のヘッダーファイル
 * @version 0.2
 * @date 2026-01-28
 *
 * @copyright Copyright (c) 2026 Gento Aiba
 * SPDX-License-Identifier: GPL-3.0
 */
#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

#include "gn10_can/core/can_id.hpp"

namespace gn10_can {

/**
 * @brief CANフレーム構造体
 *
 */
struct CANFrame {
    static constexpr std::size_t MAX_DLC = 8;

    uint32_t id = 0;                      // CAN ID
    std::array<uint8_t, MAX_DLC> data{};  // Data payload
    uint8_t dlc = 0;                      // Data length code (DLC)

    // additional attribute
    bool is_extended = false;
    bool is_rtr      = false;
    bool is_error    = false;

    CANFrame() = default;

    /**
     * @brief CANフレーム作成ヘルパー関数
     *
     * @tparam CmdEnum コマンドの列挙型
     * @param type デバイスの種類
     * @param dev_id デバイスのID
     * @param cmd コマンド
     * @param payload 送信データ
     * @param length 送信データの長さ
     * @return CANFrame 生成したCANフレーム
     */
    template <typename CmdEnum>
    static CANFrame make(id::DeviceType type,
                         uint8_t dev_id,
                         CmdEnum cmd,
                         const uint8_t* payload = nullptr,
                         std::size_t length     = 0) {
        CANFrame frame;
        frame.id = id::pack(type, dev_id, cmd);
        frame.set_data(payload, length);
        return frame;
    }

    /**
     * @brief CANフレーム作成ヘルパー関数（initializer_list版）
     *
     * @tparam CmdEnum コマンドの列挙型
     * @param type デバイスの種類
     * @param dev_id デバイスのID
     * @param cmd コマンド
     * @param payload 送信データ（{データ配列}の様に関数呼び出し時に作成可能）
     * @return CANFrame 生成したCANフレーム
     */
    template <typename CmdEnum>
    static CANFrame make(id::DeviceType type,
                         uint8_t dev_id,
                         CmdEnum cmd,
                         std::initializer_list<uint8_t> payload) {
        return make(type, dev_id, cmd, payload.begin(), payload.size());
    }

    /**
     * @brief CANフレーム作成ヘルパー関数（array版）
     *
     * @tparam CmdEnum コマンドの列挙型
     * @tparam N 送信データ長：1~8
     * @param type デバイスの種類
     * @param dev_id デバイスのID
     * @param cmd コマンド
     * @param payload 送信データ（要素数1~8のarray配列）
     * @return CANFrame 生成したCANフレーム
     */
    void set_data(const uint8_t* payload, std::size_t length) {
        std::size_t size = (length < MAX_DLC) ? length : MAX_DLC;
        if (payload != nullptr && size > 0) {
            std::copy(payload, payload + size, data.begin());
        }
        if (size < MAX_DLC) {
            std::fill(data.begin() + size, data.end(), static_cast<uint8_t>(0));
        }

        this->dlc = static_cast<uint8_t>(size);
    }

    /**
     * @brief ルーティング用のID（Command部を除外）を取得
     * 
     * このIDはデバイスの特定に使用され、下位のコマンドビットは無視されます。
     * can_id.hpp の定義に基づいて計算されます。
     * 
     * @return uint32_t ルーティングID (DeviceType + DeviceID)
     */
    uint32_t get_routing_id() const {
        // Commandビット幅分右シフトして切り捨てる
        return id >> id::BIT_WIDTH_COMMAND;
    }

    /**
     * @brief CANフレーム比較演算子
     *
     * @param other 比較対象のCANフレーム
     * @return true 等しい
     * @return false 等しくない
     */
    bool operator==(const CANFrame& other) const noexcept {
        if (id != other.id || dlc != other.dlc || is_extended != other.is_extended ||
            is_rtr != other.is_rtr || is_error != other.is_error) {
            return false;
        }

        std::size_t loop_limit =
            (static_cast<std::size_t>(dlc) > MAX_DLC) ? MAX_DLC : static_cast<std::size_t>(dlc);

        for (std::size_t i = 0; i < loop_limit; ++i) {
            if (data[i] != other.data[i]) return false;
        }
        return true;
    }

    /**
     * @brief CANフレーム不等比較演算子
     *
     * @param other 比較対象のCANフレーム
     * @return true 等しくない
     * @return false 等しい
     */
    bool operator!=(const CANFrame& other) const noexcept { return !(*this == other); }
};

}  // namespace gn10_can
