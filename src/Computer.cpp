#include "Computer.hpp"

#include <fstream>
#include <thread>
#include <chrono>
#include <print>

#include <SFML/Graphics.hpp>

#include "ComputerLogger.hpp"
namespace fs = std::filesystem;

Computer::Computer() : cpu{data, address, VPB, RDY, IRQB, MLB, NMIB, SYNC, RWB, BE, SOB, PHI2, PHI1O, PHI2O, RESB},
rom{address, data, romOutputDisable, romCS}, ram{address, data, ramOutputDisable, RWB, ramCS},
via{RWB, viaCS1, viaCS2B, data, viaPortA, viaPortB, RS0, RS1, RS2, RS3, CA1, CA2, CB1, CB2, IRQB, PHI2, RESB},
lcd{viaPortB, e, rw, rs}, lcdRenderer{lcd} {
    cpu.reset();
    via.reset();
}

void Computer::reprogram(const fs::path& binary32k) {
    std::array<uint8_t, 0x8000> binary{ };
    std::ifstream input(binary32k, std::ios::binary);
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
}

void Computer::run() {
    std::jthread render(&Computer::display, std::ref(*this));
    uint8_t cycleCount = 3;
#ifdef logging
    ComputerLogger logger(ram, cpu, via, lcd);
#endif
    while (alive) {
        auto halfCycleStart = std::chrono::high_resolution_clock::now();
        PHI2 = cycleCount & 0x02;
#ifdef logging
        if (cycleCount == 0) {
            logger.logCycle();
        }
#endif
        cycleCount = (cycleCount + 1) % 4;

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

void Computer::display() {
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(256, 144)), "Ben Eater MiniPC Output", sf::State::Windowed);
    window.setFramerateLimit(3);
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                alive = false;
                window.close();
            }
            // tab to reset computer
            if (const auto key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Tab) {
                    RESB = false;
                }
            }
            if (const auto key = event->getIf<sf::Event::KeyReleased>()) {
                if (key->code == sf::Keyboard::Key::Tab) {
                    RESB = true;
                }
            }
        }

        NMIB = !NMIB;

        window.clear(sf::Color(0, 0, 0, 255));
        lcdRenderer.draw(window);
        window.display();
    }
}
