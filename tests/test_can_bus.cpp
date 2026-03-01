#include <gtest/gtest.h>

#include "gn10_can/core/can_bus.hpp"
#include "gn10_can/core/can_device.hpp"
#include "mock_driver.hpp"

using namespace gn10_can;

class MockDevice : public CANDevice
{
public:
    MockDevice(CANBus& bus, id::DeviceType type, uint8_t id) : CANDevice(bus, type, id) {}

    void on_receive(const CANFrame& frame) override
    {
        received_frames.push_back(frame);
    }

    std::vector<CANFrame> received_frames;
};

class CANBusTest : public ::testing::Test
{
protected:
    MockDriver driver;
    CANBus bus{driver};
};

TEST_F(CANBusTest, RegisterDeviceAutomatic)
{
    // RAII registration check
    // Since attach/detach are private and automatic, we can only verify via side effects (like
    // message reception) or by trusting the implementation. However, if we fail to register (e.g.
    // max devices), we can't easily check bool. The current RAII design assumes enough capacity or
    // handles failure silently (void).

    // Check basic functionality
    MockDevice device1(bus, id::DeviceType::MotorDriver, 1);

    // Simulate frame reception
    CANFrame frame;
    // Construct routing ID for this device
    frame.id = device1.get_routing_id() << id::BIT_WIDTH_COMMAND;

    driver.push_receive_frame(frame);
    bus.update();

    ASSERT_EQ(device1.received_frames.size(), 1);
}

TEST_F(CANBusTest, RegisterMaxDevices)
{
    std::vector<std::unique_ptr<MockDevice>> devices;
    for (std::size_t i = 0; i < CANBus::MAX_DEVICES; ++i) {
        devices.push_back(std::make_unique<MockDevice>(bus, id::DeviceType::MotorDriver, i));
    }

    // This extra device should fail to attach silently inside constructor
    MockDevice extra_device(bus, id::DeviceType::MotorDriver, 100);

    // Verify extra_device does NOT receive messages
    CANFrame frame;
    frame.id = extra_device.get_routing_id() << id::BIT_WIDTH_COMMAND;
    driver.push_receive_frame(frame);
    bus.update();

    EXPECT_EQ(extra_device.received_frames.size(), 0);
}

TEST_F(CANBusTest, DetachOnDestruction)
{
    {
        MockDevice device(bus, id::DeviceType::MotorDriver, 1);

        CANFrame frame;
        frame.id = device.get_routing_id() << id::BIT_WIDTH_COMMAND;
        driver.push_receive_frame(frame);

        bus.update();
        EXPECT_EQ(device.received_frames.size(), 1);
    }
    // device destroyed here, should have detached

    // Sending frame again should not cause issues (bus should not access deleted memory)
    // This is hard to test deterministically without mocking internals, but it checks for crashes.
    CANFrame frame;
    // Reconstruct ID for the now-deleted device
    uint32_t deleted_id = (static_cast<uint32_t>(id::DeviceType::MotorDriver) & 0x0F)
                              << (id::BIT_WIDTH_COMMAND + id::BIT_WIDTH_DEV_ID) |
                          (1 & 0x0F) << id::BIT_WIDTH_COMMAND;
    frame.id = deleted_id << id::BIT_WIDTH_COMMAND;

    driver.push_receive_frame(frame);
    bus.update();  // Should run without accessing deleted object
}

TEST_F(CANBusTest, SendFrame)
{
    CANFrame frame;
    frame.id = 0x456;

    EXPECT_TRUE(bus.send_frame(frame));

    ASSERT_EQ(driver.sent_frames.size(), 1);
    EXPECT_EQ(driver.sent_frames[0].id, 0x456);
}
