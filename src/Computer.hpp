#pragma once

#include <filesystem>

#include <Cpu.hpp>
#include <Eeprom32k.hpp>
#include <VersatileInterfaceAdapter.hpp>
#include <Ram32k.hpp>
#include <MiniLCD.hpp>

class Computer final
{
private:
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
    bool BE{true}; // bus enable
    bool SOB{true};
    bool PHI2{true};
    bool PHI2O;
    bool PHI1O;
    bool RESB{true};

    // modified for rom
    uint16_t addressModifiedRom{0};

    // modified for ram
    uint16_t addressModifiedRam{0};
    bool ramOutputDisable;

    // modified Cpu pins for versatile interface adapter
    bool RS0; // register select
    bool RS1; // register select
    bool RS2; // register select
    bool RS3; // register select
    bool viaCS1;
    bool viaCS2B;

    // External pins of versatile interface adapter
    uint8_t viaPortA{0};
    uint8_t viaPortB{0};
    bool CA1; //??
    bool CA2; //??
    bool CB1; //??
    bool CB2; //??

    // display control pins
    bool e, rw, rs;

    // magic cheating
    bool alive{true};

    // hardware
    Cpu cpu;
    Eeprom32k rom;
    Ram32k ram;
    VersatileInterfaceAdapter via;
    MiniLCD screen;

public:
    Computer();
    void reprogram(const std::filesystem::path& binary32k);
    void run();

private:
    // enables visuals sets control flags
    void display();
};