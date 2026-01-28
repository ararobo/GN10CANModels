/**
 * @file can_device.hpp
 * @description デバイス抽象化クラス (Proposal 4: Forward Declaration)
 * @copyright Copyright (c) 2026 Gento Aiba
 * SPDX-License-Identifier: GPL-3.0
 */
#pragma once

#include <array>
#include <cstdint>
#include <initializer_list>

#include "gn10_can/core/can_frame.hpp"

namespace gn10_can {

// Forward Declaration
class CANManager;

/**
 * @brief デバイス（MotorDriverやServoDriverなど）の抽象化クラス
 */
class CANDevice {
  public:
    /**
     * @brief コンストラクタ
     * @param manager CANManagerへのポインタ
     * @param device_type デバイスタイプ
     * @param device_id デバイスID
     */
    CANDevice(CANManager* manager, id::DeviceType device_type, uint8_t device_id)
        : manager_(manager), device_type_(device_type), device_id_(device_id) {}
    
    virtual ~CANDevice() = default;

    /**
     * @brief CANパケット受信時の呼び出し関数
     */
    virtual void on_receive(const CANFrame& frame) = 0;

    /**
     * @brief デバイスの種類を取得
     */
    id::DeviceType get_device_type() const { return device_type_; }

    uint8_t get_device_id() const { return device_id_; }

  protected:
    // Implemented in .cpp to allow incomplete type CANManager in header
    bool send_frame_internal(const CANFrame& frame);

    template <typename CmdEnum>
    bool send(CmdEnum command, const uint8_t* data = nullptr, uint8_t len = 0) {
        auto frame = CANFrame::make(device_type_, device_id_, command, data, len);
        return send_frame_internal(frame);
    }

    template <typename CmdEnum>
    bool send(CmdEnum command, std::initializer_list<uint8_t> data) {
        return send(command, data.begin(), static_cast<uint8_t>(data.size()));
    }

    template <typename CmdEnum, std::size_t N>
    bool send(CmdEnum command, const std::array<uint8_t, N>& data) {
        return send(command, data.data(), static_cast<uint8_t>(data.size()));
    }

    CANManager* manager_;         // Use pointer
    id::DeviceType device_type_;
    uint8_t device_id_;
};

}  // namespace gn10_can
