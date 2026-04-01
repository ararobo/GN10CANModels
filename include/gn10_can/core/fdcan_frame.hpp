/**
 * @file fdcan_frame.hpp
 * @author Gento Aiba (aiba-gento)
 * @brief FDCANフレーム構造体のヘッダーファイル
 * @version 0.1.0
 * @date 2026-04-01
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include "gn10_can/core/can_frame.hpp"

namespace gn10_can {

using FDCANFrame = detail::CANFrame<64>;

}  // namespace gn10_can
