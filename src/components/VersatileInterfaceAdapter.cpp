#include "VersatileInterfaceAdapter.hpp"

#include "stdexcept"

void VersatileInterfaceAdapter::registerOperation() noexcept {
    if (RS0) {
        if (RS1) {
            if (RS2) {
                if (RS3) {
                    // IO A no "Handshake"
                    // TODO: 
                    return;
                }
                // T1 High-Order Latches
                // TODO: 
                return;
            }
            if (RS3) {
                // Auxiliary Control Register
                // TODO: 
                return;
            }
            // Data Direction A
            if (RWB) {
                if (!PHI2)
                    dataPins = portADataDirection;
                return;
            }
            if (PHI2)
                portADataDirection = dataPins;
            return;
        }
        if (RS2) {
            if (RS3) {
                // Interrupt Flag Register
                if (RWB) {
                    if (!PHI2)
                        dataPins = interruptFlagRegister;
                    return;
                }
                if (PHI2)
                    interruptFlagRegister = dataPins;
                return;
            }
            // T1 High-Order Counter
            // TODO: 
            return;
        }
        if (RS3) {
            // T2 High - Order Counter
            // TODO: 
            return;
        }
        // IO A
        interruptFlagRegister &= 0b11111101;
        if (peripheralControlRegister & 0x2) {
            interruptFlagRegister &= 0b11111110;
        }
        if (RWB) {
            if (!PHI2) {
                portARegister = portA;
                for (uint8_t i = 1; i != 0; i = i << 1) {
                    if (i & portADataDirection) {
                        // empty
                    }
                    else {
                        if (i & portARegister) {
                            dataPins |= i;
                        }
                        else {
                            dataPins &= ~i;
                        }
                    }
                }
            }
            return;
        }
        if (PHI2) {
            portARegister = dataPins;
            for (uint8_t i = 1; i != 0; i = i << 1) {
                if (i & portADataDirection) {
                    if (i & portARegister) {
                        portA |= i;
                    }
                    else {
                        portA &= ~i;
                    }
                }
            }
        }
        return;
    }
    if (RS1) {
        if (RS2) {
            if (RS3) {
                // Interrupt Enable Register
                if (RWB) {
                    if (!PHI2)
                        dataPins = interruptEnableRegister;
                    return;
                }
                if (PHI2)
                    interruptEnableRegister = dataPins;
                return;
            }
            // T1 Low-Order Latches
            // TODO:
            return;
        }
        if (RS3) {
            // Shift Register
            // TODO: 
            return;
        }
        // Data Direction B
        if (RWB) {
            dataPins = portBDataDirection;
            return;
        }
        portBDataDirection = dataPins;
        return;
    }
    if (RS2) {
        if (RS3) {
            // Peripheral Control Register
            if (RWB) {
                if (!PHI2)
                    dataPins = peripheralControlRegister;
                return;
            }
            if (PHI2)
                peripheralControlRegister = dataPins;
            return;
        }
        // T1 Low - Order Latches T1 Low - Order Counter
        // TODO: 
        return;
    }
    if (RS3) {
        // T2 Low - Order Latches T2 Low - Order Counter
        // TODO: 
        return;
    }
    // IO B
    interruptFlagRegister &= 0b11101111;
    if (peripheralControlRegister & 0x20) {
        interruptFlagRegister &= 0b11110111;
    }
    if (RWB) {
        if (!PHI2) {
            portBRegister = portB;
            for (uint8_t i = 1; i != 0; i = i << 1) {
                if (i & portBDataDirection) {
                    // empty
                }
                else {
                    if (i & portBRegister) {
                        dataPins |= i;
                    }
                    else {
                        dataPins &= ~i;
                    }
                }
            }
        }
        return;
    }
    if (PHI2) {
        portBRegister = dataPins;
        for (uint8_t i = 1; i != 0; i = i << 1) {
            if (i & portBDataDirection) {
                if (i & portBRegister) {
                    portB |= i;
                }
                else {
                    portB &= ~i;
                }
            }
        }
    }
    return;
}

VersatileInterfaceAdapter::VersatileInterfaceAdapter(const bool& _RWB, const bool& _CS1, const bool& _CS2B,
    uint8_t& _dataPins, uint8_t& _portA, uint8_t& _portB,
    const bool& _RS0, const bool& _RS1, const bool& _RS2, const bool& _RS3,
    bool& _CA1, bool& _CA2, bool& _CB1, bool& _CB2, bool& _IRQB, const bool& _PHI2, const bool& _RESB) :
    RWB{_RWB}, CS1{_CS1}, CS2B{_CS2B}, dataPins{_dataPins}, portA{_portA}, portB{_portB},
    RS0{_RS0}, RS1{_RS1}, RS2{_RS2}, RS3{_RS3}, CA1{_CA1}, CA2{_CA2}, CB1{_CB1}, CB2{_CB2},
    IRQB{_IRQB}, PHI2{_PHI2}, RESB{_RESB} { }

void VersatileInterfaceAdapter::cycle() {
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

    if (lastClockState == PHI2) {
        return;
    }
    lastClockState = PHI2;

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
}

void VersatileInterfaceAdapter::reset() {
    portADataDirection = 0;
    portBDataDirection = 0;
    peripheralControlRegister = 0;
    interruptEnableRegister = 0;
    interruptFlagRegister = 0;
    CA1before = CA1;
    CA2before = CA2;
    CB1before = CB1;
    CB2before = CB2;
}
