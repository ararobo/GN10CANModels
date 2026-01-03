#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>

#include "gn10_can/core/can_frame.hpp"

namespace gn10_can {
namespace converter {

template <typename T>
bool pack(uint8_t* buffer, size_t buffer_len, uint8_t start_byte, T value) {
    static_assert(std::is_trivially_copyable<T>::value, "Type must be POD");

    if (start_byte + sizeof(T) > buffer_len) {
        return false;
    }
    std::memcpy(&buffer[start_byte], &value, sizeof(T));
    return true;
}

template <typename T>
bool unpack(const uint8_t* buffer, size_t buffer_len, uint8_t start_byte, T& out_value) {
    static_assert(std::is_trivially_copyable<T>::value, "Type must be POD");

    if (start_byte + sizeof(T) > buffer_len) {
        return false;
    }
    std::memcpy(&out_value, &buffer[start_byte], sizeof(T));
    return true;
}

template <typename T, size_t N>
bool pack(std::array<uint8_t, N>& buffer, uint8_t start_byte, T value) {
    return pack(buffer.data(), N, start_byte, value);
}

}  // namespace converter
}  // namespace gn10_can