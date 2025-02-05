#pragma once

class VersatileInterfaceAdapter
{
private:
// pin referencens
    const bool& RWB; // readWrite to attach to Cpu
    const bool& CS1; // Chip select high
    const bool& CS2B; // Chip select low
    unsigned char& dataPins; // D7 - D0
    unsigned char& portA; // PA7 - PA0
    unsigned char& portB; // PB7 - PB0
    const bool& RS0; // register select
    const bool& RS1; // register select
    const bool& RS2; // register select
    const bool& RS3; // register select
    bool& CA1; //??
    bool& CA2; //??
    bool& CB1; //??
    bool& CB2; //??
    const bool& IRQB; // interupt?

// internals
    unsigned char portADataDirection; // PA7 - PA0
    unsigned char portBDataDirection; // PB7 - PB0
    unsigned char portARegister; // PA7 - PA0
    unsigned char portBRegister; // PB7 - PB0



public:
    VersatileInterfaceAdapter(const bool& _RWB, const bool& _CS1, const bool& _CS2B, unsigned char& _dataPins,
        unsigned char& _portA, unsigned char& _portB, const bool& _RS0, const bool& _RS1, const bool& _RS2, const bool& _RS3,
        bool& _CA1, bool& _CA2, bool& _CB1, bool& _CB2, const bool& _IRQB);
    void cycle();
    void reset();
};
