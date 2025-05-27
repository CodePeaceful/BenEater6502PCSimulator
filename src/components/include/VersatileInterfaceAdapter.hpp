#pragma once

class VersatileInterfaceAdapter final
{
private:
    // pin references
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
    bool& CA1; // interrupting IO
    bool& CA2; // interrupting IO
    bool& CB1; // interrupting IO
    bool& CB2; // interrupting IO
    bool& IRQB; // interrupt out for Cpu
    const bool& PHI2;
    const bool& RESB;

    // internals
    unsigned char portADataDirection; // PA7 - PA0
    unsigned char portBDataDirection; // PB7 - PB0
    unsigned char portARegister; // PA7 - PA0
    unsigned char portBRegister; // PB7 - PB0
    unsigned char peripheralControlRegister;
    unsigned char interruptFlagRegister; // IRQ Timer1 Timer2 CB1 CB2 ShiftRegister CA1 CA2
    unsigned char interruptEnableRegister; // SetClear Timer1 Timer2 CB1 CB2 ShiftRegister CA1 CA2

    // edge detectors
    bool CA1before;
    bool CA2before;
    bool CB1before;
    bool CB2before;

    // helpers
    bool lastClockState{true};

    void registerOperation()noexcept;

public:
    VersatileInterfaceAdapter(const bool& _RWB, const bool& _CS1, const bool& _CS2B, unsigned char& _dataPins,
        unsigned char& _portA, unsigned char& _portB, const bool& _RS0, const bool& _RS1, const bool& _RS2, const bool& _RS3,
        bool& _CA1, bool& _CA2, bool& _CB1, bool& _CB2, bool& _IRQB, const bool& _PHI2, const bool& _RESB);
    void cycle();
    void reset();
};
