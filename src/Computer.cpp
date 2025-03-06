#include "Computer.hpp"

#include <fstream>
#include <thread>
#include <chrono>
#include <print>

#include <SFML/Graphics.hpp>
namespace fs = std::filesystem;

Computer::Computer() : cpu{data, address, VPB, RDY, IRQB, MLB, NMIB, SYNC, RWB, BE, SOB, PHI2, PHI1O, PHI2O, RESB},
rom{addressModifiedRom, data}, ram{addressModifiedRam, data, ramOutputDisable, RWB},
via{RWB, viaCS1, viaCS2B, data, viaPortA, viaPortB, RS0, RS1, RS2, RS3, CA1, CA2, CB1, CB2, IRQB, PHI2, RESB},
screen{viaPortB, e, rw, rs} {
    cpu.reset();
    via.reset();
}

void Computer::reprogram(fs::path binary32k) {
    std::array<unsigned char, 0x8000> binary;
    std::ifstream input(binary32k, std::ios::binary);
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
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
    auto halfCycleStart = std::chrono::high_resolution_clock::now();
    while (alive) {
        halfCycleStart = std::chrono::high_resolution_clock::now();
        cpu.cycle();

        addressModifiedRom = address ^ 0x8000;
        rom.cycle();

        addressModifiedRam = (PHI2 ? address | 0x8000 : address);
        ramOutputDisable = address & 0x4000;
        ram.cycle();

        viaCS1 = address & 0x2000;
        viaCS2B = !(!((bool)(address & 0x8000)) && (0x4000 & address));
        RS0 = address & 0x0001;
        RS1 = address & 0x0002;
        RS2 = address & 0x0004;
        RS3 = address & 0x0008;
        via.cycle();

        e = viaPortA & 0x80;
        rw = viaPortA & 0x40;
        rs = viaPortA & 0x20;
        screen.cycle();

        PHI2 = !PHI2;
        auto halfCycleEnd = std::chrono::high_resolution_clock::now();
        while (std::chrono::duration_cast<std::chrono::nanoseconds>(halfCycleEnd - halfCycleStart).count() < 490) {
            halfCycleEnd = std::chrono::high_resolution_clock::now();
        }
    }
}

void Computer::display() {
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(256, 144)), "Ben Eater MiniPC Output", sf::State::Windowed);
    window.setFramerateLimit(60);
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
        // left mouse button for interupt
        NMIB = !sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        window.clear(sf::Color(0, 0, 0, 255));
        screen.draw(window);
        window.display();
    }
}
