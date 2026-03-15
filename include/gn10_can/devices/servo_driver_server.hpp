/**
 * @file servo_driver_server.hpp
 * @author Gento Aiba (aiba-gento)
 * @brief サーボモータードライバーのサーバークラス
 * @version 0.1
 * @date 2026-03-03
 *
 * @copyright Copyright (c) 2026
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <optional>

#include "gn10_can/core/can_device.hpp"

namespace gn10_can {
class ServoDriverServer : public CANDevice
{
public:
    ServoDriverServer(CANBus& bus, uint8_t device_id);

    bool get_new_init(float& frequency_hz);

    bool get_new_duty_cycle(uint16_t& duty_cycle);

    void on_receive(const CANFrame& frame) override;

private:
    std::optional<float> init_frequency_hz_;
    std::optional<uint16_t> target_duty_cycle_;
};
}  // namespace gn10_can
