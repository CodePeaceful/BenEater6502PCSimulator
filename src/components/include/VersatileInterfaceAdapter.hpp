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
    const bool& CA1; // interupting IO
    const bool& CA2; // interupting IO
    const bool& CB1; // interupting IO
    const bool& CB2; // interupting IO
    bool& IRQB; // interupt out for Cpu
    const bool& PHI2;
    const bool& RESB;

    // internals
    unsigned char portADataDirection; // PA7 - PA0
    unsigned char portBDataDirection; // PB7 - PB0
    unsigned char portARegister; // PA7 - PA0
    unsigned char portBRegister; // PB7 - PB0
    unsigned char peripheralControlRegister;
    unsigned char interuptFlagRegister; // IRQ Timer1 Timer2 CB1 CB2 ShiftRegister CA1 CA2
    unsigned char interuptEnableRegister; // SetClear Timer1 Timer2 CB1 CB2 ShiftRegister CA1 CA2

    // edge detectors
    bool CA1before;
    bool CA2before;
    bool CB1before;
    bool CB2before;

    // helpers
    bool lastClockState{true};

public:
    VersatileInterfaceAdapter(const bool& _RWB, const bool& _CS1, const bool& _CS2B, unsigned char& _dataPins,
        unsigned char& _portA, unsigned char& _portB, const bool& _RS0, const bool& _RS1, const bool& _RS2, const bool& _RS3,
        bool& _CA1, bool& _CA2, bool& _CB1, bool& _CB2, bool& _IRQB, const bool& _PHI2, const bool& _RESB);
    void cycle();
    void reset();
};
