/**
 * @file can_manager.hpp
 * @description CAN通信統括クラス (Proposal 4: Forward Declaration)
 * @copyright Copyright (c) 2026 Gento Aiba
 * SPDX-License-Identifier: GPL-3.0
 */
#pragma once

#include <map>
#include <cstdint>
#include "gn10_can/drivers/driver_interface.hpp"

namespace gn10_can {

class CANDevice; // Forward declaration
class CANFrame;

class CANManager {
  public:
    static constexpr std::size_t MAX_DEVICES_LEGACY = 16; 

    explicit CANManager(drivers::DriverInterface& driver);

    // Modified to take ID explicitly (or extract from device inside implementation)
    // Proposal 4 suggests registration with ID for routing.
    void register_device(CANDevice* device, uint32_t rx_id);
    
    // Legacy support or if ID is inside device.
    // If we want to support multiple IDs per device, explicit ID is better.
    // Let's keep it simple and register by explicit ID (Proposal 4 style routing).
    
    void update();

    bool send_frame(const CANFrame& frame);

  private:
    drivers::DriverInterface& driver_;
    std::map<uint32_t, CANDevice*> subscribers_;
};
}  // namespace gn10_can
