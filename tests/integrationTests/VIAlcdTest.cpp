#include <gtest/gtest.h>

#include <MiniLCD.hpp>
#include <logger/DisplayedNumberLogger.hpp>
#include <VersatileInterfaceAdapter.hpp>
#include <MiniLCDRenderer.hpp>
#include <thread>

namespace
{
void display(components::MiniLCDRenderer& lcdRenderer) {
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(256, 144)), "Ben Eater MiniPC Output", sf::State::Windowed);
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }


        window.clear(sf::Color(0, 0, 0, 255));
        lcdRenderer.draw(window);
        window.display();
    }
}

void cycleSystem(components::MiniLCD& lcd, components::VersatileInterfaceAdapter& via, uint8_t& portA, bool& E, bool& RWLCD, bool& RSLCD) {
    // VIA cycle
    via.cycle();

    // Update LCD pins from VIA port A
    RWLCD = (portA & 0x40) != 0; // VIA PA6 -> LCD RW
    RSLCD = (portA & 0x20) != 0; // VIA PA5 -> LCD RS
    E = (portA & 0x80) != 0;     // VIA PA7 -> LCD E

    // LCD cycle
    lcd.cycle();
}

// rs2 and rs3 are tied low externally
// no direct lcd pin access here, all via portA
void lcdWait(components::MiniLCD& lcd, components::VersatileInterfaceAdapter& via, uint8_t& data, bool& RS0, bool& RS1, bool& RW, bool& PHI2, uint8_t& portA, bool& E, bool& RWLCD, bool& RSLCD) {
    // set B to input
    RS0 = false;
    RS1 = true;
    RW = false;
    PHI2 = false;
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    PHI2 = true;
    data = 0x00;
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    do {
        // set RWLCD high to read busy flag
        RS0 = true;
        RS1 = false;
        RW = false;
        PHI2 = false;
        cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
        cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
        PHI2 = true;
        data = 0x40;
        cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
        cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);

        // pulse E
        RS0 = true;
        RS1 = false;
        RW = false;
        PHI2 = false;
        cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
        cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
        PHI2 = true;
        data = 0xC0;
        cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
        cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);

        // read Port B data
        RS0 = false;
        RS1 = false;
        RW = true;
        PHI2 = false;
        cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
        cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
        PHI2 = true;
        cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
        if ((data & 0x80) == 0) {
            cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
            break; // not busy
        }
        cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    } while (true); // wait until not busy

    // set B to output
    RS0 = false;
    RS1 = true;
    RW = false;
    PHI2 = false;
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    PHI2 = true;
    data = 0xFF;
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
}

void lcdInstuction(components::MiniLCD& lcd, components::VersatileInterfaceAdapter& via, uint8_t& data, bool& RS0, bool& RS1, bool& RW, bool& PHI2, uint8_t& portA, bool& E, bool& RWLCD, bool& RSLCD) {
    auto dataToSend = data;
    lcdWait(lcd, via, data, RS0, RS1, RW, PHI2, portA, E, RWLCD, RSLCD);
    // set data on port B
    RS0 = false;
    RS1 = false;
    RW = false;
    PHI2 = false;
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    PHI2 = true;
    data = dataToSend;
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);

    // clear all A pins
    RS0 = true;
    RS1 = false;
    RW = false;
    PHI2 = false;
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    PHI2 = true;
    data = 0x00;
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);

    // pulse E
    RS0 = true;
    RS1 = false;
    RW = false;
    PHI2 = false;
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    PHI2 = true;
    data = 0x80;
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);

    // clear all A pins
    RS0 = true;
    RS1 = false;
    RW = false;
    PHI2 = false;
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    PHI2 = true;
    data = 0x00;
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
}

void lcdDataWrite(components::MiniLCD& lcd, components::VersatileInterfaceAdapter& via, uint8_t& data, bool& RS0, bool& RS1, bool& RW, bool& PHI2, uint8_t& portA, bool& E, bool& RWLCD, bool& RSLCD) {
    auto dataToSend = data;
    lcdWait(lcd, via, data, RS0, RS1, RW, PHI2, portA, E, RWLCD, RSLCD);
    // set data on port B
    RS0 = false;
    RS1 = false;
    RW = false;
    PHI2 = false;
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    PHI2 = true;
    data = dataToSend;
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);

    // set A for data write
    RS0 = true;
    RS1 = false;
    RW = false;
    PHI2 = false;
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    PHI2 = true;
    data = 0x20;
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);

    // pulse E
    RS0 = true;
    RS1 = false;
    RW = false;
    PHI2 = false;
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    PHI2 = true;
    data = 0xA0;
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);

    // set A for data write
    RS0 = true;
    RS1 = false;
    RW = false;
    PHI2 = false;
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    PHI2 = true;
    data = 0x20;;
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
    cycleSystem(lcd, via, portA, E, RWLCD, RSLCD);
}
} // namespace

TEST(MiniLCDTestWithVIA, numberGoUp) {
    uint8_t data = 0;
    uint8_t portA = 0;
    uint8_t portB = 0;
    bool RWB = false;
    bool CS1 = true;
    bool CS2B = false;
    bool RS0 = false;
    bool RS1 = false;
    bool RS2 = false;
    bool RS3 = false;
    bool CA1 = false;
    bool CA2 = false;
    bool CB1 = false;
    bool CB2 = false;
    bool IRQB = true;
    bool PHI2 = false;
    bool RESB = true;
    bool E = false;
    bool RW = false;
    bool RS = false;
    components::VersatileInterfaceAdapter via(RWB, CS1, CS2B, data, portA, portB,
        RS0, RS1, RS2, RS3, CA1, CA2, CB1, CB2, IRQB, PHI2, RESB);
    via.reset();

    components::MiniLCD lcd(portB, E, RW, RS);
    components::logger::DisplayedNumberLogger logger(lcd);

    // set via port A pins 5,6,7 to output
    RS0 = false;
    RS1 = true;
    RW = false;
    PHI2 = false;
    cycleSystem(lcd, via, portA, E, RW, RS);
    cycleSystem(lcd, via, portA, E, RW, RS);
    PHI2 = true;
    data = 0xE0;
    cycleSystem(lcd, via, portA, E, RW, RS);
    cycleSystem(lcd, via, portA, E, RW, RS);

    // set via port B to output
    RS0 = true;
    RS1 = true;
    RW = false;
    PHI2 = false;
    cycleSystem(lcd, via, portA, E, RW, RS);
    cycleSystem(lcd, via, portA, E, RW, RS);
    PHI2 = true;
    data = 0xFF;
    cycleSystem(lcd, via, portA, E, RW, RS);
    cycleSystem(lcd, via, portA, E, RW, RS);

    data = 0x38; // Function Set: 8-bit, 2 Line, 5x8 Dots
    lcdInstuction(lcd, via, data, RS0, RS1, RWB, PHI2, portA, E, RW, RS);
    data = 0x0C; // Display ON, Cursor OFF, Blink OFF
    lcdInstuction(lcd, via, data, RS0, RS1, RWB, PHI2, portA, E, RW, RS);
    data = 0x06; // Entry Mode Set: Increment, No Shift
    lcdInstuction(lcd, via, data, RS0, RS1, RWB, PHI2, portA, E, RW, RS);
    data = 0x01; // Clear Display
    lcdInstuction(lcd, via, data, RS0, RS1, RWB, PHI2, portA, E, RW, RS);

    for (uint16_t i = 0; i < 64000; ++i) {
        auto s = std::to_string(i); // just to prevent optimization out

        for (char c : s) {
            data = static_cast<uint8_t>(c);
            lcdDataWrite(lcd, via, data, RS0, RS1, RWB, PHI2, portA, E, RW, RS);
        }

        // set cursor back to start
        data = 0x02; // Home
        lcdInstuction(lcd, via, data, RS0, RS1, RWB, PHI2, portA, E, RW, RS);

        uint32_t displayedNumber = logger.getDisplayedNumber();
        ASSERT_EQ(displayedNumber, i) << "Displayed number mismatch at iteration " << i;
    }
}

TEST(MiniLCDTestWithVIA, numberGoUpRender) {
    uint8_t data = 0;
    uint8_t portA = 0;
    uint8_t portB = 0;
    bool RWB = false;
    bool CS1 = true;
    bool CS2B = false;
    bool RS0 = false;
    bool RS1 = false;
    bool RS2 = false;
    bool RS3 = false;
    bool CA1 = false;
    bool CA2 = false;
    bool CB1 = false;
    bool CB2 = false;
    bool IRQB = true;
    bool PHI2 = false;
    bool RESB = true;
    bool E = false;
    bool RW = false;
    bool RS = false;
    components::VersatileInterfaceAdapter via(RWB, CS1, CS2B, data, portA, portB,
        RS0, RS1, RS2, RS3, CA1, CA2, CB1, CB2, IRQB, PHI2, RESB);
    via.reset();

    components::MiniLCD lcd(portB, E, RW, RS);
    components::logger::DisplayedNumberLogger logger(lcd);
    components::MiniLCDRenderer lcdRenderer(lcd);
    std::jthread renderThread(display, std::ref(lcdRenderer));

    // set via port A pins 5,6,7 to output
    RS0 = false;
    RS1 = true;
    RW = false;
    PHI2 = false;
    cycleSystem(lcd, via, portA, E, RW, RS);
    cycleSystem(lcd, via, portA, E, RW, RS);
    PHI2 = true;
    data = 0xE0;
    cycleSystem(lcd, via, portA, E, RW, RS);
    cycleSystem(lcd, via, portA, E, RW, RS);

    // set via port B to output
    RS0 = true;
    RS1 = true;
    RW = false;
    PHI2 = false;
    cycleSystem(lcd, via, portA, E, RW, RS);
    cycleSystem(lcd, via, portA, E, RW, RS);
    PHI2 = true;
    data = 0xFF;
    cycleSystem(lcd, via, portA, E, RW, RS);
    cycleSystem(lcd, via, portA, E, RW, RS);

    data = 0x38; // Function Set: 8-bit, 2 Line, 5x8 Dots
    lcdInstuction(lcd, via, data, RS0, RS1, RWB, PHI2, portA, E, RW, RS);
    data = 0x0C; // Display ON, Cursor OFF, Blink OFF
    lcdInstuction(lcd, via, data, RS0, RS1, RWB, PHI2, portA, E, RW, RS);
    data = 0x06; // Entry Mode Set: Increment, No Shift
    lcdInstuction(lcd, via, data, RS0, RS1, RWB, PHI2, portA, E, RW, RS);
    data = 0x01; // Clear Display
    lcdInstuction(lcd, via, data, RS0, RS1, RWB, PHI2, portA, E, RW, RS);

    for (uint16_t i = 0; i < 64000; ++i) {
        auto s = std::to_string(i); // just to prevent optimization out

        for (char c : s) {
            data = static_cast<uint8_t>(c);
            lcdDataWrite(lcd, via, data, RS0, RS1, RWB, PHI2, portA, E, RW, RS);
        }

        uint32_t displayedNumber = logger.getDisplayedNumber();
        ASSERT_EQ(displayedNumber, i) << "Displayed number mismatch at iteration " << i;

        // set cursor back to start
        data = 0x02; // Home
        lcdInstuction(lcd, via, data, RS0, RS1, RWB, PHI2, portA, E, RW, RS);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
