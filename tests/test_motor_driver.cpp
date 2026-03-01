#include <gtest/gtest.h>

#include "gn10_can/core/can_bus.hpp"
#include "gn10_can/devices/motor_driver_client.hpp"
#include "gn10_can/devices/motor_driver_server.hpp"
#include "mock_driver.hpp"

using namespace gn10_can;
using namespace gn10_can::devices;

class MotorDriverTest : public ::testing::Test
{
protected:
    MockDriver driver;
    CANBus bus{driver};
    uint8_t dev_id = 1;
    MotorDriverClient client{bus, dev_id};
    MotorDriverServer server{bus, dev_id};

    void SetUp() override
    {
        // Clear previous frames
        driver.sent_frames.clear();
        while (!driver.receive_queue.empty()) {
            driver.receive_queue.pop();
        }
    }

    void ProcessBus()
    {
        // Move sent frames to receive queue to simulate loopback
        // In a real scenario, frames go out to the bus and come back or go to another node.
        // Here we simulate that the bus reflects frames back to all devices (or devices on the same
        // bus receive them). However, usually a device does not receive its own frames unless
        // loopback is enabled. But here we are testing interaction between Client and Server on the
        // SAME bus instance for simplicity, or we can imagine they are on different buses connected
        // via the mock driver. Since CANBus dispatches to ALL devices, if we enable loopback in
        // mock, both will see it. But MockDriver doesn't automatically loopback. We have to do it
        // manually.

        // For testing Client -> Server:
        // Client.send() -> driver.sent_frames
        // We take them and push to driver.receive_queue
        // bus.update() -> reads from driver.receive_queue -> dispatches to Server.

        for (const auto& frame : driver.sent_frames) {
            driver.push_receive_frame(frame);
        }
        driver.sent_frames.clear();

        bus.update();
    }
};

TEST_F(MotorDriverTest, InitConfig)
{
    MotorConfig config;
    config.set_max_duty_ratio(0.5f);
    config.set_accel_ratio(0.2f);
    config.set_feedback_cycle(10);

    // Client sends init
    client.set_init(config);

    // Process frames
    ProcessBus();

    // Server should receive init
    MotorConfig received_config;
    EXPECT_TRUE(server.get_new_init(received_config));

    EXPECT_NEAR(received_config.get_max_duty_ratio(), 0.5f, 0.01f);
    EXPECT_NEAR(received_config.get_accel_ratio(), 0.2f, 0.01f);
    EXPECT_EQ(received_config.get_feedback_cycle(), 10);
}

TEST_F(MotorDriverTest, Target)
{
    float target = 0.8f;
    client.set_target(target);

    ProcessBus();

    float received_target;
    EXPECT_TRUE(server.get_new_target(received_target));
    EXPECT_FLOAT_EQ(received_target, target);
}

TEST_F(MotorDriverTest, Gain)
{
    float gain_val = 1.5f;
    client.set_gain(GainType::Kp, gain_val);

    ProcessBus();

    float received_gain;
    EXPECT_TRUE(server.get_new_gain(GainType::Kp, received_gain));
    EXPECT_FLOAT_EQ(received_gain, gain_val);

    // Ensure other gains are not set
    float dummy;
    EXPECT_FALSE(server.get_new_gain(GainType::Ki, dummy));
}

TEST_F(MotorDriverTest, Feedback)
{
    float feedback_val = 12.34f;
    uint8_t limit_sw   = 0x05;  // 0000 0101

    // Server sends feedback
    server.send_feedback(feedback_val, limit_sw);

    ProcessBus();

    // Client should receive feedback
    EXPECT_FLOAT_EQ(client.feedback_value(), feedback_val);
    EXPECT_EQ(client.limit_switches(), limit_sw);
}

TEST_F(MotorDriverTest, HardwareStatus)
{
    float current = 2.5f;
    int8_t temp   = 45;

    // Server sends status
    server.send_hardware_status(current, temp);

    ProcessBus();

    // Client should receive status
    EXPECT_FLOAT_EQ(client.load_current(), current);
    EXPECT_EQ(client.temperature(), temp);
}
