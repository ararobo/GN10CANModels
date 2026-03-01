/**
 * @file can_converter.hpp
 * @author Gento Aiba (aiba-gento)
 * @brief CANデータ変換ユーティリティヘッダーファイル
 * @version 0.1.0
 * @date 2026-01-10
 *
 * @copyright Copyright (c) 2026 Gento Aiba
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>

#include "gn10_can/core/can_frame.hpp"

namespace gn10_can {
namespace converter {

/**
 * @brief バッファにPOD型データを収納する関数
 *
 * @tparam T 収納するPOD型データの型
 * @param buffer データを収納するバッファ
 * @param buffer_len バッファの長さ
 * @param start_byte 開始バイト位置
 * @param value 収納するPOD型データ
 * @return true 成功
 * @return false 失敗（バッファオーバーフローなど）
 */
template <typename T>
bool pack(uint8_t* buffer, size_t buffer_len, uint8_t start_byte, T value)
{
    static_assert(std::is_trivially_copyable<T>::value, "Type must be POD");

    if (start_byte + sizeof(T) > buffer_len) {
        return false;
    }
    std::memcpy(&buffer[start_byte], &value, sizeof(T));
    return true;
}

/**
 * @brief バッファからPOD型データを取り出す関数
 *
 * @tparam T 取り出すPOD型データの型
 * @param buffer データを取り出すバッファ
 * @param buffer_len バッファの長さ
 * @param start_byte 開始バイト位置
 * @param out_value 取り出したPOD型データの格納先
 * @return true 成功
 * @return false 失敗（バッファオーバーフローなど）
 */
template <typename T>
bool unpack(const uint8_t* buffer, size_t buffer_len, uint8_t start_byte, T& out_value)
{
    static_assert(std::is_trivially_copyable<T>::value, "Type must be POD");

    if (start_byte + sizeof(T) > buffer_len) {
        return false;
    }
    std::memcpy(&out_value, &buffer[start_byte], sizeof(T));
    return true;
}

/**
 * @brief std::arrayバッファにPOD型データを収納する関数
 *
 * @tparam T 収納するPOD型データの型
 * @tparam N バッファのサイズ
 * @param buffer データを収納するバッファ
 * @param start_byte 開始バイト位置
 * @param value 収納するPOD型データ
 * @return true 成功
 * @return false 失敗（バッファオーバーフローなど）
 */
template <typename T, size_t N>
bool pack(std::array<uint8_t, N>& buffer, uint8_t start_byte, T value)
{
    return pack(buffer.data(), N, start_byte, value);
}

/**
 * @brief std::arrayバッファからPOD型データを取り出す関数
 *
 * @tparam T 取り出すPOD型データの型
 * @tparam N バッファのサイズ
 * @param buffer データを取り出すバッファ
 * @param start_byte 開始バイト位置
 * @param out_value 取り出したPOD型データの格納先
 * @return true 成功
 * @return false 失敗（バッファオーバーフローなど）
 */
template <typename T, size_t N>
bool unpack(const std::array<uint8_t, N>& buffer, uint8_t start_byte, T& out_value)
{
    return unpack(buffer.data(), N, start_byte, out_value);
}

}  // namespace converter
}  // namespace gn10_can
