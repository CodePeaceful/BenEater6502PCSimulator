#include "VersatileInterfaceAdapter.hpp"

VersatileInterfaceAdapter::VersatileInterfaceAdapter(const bool& _RWB, const bool& _CS1, const bool& _CS2B,
    unsigned char& _dataPins, unsigned char& _portA, unsigned char& _portB,
    const bool& _RS0, const bool& _RS1, const bool& _RS2, const bool& _RS3,
    bool& _CA1, bool& _CA2, bool& _CB1, bool& _CB2, const bool& _IRQB) :
    RWB { _RWB }, CS1 { _CS1 }, CS2B { _CS2B }, dataPins { _dataPins }, portA { _portA }, portB { _portB },
    RS0 { _RS0 }, RS1 { _RS1 }, RS2 { _RS2 }, RS3 { _RS3 }, CA1 { _CA1 }, CA2 { _CA2 }, CB1 { _CB1 }, CB2 { _CB2 },
    IRQB { _IRQB } {}

void VersatileInterfaceAdapter::cycle() {
    if (!CS1 || CS2B)
        return; // not selected

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
                dataPins = portADataDirection;
                return;
            }
            portADataDirection = dataPins;
            return;
        }
        if (RS2) {
            if (RS3) {
                // Interrupt Flag Register
                // TODO: 
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
        if (RWB) {
            portARegister = portA;
            for (unsigned char i = 1; i != 0; i = i << 1) {
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
            return;
        }
        portARegister = dataPins;
        for (unsigned char i = 1; i != 0; i = i << 1) {
            if (i & portADataDirection) {
                if (i & portARegister) {
                    portA |= i;
                }
                else {
                    portA &= ~i;
                }
            }
        }
        return;
    }
    if (RS1) {
        if (RS2) {
            if (RS3) {
                // Interrupt Enable Register
                // TODO: 
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
            // TODO: 
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
    if (RWB) {
        portBRegister = portB;
        for (unsigned char i = 1; i != 0; i = i << 1) {
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
        return;
    }
    portBRegister = dataPins;
    for (unsigned char i = 1; i != 0; i = i << 1) {
        if (i & portBDataDirection) {
            if (i & portBRegister) {
                portB |= i;
            }
            else {
                portB &= ~i;
            }
        }
    }
    return;
}

void VersatileInterfaceAdapter::reset() {
    portADataDirection = 0;
    portBDataDirection = 0;
}
