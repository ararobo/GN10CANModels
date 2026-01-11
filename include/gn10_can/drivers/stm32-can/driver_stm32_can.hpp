/**
 * @file driver_stm32_can.hpp
 * @author Gento Aiba (aiba-gento)
 * @brief STM32 CANのドライバ具体化クラスのヘッダファイル
 * @version 0.1
 * @date 2026-01-11
 *
 * @copyright Copyright (c) 2026
 * SPDX-License-Identifier: GPL-3.0
 */
#pragma once

#include <cstdint>

#include "gn10_can/drivers/driver_interface.hpp"
#include "main.h"

namespace gn10_can {
namespace drivers {

class DriverSTM32CAN : public DriverInterface {
  public:
    DriverSTM32CAN(CAN_HandleTypeDef* hcan) : hcan_(hcan) {}

    bool init();
    bool send(const CANFrame& frame) override;
    bool receive(CANFrame& out_frame) override;

  private:
    CAN_HandleTypeDef* hcan_;
};
}  // namespace drivers
}  // namespace gn10_can
