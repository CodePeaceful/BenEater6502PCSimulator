#include "Cpu.hpp"

void Cpu::fetch() {
    IR = data;
    TCU = 0;
    ++programCounter;
    address = programCounter;
}

void Cpu::jumpToSubroutine() {
    /*
    1 read address half
    2 goto stack
    3 write big end to stack
    4 write little end to stack
    5 read address second half
    6 run subroutine
    */
    if (TCU == 1) {
        instructionBufferLow = data;
        ++programCounter;
        return;
    }
    if (TCU == 2) {
        address = 0x100 + stackPointer;
        RWB = false;
        data = programCounter >> 8;
        --stackPointer;
        return;
    }
    if (TCU == 3) {
        address = 0x100 + stackPointer;
        data = programCounter & 0xff;
        --stackPointer;
        return;
    }
    if (TCU == 4) {
        RWB = true;
        address = programCounter;
        return;
    }
    if (TCU == 5) {
        instructionBufferHigh = data;
        programCounter = instructionBufferHigh * 0x100 + instructionBufferLow;
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

void Cpu::returnFromSubroutine() {
    /*
    1 wait
    2 goto stack
    3 read
    4 read
    5 continue normal operation
    */
    if (TCU == 1) {
        return;
    }
    if (TCU == 2) {
        ++stackPointer;
        address = 0x100 + stackPointer;
        return;
    }
    if (TCU == 3) {
        instructionBufferLow = data;
        ++stackPointer;
        address = 0x100 + stackPointer;
        return;
    }
    if (TCU == 4) {
        instructionBufferHigh = data;
        programCounter = instructionBufferHigh * 0x100 + instructionBufferLow + 1;
        address = programCounter;
        return;
    }
    fetch();
}

void Cpu::rotateRightAccumulator() {
    if (TCU == 1) {
        bool rememberCarry = prozessorStatus & 1;
        if (A & 1) {
            prozessorStatus |= 1;
        }
        else {
            prozessorStatus &= 0b11111110;
        }
        A = A >> 1;
        if (rememberCarry) {
            A |= 0x80;
        }
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

void Cpu::transferXToStackpointer() {
    if (TCU == 1) {
        stackPointer = X;
        return;
    }
    fetch();
}

void Cpu::loadXImidiate() {
    if (TCU == 1) {
        X = data;
        prozessorStatus &= 0b01111101; // clear flags n and z
        ++programCounter;
        address = programCounter;
        return;
    }
    fetch();
}

void Cpu::loadAImidiate() {
    if (TCU == 1) {
        A = data;
        prozessorStatus &= 0b01111101; // clear flags n and z
        ++programCounter;
        address = programCounter;
        return;
    }
    fetch();
}

void Cpu::noop() {
    if (TCU == 1) {
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
    case 0x20:
        jumpToSubroutine();
        break;
    case 0x4C:
        jumpAbsolute();
        break;
    case 0x60:
        returnFromSubroutine();
        break;
    case 0x6A:
        rotateRightAccumulator();
        break;
    case 0x8D:
        storeAAtAbsoluteAddress();
        break;
    case 0x9A:
        transferXToStackpointer();
        break;
    case 0xA2:
        loadXImidiate();
        break;
    case 0xA9:
        loadAImidiate();
        break;
    case 0xEA:
        noop();
        break;
    default:
        throw IR;
    }
}

void Cpu::reset() {
    resetTimer = 7;
    IR = 0x6C;
}
