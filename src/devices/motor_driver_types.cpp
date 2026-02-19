#include "gn10_can/devices/motor_driver_types.hpp"

namespace gn10_can {
namespace devices {

void MotorConfig::set_max_duty_ratio(float ratio) {
    data_.max_duty_ratio = map_ratio_to_u8(ratio);
}

float MotorConfig::get_max_duty_ratio() const {
    return static_cast<float>(data_.max_duty_ratio) / 255.0f;
}

void MotorConfig::set_accel_ratio(float ratio) {
    data_.max_accel_rate = map_ratio_to_u8(ratio);
}

float MotorConfig::get_accel_ratio() const {
    return static_cast<float>(data_.max_accel_rate) / 255.0f;
}

void MotorConfig::set_forward_limit_switch(bool enable_stop, uint8_t switch_id) {
    uint8_t val = data_.limit_switches_config;
    // Clear forward bits (7-4)
    val &= 0x0F;
    // Set new forward bits
    if (enable_stop) val |= (1 << 7);
    val |= (switch_id & 0x07) << 4;
    data_.limit_switches_config = val;
}

void MotorConfig::get_forward_limit_switch(bool& enable_stop, uint8_t& switch_id) const {
    uint8_t val = data_.limit_switches_config;
    enable_stop = (val >> 7) & 0x01;
    switch_id   = (val >> 4) & 0x07;
}

void MotorConfig::set_reverse_limit_switch(bool enable_stop, uint8_t switch_id) {
    uint8_t val = data_.limit_switches_config;
    // Clear reverse bits (3-0)
    val &= 0xF0;
    // Set new reverse bits
    if (enable_stop) val |= (1 << 3);
    val |= (switch_id & 0x07);
    data_.limit_switches_config = val;
}

void MotorConfig::get_reverse_limit_switch(bool& enable_stop, uint8_t& switch_id) const {
    uint8_t val = data_.limit_switches_config;
    enable_stop = (val >> 3) & 0x01;
    switch_id   = val & 0x07;
}

void MotorConfig::set_feedback_cycle(uint8_t ms) {
    data_.feedback_cycle_ms = ms;
}

uint8_t MotorConfig::get_feedback_cycle() const {
    return data_.feedback_cycle_ms;
}

void MotorConfig::set_encoder_type(EncoderType type) {
    data_.encoder_type = static_cast<uint8_t>(type);
}

EncoderType MotorConfig::get_encoder_type() const {
    return static_cast<EncoderType>(data_.encoder_type);
}

void MotorConfig::set_user_option(uint8_t option) {
    data_.user_option = option;
}

uint8_t MotorConfig::get_user_option() const {
    return data_.user_option;
}

std::array<uint8_t, 8> MotorConfig::to_bytes() const {
    std::array<uint8_t, 8> bytes{};
    static_assert(sizeof(PackedData) == 8, "PackedData size must be 8 bytes");

    // Zero-initialize the array first (optional if memcpy covers all)
    bytes.fill(0);

    std::memcpy(bytes.data(), &data_, sizeof(PackedData));
    return bytes;
}

MotorConfig MotorConfig::from_bytes(const std::array<uint8_t, 8>& bytes) {
    MotorConfig config;
    static_assert(sizeof(PackedData) == 8, "PackedData size must be 8 bytes");
    std::memcpy(&config.data_, bytes.data(), sizeof(PackedData));
    return config;
}

uint8_t MotorConfig::map_ratio_to_u8(float ratio) {
    if (ratio < 0.0f) return 0;
    if (ratio > 1.0f) return 255;
    return static_cast<uint8_t>(ratio * 255.0f);
}

}  // namespace devices
}  // namespace gn10_can
