#include "Cpu.hpp"

void Cpu::fetch() {
    IR = data;
    TCU = 0;
    ++programCounter;
    address = programCounter;
}

Cpu::Cpu(unsigned char& _data, unsigned short& _address, bool& _VPB, const bool& _RDY, const bool& _IRQB, bool& _MLB,
    const bool& _NMIB, bool& _SYNC, bool& _RWB, const bool& _BE, const bool& _SOB) :
    data { _data }, address { _address }, VPB { _VPB }, RDY { _RDY }, IRQB { _IRQB }, MLB { _MLB },
    NMIB { _NMIB }, SYNC { _SYNC }, RWB { _RWB }, BE { _BE }, SOB { _SOB } {}

void Cpu::cycle() {
    if (!RDY)
        return;
    if (resetTimer) {
        RWB = true;
        --resetTimer;
        if (resetTimer > 1) {
            address = 0xFFFC;
            return;
        }
        if (resetTimer == 1) {
            instructionBufferLow = data;
            ++address;
            return;
        }
        instructionBufferHigh = data;
        programCounter = instructionBufferHigh * 0x100 + instructionBufferLow;
        TCU = 0xFF;
        IR = 0x4C;
        address = programCounter;
        return;
    }
    ++TCU;
    switch (IR) {
    case 0xEA: // NOOP
        if (TCU >= implied) {
            return fetch();
        }
        return;
    case 0x4C:
    //TODO:
        return fetch();

    default:
        throw 0;
    }
}

void Cpu::reset() {
    resetTimer = 9;
    IR = 0x6C;
}
