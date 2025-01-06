#include "Cpu.hpp"

void Cpu::fetch() {
    IR = data;
    TCU = 0;
    ++programCounter;
    address = programCounter;
}

void Cpu::noop() {
    if (TCU == 1) {
        return;
    }
    fetch();
}

void Cpu::loadAImidiate() {
    if (TCU == 1) {
        A = data;
        ++programCounter;
        address = programCounter;
        return;
    }
    fetch();
}

void Cpu::storeAAtAbsoluteAddress() {
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
    if (TCU == 3) {
        RWB = true;
        address = programCounter;
        return;
    }
    fetch();
}

void Cpu::jumpAbsolute() {
    if (TCU == 1) {
        instructionBufferLow = data;
        ++programCounter;
        address = programCounter;
        return;
    }
    if (TCU == 2) {
        instructionBufferHigh = data;
        programCounter = instructionBufferHigh * 0x100 + instructionBufferLow;
        address = programCounter;
        return;
    }
    fetch();
}

void Cpu::rotateRightAccumulator() {
    if (TCU == 1) {
        bool rememberCarry = carry;
        if (A & 1) {
            carry = true;
        }
        else {
            carry = false;
        }
        A = A >> 1;
        if (rememberCarry) {
            A |= 0x80;
        }
        return;
    }
    fetch();
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
        if (resetTimer == 0) {
            programCounter = 0xFFFC;
            TCU = 0;
            address = programCounter;
            IR = 0x4C;
        }
        return;
    }
    ++TCU;
    switch (IR) {
    case 0xEA:
        noop();
        break;
    case 0xA9:
        loadAImidiate();
        break;
    case 0x8D:
        storeAAtAbsoluteAddress();
        break;
    case 0x4C: // jump absolute
        jumpAbsolute();
        break;
    case 0x6A: // rotate right accumulator
        rotateRightAccumulator();
        break;
    default:
        throw 0;
    }
}

void Cpu::reset() {
    resetTimer = 7;
    IR = 0x6C;
}
