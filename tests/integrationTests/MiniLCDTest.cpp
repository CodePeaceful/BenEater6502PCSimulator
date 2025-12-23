#include <gtest/gtest.h>

#include <MiniLCD.hpp>
#include <MiniLCDRenderer.hpp>
#include <logger/DisplayedNumberLogger.hpp>
#include <SFML/Graphics.hpp>
#include <thread>

TEST(MiniLCDTest, instantiate) {
    uint8_t data = 0;
    bool E = false;
    bool RW = false;
    bool RS = false;

    components::MiniLCD lcd(data, E, RW, RS);
    // If we reach this point, the instantiation was successful
    SUCCEED();
}

namespace
{
void lcdWait(components::MiniLCD& lcd, uint8_t& data, bool& E, bool& RW, bool& RS) {
    // Wait until not busy
    do {
        RW = true; RS = false; // Set to read busy flag
        E = true; lcd.cycle(); E = false; lcd.cycle();
        uint8_t status = data;
        if ((status & 0x80) == 0) break; // Busy flag is bit 7
    } while (true);
    RW = false; // Set back to write mode
}

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
} // namespace

TEST(MiniLCDTest, numberGoUp) {
    uint8_t data = 0;
    bool E = false;
    bool RW = false;
    bool RS = false;

    components::MiniLCD lcd(data, E, RW, RS);
    components::logger::DisplayedNumberLogger logger(lcd);

    data = 0x38; // Function Set: 8-bit, 2 Line, 5x8 Dots
    E = true; lcd.cycle(); E = false; lcd.cycle();
    lcdWait(lcd, data, E, RW, RS);
    data = 0x0C; // Display ON, Cursor OFF, Blink OFF
    E = true; lcd.cycle(); E = false; lcd.cycle();
    lcdWait(lcd, data, E, RW, RS);
    data = 0x06; // Entry Mode Set: Increment, No Shift
    E = true; lcd.cycle(); E = false; lcd.cycle();
    lcdWait(lcd, data, E, RW, RS);
    data = 0x01; // Clear Display
    E = true; lcd.cycle(); E = false; lcd.cycle();
    lcdWait(lcd, data, E, RW, RS);

    for (uint16_t i = 0; i < 64000; ++i) {
        auto s = std::to_string(i); // just to prevent optimization out

        for (char c : s) {
            data = static_cast<uint8_t>(c);
            RS = true; // Data mode
            E = true; lcd.cycle(); E = false; lcd.cycle();
            lcdWait(lcd, data, E, RW, RS);
            RS = false; // Command mode
        }

        uint32_t displayedNumber = logger.getDisplayedNumber();
        ASSERT_EQ(displayedNumber, i) << "Displayed number mismatch at iteration " << i;

        // set cursor back to start
        data = 0x02; // Home
        E = true; lcd.cycle(); E = false; lcd.cycle();
        lcdWait(lcd, data, E, RW, RS);
    }
}



TEST(MiniLCDTestRender, numberGoUpRender) {
    uint8_t data = 0;
    bool E = false;
    bool RW = false;
    bool RS = false;

    components::MiniLCD lcd(data, E, RW, RS);
    components::logger::DisplayedNumberLogger logger(lcd);
    components::MiniLCDRenderer lcdRenderer(lcd);
    std::jthread renderThread(display, std::ref(lcdRenderer));

    data = 0x38; // Function Set: 8-bit, 2 Line, 5x8 Dots
    E = true; lcd.cycle(); E = false; lcd.cycle();
    lcdWait(lcd, data, E, RW, RS);
    data = 0x0C; // Display ON, Cursor OFF, Blink OFF
    E = true; lcd.cycle(); E = false; lcd.cycle();
    lcdWait(lcd, data, E, RW, RS);
    data = 0x06; // Entry Mode Set: Increment, No Shift
    E = true; lcd.cycle(); E = false; lcd.cycle();
    lcdWait(lcd, data, E, RW, RS);
    data = 0x01; // Clear Display
    E = true; lcd.cycle(); E = false; lcd.cycle();
    lcdWait(lcd, data, E, RW, RS);

    for (uint16_t i = 0; i < 64000; ++i) {
        auto s = std::to_string(i); // just to prevent optimization out

        for (char c : s) {
            data = static_cast<uint8_t>(c);
            RS = true; // Data mode
            E = true; lcd.cycle(); E = false; lcd.cycle();
            lcdWait(lcd, data, E, RW, RS);
            RS = false; // Command mode
        }

        uint32_t displayedNumber = logger.getDisplayedNumber();
        ASSERT_EQ(displayedNumber, i) << "Displayed number mismatch at iteration " << i;

        // set cursor back to start
        data = 0x02; // Home
        E = true; lcd.cycle(); E = false; lcd.cycle();
        lcdWait(lcd, data, E, RW, RS);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
