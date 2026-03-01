#include <gtest/gtest.h>

#include "gn10_can/utils/can_converter.hpp"

using namespace gn10_can::converter;

TEST(ConverterTest, PackUnpackInt32)
{
    uint8_t buffer[8] = {0};
    int32_t value     = -123456789;

    EXPECT_TRUE(pack(buffer, 8, 0, value));

    int32_t unpacked_value = 0;
    EXPECT_TRUE(unpack(buffer, 8, 0, unpacked_value));

    EXPECT_EQ(value, unpacked_value);
}

TEST(ConverterTest, PackUnpackFloat)
{
    uint8_t buffer[8] = {0};
    float value       = 3.14159f;

    EXPECT_TRUE(pack(buffer, 8, 2, value));  // Start at offset 2

    float unpacked_value = 0.0f;
    EXPECT_TRUE(unpack(buffer, 8, 2, unpacked_value));

    EXPECT_FLOAT_EQ(value, unpacked_value);
}

TEST(ConverterTest, BufferOverflow)
{
    uint8_t buffer[4] = {0};
    int32_t value     = 100;

    // Trying to pack 4 bytes at offset 1 in a 4-byte buffer (needs 5 bytes total)
    EXPECT_FALSE(pack(buffer, 4, 1, value));
}

TEST(ConverterTest, ArrayPack)
{
    std::array<uint8_t, 8> buffer{};
    uint16_t value = 0xABCD;

    EXPECT_TRUE(pack(buffer, 0, value));

    uint16_t unpacked_value = 0;
    EXPECT_TRUE(unpack(buffer.data(), 8, 0, unpacked_value));

    EXPECT_EQ(value, unpacked_value);
}
