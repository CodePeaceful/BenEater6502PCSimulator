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
        TCU = absolute;
        IR = 0x4C;
        address = programCounter;
        return;
    }
    ++TCU;
    switch (IR) {
    case 0xEA: // NOP
        if (TCU >= implied) {
            return fetch();
        }
        return;
    case 0xA9: // load a imidiate
        if (TCU >= imidiate) {
            return fetch();
        }
        A = data;
        ++programCounter;
        address = programCounter;
        return;
    case 0x8D: // store A at address
        if (TCU >= absolute) {
            return fetch();
        }
        if (TCU == 1) {
            instructionBufferLow = data;
            ++programCounter;
            address = programCounter;
            return;
        }
        if (TCU == 2) {
            instructionBufferHigh = data;
            ++programCounter;
            address = instructionBufferHigh * 0x100 + instructionBufferLow;
            RWB = false;
            data = A;
            return;
        }
        RWB = true;
        address = programCounter;
        return;
    case 0x4C:
        if (TCU >= absolute - 1) { // hardware is strange
            return fetch();
        }
        if (TCU == 1) {
            instructionBufferLow = data;
            ++programCounter;
            address = programCounter;
            return;
        }
        instructionBufferHigh = data;
        programCounter = instructionBufferHigh * 0x100 + instructionBufferLow;
        address = programCounter;
        return;

    default:
        throw 0;
    }
}

void Cpu::reset() {
    resetTimer = 9;
    IR = 0x6C;
}
