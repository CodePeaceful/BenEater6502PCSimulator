#include <gtest/gtest.h>
#include <tuple>
#include <vector>
#include <cstdint>

#include <ALUFunctions.hpp>

// values from https://www.righto.com/2012/12/the-6502-overflow-flag-explained.html
TEST(addWithCarry, noDecimal_noCarry_params_resultA_resultStatus_A_Status_second) {
    std::vector<std::tuple<uint8_t, uint8_t, uint8_t, uint8_t, uint8_t>> cases{
        {0x60, 0b00110100, 0x50, 0b00110100, 0x10},
        {0xa0, 0b11110100, 0x50, 0b00110100, 0x50},
        {0xe0, 0b10110100, 0x50, 0b00110100, 0x90},
        {0x20, 0b00110101, 0x50, 0b00110100, 0xd0},
        {0xe0, 0b10110100, 0xd0, 0b00110100, 0x10},
        {0x20, 0b00110101, 0xd0, 0b00110100, 0x50},
        {0x60, 0b01110101, 0xd0, 0b00110100, 0x90},
        {0xa0, 0b10110101, 0xd0, 0b00110100, 0xd0}
    };

    for (const auto& [resultA, resultFlags, inA, inFlags, inValue] : cases) {
        auto flags = inFlags;
        uint8_t A = ALUFunctions::addWithCarry(inA, inValue, flags);
        ASSERT_EQ(resultA, A);
        ASSERT_EQ(resultFlags, flags);
    }
}

TEST(addWithCarry, noDecimal_yesCarry_params_resultA_resultStatus_A_Status_second) {
    std::vector<std::tuple<uint8_t, uint8_t, uint8_t, uint8_t, uint8_t>> cases{
        {0x61, 0b00110100, 0x50, 0b00110101, 0x10},
        {0xa1, 0b11110100, 0x50, 0b00110101, 0x50},
        {0xe1, 0b10110100, 0x50, 0b00110101, 0x90},
        {0x21, 0b00110101, 0x50, 0b00110101, 0xd0},
        {0xe1, 0b10110100, 0xd0, 0b00110101, 0x10},
        {0x21, 0b00110101, 0xd0, 0b00110101, 0x50},
        {0x61, 0b01110101, 0xd0, 0b00110101, 0x90},
        {0xa1, 0b10110101, 0xd0, 0b00110101, 0xd0}
    };

    for (const auto& [resultA, resultFlags, inA, inFlags, inValue] : cases) {
        auto flags = inFlags;
        uint8_t A = ALUFunctions::addWithCarry(inA, inValue, flags);
        ASSERT_EQ(resultA, A);
        ASSERT_EQ(resultFlags, flags);
    }
}

TEST(subtractWithCarry, noDecimal_noCarry_params_resultA_resultStatus_A_Status_second) {
    std::vector<std::tuple<uint8_t, uint8_t, uint8_t, uint8_t, uint8_t>> cases{
        {0x60, 0b00110100, 0x51, 0b00110100, 0xf0},
        {0xa0, 0b11110100, 0x51, 0b00110100, 0xb0},
        {0xe0, 0b10110100, 0x51, 0b00110100, 0x70},
        {0x20, 0b00110101, 0x51, 0b00110100, 0x30},
        {0xe0, 0b10110100, 0xd1, 0b00110100, 0xf0},
        {0x20, 0b00110101, 0xd1, 0b00110100, 0xb0},
        {0x60, 0b01110101, 0xd1, 0b00110100, 0x70},
        {0xa0, 0b10110101, 0xd1, 0b00110100, 0x30}
    };
    for (const auto& [resultA, resultFlags, inA, inFlags, inValue] : cases) {
        auto flags = inFlags;
        uint8_t A = ALUFunctions::subtractWithCarry(inA, inValue, flags);
        ASSERT_EQ(resultA, A);
        ASSERT_EQ(resultFlags, flags);
    }
}

TEST(subtractWithCarry, noDecimal_yesCarry_params_resultA_resultStatus_A_Status_second) {
    std::vector<std::tuple<uint8_t, uint8_t, uint8_t, uint8_t, uint8_t>> cases{
        {0x60, 0b00110100, 0x50, 0b00110101, 0xf0},
        {0xa0, 0b11110100, 0x50, 0b00110101, 0xb0},
        {0xe0, 0b10110100, 0x50, 0b00110101, 0x70},
        {0x20, 0b00110101, 0x50, 0b00110101, 0x30},
        {0xe0, 0b10110100, 0xd0, 0b00110101, 0xf0},
        {0x20, 0b00110101, 0xd0, 0b00110101, 0xb0},
        {0x60, 0b01110101, 0xd0, 0b00110101, 0x70},
        {0xa0, 0b10110101, 0xd0, 0b00110101, 0x30}
    };
    for (const auto& [resultA, resultFlags, inA, inFlags, inValue] : cases) {
        auto flags = inFlags;
        uint8_t A = ALUFunctions::subtractWithCarry(inA, inValue, flags);
        ASSERT_EQ(resultA, A);
        ASSERT_EQ(resultFlags, flags);
    }
}

TEST(compare, compare) {
    std::vector<std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>> cases{
        {0x50, 0b00110100, 0x60, 0b00110100},
        {0x50, 0b00110100, 0xa0, 0b00110100},
        {0x50, 0b00110100, 0xe0, 0b00110100},
        {0x50, 0b00110100, 0x20, 0b00110101},
        {0xd0, 0b00110100, 0xe0, 0b10110100},
        {0xd0, 0b00110100, 0x20, 0b10110101},
        {0xd0, 0b00110100, 0x60, 0b10110101},
        {0xd0, 0b00110100, 0xa0, 0b10110101},
        {0x50, 0b00110101, 0x60, 0b00110100},
        {0x50, 0b00110101, 0xa0, 0b00110100},
        {0x50, 0b00110101, 0xe0, 0b00110100},
        {0x50, 0b00110101, 0x20, 0b00110101},
        {0xd0, 0b00110101, 0xe0, 0b10110100},
        {0xd0, 0b00110101, 0x20, 0b10110101},
        {0xd0, 0b00110101, 0x60, 0b10110101},
        {0xd0, 0b00110101, 0xa0, 0b10110101},
        {0x50, 0b00110110, 0x60, 0b00110100},
        {0x50, 0b00110110, 0xa0, 0b00110100},
        {0x50, 0b00110110, 0xe0, 0b00110100},
        {0x50, 0b00110110, 0x20, 0b00110101},
        {0xd0, 0b00110110, 0xe0, 0b10110100},
        {0xd0, 0b00110110, 0x20, 0b10110101},
        {0xd0, 0b00110110, 0x60, 0b10110101},
        {0xd0, 0b00110110, 0xa0, 0b10110101},
        {0x50, 0b00110111, 0x60, 0b00110100},
        {0x50, 0b00110111, 0xa0, 0b00110100},
        {0x50, 0b00110111, 0xe0, 0b00110100},
        {0x50, 0b00110111, 0x20, 0b00110101},
        {0xd0, 0b00110111, 0xe0, 0b10110100},
        {0xd0, 0b00110111, 0x20, 0b10110101},
        {0xd0, 0b00110111, 0x60, 0b10110101},
        {0xd0, 0b00110111, 0xa0, 0b10110101},
        {0x60, 0b00110100, 0x60, 0b00110111},
        {0xa0, 0b00110100, 0xa0, 0b00110111}
    };
    for (const auto& [inA, inFlags, inValue, outFlags] : cases) {
        auto flags = inFlags;
        ALUFunctions::compare(inA, inValue, flags);
        ASSERT_EQ(outFlags, flags);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}