#include <gtest/gtest.h>
#include <tuple>
#include <vector>

#define private public
#include <Cpu.hpp>

// values from https://www.righto.com/2012/12/the-6502-overflow-flag-explained.html
TEST(addWithCarry, noDecimal_noCarry_params_resultA_resultStatus_A_Status_second) {
    std::vector<std::tuple<unsigned char, unsigned char, unsigned char, unsigned char, unsigned char>> cases{
        {0x60, 0b00110100, 0x50, 0b00110100, 0x10},
        {0xa0, 0b11110100, 0x50, 0b00110100, 0x50},
        {0xe0, 0b10110100, 0x50, 0b00110100, 0x90},
        {0x20, 0b00110101, 0x50, 0b00110100, 0xd0},
        {0xe0, 0b10110100, 0xd0, 0b00110100, 0x10},
        {0x20, 0b00110101, 0xd0, 0b00110100, 0x50},
        {0x60, 0b01110101, 0xd0, 0b00110100, 0x90},
        {0xa0, 0b10110101, 0xd0, 0b00110100, 0xd0}
    };
    unsigned char data{0xEA};
    unsigned short address{0};
    bool VPB{true}; // Vector Pull
    bool RDY{true}; // ready: run on high
    bool IRQB{true}; // interupt: on low
    bool MLB{true}; // memory lock
    bool NMIB{true}; // non maskable interupt: on low
    bool SYNC{true}; //??
    bool RWB{true}; // read or write: high = read, low = write
    bool BE{true}; // bus enable
    bool SOB{true};
    bool PHI2{true};
    bool PHI2O;
    bool PHI1O;
    bool RESB{true};
    Cpu cpu(data, address, VPB, RDY, IRQB, MLB, NMIB, SYNC, RWB, BE, SOB, PHI2, PHI2O, PHI1O, RESB);
    for (const auto& [resultA, resultFlags, inA, inFlags, inValue] : cases) {
        cpu.A = inA;
        cpu.prozessorStatus = inFlags;
        cpu.addWithCarry(inValue);
        ASSERT_EQ(resultA, cpu.A);
        ASSERT_EQ(resultFlags, cpu.prozessorStatus);
    }
}

TEST(addWithCarry, noDecimal_yesCarry_params_resultA_resultStatus_A_Status_second) {
    std::vector<std::tuple<unsigned char, unsigned char, unsigned char, unsigned char, unsigned char>> cases{
        {0x61, 0b00110100, 0x50, 0b00110101, 0x10},
        {0xa1, 0b11110100, 0x50, 0b00110101, 0x50},
        {0xe1, 0b10110100, 0x50, 0b00110101, 0x90},
        {0x21, 0b00110101, 0x50, 0b00110101, 0xd0},
        {0xe1, 0b10110100, 0xd0, 0b00110101, 0x10},
        {0x21, 0b00110101, 0xd0, 0b00110101, 0x50},
        {0x61, 0b01110101, 0xd0, 0b00110101, 0x90},
        {0xa1, 0b10110101, 0xd0, 0b00110101, 0xd0}
    };
    unsigned char data{0xEA};
    unsigned short address{0};
    bool VPB{true}; // Vector Pull
    bool RDY{true}; // ready: run on high
    bool IRQB{true}; // interupt: on low
    bool MLB{true}; // memory lock
    bool NMIB{true}; // non maskable interupt: on low
    bool SYNC{true}; //??
    bool RWB{true}; // read or write: high = read, low = write
    bool BE{true}; // bus enable
    bool SOB{true};
    bool PHI2{true};
    bool PHI2O;
    bool PHI1O;
    bool RESB{true};
    Cpu cpu(data, address, VPB, RDY, IRQB, MLB, NMIB, SYNC, RWB, BE, SOB, PHI2, PHI2O, PHI1O, RESB);
    for (const auto& [resultA, resultFlags, inA, inFlags, inValue] : cases) {
        cpu.A = inA;
        cpu.prozessorStatus = inFlags;
        cpu.addWithCarry(inValue);
        ASSERT_EQ(resultA, cpu.A);
        ASSERT_EQ(resultFlags, cpu.prozessorStatus);
    }
}

TEST(subtractWithCarry, noDecimal_noCarry_params_resultA_resultStatus_A_Status_second) {
    std::vector<std::tuple<unsigned char, unsigned char, unsigned char, unsigned char, unsigned char>> cases{
        {0x60, 0b00110100, 0x51, 0b00110100, 0xf0},
        {0xa0, 0b11110100, 0x51, 0b00110100, 0xb0},
        {0xe0, 0b10110100, 0x51, 0b00110100, 0x70},
        {0x20, 0b00110101, 0x51, 0b00110100, 0x30},
        {0xe0, 0b10110100, 0xd1, 0b00110100, 0xf0},
        {0x20, 0b00110101, 0xd1, 0b00110100, 0xb0},
        {0x60, 0b01110101, 0xd1, 0b00110100, 0x70},
        {0xa0, 0b10110101, 0xd1, 0b00110100, 0x30}
    };
    unsigned char data{0xEA};
    unsigned short address{0};
    bool VPB{true}; // Vector Pull
    bool RDY{true}; // ready: run on high
    bool IRQB{true}; // interupt: on low
    bool MLB{true}; // memory lock
    bool NMIB{true}; // non maskable interupt: on low
    bool SYNC{true}; //??
    bool RWB{true}; // read or write: high = read, low = write
    bool BE{true}; // bus enable
    bool SOB{true};
    bool PHI2{true};
    bool PHI2O;
    bool PHI1O;
    bool RESB{true};
    Cpu cpu(data, address, VPB, RDY, IRQB, MLB, NMIB, SYNC, RWB, BE, SOB, PHI2, PHI2O, PHI1O, RESB);
    for (const auto& [resultA, resultFlags, inA, inFlags, inValue] : cases) {
        cpu.A = inA;
        cpu.prozessorStatus = inFlags;
        cpu.subtractWithCarry(inValue);
        ASSERT_EQ(resultA, cpu.A);
        ASSERT_EQ(resultFlags, cpu.prozessorStatus);
    }
}

TEST(subtractWithCarry, noDecimal_yesCarry_params_resultA_resultStatus_A_Status_second) {
    std::vector<std::tuple<unsigned char, unsigned char, unsigned char, unsigned char, unsigned char>> cases{
        {0x60, 0b00110100, 0x50, 0b00110101, 0xf0},
        {0xa0, 0b11110100, 0x50, 0b00110101, 0xb0},
        {0xe0, 0b10110100, 0x50, 0b00110101, 0x70},
        {0x20, 0b00110101, 0x50, 0b00110101, 0x30},
        {0xe0, 0b10110100, 0xd0, 0b00110101, 0xf0},
        {0x20, 0b00110101, 0xd0, 0b00110101, 0xb0},
        {0x60, 0b01110101, 0xd0, 0b00110101, 0x70},
        {0xa0, 0b10110101, 0xd0, 0b00110101, 0x30}
    };
    unsigned char data{0xEA};
    unsigned short address{0};
    bool VPB{true}; // Vector Pull
    bool RDY{true}; // ready: run on high
    bool IRQB{true}; // interupt: on low
    bool MLB{true}; // memory lock
    bool NMIB{true}; // non maskable interupt: on low
    bool SYNC{true}; //??
    bool RWB{true}; // read or write: high = read, low = write
    bool BE{true}; // bus enable
    bool SOB{true};
    bool PHI2{true};
    bool PHI2O;
    bool PHI1O;
    bool RESB{true};
    Cpu cpu(data, address, VPB, RDY, IRQB, MLB, NMIB, SYNC, RWB, BE, SOB, PHI2, PHI2O, PHI1O, RESB);
    for (const auto& [resultA, resultFlags, inA, inFlags, inValue] : cases) {
        cpu.A = inA;
        cpu.prozessorStatus = inFlags;
        cpu.subtractWithCarry(inValue);
        ASSERT_EQ(resultA, cpu.A);
        ASSERT_EQ(resultFlags, cpu.prozessorStatus);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}