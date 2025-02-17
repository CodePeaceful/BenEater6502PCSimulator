#include "Cpu.hpp"

#include <stdexcept>

void Cpu::fetch() {
    if (!PHI2) {
        addressBuffer = programCounter;
        readWriteBuffer = true;
        return;
    }
    TCU = 0;
    if (interuptRequested) {
        handleInterupt();
        return;
    }
    IR = dataBuffer;
    ++programCounter;
}

void Cpu::toBus() {
    if (BE) {
        address = addressBuffer;
        RWB = readWriteBuffer;
        if (!readWriteBuffer) {
            data = dataBuffer;
        }
    }
}

void Cpu::handleInterupt() {
    throw std::runtime_error("interupts not implementet");
}

void Cpu::negativeZeroCheck(unsigned char value) {
    if (value == 0) {
        prozessorStatus |= 0b00000010;
    }
    else {
        prozessorStatus &= 0b11111101;
    }
    if (value & 0x80) {
        prozessorStatus |= 0b10000000;
    }
    else {
        prozessorStatus &= 0b01111111;
    }
}

void Cpu::push(unsigned char data) {
    addressBuffer = 0x100 + stackPointer;
    readWriteBuffer = false;
    dataBuffer = data;
    --stackPointer;
}

void Cpu::pull() {
    ++stackPointer;
    addressBuffer = 0x100 + stackPointer;
}

void Cpu::zeroPage() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return;
        }
        instructionBufferLow = dataBuffer;
        ++programCounter;
        return;
    }
    if (TCU == 2) {
        if (!PHI2) {
            addressBuffer = instructionBufferLow;
            return;
        }
    }
}

void Cpu::zeroPageIndirect() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return;
        }
        instructionBufferLow = dataBuffer;
        ++programCounter;
        return;
    }
    if (TCU == 2) {
        if (!PHI2) {
            addressBuffer = instructionBufferLow;
            return;
        }
        instructionBufferLow = dataBuffer;
        return;
    }
    if (TCU == 3) {
        if (!PHI2) {
            ++addressBuffer;
            addressBuffer %= 256;
            return;
        }
        instructionBufferHigh = dataBuffer;
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            addressBuffer = instructionBufferHigh * 0x100 + instructionBufferLow;
            return;
        }
    }
}

void Cpu::zeroPageWithXIndirect() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return;
        }
        instructionBufferLow = dataBuffer;
        ++programCounter;
        return;
    }
    if (TCU == 2) {
        if (!PHI2) {
            instructionBufferLow += X;
        }
        return;
    }
    if (TCU == 3) {
        if (!PHI2) {
            addressBuffer = instructionBufferLow;
            return;
        }
        instructionBufferLow = dataBuffer;
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            ++addressBuffer;
            addressBuffer %= 256;
            return;
        }
        instructionBufferHigh = dataBuffer;
        return;
    }
    if (TCU == 5) {
        if (!PHI2) {
            addressBuffer = instructionBufferHigh * 0x100 + instructionBufferLow;
            return;
        }
    }
}

void Cpu::zeroPageIndirectWithY() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return;
        }
        instructionBufferLow = dataBuffer;
        ++programCounter;
        return;
    }
    if (TCU == 2) {
        if (!PHI2) {
            addressBuffer = instructionBufferLow;
            return;
        }
        instructionBufferLow = dataBuffer;
        return;
    }
    if (TCU == 3) {
        if (!PHI2) {
            ++addressBuffer;
            addressBuffer %= 256;
            return;
        }
        instructionBufferHigh = dataBuffer;
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            addressBuffer = instructionBufferHigh * 0x100 + instructionBufferLow + Y;
            return;
        }
    }
}

void Cpu::zeroPageWithX() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return;
        }
        instructionBufferLow = dataBuffer;
        ++programCounter;
        return;
    }
    if (TCU == 2) {
        if (!PHI2) {
            instructionBufferLow += X;
            return;
        }
        return;
    }
    if (TCU == 3) {
        if (!PHI2) {
            addressBuffer = instructionBufferLow;
            return;
        }
        return;
    }
}

void Cpu::zeroPageWithY() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return;
        }
        instructionBufferLow = dataBuffer;
        ++programCounter;
        return;
    }
    if (TCU == 2) {
        if (!PHI2) {
            instructionBufferLow += Y;
            return;
        }
        return;
    }
    if (TCU == 3) {
        if (!PHI2) {
            addressBuffer = instructionBufferLow;
            return;
        }
        return;
    }
}

void Cpu::absoluteIndirect() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return;
        }
        instructionBufferLow = dataBuffer;
        ++programCounter;
        return;
    }
    if (TCU == 2) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return;
        }
        instructionBufferHigh = dataBuffer;
        ++programCounter;
        return;
    }
    if (TCU == 3) {
        if (!PHI2) {
            addressBuffer = instructionBufferHigh * 0x100 + instructionBufferLow;
            return;
        }
        instructionBufferLow = dataBuffer;
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            ++addressBuffer;
            return;
        }
        instructionBufferHigh = dataBuffer;
        return;
    }
    if (TCU == 5) {
        if (!PHI2) {
            addressBuffer = instructionBufferHigh * 0x100 + instructionBufferLow;
            return;
        }
    }

}

void Cpu::absolute() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return;
        }
        instructionBufferLow = dataBuffer;
        ++programCounter;
        return;
    }
    if (TCU == 2) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return;
        }
        instructionBufferHigh = dataBuffer;
        ++programCounter;
        return;
    }
    if (TCU == 3) {
        if (!PHI2) {
            addressBuffer = instructionBufferHigh * 0x100 + instructionBufferLow;
            return;
        }
    }
}

void Cpu::absoluteWithX() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return;
        }
        instructionBufferLow = dataBuffer;
        ++programCounter;
        return;
    }
    if (TCU == 2) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return;
        }
        instructionBufferHigh = dataBuffer;
        ++programCounter;
        return;
    }
    if (TCU == 3) {
        if (!PHI2) {
            addressBuffer = instructionBufferHigh * 0x100 + instructionBufferLow + X;
            return;
        }
    }
}

void Cpu::absoluteWithY() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return;
        }
        instructionBufferLow = dataBuffer;
        ++programCounter;
        return;
    }
    if (TCU == 2) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return;
        }
        instructionBufferHigh = dataBuffer;
        ++programCounter;
        return;
    }
    if (TCU == 3) {
        if (!PHI2) {
            addressBuffer = instructionBufferHigh * 0x100 + instructionBufferLow + Y;
            return;
        }
    }
}

void Cpu::breakRequest() {
    throw std::runtime_error("no break");
}

void Cpu::orWithZeroPageWithXIndirect() {
    if (TCU <= 5) {
        zeroPageWithXIndirect();
        if (TCU == 5 && PHI2) {
            A |= dataBuffer;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::testAndSetMemoryBitZeroPage() {
    if (TCU <= 2) {
        zeroPage();
        if (TCU == 2 && PHI2) {
            instructionBufferLow = dataBuffer;
        }
        return;
    }
    if (TCU == 3) {
        if (!PHI2) {
            instructionBufferLow = testAndSetMemoryBit(instructionBufferLow);
        }
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
        }
    }
    fetch();
}

void Cpu::orWithZeroPage() {
    if (TCU <= 2) {
        zeroPage();
        if (TCU == 2 && PHI2) {
            A |= dataBuffer;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}


void Cpu::shiftLeftZeroPage() {
    if (TCU <= 2) {
        zeroPage();
        if (TCU == 2 && PHI2) {
            instructionBufferLow = dataBuffer;
        }
        return;
    }
    if (TCU == 3) {
        if (!PHI2) {
            instructionBufferLow = shiftLeft(instructionBufferLow);
        }
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
        }
        return;
    }
    fetch();
}

void Cpu::resetMemoryBit0() {
    resetMemoryBit(0);
}

void Cpu::pushProzessorStatus() {
    if (TCU == 1) {
        addressBuffer = 0x100 + stackPointer;
        return;
    }
    if (TCU == 2) {
        if (!PHI2) {
            push(prozessorStatus);
        }
        return;
    }
    fetch();
}

void Cpu::orWithImmediate() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            ++programCounter;
            return;
        }
        A |= dataBuffer;
        negativeZeroCheck(A);
        return;
    }
    fetch();
}

void Cpu::shiftLeftA() {
    if (TCU == 1) {
        if (!PHI2) {
            A = shiftLeft(A);
        }
        return;
    }
    fetch();
}

void Cpu::testAndSetMemoryBitAbsolute() {
    if (TCU <= 3) {
        absolute();
        if (PHI2 && TCU == 3) {
            instructionBufferLow = dataBuffer;
        }
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            instructionBufferLow = testAndResetMemoryBit(instructionBufferLow);
        }
        return;
    }
    if (TCU == 5) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
        }
        return;
    }
    fetch();
}

void Cpu::orWithAbsolute() {
    if (TCU <= 3) {
        absolute();
        if (PHI2 && TCU == 3) {
            A |= dataBuffer;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::shiftLeftAbsolute() {
    if (TCU <= 3) {
        absolute();
        if (PHI2 && TCU == 3) {
            instructionBufferLow = dataBuffer;
        }
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            instructionBufferLow = shiftLeft(instructionBufferLow);
        }
        return;
    }
    if (TCU == 5) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
        }
        return;
    }
    fetch();
}

void Cpu::branchOnBitReset0() {
    branchOnBitReset(0);
}

void Cpu::branchIfResultPlus() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            ++programCounter;
            return;
        }
        instructionBufferLow = dataBuffer;
    }
    if (TCU == 2) {
        if (prozessorStatus & 0x80) {
            // empty
        }
        else {
            programCounter += instructionBufferLow;
            return;
        }
    }
    fetch();
}

void Cpu::orWithZeroPageIndirectWithY() {
    if (TCU <= 4) {
        zeroPageIndirectWithY();
        if (PHI2 && TCU == 4) {
            A |= dataBuffer;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::orWithZeroPageIndirect() {
    if (TCU <= 4) {
        zeroPageIndirect();
        if (PHI2 && TCU == 4) {
            A |= dataBuffer;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::testAndResetMemoryBitZeroPage() {
    if (TCU <= 2) {
        zeroPage();
        if (TCU == 2 && PHI2) {
            instructionBufferLow = dataBuffer;
        }
        return;
    }
    if (TCU == 3) {
        if (!PHI2) {
            instructionBufferLow = testAndResetMemoryBit(instructionBufferLow);
        }
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
        }
    }
    fetch();
}

void Cpu::orWithZeroPageWithX() {
    if (TCU <= 3) {
        zeroPageWithX();
        if (PHI2 && TCU == 3) {
            A |= dataBuffer;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::shiftLeftZeroPageWithX() {
    if (TCU <= 3) {
        zeroPageWithX();
        if (PHI2 && TCU == 3) {
            instructionBufferLow = dataBuffer;
        }
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            instructionBufferLow = shiftLeft(instructionBufferLow);
        }
        return;
    }
    if (TCU == 5) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
        }
        return;
    }
    fetch();
}

void Cpu::resetMemoryBit1() {
    resetMemoryBit(1);
}

void Cpu::clearCarry() {
    if (TCU == 1) {
        if (!PHI2) {
            prozessorStatus &= 0b11111110;
        }
        return;
    }
    fetch();
}

void Cpu::orWithAbsoluteWithY() {
    if (TCU <= 3) {
        absoluteWithY();
        if (TCU == 3 && PHI2) {
            A |= dataBuffer;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::incrementA() {
    if (TCU == 1) {
        if (!PHI2) {
            ++A;
        }
        return;
    }
    fetch();
}

void Cpu::testAndResetMemoryBitAbsolute() {
    if (TCU <= 3) {
        absolute();
        if (TCU == 3 && PHI2) {
            instructionBufferLow = dataBuffer;
        }
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            instructionBufferLow = testAndResetMemoryBit(instructionBufferLow);
        }
        return;
    }
    if (TCU == 5) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
        }
        return;
    }
    fetch();
}

void Cpu::orWithAbsoluteWithX() {
    if (TCU <= 3) {
        absoluteWithX();
        if (TCU == 3 && PHI2) {
            A |= dataBuffer;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::shiftLeftAbsoluteWithX() {
    if (TCU <= 3) {
        absoluteWithX();
        if (TCU == 3 && PHI2) {
            instructionBufferLow = dataBuffer;
        }
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            instructionBufferLow = shiftLeft(instructionBufferLow);
        }
        return;
    }
    if (TCU == 5) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
        }
        return;
    }
    fetch();
}

void Cpu::branchOnBitReset1() {
    branchOnBitReset(1);
}

void Cpu::jumpToSubroutine() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return;
        }
        instructionBufferLow = dataBuffer;
        ++programCounter;
        return;
    }
    if (TCU == 2) {
        if (!PHI2) {
            addressBuffer = 0x100 + stackPointer;
            return;
        }
        return;
    }
    if (TCU == 3) {
        if (!PHI2) {
            push(programCounter >> 8);
            return;
        }
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            push(programCounter & 0xff);
            return;
        }
        return;
    }
    if (TCU == 5) {
        if (!PHI2) {
            readWriteBuffer = true;
            addressBuffer = programCounter;
            return;
        }
        instructionBufferHigh = dataBuffer;
        programCounter = instructionBufferHigh * 0x100 + instructionBufferLow;
        return;
    }
    fetch();
}

void Cpu::andWithZeroPageWithXIndirect() {
    if (TCU <= 5) {
        zeroPageWithXIndirect();
        if (TCU == 5 && PHI2) {
            A &= dataBuffer;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::bitTestWithZeroPage() {
    if (TCU <= 2) {
        zeroPage();
        if (TCU == 2 && PHI2) {
            bitTest(dataBuffer);
        }
        return;
    }
    fetch();
}

void Cpu::andWithZeroPage() {
    if (TCU <= 2) {
        zeroPage();
        if (TCU == 2 && PHI2) {
            A &= dataBuffer;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::rotateLeftZeroPage() {
    if (TCU <= 2) {
        zeroPage();
        if (TCU == 2 && PHI2) {
            instructionBufferLow = dataBuffer;
        }
        return;
    }
    if (TCU == 3) {
        if (!PHI2) {
            instructionBufferLow = rotateLeft(instructionBufferLow);
        }
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
        }
        return;
    }
    fetch();
}

void Cpu::resetMemoryBit2() {
    resetMemoryBit(2);
}

void Cpu::pullProcessorStatus() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = 0x100 + stackPointer;
        }
        return;
    }
    if (TCU == 2) {
        if (!PHI2) {
            pull();
            return;
        }
        prozessorStatus = dataBuffer;
        return;
    }
    fetch();
}

void Cpu::andWithImediate() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            ++programCounter;
            return;
        }
        A &= dataBuffer;
        negativeZeroCheck(A);
        return;
    }
    fetch();
}

void Cpu::rotateLeftA() {
    if (TCU == 1) {
        if (!PHI2) {
            A = rotateLeft(A);
        }
        return;
    }
    fetch();
}

void Cpu::bitTestWithAbsolute() {
    if (TCU <= 3) {
        absolute();
        if (TCU == 3 && PHI2) {
            bitTest(dataBuffer);
        }
        return;
    }
    fetch();
}

void Cpu::andWithAbsolute() {
    if (TCU <= 3) {
        absolute();
        if (TCU == 3 && PHI2) {
            A &= dataBuffer;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::rotateLeftAbsolute() {
    if (TCU <= 3) {
        absolute();
        if (TCU == 3 && PHI2) {
            instructionBufferLow = dataBuffer;
        }
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            instructionBufferLow = rotateLeft(instructionBufferLow);
        }
        return;
    }
    if (TCU == 5) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
        }
        return;
    }
    fetch();
}

void Cpu::branchOnBitReset2() {
    branchOnBitReset(2);
}

void Cpu::branchIfResultMinus() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            ++programCounter;
            return;
        }
        instructionBufferLow = dataBuffer;
    }
    if (TCU == 2) {
        if (prozessorStatus & 0x80) {
            programCounter += instructionBufferLow;
            return;
        }
    }
    fetch();
}

void Cpu::andWithZeroPageIndirectWithY() {
    if (TCU <= 4) {
        zeroPageIndirectWithY();
        if (TCU == 4 && PHI2) {
            A &= dataBuffer;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::andWithZeroPageIndirect() {
    if (TCU <= 4) {
        zeroPageIndirect();
        if (TCU == 4 && PHI2) {
            A &= dataBuffer;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::bitTestWithZeroPageWithX() {
    if (TCU <= 3) {
        zeroPageWithX();
        if (TCU == 3 && PHI2) {
            bitTest(dataBuffer);
        }
        return;
    }
    fetch();
}

void Cpu::andWithZeroPageWithX() {
    if (TCU <= 3) {
        zeroPageWithX();
        if (TCU == 3 && PHI2) {
            A &= dataBuffer;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::rotateLeftZeroPageWithX() {
    if (TCU <= 3) {
        zeroPageWithX();
        if (TCU == 3 && PHI2) {
            instructionBufferLow = dataBuffer;
        }
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            instructionBufferLow = rotateLeft(instructionBufferLow);
        }
        return;
    }
    if (TCU == 5) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
        }
        return;
    }
    fetch();
}

void Cpu::resetMemoryBit3() {
    resetMemoryBit(3);
}

void Cpu::setCarry() {
    if (TCU == 1) {
        if (!PHI2) {
            prozessorStatus |= 0x1;
        }
        return;
    }
    fetch();
}

void Cpu::andWithAbsoluteWithY() {
    if (TCU <= 3) {
        absoluteWithY();
        if (TCU == 3 && PHI2) {
            A &= dataBuffer;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::decrementA() {
    if (TCU == 1) {
        if (!PHI2) {
            --A;
        }
        return;
    }
    fetch();
}

void Cpu::bitTestWithAbsoluteWithX() {
    if (TCU <= 3) {
        absoluteWithX();
        if (TCU == 3 && PHI2) {
            bitTest(dataBuffer);
        }
        return;
    }
    fetch();
}

void Cpu::andWithAbsoluteWithX() {
    if (TCU <= 3) {
        absoluteWithX();
        if (TCU == 3 && PHI2) {
            A &= dataBuffer;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::rotateLeftAbsoluteWithX() {
    if (TCU <= 3) {
        absoluteWithX();
        if (TCU == 3 && PHI2) {
            instructionBufferLow = dataBuffer;
        }
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            instructionBufferLow = rotateLeft(instructionBufferLow);
        }
        return;
    }
    if (TCU == 5) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
        }
        return;
    }
    fetch();
}

void Cpu::branchOnBitReset3() {
    branchOnBitReset(3);
}

void Cpu::returnFromInterupt() {
    throw std::runtime_error("interupt not implemented");
}

void Cpu::xorWithZeroPageWithXIndirect() {
    if (TCU <= 5) {
        zeroPageWithXIndirect();
        if (TCU == 5 && PHI2) {
            A ^= dataBuffer;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::xorZeroPage() {
    if (TCU <= 3) {
        zeroPage();
        if (TCU == 3 && PHI2) {
            A ^= dataBuffer;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::shiftRightZeroPage() {
    if (TCU <= 3) {
        zeroPage();
        if (TCU == 3 && PHI2) {
            instructionBufferLow = dataBuffer;
        }
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            instructionBufferLow = shiftRight(instructionBufferLow);
        }
        return;
    }
    if (TCU == 5) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
        }
        return;
    }
    fetch();
}

void Cpu::resetMemoryBit4() {
    resetMemoryBit(4);
}

void Cpu::pushA() {
    if (TCU == 1) {
        addressBuffer = 0x100 + stackPointer;
        return;
    }
    if (TCU == 2) {
        if (!PHI2) {
            push(A);
        }
        return;
    }
    fetch();
}

void Cpu::xorImediate() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            ++programCounter;
            return;
        }
        A ^= dataBuffer;
        negativeZeroCheck(A);
        return;
    }
    fetch();
}

void Cpu::shiftRightA() {
    if (TCU == 1) {
        if (!PHI2) {
            A = shiftRight(A);
        }
        return;
    }
    fetch();
}

void Cpu::jumpAbsolute() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return;
        }
        instructionBufferLow = dataBuffer;
        ++programCounter;
        return;
    }
    if (TCU == 2) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return;
        }
        instructionBufferHigh = dataBuffer;
        programCounter = instructionBufferHigh * 0x100 + instructionBufferLow;
        return;
    }
    fetch();
}

void Cpu::xorWithAbsolute() {
    if (TCU <= 3) {
        absolute();
        if (TCU == 3 && PHI2) {
            A ^= dataBuffer;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::shiftRightAbsolute() {
    if (TCU <= 3) {
        absolute();
        if (TCU == 3 && PHI2) {
            instructionBufferLow = dataBuffer;
        }
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            instructionBufferLow = shiftRight(instructionBufferLow);
        }
        return;
    }
    if (TCU == 5) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
        }
        return;
    }
    fetch();
}

void Cpu::branchOnBitReset4() {
    branchOnBitReset(4);
}

void Cpu::branchIfOverflowClear() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            ++programCounter;
            return;
        }
        dataBuffer = instructionBufferLow;
        return;
    }
    if (prozessorStatus & 0x40) {
        programCounter += instructionBufferLow;
        return;
    }
    fetch();
}

void Cpu::xorWithZeroPageIndirectWithY() {
    if (TCU <= 4) {
        zeroPageIndirectWithY();
        if (TCU == 4 && PHI2) {
            A ^= dataBuffer;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::xorWithZeroPageIndirect() {
    if (TCU <= 4) {
        zeroPageIndirect();
        if (TCU == 4 && PHI2) {
            A ^= dataBuffer;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::xorWithZeroPageWithX() {
    if (TCU <= 3) {
        zeroPageWithX();
        if (TCU == 3 && PHI2) {
            A ^= dataBuffer;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::shiftRightZeroPageWithX() {
    if (TCU <= 3) {
        zeroPageWithX();
        if (TCU == 3 && PHI2) {
            instructionBufferLow = dataBuffer;
        }
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            instructionBufferLow = shiftRight(instructionBufferLow);
        }
        return;
    }
    if (TCU == 5) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
        }
        return;
    }
    fetch();
}

void Cpu::resetMemoryBit5() {
    resetMemoryBit(5);
}

void Cpu::clearInteruptDisableFlag() {
    if (TCU == 1) {
        if (!PHI2) {
            prozessorStatus &= 0b11111011;
        }
        return;
    }
    fetch();
}

void Cpu::xorWithAbsoluteWithY() {
    if (TCU <= 3) {
        absoluteWithY();
        if (TCU == 3 && PHI2) {
            A ^= dataBuffer;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::pushY() {
    if (TCU == 1) {
        addressBuffer = 0x100 + stackPointer;
        return;
    }
    if (TCU == 2) {
        if (!PHI2) {
            push(Y);
        }
        return;
    }
    fetch();
}

void Cpu::xorWithAbsoluteWithX() {
    if (TCU <= 3) {
        absoluteWithX();
        if (TCU == 3 && PHI2) {
            A ^= dataBuffer;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::shiftRightAbsoluteWithX() {
    if (TCU <= 3) {
        absoluteWithX();
        if (TCU == 3 && PHI2) {
            instructionBufferLow = dataBuffer;
        }
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            instructionBufferLow = shiftRight(instructionBufferLow);
        }
        return;
    }
    if (TCU == 5) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
        }
        return;
    }
    fetch();
}

void Cpu::branchOnBitReset5() {
    branchOnBitReset(5);
}

void Cpu::returnFromSubroutine() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return;
        }
        return;
    }
    if (TCU == 2) {
        if (!PHI2) {
            addressBuffer = 0x100 + stackPointer;
            return;
        }
        return;
    }
    if (TCU == 3) {
        if (!PHI2) {
            pull();
            return;
        }
        instructionBufferLow = dataBuffer;
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            pull();
            return;
        }
        instructionBufferHigh = dataBuffer;
        programCounter = instructionBufferHigh * 0x100 + instructionBufferLow + 1;
        return;
    }
    fetch();
}

void Cpu::addWithZeroPageWithXIndirect() {
    if (TCU <= 5) {
        zeroPageWithXIndirect();
        if (TCU == 5 && PHI2) {
            addWithCarry(dataBuffer);
        }
        return;
    }
    fetch();
}

void Cpu::store0AtZeroPage() {
    if (TCU <= 2) {
        zeroPage();
        if (TCU == 2 && !PHI2) {
            readWriteBuffer = false;
            dataBuffer = 0;
        }
        return;
    }
    fetch();
}

void Cpu::addWithZeroPage() {
    if (TCU <= 2) {
        zeroPage();
        if (TCU == 2 && PHI2) {
            addWithCarry(dataBuffer);
        }
        return;
    }
    fetch();
}

void Cpu::rotateRightZeroPage() {
    if (TCU <= 2) {
        zeroPage();
        if (TCU == 2 && PHI2) {
            instructionBufferLow = dataBuffer;
        }
        return;
    }
    if (TCU == 3) {
        if (!PHI2) {
            instructionBufferLow = rotateRight(instructionBufferLow);
        }
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
        }
        return;
    }
    fetch();
}

void Cpu::resetMemoryBit6() {
    resetMemoryBit(6);
}

void Cpu::pullA() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = 0x100 + stackPointer;
        }
        return;
    }
    if (TCU == 2) {
        if (!PHI2) {
            pull();
            return;
        }
        A = dataBuffer;
        return;
    }
    fetch();
}

void Cpu::addWithImediate() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            ++programCounter;
            return;
        }
        addWithCarry(dataBuffer);
        return;
    }
    fetch();
}

void Cpu::rotateRightA() {
    if (TCU == 1) {
        if (!PHI2) {
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
            negativeZeroCheck(A);
            return;
        }
        return;
    }
    fetch();
}

void Cpu::jumpAbsoluteIndirect() {
    if (TCU <= 5) {
        absoluteIndirect();
        if (TCU == 5 && !PHI2) {
            programCounter = addressBuffer;
        }
        return;
    }
    fetch();
}

void Cpu::storeAAtAbsolute() {
    if (TCU <= 3) {
        absolute();
        if (!PHI2 && TCU == 3) {
            readWriteBuffer = false;
            dataBuffer = A;
        }
        return;
    }
    fetch();
}

void Cpu::transferXToStackpointer() {
    if (TCU == 1) {
        if (!PHI2) {
            stackPointer = X;
            return;
        }
        return;
    }
    fetch();
}

void Cpu::loadXImidiate() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return;
        }
        X = dataBuffer;
        prozessorStatus &= 0b01111101; // clear flags n and z
        ++programCounter;
        negativeZeroCheck(X);
        return;
    }
    fetch();
}

void Cpu::loadAImidiate() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return;
        }
        A = dataBuffer;
        prozessorStatus &= 0b01111101; // clear flags n and z
        ++programCounter;
        negativeZeroCheck(A);
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


Cpu::Cpu(unsigned char& _data, unsigned short& _address, bool& _VPB, bool& _RDY, const bool& _IRQB, bool& _MLB, const bool& _NMIB,
    bool& _SYNC, bool& _RWB, const bool& _BE, const bool& _SOB, const bool& _PHI2, bool& _PHI1O, bool& _PHI2O) :
    data { _data }, address { _address }, VPB { _VPB }, RDY { _RDY }, IRQB { _IRQB }, MLB { _MLB }, NMIB { _NMIB },
    SYNC { _SYNC }, RWB { _RWB }, BE { _BE }, SOB { _SOB }, PHI2 { _PHI2 }, PHI1O { _PHI1O }, PHI2O { _PHI2O } {}

void Cpu::cycle() {
    if (BE && readWriteBuffer) {
        dataBuffer = data;
    }
    if (!PHI2 && !RDY) {
        toBus();
        return;
    }
    PHI1O = !PHI2;
    PHI2O = PHI2;
    if (!(IRQB || (prozessorStatus & 0x4))) {
        interuptRequested = true;
    }
    if (NMIB) {
        interuptRequested = true;
    }
    if (lastClockState == PHI2) {
        toBus();
        return;
    }
    lastClockState = PHI2;
    if (resetTimer) {
        if (!PHI2) {
            readWriteBuffer = true;
            --resetTimer;
            if (resetTimer == 0) {
                programCounter = 0xFFFC;
                TCU = 0;
                addressBuffer = programCounter;
                IR = 0x4C;
            }
            toBus();
        }
        return;
    }
    if (!PHI2) {
        ++TCU;
    }
    if (TCU == 0) {
        toBus();
        return;
    }
    switch (IR) {
    case 0x00:
        breakRequest();
        break;
    case 0x01:
        orWithZeroPageWithXIndirect();
        break;
    case 0x04:
        testAndSetMemoryBitZeroPage();
        break;
    case 0x05:
        orWithZeroPage();
        break;
    case 0x06:
        shiftLeftZeroPage();
        break;
    case 0x07:
        resetMemoryBit0();
        break;
    case 0x08:
        pushProzessorStatus();
        break;
    case 0x09:
        orWithImmediate();
        break;
    case 0x0a:
        shiftLeftA();
        break;
    case 0x0c:
        testAndSetMemoryBitAbsolute();
        break;
    case 0x0d:
        orWithAbsolute();
        break;
    case 0x0e:
        shiftLeftAbsolute();
        break;
    case 0x0f:
        branchOnBitReset0();
        break;
    case 0x10:
        branchIfResultPlus();
        break;
    case 0x11:
        orWithZeroPageIndirectWithY();
        break;
    case 0x12:
        orWithZeroPageIndirect();
        break;
    case 0x14:
        testAndResetMemoryBitZeroPage();
        break;
    case 0x15:
        orWithZeroPageWithX();
        break;
    case 0x16:
        shiftLeftZeroPageWithX();
        break;
    case 0x17:
        resetMemoryBit1();
        break;
    case 0x18:
        clearCarry();
        break;
    case 0x19:
        orWithAbsoluteWithY();
        break;
    case 0x1a:
        incrementA();
        break;
    case 0x1c:
        testAndResetMemoryBitAbsolute();
        break;
    case 0x1d:
        orWithAbsoluteWithX();
        break;
    case 0x1e:
        shiftLeftAbsoluteWithX();
        break;
    case 0x1f:
        branchOnBitReset1();
        break;
    case 0x20:
        jumpToSubroutine();
        break;
    case 0x21:
        andWithZeroPageWithXIndirect();
        break;
    case 0x24:
        bitTestWithZeroPage();
        break;
    case 0x25:
        andWithZeroPage();
        break;
    case 0x26:
        rotateLeftZeroPage();
        break;
    case 0x27:
        resetMemoryBit2();
        break;
    case 0x28:
        pullProcessorStatus();
        break;
    case 0x29:
        andWithImediate();
        break;
    case 0x2a:
        rotateLeftA();
        break;
    case 0x2c:
        bitTestWithAbsolute();
        break;
    case 0x2d:
        andWithAbsolute();
        break;
    case 0x2e:
        rotateLeftAbsolute();
        break;
    case 0x2f:
        branchOnBitReset2();
        break;
    case 0x30:
        branchIfResultMinus();
        break;
    case 0x31:
        andWithZeroPageIndirectWithY();
        break;
    case 0x32:
        andWithZeroPageIndirect();
        break;
    case 0x34:
        bitTestWithZeroPageWithX();
        break;
    case 0x35:
        andWithZeroPageWithX();
        break;
    case 0x36:
        rotateLeftZeroPageWithX();
        break;
    case 0x37:
        resetMemoryBit3();
        break;
    case 0x38:
        setCarry();
        break;
    case 0x39:
        andWithAbsoluteWithY();
        break;
    case 0x3a:
        decrementA();
        break;
    case 0x3c:
        bitTestWithAbsoluteWithX();
        break;
    case 0x3d:
        andWithAbsoluteWithX();
        break;
    case 0x3e:
        rotateLeftAbsoluteWithX();
        break;
    case 0x3f:
        branchOnBitReset3();
        break;
    case 0x40:
        returnFromInterupt();
        break;
    case 0x41:
        xorWithZeroPageWithXIndirect();
        break;
    case 0x45:
        xorZeroPage();
        break;
    case 0x46:
        shiftRightZeroPage();
        break;
    case 0x47:
        resetMemoryBit4();
        break;
    case 0x48:
        pushA();
        break;
    case 0x49:
        xorImediate();
        break;
    case 0x4a:
        shiftRightA();
        break;
    case 0x4c:
        jumpAbsolute();
        break;
    case 0x4d:
        xorWithAbsolute();
        break;
    case 0x4e:
        shiftRightAbsolute();
        break;
    case 0x4f:
        branchOnBitReset4();
        break;
    case 0x50:
        branchIfOverflowClear();
        break;
    case 0x51:
        xorWithZeroPageIndirectWithY();
        break;
    case 0x52:
        xorWithZeroPageIndirect();
        break;
    case 0x55:
        xorWithZeroPageWithX();
        break;
    case 0x56:
        shiftRightZeroPageWithX();
        break;
    case 0x57:
        resetMemoryBit5();
        break;
    case 0x58:
        clearInteruptDisableFlag();
        break;
    case 0x59:
        xorWithAbsoluteWithY();
        break;
    case 0x5a:
        pushY();
        break;
    case 0x5d:
        xorWithAbsoluteWithX();
        break;
    case 0x5e:
        shiftRightAbsoluteWithX();
        break;
    case 0x5f:
        branchOnBitReset5();
        break;
    case 0x60:
        returnFromSubroutine();
        break;
    case 0x61:
        addWithZeroPageWithXIndirect();
        break;
    case 0x64:
        store0AtZeroPage();
        break;
    case 0x65:
        addWithZeroPage();
        break;
    case 0x66:
        rotateRightZeroPage();
        break;
    case 0x67:
        resetMemoryBit6();
        break;
    case 0x68:
        pullA();
        break;
    case 0x69:
        addWithImediate();
        break;
    case 0x6a:
        rotateRightA();
        break;
    case 0x6c:
        jumpAbsoluteIndirect();
        break;
    case 0x6d:
        addWithAbsolute();
        break;
    case 0x6e:
        rotateRightAbsolute();
        break;
    case 0x6f:
        branchOnBitReset6();
        break;
    case 0x70:
        branchIfOverflowSet();
        break;
    case 0x71:
        addWithZeroPageIndirectWithY();
        break;
    case 0x72:
        addWithZeroPageIndirect();
        break;
    case 0x74:
        store0AtZeroPageWithX();
        break;
    case 0x75:
        addWithZeroPageWithX();
        break;
    case 0x76:
        rotateRightZeroPageWithX();
        break;
    case 0x77:
        resetMemoryBit7();
        break;
    case 0x78:
        setInteruptDisable();
        break;
    case 0x79:
        addWithAbsoluteWithY();
        break;
    case 0x7a:
        pullY();
        break;
    case 0x7c:
        jumpAbsoluteWithXIndirect();
        break;
    case 0x7d:
        addWithAbsoluteWithX();
        break;
    case 0x7e:
        rotateRightAbsoluteWithX();
        break;
    case 0x7f:
        branchOnBitReset7();
        break;
    case 0x80:
        branchAlways();
        break;
    case 0x81:
        storeAAtZeroPageWithXIndirect();
        break;
    case 0x84:
        storeYAtZeroPage();
        break;
    case 0x85:
        storeAAtZeroPage();
        break;
    case 0x86:
        storeXAtZeroPage();
        break;
    case 0x87:
        setMemoryBit0();
        break;
    case 0x88:
        decrementY();
        break;
    case 0x89:
        bitTestWithImediate();
        break;
    case 0x8a:
        transferXtoA();
        break;
    case 0x8c:
        storeYAtAbsolute();
        break;
    case 0x8d:
        storeAAtAbsolute();
        break;
    case 0x8e:
        storeXAtAbsolute();
        break;
    case 0x8f:
        branchOnBitSet0();
        break;
    case 0x90:
        branchIfCarryClear();
        break;
    case 0x91:
        storeAAtZeroPageIndirectWithY();
        break;
    case 0x92:
        storeAAtZeroPageIndirect();
        break;
    case 0x94:
        storeYAtZeroPageWithX();
        break;
    case 0x95:
        storeAAtZeroPageWithX();
        break;
    case 0x96:
        storeXAtZeroPageWithY();
        break;
    case 0x97:
        setMemoryBit1();
        break;
    case 0x98:
        transferYToA();
        break;
    case 0x99:
        storeAAtabsoluteWithY();
        break;
    case 0x9a:
        transferXToStackpointer();
        break;
    case 0x9c:
        store0AtAbsolute();
        break;
    case 0x9d:
        storeAAtAbsoluteWithX();
        break;
    case 0x9e:
        store0AtAbsoluteWithX();
        break;
    case 0x9f:
        branchOnBitSet1();
        break;
    case 0xa0:
        loadYImediate();
        break;
    case 0xa1:
        loadAZeroPageWithXIndirect();
        break;
    case 0xa2:
        loadXImidiate();
        break;
    case 0xa4:
        loadYZeroPage();
        break;
    case 0xa5:
        loadAZeroPage();
        break;
    case 0xa6:
        loadXZeroPage();
        break;
    case 0xa7:
        setMemoryBit2();
        break;
    case 0xa8:
        transferAToY();
        break;
    case 0xa9:
        loadAImidiate();
        break;
    case 0xaa:
        transferAToX();
        break;
    case 0xac:
        loadYAbsolute();
        break;
    case 0xad:
        loadAAbsolute();
        break;
    case 0xae:
        loadXAbsolute();
        break;
    case 0xaf:
        branchOnBitSet2();
        break;
    case 0xb0:
        branchIfCarrySet();
        break;
    case 0xb1:
        loadAZeroPageIndirectWithY();
        break;
    case 0xb2:
        loadAZeroPageIndirect();
        break;
    case 0xb4:
        loadYZeroPageWithX();
        break;
    case 0xb5:
        loadAZeroPageWithX();
        break;
    case 0xb6:
        loadXZeroPageWithY();
        break;
    case 0xb7:
        setMemoryBit3();
        break;
    case 0xb8:
        clearOverflow();
        break;
    case 0xb9:
        loadAAbsoluteWithY();
        break;
    case 0xba:
        transferStackpointerToX();
        break;
    case 0xbc:
        loadYAbsoluteWithX();
        break;
    case 0xbd:
        loadAAbsoluteWithX();
        break;
    case 0xbe:
        loadXAbsoluteWithY();
        break;
    case 0xbf:
        branchOnBitSet3();
        break;
    case 0xc0:
        compareYWithImediate();
        break;
    case 0xc1:
        compareAWithZeroPageWithXIndirect();
        break;
    case 0xc4:
        compareYWithZeroPage();
        break;
    case 0xc5:
        compareAWithZeroPage();
        break;
    case 0xc6:
        decrementZeroPage();
        break;
    case 0xc7:
        setMemoryBit4();
        break;
    case 0xc8:
        incrementY();
        break;
    case 0xc9:
        compareAWithImediate();
        break;
    case 0xca:
        decrementX();
        break;
    case 0xcb:
        waitForInterupt();
        break;
    case 0xcc:
        compareYWithAbsolute();
        break;
    case 0xcd:
        compareAWithAbsolute();
        break;
    case 0xce:
        decrementAbsolute();
        break;
    case 0xcf:
        branchOnBitSet4();
        break;
    case 0xd0:
        branchNotEqual();
        break;
    case 0xd1:
        compareAWithZeroPageIndirectWithY();
        break;
    case 0xd2:
        compareAWithZeroPageIndirect();
        break;
    case 0xd5:
        compareAWithZeroPageWithX();
        break;
    case 0xd6:
        decrementZeroPageWithX();
        break;
    case 0xd7:
        setMemoryBit5();
        break;
    case 0xd8:
        clearDecimalMode();
        break;
    case 0xd9:
        compareAWithAbsoluteWithY();
        break;
    case 0xda:
        pushX();
        break;
    case 0xdb:
        stopMode();
        break;
    case 0xdd:
        compareAWithAbsoluteWithX();
        break;
    case 0xde:
        decrementAbsoluteWithX();
        break;
    case 0xdf:
        branchOnBitSet5();
        break;
    case 0xe0:
        compareXWithImediate();
        break;
    case 0xe1:
        subtractWithZeroPageWithXIndirect();
        break;
    case 0xe4:
        compareXWithZeroPage();
        break;
    case 0xe5:
        subtractWithZeroPage();
        break;
    case 0xe6:
        incrementZeroPage();
        break;
    case 0xe7:
        setMemoryBit6();
        break;
    case 0xe8:
        incrementX();
        break;
    case 0xe9:
        subtractWithImediate();
        break;
    case 0xea:
        noop();
        break;
    case 0xec:
        compareXWithAbsolute();
        break;
    case 0xed:
        subtractWithAbsolute();
        break;
    case 0xee:
        incrementAbsolute();
        break;
    case 0xef:
        branchOnBitSet6();
        break;
    case 0xf0:
        branchIfEqual();
        break;
    case 0xf1:
        subtractWithZeroPageIndirectWithY();
        break;
    case 0xf2:
        subtractWithZeroPageIndirect();
        break;
    case 0xf5:
        subtractWithZeroPageWithX();
        break;
    case 0xf6:
        incrementZeroPageWithX();
        break;
    case 0xf7:
        setMemoryBit7();
        break;
    case 0xf8:
        setDecimalMode();
        break;
    case 0xf9:
        subtractWithAbsoluteWithY();
        break;
    case 0xfa:
        pullX();
        break;
    case 0xfd:
        subtractWithAbsoluteWithX();
        break;
    case 0xfe:
        incrementAbsoluteWithX();
        break;
    case 0xff:
        branchOnBitSet7();
        break;
    default:
        throw IR;
    }
    toBus();
}

void Cpu::reset() {
    prozessorStatus &= 0b11110111;
    prozessorStatus |= 0b00110100;
    resetTimer = 7;
    IR = 0x6C;
}
