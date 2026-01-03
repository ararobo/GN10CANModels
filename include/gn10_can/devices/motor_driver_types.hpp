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

class MotorConfig {
  public:
    MotorConfig() {}

    /**
     * @brief Set the maximum duty cycle limit using a ratio (0.0 - 1.0).
     * @param ratio Float value from 0.0 (0%) to 1.0 (100%).
     */
    void set_max_duty_ratio(float ratio) { data_.max_duty_ratio = map_ratio_to_u8(ratio); }
    /**
     * @brief Set the maximum acceleration rate using a ratio.
     * @param ratio 0.0 (Slowest) to 1.0 (Fastest/Immediate).
     */
    void set_accel_ratio(float ratio) { data_.max_accel_rate = map_ratio_to_u8(ratio); }
    /**
     * @brief Configure limit switches.
     * @param fwd_stop Enable stop on forward limit switch trigger.
     * @param fwd_sw_id ID of the forward limit switch (0-7).
     * @param bwd_stop Enable stop on backward limit switch trigger.
     * @param bwd_sw_id ID of the backward limit switch (0-7).
     */
    void set_limit_switches(bool fwd_stop, uint8_t fwd_sw_id, bool bwd_stop, uint8_t bwd_sw_id) {
        uint8_t val = 0;
        // Construct the bitfield
        if (fwd_stop) val |= (1 << 7);
        val |= (fwd_sw_id & 0x07) << 4;

        if (bwd_stop) val |= (1 << 3);
        val |= (bwd_sw_id & 0x07);

        data_.limit_sw_config = val;
    }
    void set_telemetry_cycle(uint8_t ms) { data_.telemetry_cycle_ms = ms; }
    void set_encoder_type(EncoderType type) { data_.encoder_type = static_cast<uint8_t>(type); }
    void set_user_option(uint8_t option) { data_.user_option = option; }

    /**
     * @brief Serialize the config data to a byte array.
     * @return std::array<uint8_t, 8> The payload for the CAN frame.
     */
    std::array<uint8_t, 8> to_bytes() const {
        std::array<uint8_t, 8> bytes{};
        static_assert(sizeof(PackedData) == 8, "PackedData size must be 8 bytes");

        // Zero-initialize the array first (optional if memcpy covers all)
        bytes.fill(0);

        std::memcpy(bytes.data(), &data_, sizeof(PackedData));
        return bytes;
    }

    static MotorConfig from_bytes(const std::array<uint8_t, 8>& bytes) {
        MotorConfig config;
        static_assert(sizeof(PackedData) == 8, "PackedData size must be 8 bytes");
        std::memcpy(&config.data_, bytes.data(), sizeof(PackedData));
        return config;
    }

  private:
    static uint8_t map_ratio_to_u8(float ratio) {
        if (ratio < 0.0f) return 0;
        if (ratio > 1.0f) return 255;
        return static_cast<uint8_t>(ratio * 255.0f);
    }

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
        uint8_t user_option;
        uint8_t reserved[2];
    } __attribute__((__packed__));

    PackedData data_{};
};

}  // namespace devices
}  // namespace gn10_can