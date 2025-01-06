#include "components/Cpu.hpp"
#include "components/Eeprom32k.hpp"
#include "components/VersitileInterfaceAdapter.hpp"
#include "assembler/Assembler.hpp"

#include <iostream>
#include <bitset>

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

    Eeprom32k eeprom(addressModified, data);
    eeprom.program(assemble<0x8000>("program.6502"));

    Cpu cpu(data, address, VPB, RDY, IRQB, MLB, NMIB, SYNC, RWB, BE, SOB);
    cpu.reset();
    VersitileInterfaceAdapter via(RWB, viaCS1, viaCS2B, data, viaPortA, viaPortB, RS0, RS1, RS2, RS3, CA1, CA2, CB1, CB2, viaIRQB);
    via.reset();
    while (true) {
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
        std::bitset<8> x(viaPortB);
        std::cout << x << '\n';
    }
}