#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

#include "gn10_can/core/can_id.hpp"

namespace gn10_can {

/**
 * @brief CAN frame structure
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

    template <typename CmdEnum>
    static CANFrame make(id::DeviceType type,
                         uint8_t dev_id,
                         CmdEnum cmd,
                         std::initializer_list<uint8_t> payload) {
        return make(type, dev_id, cmd, payload.begin(), payload.size());
    }

    void set_data(const uint8_t* payload, std::size_t length) {
        std::size_t size = std::min(length, MAX_DLC);
        if (payload != nullptr && size > 0) {
            std::copy(payload, payload + size, data.begin());
        }
        if (size < MAX_DLC) {
            std::fill(data.begin() + size, data.end(), 0);
        }

        this->dlc = static_cast<uint8_t>(size);
    }

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

    bool operator!=(const CANFrame& other) const noexcept { return !(*this == other); }
};

}  // namespace gn10_can