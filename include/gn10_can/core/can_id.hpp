/**
 * @file can_id.hpp
 * @author Gento Aiba (aiba-gento)
 * @brief CAN-IDに関する機能を提供するヘッダーファイル
 * @version 0.1.0
 * @date 2026-01-10
 *
 * @copyright Copyright (c) 2026 Gento Aiba
 * SPDX-License-Identifier: GPL-3.0
 */
#pragma once

#include <cstdint>
#include <type_traits>

namespace gn10_can {
namespace id {

static constexpr uint8_t BIT_WIDTH_DEV_TYPE = 4;
static constexpr uint8_t BIT_WIDTH_DEV_ID   = 4;
static constexpr uint8_t BIT_WIDTH_COMMAND  = 3;

/**
 * @brief デバイスの種類
 *
 */
enum class DeviceType : uint8_t {
    EmergencyStop       = 0,
    MotorDriver         = 1,
    ServoDriver         = 2,
    SolenoidDriver      = 3,
    CommunicationModule = 4,
    SensorHub           = 5,
    LED                 = 6,
};

/**
 * @brief 非常停止スイッチのメッセージ種類（コマンド）
 *
 */
enum class MsgTypeEmergencyStop : uint8_t {
    Init          = 0,
    Status        = 1,
    EmergencyStop = 2,
};

/**
 * @brief モータードライバーのメッセージ種類（コマンド）
 *
 */
enum class MsgTypeMotorDriver : uint8_t {
    Init           = 0,
    Target         = 1,
    Gain           = 3,
    Feedback       = 4,
    HardwareStatus = 5,
};

/**
 * @brief サーボドライバーのメッセージ種類（コマンド）
 *
 */
enum class MsgTypeServoDriver : uint8_t {
    Init      = 0,
    Target    = 1,
    Frequency = 2,
};

/**
 * @brief ソレノイドドライバーのメッセージ種類（コマンド）
 *
 */
enum class MsgTypeSolenoidDriver : uint8_t {
    Init   = 0,
    Target = 1,
};

/**
 * @brief 通信モジュールのメッセージ種類（コマンド）
 *
 */
enum class MsgTypeCommunicationModule : uint8_t {
    Init           = 0,
    Heartbeat      = 1,
    ControllerData = 2,
};

/**
 * @brief センサーハブのメッセージ種類（コマンド）
 *
 */
enum class MsgTypeSensorHub : uint8_t {
    Init = 0,
    ToF  = 1,
};

/**
 * @brief LEDのメッセージ種類（コマンド）
 *
 */
enum class MsgTypeLED : uint8_t {
    Init = 0,
};

/**
 * @brief CAN-IDから取り出した通信パケットの種類
 *
 */
struct IdFields {
    DeviceType type;
    uint8_t dev_id;
    uint8_t command;

    template <typename CmdEnum>
    bool is_command(CmdEnum cmd_enum) const {
        return command == static_cast<uint8_t>(cmd_enum);
    }
};

/**
 * @brief 通信パケットの種類からCAN-IDにまとめる
 *
 * @tparam CmdEnum コマンド
 * @param type デバイスの種類
 * @param dev_id デバイスのID
 * @param cmd コマンド
 * @return uint32_t 生成したCAN-ID
 */
template <typename CmdEnum>
uint32_t pack(DeviceType type, uint8_t dev_id, CmdEnum cmd) {
    static_assert(std::is_enum<CmdEnum>::value, "Command must be an Enum class");

    uint32_t id = 0;

    uint8_t val_type = static_cast<uint8_t>(type) & 0x0F;
    uint8_t val_id   = dev_id & 0x0F;
    uint8_t val_cmd  = static_cast<uint8_t>(cmd) & 0x07;

    id |= (static_cast<uint32_t>(val_type) << 7);
    id |= (static_cast<uint32_t>(val_id) << 3);
    id |= (static_cast<uint32_t>(val_cmd) << 0);

    return id;
}

/**
 * @brief CAN-IDから通信パケットの種類を取り出す
 *
 * @param std_id CAN-ID
 * @return IdFields 通信パケットの種類が含まれる構造体
 */
inline IdFields unpack(uint32_t std_id) {
    IdFields result;

    uint8_t type_val = (std_id >> 7) & 0x0F;
    result.dev_id    = (std_id >> 3) & 0x0F;
    result.command   = (std_id >> 0) & 0x07;

    result.type = static_cast<DeviceType>(type_val);

    return result;
}

}  // namespace id
}  // namespace gn10_can
