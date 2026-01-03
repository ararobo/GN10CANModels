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
    void set_max_duty_ratio(float ratio);
    float get_max_duty_ratio() const;

    /**
     * @brief Set the maximum acceleration rate using a ratio.
     * @param ratio 0.0 (Slowest) to 1.0 (Fastest/Immediate).
     */
    void set_accel_ratio(float ratio);
    float get_accel_ratio() const;

    /**
     * @brief Configure only the forward limit switch.
     * @param enable_stop Enable stop on forward limit switch trigger.
     * @param switch_id ID of the forward limit switch (0-7).
     */
    void set_forward_limit_switch(bool enable_stop, uint8_t switch_id);
    void get_forward_limit_switch(bool& enable_stop, uint8_t& switch_id) const;

    /**
     * @brief Configure only the reverse limit switch.
     * @param enable_stop Enable stop on reverse limit switch trigger.
     * @param switch_id ID of the reverse limit switch (0-7).
     */
    void set_reverse_limit_switch(bool enable_stop, uint8_t switch_id);
    void get_reverse_limit_switch(bool& enable_stop, uint8_t& switch_id) const;

    void set_telemetry_cycle(uint8_t ms);
    uint8_t get_telemetry_cycle() const;

    void set_encoder_type(EncoderType type);
    EncoderType get_encoder_type() const;

    void set_user_option(uint8_t option);
    uint8_t get_user_option() const;

    /**
     * @brief Serialize the config data to a byte array.
     * @return std::array<uint8_t, 8> The payload for the CAN frame.
     */
    std::array<uint8_t, 8> to_bytes() const;

    static MotorConfig from_bytes(const std::array<uint8_t, 8>& bytes);

  private:
    static uint8_t map_ratio_to_u8(float ratio);

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