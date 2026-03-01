#include <gtest/gtest.h>

#include "gn10_can/core/can_frame.hpp"
#include "gn10_can/core/can_id.hpp"

using namespace gn10_can;

TEST(CANFrameTest, DefaultConstructor)
{
    CANFrame frame;
    EXPECT_EQ(frame.id, 0);
    EXPECT_EQ(frame.dlc, 0);
    EXPECT_FALSE(frame.is_extended);
    EXPECT_FALSE(frame.is_rtr);
    EXPECT_FALSE(frame.is_error);
}

TEST(CANFrameTest, MakeWithPointer)
{
    uint8_t data[] = {0x01, 0x02, 0x03};
    auto frame =
        CANFrame::make(id::DeviceType::MotorDriver, 1, id::MsgTypeMotorDriver::Target, data, 3);

    // Verify ID packing (implementation dependent, but checking non-zero is a start)
    EXPECT_NE(frame.id, 0);

    EXPECT_EQ(frame.dlc, 3);
    EXPECT_EQ(frame.data[0], 0x01);
    EXPECT_EQ(frame.data[1], 0x02);
    EXPECT_EQ(frame.data[2], 0x03);
}

TEST(CANFrameTest, MakeWithInitializerList)
{
    auto frame = CANFrame::make(
        id::DeviceType::SolenoidDriver, 2, id::MsgTypeSolenoidDriver::Target, {0xAA, 0xBB}
    );

    EXPECT_EQ(frame.dlc, 2);
    EXPECT_EQ(frame.data[0], 0xAA);
    EXPECT_EQ(frame.data[1], 0xBB);
}

TEST(CANFrameTest, SetData)
{
    CANFrame frame;
    uint8_t data[] = {
        0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90
    };  // 9 bytes, should truncate to 8

    frame.set_data(data, 9);

    EXPECT_EQ(frame.dlc, 8);
    EXPECT_EQ(frame.data[7], 0x80);
}
