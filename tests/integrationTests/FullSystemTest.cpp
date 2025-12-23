#include <gtest/gtest.h>

#include <fstream>
#include <random>

#include "Cpu.hpp"
#include "Eeprom32k.hpp"
#include "Ram32k.hpp"
#include "VersatileInterfaceAdapter.hpp"
#include "MiniLCD.hpp"
#include "MiniLCDRenderer.hpp"
#include "logger/DisplayedNumberLogger.hpp"

namespace
{
void cycleSystem(components::Cpu& cpu, components::Eeprom32k& rom, components::Ram32k& ram,
    components::VersatileInterfaceAdapter& via, components::MiniLCD& lcd,
    uint16_t& address, bool& PHI2,
    bool& romCS, bool& ramOutputDisable, bool& ramCS,
    bool& RS0, bool& RS1, bool& RS2, bool& RS3,
    bool& viaCS1, bool& viaCS2B,
    uint8_t& viaPortA, bool& e, bool& rw, bool& rs) {

    for (int cycleCount = 0; cycleCount < 4; ++cycleCount) {
        auto halfCycleStart = std::chrono::high_resolution_clock::now();

        PHI2 = cycleCount & 0x02;

        cpu.cycle();

        romCS = !(address & 0x8000);

        ramCS = !(romCS && PHI2);
        ramOutputDisable = address & 0x4000;

        viaCS1 = address & 0x2000;
        viaCS2B = !(romCS && (0x4000 & address));
        RS0 = address & 0x0001;
        RS1 = address & 0x0002;
        RS2 = address & 0x0004;
        RS3 = address & 0x0008;

        rom.cycle();
        ram.cycle();
        via.cycle();

        e = viaPortA & 0x80;
        rw = viaPortA & 0x40;
        rs = viaPortA & 0x20;
        lcd.cycle();

        auto halfCycleEnd = std::chrono::high_resolution_clock::now();
        while (std::chrono::duration_cast<std::chrono::nanoseconds>(halfCycleEnd - halfCycleStart).count() < 240) {
            halfCycleEnd = std::chrono::high_resolution_clock::now();
        }
    }
}
} // namespace

TEST(FullSystemTest, numberGoUp) {
    // CPU Connections
    uint8_t data{0xEA};
    uint16_t address{0};
    bool VPB{true}; // Vector Pull
    bool RDY{true}; // ready: run on high
    bool IRQB{true}; // interrupt: on low
    bool MLB{true}; // memory lock
    bool NMIB{true}; // non maskable interrupt: on low
    bool SYNC{true}; //??
    bool RWB{true}; // read or write: high = read, low = write
    const bool BE{true}; // bus enable
    bool SOB{true};
    bool PHI2{true};
    bool PHI2O{true};
    bool PHI1O{true};
    bool RESB{true};

    // modified for rom
    const bool romOutputDisable{false};
    bool romCS{false};

    // modified for ram
    bool ramOutputDisable;
    bool ramCS{false};

    // modified Cpu pins for versatile interface adapter
    bool RS0{ }; // register select
    bool RS1{ }; // register select
    bool RS2{ }; // register select
    bool RS3{ }; // register select
    bool viaCS1{ };
    bool viaCS2B{ };

    // External pins of versatile interface adapter
    uint8_t viaPortA{0};
    uint8_t viaPortB{0};
    bool CA1{ }; //??
    bool CA2{ }; //??
    bool CB1{ }; //??
    bool CB2{ }; //??

    // display control pins
    bool e, rw, rs;

    components::VersatileInterfaceAdapter via(RWB, viaCS1, viaCS2B, data, viaPortA, viaPortB,
        RS0, RS1, RS2, RS3, CA1, CA2, CB1, CB2, IRQB, PHI2, RESB);
    via.reset();
    components::MiniLCD lcd(viaPortB, e, rw, rs);
    components::logger::DisplayedNumberLogger logger(lcd);
    components::Cpu cpu(data, address, VPB, RDY, IRQB, MLB, NMIB, SYNC, RWB, BE, SOB, PHI2, PHI1O, PHI2O, RESB);
    cpu.reset();
    components::Eeprom32k rom(address, data, romOutputDisable, romCS);
    components::Ram32k ram(address, data, ramOutputDisable, RWB, ramCS);
    std::array<uint8_t, 0x8000> binary{ };
    std::ifstream input("a.out", std::ios::binary);
    std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(input), { });
    if (binary.size() != buffer.size()) {
        throw std::runtime_error("binary size wrong");
    }
    int i = 0;
    for (auto c : buffer) {
        binary[i] = c;
        ++i;
    }
    rom.program(binary);

    std::random_device rd{ };
    std::mt19937 gen{rd()};
    std::uniform_int_distribution<> dist{2, 500};
    for (uint16_t expectedNumber = 0; expectedNumber < 1000; ++expectedNumber) {
        int fullNumbersDisplayed = 0;
        while (true) {
            // because it stays on the home command for a few cycles
            while (viaPortB == 0x02 && viaPortA == 0x80) {
                cycleSystem(cpu, rom, ram, via, lcd,
                    address, PHI2,
                    romCS, ramOutputDisable, ramCS,
                    RS0, RS1, RS2, RS3,
                    viaCS1, viaCS2B,
                    viaPortA, e, rw, rs);
            }
            cycleSystem(cpu, rom, ram, via, lcd,
                address, PHI2,
                romCS, ramOutputDisable, ramCS,
                RS0, RS1, RS2, RS3,
                viaCS1, viaCS2B,
                viaPortA, e, rw, rs);

            if (viaPortB == 0x02 && viaPortA == 0x80) {
                // lcd send home command -> number fully displayed
                fullNumbersDisplayed++;
                // display twice incase start was during number update
                if (fullNumbersDisplayed == 3) {
                    break;
                }
            }
        }
        ASSERT_EQ(logger.getDisplayedNumber(), expectedNumber);
        int randomWaitCycles = dist(gen);
        for (int i = 0; i < randomWaitCycles; ++i) {
            cycleSystem(cpu, rom, ram, via, lcd,
                address, PHI2,
                romCS, ramOutputDisable, ramCS,
                RS0, RS1, RS2, RS3,
                viaCS1, viaCS2B,
                viaPortA, e, rw, rs);
        }
        NMIB = false; // trigger non maskable interrupt to increment number
        int randomPostInterruptCycles = dist(gen);
        for (int i = 0; i < randomPostInterruptCycles; ++i) {
            cycleSystem(cpu, rom, ram, via, lcd,
                address, PHI2,
                romCS, ramOutputDisable, ramCS,
                RS0, RS1, RS2, RS3,
                viaCS1, viaCS2B,
                viaPortA, e, rw, rs);
        }
        NMIB = true;
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}