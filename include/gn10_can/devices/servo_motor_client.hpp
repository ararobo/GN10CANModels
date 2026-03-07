/**
 * @file servo_motor_client.hpp
 * @author Akeru Kamagata
 * @brief サーボモータのクライアントのヘッダーファイル。
 * @version 0.1
 * @date 2026-03-07
 *
 * @copyright Copyright (c) 2026 ararobo
 *
 */
#pragma once

#include "gn10_can/core/can_bus.hpp"
#include "gn10_can/core/can_device.hpp"
#include "gn10_can/core/can_frame.hpp"

namespace gn10_can {
namespace devices {
class ServoMotorClient : public CANDevice
{
public:
    ServoMotorClient(CANBus& bus, uint8_t device_id);
    /**
     * @brief サーボモータのパルス幅最大値と最小値の設定
     *
     * @param min_us　パルス幅最大値
     * @param max_us　パルス幅最小値
     */
    void set_init(uint16_t min_us, uint16_t max_us);
    /**
     * @brief　サーボモータで指定したい角度の設定
     *
     * @param angle_rad サーボモータの角度
     */
    void set_angle_rad(float angle_rad);
};
}  // namespace devices
}  // namespace gn10_can