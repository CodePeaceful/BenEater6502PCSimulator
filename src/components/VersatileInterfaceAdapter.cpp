#include "VersatileInterfaceAdapter.hpp"

#include <stdexcept>
#include <utility>

void VersatileInterfaceAdapter::registerOperation() noexcept {

    address = (RS3 << 3) | (RS2 << 2) | (RS1 << 1) | RS0;

    switch (address) {
    case 0:
        // IO B
        interruptFlagRegister &= 0b11101111;
        if (peripheralControlRegister & 0x20) {
            interruptFlagRegister &= 0b11110111;
        }
        if (RWB) {
            if (!PHI2) {
                portBInputRegister = portB;
                // happens any way
                dataPins = portBInputRegister;
                if (auxiliaryControlRegister & 0x2) {
                    // empty
                }
                else {
                    for (uint8_t i = 1; i != 0; i = i << 1) {
                        if (i & portBDataDirection) {
                            // empty 
                        }
                        else {
                            if (i & portBInputRegister) {
                                dataPins |= i;
                            }
                            else {
                                dataPins &= ~i;
                            }
                        }
                    }
                }
            }
            return;
        }
        if (PHI2 && lastClockState) {
            portBOutputRegister = dataPins;

        }
        return;
    case 1:
        // IO A
        interruptFlagRegister &= 0b11111101;
        if (peripheralControlRegister & 0x2) {
            interruptFlagRegister &= 0b11111110;
        }
        if (RWB) {
            if (PHI2) {
                if (auxiliaryControlRegister & 0x1) {
                    dataPins = portAInputRegister;
                }
                else {
                    dataPins = portA;
                }
                return;
            }
        }
        if (PHI2 && lastClockState) {
            portAOutputRegister = dataPins;
        }
        return;
    case 2:
        // Data Direction B
        if (RWB) {
            if (PHI2) {
                dataPins = portBDataDirection;
            }
            return;
        }
        if (PHI2 && lastClockState) {
            portBDataDirection = dataPins;
        }
        return;
    case 3:
        // Data Direction A
        if (RWB) {
            if (PHI2) {
                dataPins = portADataDirection;
            }
            return;
        }
        if (PHI2 && lastClockState) {
            portADataDirection = dataPins;
        }
        return;
    case 4:
        // T1 Low - Order Latches T1 Low - Order Counter
        // TODO: 
        break;
    case 5:
        // T1 High-Order Counter
        // TODO: 
        break;
    case 6:
        // T1 Low-Order Latches
        // TODO:
        break;
    case 7:
        // T1 High-Order Latches
        // TODO: 
        break;
    case 8:
        // T2 Low - Order Latches T2 Low - Order Counter
        // TODO: 
        break;
    case 9:
        // T2 High - Order Counter
        // TODO: 
        break;
    case 10:
        // Shift Register
        // TODO: 
        break;
    case 11:
        // Auxiliary Control Register
        // TODO: 
        break;
    case 12:
        // Peripheral Control Register
        if (RWB) {
            if (PHI2)
                dataPins = peripheralControlRegister;
            return;
        }
        if (PHI2 && lastClockState)
            peripheralControlRegister = dataPins;
        return;
    case 13:
        // Interrupt Flag Register
        if (RWB) {
            if (PHI2)
                dataPins = interruptFlagRegister;
            return;
        }
        if (PHI2 && lastClockState)
            interruptFlagRegister = dataPins;
        return;
    case 14:
        // Interrupt Enable Register
        if (RWB) {
            if (PHI2)
                dataPins = interruptEnableRegister;
            return;
        }
        if (PHI2 && lastClockState)
            interruptEnableRegister = dataPins;
        return;
    case 15:
        // IO A no "Handshake"
        // TODO: 
        break;
    default:
        std::unreachable();
        break;
    }
}

VersatileInterfaceAdapter::VersatileInterfaceAdapter(const bool& _RWB, const bool& _CS1, const bool& _CS2B,
    uint8_t& _dataPins, uint8_t& _portA, uint8_t& _portB,
    const bool& _RS0, const bool& _RS1, const bool& _RS2, const bool& _RS3,
    bool& _CA1, bool& _CA2, bool& _CB1, bool& _CB2, bool& _IRQB, const bool& _PHI2, const bool& _RESB) :
    RWB{_RWB}, CS1{_CS1}, CS2B{_CS2B}, dataPins{_dataPins}, portA{_portA}, portB{_portB},
    RS0{_RS0}, RS1{_RS1}, RS2{_RS2}, RS3{_RS3}, CA1{_CA1}, CA2{_CA2}, CB1{_CB1}, CB2{_CB2},
    IRQB{_IRQB}, PHI2{_PHI2}, RESB{_RESB} { }

void VersatileInterfaceAdapter::cycle() {
    // update port A outputs
    for (uint8_t i = 1; i != 0; i = i << 1) {
        if (i & portADataDirection) {
            if (i & portAOutputRegister) {
                portA |= i;
            }
            else {
                portA &= ~i;
            }
        }
    }

    // update port B outputs
    for (uint8_t i = 1; i != 0; i = i << 1) {
        if (i & portBDataDirection) {
            if (i & portBOutputRegister) {
                portB |= i;
            }
            else {
                portB &= ~i;
            }
        }
    }

    // TODO: external interrupts over interrupting pins
    // CA1 check
    if (interruptEnableRegister & 0x2) {
        if (peripheralControlRegister & 0x1) {
            if (!CA1before && CA1) {
                interruptFlagRegister |= 0x2;
            }
        }
        else if (CA1before && !CA1) {
            interruptFlagRegister |= 0x2;
        }
    }
    if (!CA1before && CA1) {
        portAInputRegister = portA;
    }
    // CB1 check
    if (interruptEnableRegister & 0x10) {
        if (peripheralControlRegister & 0x10) {
            if (!CB1before && CB1) {
                interruptFlagRegister |= 0x10;
            }
        }
        else if (CB1before && !CB1) {
            interruptFlagRegister |= 0x10;
        }
    }
    if (!CB1before && CB1) {
        portBInputRegister = portB;
    }
    // CA2 check
    if (interruptEnableRegister & 0x1) {
        if (peripheralControlRegister & 0x8) {
            throw std::runtime_error("CA2 output not implemented");
        }
        else if (peripheralControlRegister & 0x4) {
            if (!CA2before && CA2) {
                interruptFlagRegister |= 0x1;
            }
        }
        else {
            if (CA2before && !CA2) {
                interruptFlagRegister |= 0x1;
            }
        }
    }
    // CB2 check
    if (interruptEnableRegister & 0x8) {
        if (peripheralControlRegister & 0x80) {
            throw std::runtime_error("CB2 output not implemented");
        }
        else if (peripheralControlRegister & 0x40) {
            if (!CB2before && CB2) {
                interruptFlagRegister |= 0x8;
            }
        }
        else {
            if (CB2before && !CB2) {
                interruptFlagRegister |= 0x8;
            }
        }
    }

    CA1before = CA1;
    CA2before = CA2;
    CB1before = CB1;
    CB2before = CB2;


    if (!RESB && !PHI2) {
        reset();
        return;
    }

    //TODO: timer?

    if (interruptFlagRegister & 0x7f) {
        interruptFlagRegister |= 0x80;
        IRQB = false;
    }
    else {
        interruptFlagRegister &= 0x7f;
        IRQB = true;
    }

    if (!CS1 || CS2B)
        return; // not selected

    registerOperation();

    lastClockState = PHI2;
}

void VersatileInterfaceAdapter::reset() {
    portADataDirection = 0;
    portAInputRegister = 0;
    portAOutputRegister = 0;
    portBDataDirection = 0;
    portBInputRegister = 0;
    portBOutputRegister = 0;
    peripheralControlRegister = 0;
    interruptEnableRegister = 0;
    interruptFlagRegister = 0;
    auxiliaryControlRegister = 0;
    CA1before = CA1;
    CA2before = CA2;
    CB1before = CB1;
    CB2before = CB2;
}
