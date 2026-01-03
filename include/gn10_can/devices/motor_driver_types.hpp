#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>

namespace gn10_can {
namespace devices {

/**
 * @brief Encoder type definitions.
 */
enum class EncoderType : uint8_t {
    None             = 0,
    IncrementalSpeed = 1,
    Absolute         = 2,
    IncrementalTotal = 3,
};

struct MotorConfig {
    struct PackedData {
        /**
         * @brief Maximum Output Limit (Normalized).
         * Range: 0 (0%) to 255 (100%).
         */
        uint8_t max_duty_ratio;
        /**
         * @brief Maximum Acceleration Rate (Ramp Rate).
         * Unit: Normalized duty change per millisecond.
         * Range: 0 to 255.
         * Example: Value 5 means the duty can change by approx 2% (5/255) per millisecond.
         */
        uint8_t max_accel_rate;
        /**
         * @brief Telemetry (Feedback) transmission cycle.
         * Unit: milliseconds (ms).
         * Value 0 means telemetry is disabled.
         */
        uint8_t telemetry_cycle_ms;
        /**
         * @brief Encoder type configuration.
         * See EncoderType enum.
         */
        uint8_t encoder_type;
        /**
         * @brief Limit switch configuration flags.
         * Bitfield: [StopFwd(1) | FwdID(3) | StopBwd(1) | BwdID(3)]
         */
        uint8_t limit_sw_config;
    } __attribute__((__packed__));

    PackedData data{};

} __attribute__((__packed__));

}  // namespace devices
}  // namespace gn10_can