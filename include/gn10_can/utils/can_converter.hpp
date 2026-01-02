#pragma once

#include <algorithm>
#include <cstdint>
#include <type_traits>

#include "gn10_can/core/can_frame.hpp"

namespace gn10_can {
namespace converter {

template <typename T>
bool write(CANFrame& frame, uint8_t start_byte, T value) {
    static_assert(std::is_trivially_copyable<T>::value,
                  "Type must be trivially copyable (POD type)");

    if (start_byte + sizeof(T) > CANFrame::MAX_DLC) {
        return false;
    }

    std::memcpy(&frame.data[start_byte], &value, sizeof(T));

    uint8_t required_len = start_byte + static_cast<uint8_t>(sizeof(T));
    if (frame.dlc < required_len) {
        frame.dlc = required_len;
    }

    return true;
}

template <typename T>
bool read(const CANFrame& frame, uint8_t start_byte, T& out_value) {
    static_assert(std::is_trivially_copyable<T>::value,
                  "Type must be trivially copyable (POD type)");

    if (start_byte + sizeof(T) > CANFrame::MAX_DLC) {
        return false;
    }

    if (start_byte + sizeof(T) > frame.dlc) {
        return false;
    }

    std::memcpy(&out_value, &frame.data[start_byte], sizeof(T));
    return true;
}

template <typename T>
T get(const CANFrame& frame, uint8_t start_byte) {
    T val{};
    read<T>(frame, start_byte, val);
    return val;
}

}  // namespace converter
}  // namespace gn10_can