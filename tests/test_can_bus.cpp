#include <gtest/gtest.h>

#include "gn10_can/core/can_device.hpp"
#include "gn10_can/core/can_bus.hpp"
#include "mock_driver.hpp"

using namespace gn10_can;

class MockDevice : public CANDevice {
  public:
    MockDevice(CANBus& bus, uint32_t id) : CANDevice(bus, id) {}

    void on_receive(const CANFrame& frame) override { received_frames.push_back(frame); }

    std::vector<CANFrame> received_frames;
};

class CANBusTest : public ::testing::Test {
  protected:
    MockDriver driver;
    CANBus bus{driver};
};

TEST_F(CANBusTest, Routing) {
    MockDevice dev1(bus, 0x100);
    MockDevice dev2(bus, 0x200);

    CANFrame f1; f1.id = 0x100;
    CANFrame f2; f2.id = 0x200;
    CANFrame f3; f3.id = 0x300; 

    driver.push_receive_frame(f1);
    driver.push_receive_frame(f2);
    driver.push_receive_frame(f3);

    bus.update();

    ASSERT_EQ(dev1.received_frames.size(), 1);
    EXPECT_EQ(dev1.received_frames[0].id, 0x100);

    ASSERT_EQ(dev2.received_frames.size(), 1);
    EXPECT_EQ(dev2.received_frames[0].id, 0x200);
}

TEST_F(CANBusTest, AutoDetach) {
    CANFrame f1; f1.id = 0x100;
    
    {
        MockDevice dev1(bus, 0x100);
        driver.push_receive_frame(f1);
        bus.update();
        EXPECT_EQ(dev1.received_frames.size(), 1);
    }
    // dev1 is destroyed.
    
    driver.push_receive_frame(f1);
    bus.update(); 
    // Should not crash, and nothing happens.
    // If we had a mechanism to spy on bus devices, we could check.
    // But no crash is good enough for basic check.
}
