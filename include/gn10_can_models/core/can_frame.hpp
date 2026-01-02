#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

#include "gn10_can_models/core/can_id.hpp"

namespace gn10_can {

/**
 * @brief CAN frame structure
 *
 */
struct CANFrame {
    static constexpr uint8_t MAX_DLC = 8;

    uint32_t id = 0;                      // CAN ID
    std::array<uint8_t, MAX_DLC> data{};  // Data payload
    uint8_t dlc = 0;                      // Data length code (DLC)

    // additional attribute
    bool is_extended = false;
    bool is_rtr      = false;
    bool is_error    = false;

    CANFrame() = default;

    template <typename CmdEnum>
    static CANFrame make(id::DeviceType type,
                         uint8_t dev_id,
                         CmdEnum cmd,
                         const std::vector<uint8_t>& payload = {}) {
        CANFrame frame;
        frame.id = id::pack(type, dev_id, cmd);
        frame.set_data(payload);
        return frame;
    }

    void set_data(const std::vector<uint8_t>& payload) {
        size_t size = std::min(payload.size(), static_cast<size_t>(MAX_DLC));
        std::copy(payload.begin(), payload.begin() + size, data.begin());
        if (size < MAX_DLC) {
            std::fill(data.begin() + size, data.end(), 0);
        }

        this->dlc = static_cast<uint8_t>(size);
    }

    bool operator==(const CANFrame& other) const {
        if (id != other.id || dlc != other.dlc || is_extended != other.is_extended ||
            is_rtr != other.is_rtr || is_error != other.is_error) {
            return false;
        }

        size_t loop_limit = (dlc > MAX_DLC) ? MAX_DLC : dlc;

        for (size_t i = 0; i < loop_limit; i++) {
            if (data[i] != other.data[i]) return false;
        }
        return true;
    }

    bool operator!=(const CANFrame& other) const { return !(*this == other); }
};

}  // namespace gn10_can