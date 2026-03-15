/**
 * @file servo_driver_client.hpp
 * @author Gento Aiba (aiba-gento)
 * @brief サーボモータードライバーのクライアントクラス
 * @version 0.1
 * @date 2026-03-03
 *
 * @copyright Copyright (c) 2026
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "gn10_can/core/can_device.hpp"

namespace gn10_can {
class ServoDriverClient : public CANDevice
{
public:
    ServoDriverClient(CANBus& bus, uint8_t device_id);

    void set_init(float frequency_hz);

    void set_duty_cycle(uint16_t duty_cycle);

    void on_receive(const CANFrame& frame) override;
};
}  // namespace gn10_can
