#include <Cpu.hpp>
#include <Eeprom32k.hpp>
#include <VersatileInterfaceAdapter.hpp>
#include <MiniLCD.hpp>
#include <Ram32k.hpp>
#include "assembler/Assembler.hpp"

#include <iostream>
#include <bitset>
#include <chrono>
#include <thread>

void runDisplay(MiniLCD& display, bool& alive) {
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(384, 216)), "Ben Eater MiniPC Output", sf::State::Windowed);
    window.setFramerateLimit(144);
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                alive = false;
                window.close();
            }
        }
        window.clear(sf::Color(0, 0, 0, 255));
        display.draw(window);
        window.display();
    }
}

int main() {
    unsigned char data = 0xEA;
    unsigned short address = 0;
    unsigned short addressModified = 0;
    bool VPB = true; // Vector Pull
    bool RDY = true; // ready: run on high
    bool IRQB = true; // interupt: on low
    bool MLB = true; // memory lock
    bool NMIB = true; // non maskable interupt: on low
    bool SYNC = true; //??
    bool RWB = true; // read or write: high = read, low = write
    bool BE = true; // bus enable
    bool SOB = true;

    // versitale interface pins
    bool viaCS1 = true;
    bool viaCS2B = true;
    unsigned char viaPortA = 0;
    unsigned char viaPortB = 0;
    bool RS0; // register select
    bool RS1; // register select
    bool RS2; // register select
    bool RS3; // register select
    bool CA1; //??
    bool CA2; //??
    bool CB1; //??
    bool CB2; //??
    bool viaIRQB; // interupt?

    //ram Output enable on low
    bool ramOutputDisable;

    //display controll pins
    bool e, rw, rs;

    Eeprom32k eeprom(addressModified, data);
    std::array<unsigned char, 0x8000> binary;
    std::ifstream input("a.out", std::ios::binary);
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
    if (binary.size() != buffer.size()) {
        throw std::runtime_error("binary size wrong");
    }
    {
        int i = 0;
        for (auto c : buffer) {
            binary[i] = c;
            ++i;
        }
    }
    eeprom.program(binary);

    Cpu cpu(data, address, VPB, RDY, IRQB, MLB, NMIB, SYNC, RWB, BE, SOB);
    cpu.reset();

    VersatileInterfaceAdapter via(RWB, viaCS1, viaCS2B, data, viaPortA, viaPortB, RS0, RS1, RS2, RS3, CA1, CA2, CB1, CB2, viaIRQB);
    via.reset();

    Ram32k ram(address, data, ramOutputDisable, RWB);

    MiniLCD display(viaPortB, e, rw, rs);
    bool alive = true;
    std::jthread t(runDisplay, std::ref(display), std::ref(alive));

    auto start = std::chrono::steady_clock::now();
    while (alive) {
        cpu.cycle();

        viaCS1 = address & 0x2000;
        viaCS2B = !(address < 0x8000 && 0x4000 & address);
        RS0 = address & 0x0001;
        RS1 = address & 0x0002;
        RS2 = address & 0x0004;
        RS3 = address & 0x0008;
        via.cycle();

        addressModified = address ^ 0x8000; // to set eeprom to second memory half
        eeprom.cycle();

        ramOutputDisable = address & 0x4000;
        ram.cycle();

        e = viaPortA & 0x80;
        rw = viaPortA & 0x40;
        rs = viaPortA & 0x20;
        display.cycle();

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    auto end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "[ms]\n";
}