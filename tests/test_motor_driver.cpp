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
    // 0x200 is just an example ID
    MotorDriver motor{bus, 0x200}; 
};

TEST_F(MotorDriverTest, SendTarget) {
    float target = 123.45f;
    motor.send_target(target);

    ASSERT_EQ(driver.sent_frames.size(), 1);
    const auto& frame = driver.sent_frames[0];

    // Verify ID matches the constructor ID
    EXPECT_EQ(frame.id, 0x200);

    // Verify Command in data[0]
    ASSERT_GE(frame.dlc, 1);
    EXPECT_EQ(frame.data[0], static_cast<uint8_t>(id::MsgTypeMotorDriver::Target));

    // Verify Data (Payload starts at index 1)
    float unpacked_target = 0.0f;
    converter::unpack(frame.data.data(), frame.dlc, 1, unpacked_target);
    EXPECT_FLOAT_EQ(target, unpacked_target);
}

TEST_F(MotorDriverTest, ReceiveFeedback) {
    float feedback   = 543.21f;
    uint8_t limit_sw = 1;

    // Construct a feedback frame
    // New protocol: ID=0x200, Data[0]=Cmd, Data[1..]=Payload
    CANFrame frame;
    frame.id = 0x200;
    
    // Payload length = 5 (float + uint8_t in pack)
    // Total dlc = 1 (cmd) + 5 = 6
    frame.dlc = 6;
    frame.data[0] = static_cast<uint8_t>(id::MsgTypeMotorDriver::Feedback);
    
    // Pack payload into data[1..]
    // Note: pack takes standard array or buffer. 
    // We can pack into a temp buffer then copy, or pack directly if pack supports offset pointers efficiently.
    // The utility `converter::pack` takes `std::array` or similar.
    std::array<uint8_t, 8> temp_payload{};
    converter::pack(temp_payload, 0, feedback);
    converter::pack(temp_payload, 4, limit_sw);
    
    std::memcpy(&frame.data[1], temp_payload.data(), 5);

    // In Proposal 5, `CANBus::update` calls `motor.on_receive`.
    // OR calling `motor.on_receive` directly for unit testing logic.
    motor.on_receive(frame);
    
    EXPECT_FLOAT_EQ(motor.get_feedback_value(), feedback);
    EXPECT_EQ(motor.get_limit_switch_state(), limit_sw);
}
