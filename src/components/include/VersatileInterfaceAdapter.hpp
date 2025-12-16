#pragma once
#include <cstdint>

namespace components
{
namespace logger
{
class VIALogger;
};
class VersatileInterfaceAdapter final
{
    friend class logger::VIALogger;
private:
    // pin references
    const bool& RWB; // readWrite to attach to Cpu
    const bool& CS1; // Chip select high
    const bool& CS2B; // Chip select low
    uint8_t& dataPins; // D7 - D0
    uint8_t& portA; // PA7 - PA0
    uint8_t& portB; // PB7 - PB0
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
    uint8_t portADataDirection; // PA7 - PA0
    uint8_t portBDataDirection; // PB7 - PB0
    uint8_t portAInputRegister; // PA7 - PA0
    uint8_t portAOutputRegister;// PA7 - PA0
    uint8_t portBInputRegister; // PB7 - PB0
    uint8_t portBOutputRegister;// PB7 - PB0
    uint8_t peripheralControlRegister;
    uint8_t auxiliaryControlRegister;
    uint8_t interruptFlagRegister; // IRQ Timer1 Timer2 CB1 CB2 ShiftRegister CA1 CA2
    uint8_t interruptEnableRegister; // SetClear Timer1 Timer2 CB1 CB2 ShiftRegister CA1 CA2

    // edge detectors
    bool CA1before;
    bool CA2before;
    bool CB1before;
    bool CB2before;

    // helpers
    bool lastClockState{true};
    uint8_t address; // internal address bus for register selection

    void registerOperation()noexcept;

public:
    VersatileInterfaceAdapter(const bool& _RWB, const bool& _CS1, const bool& _CS2B, uint8_t& _dataPins,
        uint8_t& _portA, uint8_t& _portB, const bool& _RS0, const bool& _RS1, const bool& _RS2, const bool& _RS3,
        bool& _CA1, bool& _CA2, bool& _CB1, bool& _CB2, bool& _IRQB, const bool& _PHI2, const bool& _RESB);
    void cycle();
    void reset();
};
} // namespace components