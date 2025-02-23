#pragma once

#include <filesystem>

#include <Cpu.hpp>
#include <Eeprom32k.hpp>
#include <VersatileInterfaceAdapter.hpp>
#include <Ram32k.hpp>
#include <MiniLCD.hpp>

class Computer
{
private:
// CPU Connections
    unsigned char data { 0xEA };
    unsigned short address { 0 };
    bool VPB { true }; // Vector Pull
    bool RDY { true }; // ready: run on high
    bool IRQB { true }; // interupt: on low
    bool MLB { true }; // memory lock
    bool NMIB { true }; // non maskable interupt: on low
    bool SYNC { true }; //??
    bool RWB { true }; // read or write: high = read, low = write
    bool BE { true }; // bus enable
    bool SOB { true };
    bool PHI2 { true };
    bool PHI2O;
    bool PHI1O;

// modified for rom
    unsigned short addressModifiedRom = 0;

// modified for ram
    unsigned short addressModifiedRam { 0 };
    bool ramOutputDisable;

// modified Cpu pins for versitale interface adapter
    bool RS0; // register select
    bool RS1; // register select
    bool RS2; // register select
    bool RS3; // register select
    bool viaCS1;
    bool viaCS2B;

// External pins of versitale interface adapter
    unsigned char viaPortA { 0 };
    unsigned char viaPortB { 0 };
    bool CA1; //??
    bool CA2; //??
    bool CB1; //??
    bool CB2; //??

// display control pins
    bool e, rw, rs;

// magic cheating
    bool alive { true };

// hardware
    Cpu cpu;
    Eeprom32k rom;
    Ram32k ram;
    VersatileInterfaceAdapter via;
    MiniLCD screen;

public:
    Computer();
    void reprogram(std::filesystem::path binary32k);
    void run();

private:
// enables visuals sets control flags
    void display();

};