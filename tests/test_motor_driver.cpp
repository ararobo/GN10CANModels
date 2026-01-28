#include <gtest/gtest.h>

#include "gn10_can/core/can_id.hpp"
#include "gn10_can/core/can_bus.hpp"
#include "gn10_can/devices/motor_driver.hpp"
#include "gn10_can/utils/can_converter.hpp"
#include "mock_driver.hpp"

using namespace gn10_can;
using namespace gn10_can::devices;

class MotorDriverTest : public ::testing::Test {
  protected:
    MockDriver driver;
    CANBus bus{driver};
    MotorDriver motor{bus, 1};
};

TEST_F(MotorDriverTest, SendTarget) {
    float target = 123.45f;
    motor.send_target(target);

    ASSERT_EQ(driver.sent_frames.size(), 1);
    const auto& frame = driver.sent_frames[0];

    // Verify ID (DeviceType::MotorDriver, ID=1, MsgType::Target)
    auto id_fields = id::unpack(frame.id);
    EXPECT_EQ(id_fields.type, id::DeviceType::MotorDriver);
    EXPECT_EQ(id_fields.dev_id, 1);
    EXPECT_EQ(id_fields.command, static_cast<uint8_t>(id::MsgTypeMotorDriver::Target));

    // Verify Data
    float unpacked_target = 0.0f;
    converter::unpack(frame.data.data(), frame.dlc, 0, unpacked_target);
    EXPECT_FLOAT_EQ(target, unpacked_target);
}

TEST_F(MotorDriverTest, ReceiveFeedback) {
    float feedback   = 543.21f;
    uint8_t limit_sw = 1;

    // Construct a feedback frame
    std::array<uint8_t, 8> data{};
    converter::pack(data, 0, feedback);
    converter::pack(data, 4, limit_sw);

    CANFrame frame = CANFrame::make(
        id::DeviceType::MotorDriver, 1, id::MsgTypeMotorDriver::Feedback, data.data(), 5);

    motor.on_receive(frame);

    EXPECT_FLOAT_EQ(motor.get_feedback_value(), feedback);
    EXPECT_EQ(motor.get_limit_switch_state(), limit_sw);
}

TEST_F(MotorDriverTest, ReceiveStatus) {
    float load_current = 2.5f;
    int8_t temperature = 45;

    // Construct a status frame
    std::array<uint8_t, 8> data{};
    converter::pack(data, 0, load_current);
    converter::pack(data, 4, temperature);

    CANFrame frame = CANFrame::make(
        id::DeviceType::MotorDriver, 1, id::MsgTypeMotorDriver::Status, data.data(), 5);

    motor.on_receive(frame);

    EXPECT_FLOAT_EQ(motor.get_load_current(), load_current);
    EXPECT_EQ(motor.get_temperature(), temperature);
}

TEST_F(MotorDriverTest, IgnoreOtherDeviceFrame) {
    float feedback = 543.21f;

    // Construct a feedback frame for device ID 2 (our motor is ID 1)
    std::array<uint8_t, 8> data{};
    converter::pack(data, 0, feedback);

    CANFrame frame = CANFrame::make(
        id::DeviceType::MotorDriver, 2, id::MsgTypeMotorDriver::Feedback, data.data(), 5);

    motor.on_receive(frame);

    // Should remain default (0.0f)
    EXPECT_FLOAT_EQ(motor.get_feedback_value(), 0.0f);
}
