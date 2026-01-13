#include <gtest/gtest.h>

#include "gn10_can/core/can_device.hpp"
#include "gn10_can/core/can_manager.hpp"
#include "mock_driver.hpp"

using namespace gn10_can;

class MockDevice : public CANDevice {
  public:
    MockDevice(CANManager& manager, id::DeviceType type, uint8_t id)
        : CANDevice(manager, type, id) {}

    void on_receive(const CANFrame& frame) override { received_frames.push_back(frame); }

    std::vector<CANFrame> received_frames;
};

class CANManagerTest : public ::testing::Test {
  protected:
    MockDriver driver;
    CANManager manager{driver};
};

TEST_F(CANManagerTest, RegisterDevice) {
    MockDevice device1(manager, id::DeviceType::MotorDriver, 1);
    EXPECT_TRUE(manager.register_device(&device1));
}

TEST_F(CANManagerTest, RegisterMaxDevices) {
    std::vector<std::unique_ptr<MockDevice>> devices;
    for (std::size_t i = 0; i < CANManager::MAX_DEVICES; ++i) {
        devices.push_back(std::make_unique<MockDevice>(manager, id::DeviceType::MotorDriver, i));
        EXPECT_TRUE(manager.register_device(devices.back().get()));
    }

    MockDevice extra_device(manager, id::DeviceType::MotorDriver, 100);
    EXPECT_FALSE(manager.register_device(&extra_device));
}

TEST_F(CANManagerTest, UpdateReceivesFrames) {
    MockDevice device(manager, id::DeviceType::MotorDriver, 1);
    manager.register_device(&device);

    CANFrame frame;
    frame.id = 0x123;
    driver.push_receive_frame(frame);

    manager.update();

    ASSERT_EQ(device.received_frames.size(), 1);
    EXPECT_EQ(device.received_frames[0].id, 0x123);
}

TEST_F(CANManagerTest, SendFrame) {
    CANFrame frame;
    frame.id = 0x456;

    EXPECT_TRUE(manager.send_frame(frame));

    ASSERT_EQ(driver.sent_frames.size(), 1);
    EXPECT_EQ(driver.sent_frames[0].id, 0x456);
}
