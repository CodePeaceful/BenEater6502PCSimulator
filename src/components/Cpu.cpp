#include "Cpu.hpp"

#include <stdexcept>
#include <functional>

void Cpu::fetch() {
    if (!PHI2) {
        addressBuffer = programCounter;
        readWriteBuffer = true;
        return;
    }
    TCU = 0;
    writeBackCounter = 0;
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
    throw std::runtime_error("interupts not implemented");
}

void Cpu::negativeZeroCheck(unsigned char value) {
    if (value == 0) {
        prozessorStatus |= 0b0000'0010;
    }
    else {
        prozessorStatus &= 0b1111'1101;
    }
    if (value & 0x80) {
        prozessorStatus |= 0b1000'0000;
    }
    else {
        prozessorStatus &= 0b0111'1111;
    }
}

void Cpu::addWithCarry(unsigned char value) {
    bool A7 = A & 0b1000'0000;
    bool v7 = value & 0b1000'0000;
    bool carry = prozessorStatus & 0b1000'0000;
    A += value;
    if (carry) {
        ++A;
    }
    bool A7new = A & 0b1000'0000;
    // clear Overflow and carry
    prozessorStatus &= 0b0011'1111;
    // overflow
    if (A7 == v7 && A7 != A7new) {
        prozessorStatus |= 0b0100'0000;
    }
    // carry
    if ((A7 && v7) || ((A7 ^ v7) && !A7new)) {
        prozessorStatus |= 0b1000'0000;
    }
    negativeZeroCheck(A);
}

void Cpu::addWithCarry(bool(Cpu::* address)()) {
    if (!std::invoke(address, *this)) {
        return;
    }
    if (writeBackCounter == 0) {
        if (!PHI2) {
            return;
        }
        ++writeBackCounter;
        addWithCarry(dataBuffer);
        return;
    }
    fetch();
}

void Cpu::subtractWithCarry(unsigned char value) {
    bool A7 = A & 0b1000'0000;
    bool v7 = value & 0b1000'0000;
    bool carry = prozessorStatus & 0b1000'0000;

    if (carry) {
        ++A;
    }
    bool borrow = A < value;
    A -= value;
    bool A7new = A & 0b1000'0000;
    // clear Overflow and carry
    prozessorStatus &= 0b0011'1111;
    // overflow
    if (A7 != v7 && A7 != A7new) {
        prozessorStatus |= 0b0100'0000;
    }
    // carry
    if (!borrow) {
        prozessorStatus |= 0b1000'0000;
    }
    negativeZeroCheck(A);
}

void Cpu::subtractWithCarry(bool(Cpu::* address)()) {
    if (!std::invoke(address, *this)) {
        return;
    }
    if (writeBackCounter == 0) {
        if (!PHI2) {
            return;
        }
        ++writeBackCounter;
        subtractWithCarry(dataBuffer);
        return;
    }
    fetch();
}

void Cpu::orA(bool(Cpu::* address)()) {
    if (!std::invoke(address, *this)) {
        return;
    }
    if (writeBackCounter == 0) {
        if (!PHI2) {
            return;
        }
        ++writeBackCounter;
        A |= dataBuffer;
        negativeZeroCheck(A);
        return;
    }
    fetch();
}

void Cpu::andA(bool(Cpu::* address)()) {
    if (!std::invoke(address, *this)) {
        return;
    }
    if (writeBackCounter == 0) {
        if (!PHI2) {
            return;
        }
        ++writeBackCounter;
        A &= dataBuffer;
        negativeZeroCheck(A);
        return;
    }
    fetch();
}

void Cpu::xorA(bool(Cpu::* address)()) {
    if (!std::invoke(address, *this)) {
        return;
    }
    if (writeBackCounter == 0) {
        if (!PHI2) {
            return;
        }
        ++writeBackCounter;
        A ^= dataBuffer;
        negativeZeroCheck(A);
        return;
    }
    fetch();
}

void Cpu::load(unsigned char& cpuRegister, bool(Cpu::* address)()) {
    if (!std::invoke(address, *this)) {
        return;
    }
    if (writeBackCounter == 0) {
        if (!PHI2) {
            return;
        }
        ++writeBackCounter;
        cpuRegister = dataBuffer;
        negativeZeroCheck(dataBuffer);
        return;
    }
    fetch();
}

void Cpu::store(unsigned char value, bool(Cpu::* address)()) {
    if (!std::invoke(address, *this)) {
        return;
    }
    if (writeBackCounter == 0) {
        if (!PHI2) {
            dataBuffer = value;
            readWriteBuffer = false;
            return;
        }
        ++writeBackCounter;
        return;
    }
    fetch();
}

void Cpu::bitTest(unsigned char second) {
    if (A & second) {
        prozessorStatus &= 0b1111'1101;
    }
    else {
        prozessorStatus |= 2;
    }
    if (second & 0b1000'0000) {
        prozessorStatus |= 0b1000'0000;
    }
    else {
        prozessorStatus &= 0b0111'1111;
    }
    if (second & 0b0100'0000) {
        prozessorStatus |= 0b0100'0000;
    }
    else {
        prozessorStatus &= 0b1011'1111;
    }
}

void Cpu::bitTest(bool(Cpu::* address)()) {
    if (!std::invoke(address, *this)) {
        return;
    }
    if (writeBackCounter == 0) {
        if (!PHI2) {
            return;
        }
        bitTest(dataBuffer);
        ++writeBackCounter;
        return;
    }
    fetch();
}

void Cpu::compare(unsigned char first, unsigned char second) {
    if (first > second) {
        prozessorStatus |= 0b1000'0000;
        prozessorStatus &= 0b1011'1110;
    }
    else if (first == second) {
        prozessorStatus |= 0b1100'0000;
        prozessorStatus &= 0b1111'1110;
    }
    else {
        prozessorStatus |= 0b0000'0001;
        prozessorStatus &= 0b0011'1111;
    }
}

void Cpu::compare(unsigned char first, bool(Cpu::* address)()) {
    if (!std::invoke(address, *this)) {
        return;
    }
    if (writeBackCounter == 0) {
        if (!PHI2) {
            return;
        }
        compare(first, dataBuffer);
        ++writeBackCounter;
        return;
    }
    fetch();
}

void Cpu::testAndSetMemoryBit(bool(Cpu::* address)()) {
    if (!std::invoke(address, *this)) {
        return;
    }
    if (writeBackCounter == 0) {
        if (!PHI2) {
            return;
        }
        instructionBufferLow = dataBuffer;
        ++writeBackCounter;
        return;
    }
    if (writeBackCounter == 1) {
        if (!PHI2) {
            if ((A & instructionBufferLow) == 0) {
                prozessorStatus |= 2;
            }
            else {
                prozessorStatus &= 0b1111'1101;
            }
            instructionBufferLow |= A;
            return;
        }
        ++writeBackCounter;
        return;
    }
    if (writeBackCounter == 2) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
            return;
        }
        ++writeBackCounter;
        return;
    }
    fetch();
}

void Cpu::testAndResetMemoryBit(bool(Cpu::* address)()) {
    if (!std::invoke(address, *this)) {
        return;
    }
    if (writeBackCounter == 0) {
        if (!PHI2) {
            return;
        }
        instructionBufferLow = dataBuffer;
        ++writeBackCounter;
        return;
    }
    if (writeBackCounter == 1) {
        if (!PHI2) {
            if ((A & instructionBufferLow) == 0) {
                prozessorStatus |= 2;
            }
            else {
                prozessorStatus &= 0b1111'1101;
            }
            instructionBufferLow &= ~A;
            return;
        }
        ++writeBackCounter;
        return;
    }
    if (writeBackCounter == 2) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
            return;
        }
        ++writeBackCounter;
        return;
    }
    fetch();
}

void Cpu::rotateRight(bool(Cpu::* address)()) {
    if (!std::invoke(address, *this)) {
        return;
    }
    if (writeBackCounter == 0) {
        if (!PHI2) {
            return;
        }
        instructionBufferLow = dataBuffer;
        ++writeBackCounter;
        return;
    }
    if (writeBackCounter == 1) {
        if (!PHI2) {
            bool rememberCarry = prozessorStatus & 1;
            if (instructionBufferLow & 1) {
                prozessorStatus |= 1;
            }
            else {
                prozessorStatus &= 0b1111'1110;
            }
            instructionBufferLow = instructionBufferLow >> 1;
            if (rememberCarry) {
                instructionBufferLow |= 0x80;
            }
            negativeZeroCheck(instructionBufferLow);
            return;
        }
        ++writeBackCounter;
        return;
    }
    if (writeBackCounter == 2) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
            return;
        }
        ++writeBackCounter;
        return;
    }
    fetch();
}

void Cpu::rotateLeft(bool(Cpu::* address)()) {
    if (!std::invoke(address, *this)) {
        return;
    }
    if (writeBackCounter == 0) {
        if (!PHI2) {
            return;
        }
        instructionBufferLow = dataBuffer;
        ++writeBackCounter;
        return;
    }
    if (writeBackCounter == 1) {
        if (!PHI2) {
            bool rememberCarry = prozessorStatus & 0x80;
            if (instructionBufferLow & 1) {
                prozessorStatus |= 1;
            }
            else {
                prozessorStatus &= 0b1111'1110;
            }
            instructionBufferLow = instructionBufferLow << 1;
            if (rememberCarry) {
                instructionBufferLow |= 1;
            }
            negativeZeroCheck(instructionBufferLow);
            return;
        }
        ++writeBackCounter;
        return;
    }
    if (writeBackCounter == 2) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
            return;
        }
        ++writeBackCounter;
        return;
    }
    fetch();
}

void Cpu::shiftRight(bool(Cpu::* address)()) {
    if (!std::invoke(address, *this)) {
        return;
    }
    if (writeBackCounter == 0) {
        if (!PHI2) {
            return;
        }
        instructionBufferLow = dataBuffer;
        ++writeBackCounter;
        return;
    }
    if (writeBackCounter == 1) {
        if (!PHI2) {
            instructionBufferLow = instructionBufferLow >> 1;
            negativeZeroCheck(instructionBufferLow);
            return;
        }
        ++writeBackCounter;
        return;
    }
    if (writeBackCounter == 2) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
            return;
        }
        ++writeBackCounter;
        return;
    }
    fetch();
}

void Cpu::shiftLeft(bool(Cpu::* address)()) {
    if (!std::invoke(address, *this)) {
        return;
    }
    if (writeBackCounter == 0) {
        if (!PHI2) {
            return;
        }
        instructionBufferLow = dataBuffer;
        ++writeBackCounter;
        return;
    }
    if (writeBackCounter == 1) {
        if (!PHI2) {
            instructionBufferLow = instructionBufferLow << 1;
            negativeZeroCheck(instructionBufferLow);
            return;
        }
        ++writeBackCounter;
        return;
    }
    if (writeBackCounter == 2) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
            return;
        }
        ++writeBackCounter;
        return;
    }
    fetch();
}

void Cpu::increment(bool(Cpu::* address)()) {
    if (!std::invoke(address, *this)) {
        return;
    }
    if (writeBackCounter == 0) {
        if (!PHI2) {
            return;
        }
        instructionBufferLow = dataBuffer;
        ++writeBackCounter;
        return;
    }
    if (writeBackCounter == 1) {
        if (!PHI2) {
            ++instructionBufferLow;
            negativeZeroCheck(instructionBufferLow);
            return;
        }
        ++writeBackCounter;
        return;
    }
    if (writeBackCounter == 2) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
            return;
        }
        ++writeBackCounter;
        return;
    }
    fetch();
}

void Cpu::decrement(bool(Cpu::* address)()) {
    if (!std::invoke(address, *this)) {
        return;
    }
    if (writeBackCounter == 0) {
        if (!PHI2) {
            return;
        }
        instructionBufferLow = dataBuffer;
        ++writeBackCounter;
        return;
    }
    if (writeBackCounter == 1) {
        if (!PHI2) {
            --instructionBufferLow;
            negativeZeroCheck(instructionBufferLow);
            return;
        }
        ++writeBackCounter;
        return;
    }
    if (writeBackCounter == 2) {
        if (!PHI2) {
            readWriteBuffer = false;
            dataBuffer = instructionBufferLow;
            return;
        }
        ++writeBackCounter;
        return;
    }
    fetch();
}

void Cpu::resetMemoryBit(unsigned char bitId) {
    if (TCU >= 2) {
        zeroPage();
        if (TCU == 2 && PHI2) {
            instructionBufferLow = dataBuffer;
        }
        return;
    }
    if (TCU == 3) {
        unsigned char thisBit = 1 << bitId;
        thisBit = ~thisBit;
        instructionBufferLow &= thisBit;
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

void Cpu::setMemoryBit(unsigned char bitId) {
    if (TCU >= 2) {
        zeroPage();
        if (TCU == 2 && PHI2) {
            instructionBufferLow = dataBuffer;
        }
        return;
    }
    if (TCU == 3) {
        unsigned char thisBit = 1 << bitId;
        instructionBufferLow |= thisBit;
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

void Cpu::branchOnBitReset(unsigned char bitId) {
    if (TCU <= 2) {
        zeroPage();
        if (TCU == 2 && PHI2) {
            instructionBufferHigh = dataBuffer;
        }
        return;
    }
    if (TCU == 3) {
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            addressBuffer = programCounter;
            ++programCounter;
            return;
        }
        instructionBufferLow = dataBuffer;
        return;
    }
    if (TCU == 5) {
        unsigned char thisBit = 1 << bitId;
        bool condition = thisBit & instructionBufferHigh;
        if (!condition) {
            if (!PHI2) {
                programCounter += instructionBufferLow;
            }
            return;
        }
    }
    fetch();
}

void Cpu::branchOnBitSet(unsigned char bitId) {
    if (TCU <= 2) {
        zeroPage();
        if (TCU == 2 && PHI2) {
            instructionBufferHigh = dataBuffer;
        }
        return;
    }
    if (TCU == 3) {
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            addressBuffer = programCounter;
            ++programCounter;
            return;
        }
        instructionBufferLow = dataBuffer;
        return;
    }
    if (TCU == 5) {
        unsigned char thisBit = 1 << bitId;
        bool condition = thisBit & instructionBufferHigh;
        if (condition) {
            if (!PHI2) {
                programCounter += instructionBufferLow;
            }
            return;
        }
    }
    fetch();
}

void Cpu::branchIf(bool condition) {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            ++programCounter;
            return;
        }
        instructionBufferLow = dataBuffer;
        return;
    }
    if (TCU == 2) {
        if (condition) {
            if (!PHI2) {
                programCounter += instructionBufferLow;
            }
            return;
        }
    }
    fetch();
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

bool Cpu::zeroPage() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return false;
        }
        instructionBufferLow = dataBuffer;
        ++programCounter;
        return false;
    }
    if (TCU == 2) {
        if (!PHI2) {
            addressBuffer = instructionBufferLow;
            return true;
        }
    }
    return true;
}

bool Cpu::zeroPageIndirect() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return false;
        }
        instructionBufferLow = dataBuffer;
        ++programCounter;
        return false;
    }
    if (TCU == 2) {
        if (!PHI2) {
            addressBuffer = instructionBufferLow;
            return false;
        }
        instructionBufferLow = dataBuffer;
        return false;
    }
    if (TCU == 3) {
        if (!PHI2) {
            ++addressBuffer;
            addressBuffer %= 256;
            return false;
        }
        instructionBufferHigh = dataBuffer;
        return false;
    }
    if (TCU == 4) {
        if (!PHI2) {
            addressBuffer = instructionBufferHigh * 0x100 + instructionBufferLow;
            return true;
        }
    }
    return true;
}

bool Cpu::zeroPageWithXIndirect() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return false;
        }
        instructionBufferLow = dataBuffer;
        ++programCounter;
        return false;
    }
    if (TCU == 2) {
        if (!PHI2) {
            instructionBufferLow += X;
        }
        return false;
    }
    if (TCU == 3) {
        if (!PHI2) {
            addressBuffer = instructionBufferLow;
            return false;
        }
        instructionBufferLow = dataBuffer;
        return false;
    }
    if (TCU == 4) {
        if (!PHI2) {
            ++addressBuffer;
            addressBuffer %= 256;
            return false;
        }
        instructionBufferHigh = dataBuffer;
        return false;
    }
    if (TCU == 5) {
        if (!PHI2) {
            addressBuffer = instructionBufferHigh * 0x100 + instructionBufferLow;
            return true;
        }
    }
    return true;
}

bool Cpu::zeroPageIndirectWithY() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return false;
        }
        instructionBufferLow = dataBuffer;
        ++programCounter;
        return false;
    }
    if (TCU == 2) {
        if (!PHI2) {
            addressBuffer = instructionBufferLow;
            return false;
        }
        instructionBufferLow = dataBuffer;
        return false;
    }
    if (TCU == 3) {
        if (!PHI2) {
            ++addressBuffer;
            addressBuffer %= 256;
            return false;
        }
        instructionBufferHigh = dataBuffer;
        return false;
    }
    if (TCU == 4) {
        if (!PHI2) {
            addressBuffer = instructionBufferHigh * 0x100 + instructionBufferLow + Y;
            return true;
        }
    }
    return true;
}

bool Cpu::zeroPageWithX() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return false;
        }
        instructionBufferLow = dataBuffer;
        ++programCounter;
        return false;
    }
    if (TCU == 2) {
        if (!PHI2) {
            instructionBufferLow += X;
        }
        return false;
    }
    if (TCU == 3) {
        if (!PHI2) {
            addressBuffer = instructionBufferLow;
            return true;
        }
    }
    return true;
}

bool Cpu::zeroPageWithY() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return false;
        }
        instructionBufferLow = dataBuffer;
        ++programCounter;
        return false;
    }
    if (TCU == 2) {
        if (!PHI2) {
            instructionBufferLow += Y;
        }
        return false;
    }
    if (TCU == 3) {
        if (!PHI2) {
            addressBuffer = instructionBufferLow;
            return true;
        }
    }
    return true;
}

bool Cpu::absoluteIndirect() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return false;
        }
        instructionBufferLow = dataBuffer;
        ++programCounter;
        return false;
    }
    if (TCU == 2) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return false;
        }
        instructionBufferHigh = dataBuffer;
        ++programCounter;
        return false;
    }
    if (TCU == 3) {
        if (!PHI2) {
            addressBuffer = instructionBufferHigh * 0x100 + instructionBufferLow;
            return false;
        }
        instructionBufferLow = dataBuffer;
        return false;
    }
    if (TCU == 4) {
        if (!PHI2) {
            ++addressBuffer;
            return false;
        }
        instructionBufferHigh = dataBuffer;
        return false;
    }
    if (TCU == 5) {
        if (!PHI2) {
            addressBuffer = instructionBufferHigh * 0x100 + instructionBufferLow;
            return true;
        }
    }
    return true;
}

bool Cpu::absolute() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return false;
        }
        instructionBufferLow = dataBuffer;
        ++programCounter;
        return false;
    }
    if (TCU == 2) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return false;
        }
        instructionBufferHigh = dataBuffer;
        ++programCounter;
        return false;
    }
    if (TCU == 3) {
        if (!PHI2) {
            addressBuffer = instructionBufferHigh * 0x100 + instructionBufferLow;
            return true;
        }
    }
    return true;
}

bool Cpu::absoluteWithX() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return false;
        }
        instructionBufferLow = dataBuffer;
        ++programCounter;
        return false;
    }
    if (TCU == 2) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return false;
        }
        instructionBufferHigh = dataBuffer;
        ++programCounter;
        return false;
    }
    if (TCU == 3) {
        if (!PHI2) {
            addressBuffer = instructionBufferHigh * 0x100 + instructionBufferLow + X;
            return true;
        }
    }
    return true;
}

bool Cpu::absoluteWithY() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return false;
        }
        instructionBufferLow = dataBuffer;
        ++programCounter;
        return false;
    }
    if (TCU == 2) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return false;
        }
        instructionBufferHigh = dataBuffer;
        ++programCounter;
        return false;
    }
    if (TCU == 3) {
        if (!PHI2) {
            addressBuffer = instructionBufferHigh * 0x100 + instructionBufferLow + Y;
            return true;
        }
    }
    return true;
}

void Cpu::breakRequest() {
    throw std::runtime_error("no break");
}

void Cpu::orWithZeroPageWithXIndirect() {
    orA(&Cpu::zeroPageWithXIndirect);
}

void Cpu::testAndSetMemoryBitZeroPage() {
    testAndResetMemoryBit(&Cpu::zeroPage);
}

void Cpu::orWithZeroPage() {
    orA(&Cpu::zeroPage);
}


void Cpu::shiftLeftZeroPage() {
    shiftLeft(&Cpu::zeroPage);
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
            A = A << 1;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::testAndSetMemoryBitAbsolute() {
    testAndSetMemoryBit(&Cpu::absolute);
}

void Cpu::orWithAbsolute() {
    orA(&Cpu::absolute);
}

void Cpu::shiftLeftAbsolute() {
    shiftLeft(&Cpu::absolute);
}

void Cpu::branchOnBitReset0() {
    branchOnBitReset(0);
}

void Cpu::branchIfResultPlus() {
    bool negative = prozessorStatus & 0x80;
    branchIf(!negative);
}

void Cpu::orWithZeroPageIndirectWithY() {
    orA(&Cpu::zeroPageIndirectWithY);
}

void Cpu::orWithZeroPageIndirect() {
    orA(&Cpu::zeroPageIndirect);
}

void Cpu::testAndResetMemoryBitZeroPage() {
    testAndResetMemoryBit(&Cpu::zeroPage);
}

void Cpu::orWithZeroPageWithX() {
    orA(&Cpu::zeroPageWithX);
}

void Cpu::shiftLeftZeroPageWithX() {
    shiftLeft(&Cpu::zeroPageWithX);
}

void Cpu::resetMemoryBit1() {
    resetMemoryBit(1);
}

void Cpu::clearCarry() {
    if (TCU == 1) {
        if (!PHI2) {
            prozessorStatus &= 0b1111'1110;
        }
        return;
    }
    fetch();
}

void Cpu::orWithAbsoluteWithY() {
    orA(&Cpu::absoluteWithY);
}

void Cpu::incrementA() {
    if (TCU == 1) {
        if (!PHI2) {
            ++A;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::testAndResetMemoryBitAbsolute() {
    testAndResetMemoryBit(&Cpu::absolute);
}

void Cpu::orWithAbsoluteWithX() {
    orA(&Cpu::absoluteWithX);
}

void Cpu::shiftLeftAbsoluteWithX() {
    shiftLeft(&Cpu::absoluteWithX);
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
    andA(&Cpu::zeroPageWithXIndirect);
}

void Cpu::bitTestWithZeroPage() {
    bitTest(&Cpu::zeroPage);
}

void Cpu::andWithZeroPage() {
    andA(&Cpu::zeroPage);
}

void Cpu::rotateLeftZeroPage() {
    rotateLeft(&Cpu::zeroPage);
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
            bool rememberCarry = prozessorStatus & 0x80;
            if (A & 1) {
                prozessorStatus |= 1;
            }
            else {
                prozessorStatus &= 0b1111'1110;
            }
            A = A << 1;
            if (rememberCarry) {
                A |= 1;
            }
            negativeZeroCheck(A);
            return;
        }
        return;
    }
    fetch();
}

void Cpu::bitTestWithAbsolute() {
    bitTest(&Cpu::absolute);
}

void Cpu::andWithAbsolute() {
    andA(&Cpu::absolute);
}

void Cpu::rotateLeftAbsolute() {
    rotateLeft(&Cpu::absolute);
}

void Cpu::branchOnBitReset2() {
    branchOnBitReset(2);
}

void Cpu::branchIfResultMinus() {
    bool negative = 0x80;
    branchIf(negative);
}

void Cpu::andWithZeroPageIndirectWithY() {
    andA(&Cpu::zeroPageIndirectWithY);
}

void Cpu::andWithZeroPageIndirect() {
    andA(&Cpu::zeroPageIndirect);
}

void Cpu::bitTestWithZeroPageWithX() {
    bitTest(&Cpu::zeroPageWithX);
}

void Cpu::andWithZeroPageWithX() {
    andA(&Cpu::zeroPageWithX);
}

void Cpu::rotateLeftZeroPageWithX() {
    rotateLeft(&Cpu::zeroPageWithX);
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
    andA(&Cpu::absoluteWithY);
}

void Cpu::decrementA() {
    if (TCU == 1) {
        if (!PHI2) {
            --A;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::bitTestWithAbsoluteWithX() {
    bitTest(&Cpu::absoluteWithX);
}

void Cpu::andWithAbsoluteWithX() {
    andA(&Cpu::absoluteWithX);
}

void Cpu::rotateLeftAbsoluteWithX() {
    rotateLeft(&Cpu::absoluteWithX);
}

void Cpu::branchOnBitReset3() {
    branchOnBitReset(3);
}

void Cpu::returnFromInterupt() {
    throw std::runtime_error("interupt not implemented");
}

void Cpu::xorWithZeroPageWithXIndirect() {
    xorA(&Cpu::zeroPageWithXIndirect);
}

void Cpu::xorZeroPage() {
    xorA(&Cpu::zeroPage);
}

void Cpu::shiftRightZeroPage() {
    shiftRight(&Cpu::zeroPage);
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
            A = A >> 1;
            negativeZeroCheck(A);
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
    xorA(&Cpu::absolute);
}

void Cpu::shiftRightAbsolute() {
    shiftRight(&Cpu::absolute);
}

void Cpu::branchOnBitReset4() {
    branchOnBitReset(4);
}

void Cpu::branchIfOverflowClear() {
    bool overflow = prozessorStatus & 0x40;
    branchIf(!overflow);
}

void Cpu::xorWithZeroPageIndirectWithY() {
    xorA(&Cpu::zeroPageIndirectWithY);
}

void Cpu::xorWithZeroPageIndirect() {
    xorA(&Cpu::zeroPageIndirect);
}

void Cpu::xorWithZeroPageWithX() {
    xorA(&Cpu::zeroPageWithX);
}

void Cpu::shiftRightZeroPageWithX() {
    shiftRight(&Cpu::zeroPageWithX);
}

void Cpu::resetMemoryBit5() {
    resetMemoryBit(5);
}

void Cpu::clearInteruptDisableFlag() {
    if (TCU == 1) {
        if (!PHI2) {
            prozessorStatus &= 0b1111'1011;
        }
        return;
    }
    fetch();
}

void Cpu::xorWithAbsoluteWithY() {
    xorA(&Cpu::absoluteWithY);
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
    xorA(&Cpu::absoluteWithX);
}

void Cpu::shiftRightAbsoluteWithX() {
    shiftRight(&Cpu::absoluteWithX);
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
    addWithCarry(&Cpu::zeroPageWithXIndirect);
}

void Cpu::store0AtZeroPage() {
    store(0, &Cpu::zeroPage);
}

void Cpu::addWithZeroPage() {
    addWithCarry(&Cpu::zeroPage);
}

void Cpu::rotateRightZeroPage() {
    rotateRight(&Cpu::zeroPage);
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
                prozessorStatus &= 0b1111'1110;
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
        programCounter = instructionBufferHigh * 0x100 + instructionBufferLow;
        return;
    }
    fetch();
}

void Cpu::addWithAbsolute() {
    addWithCarry(&Cpu::absolute);
}

void Cpu::rotateRightAbsolute() {
    rotateRight(&Cpu::absolute);
}

void Cpu::branchOnBitReset6() {
    branchOnBitReset(6);
}

void Cpu::branchIfOverflowSet() {
    bool overflow = prozessorStatus & 0x40;
    branchIf(overflow);
}

void Cpu::addWithZeroPageIndirectWithY() {
    addWithCarry(&Cpu::zeroPageIndirectWithY);
}

void Cpu::addWithZeroPageIndirect() {
    addWithCarry(&Cpu::zeroPageIndirect);
}

void Cpu::store0AtZeroPageWithX() {
    store(0, &Cpu::zeroPageWithX);
}

void Cpu::addWithZeroPageWithX() {
    addWithCarry(&Cpu::zeroPageWithX);
}

void Cpu::rotateRightZeroPageWithX() {
    rotateRight(&Cpu::zeroPageWithX);
}

void Cpu::resetMemoryBit7() {
    resetMemoryBit(7);
}

void Cpu::setInteruptDisable() {
    if (TCU == 1) {
        if (!PHI2) {
            prozessorStatus |= 0b0000'0100;
        }
    }
    fetch();
}

void Cpu::addWithAbsoluteWithY() {
    addWithCarry(&Cpu::absoluteWithY);
}

void Cpu::pullY() {
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
        Y = dataBuffer;
        return;
    }
    fetch();
}

void Cpu::jumpAbsoluteWithXIndirect() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            ++programCounter;
            return;
        }
        instructionBufferLow = dataBuffer;
        return;
    }
    if (TCU == 2) {
        if (!PHI2) {
            addressBuffer = programCounter;
            ++programCounter;
            return;
        }
        instructionBufferHigh = dataBuffer;
        return;
    }
    if (TCU == 3) {
        if (!PHI2) {
            addressBuffer = 0x100 * instructionBufferHigh + instructionBufferLow;
        }
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            addressBuffer += X;
            return;
        }
        instructionBufferLow = dataBuffer;
        return;
    }
    if (TCU == 5) {
        if (!PHI2) {
            ++addressBuffer;
            return;
        }
        instructionBufferHigh = dataBuffer;
        programCounter = 0x100 * instructionBufferHigh + instructionBufferLow;
        return;
    }
    fetch();
}

void Cpu::addWithAbsoluteWithX() {
    addWithCarry(&Cpu::absoluteWithX);
}

void Cpu::rotateRightAbsoluteWithX() {
    rotateRight(&Cpu::absoluteWithX);
}

void Cpu::branchOnBitReset7() {
    branchOnBitReset(7);
}

void Cpu::branchAlways() {
    branchIf(true);
}

void Cpu::storeAAtZeroPageWithXIndirect() {
    store(A, &Cpu::zeroPageWithXIndirect);
}

void Cpu::storeYAtZeroPage() {
    store(Y, &Cpu::zeroPage);
}

void Cpu::storeAAtZeroPage() {
    store(A, &Cpu::zeroPage);
}

void Cpu::storeXAtZeroPage() {
    store(X, &Cpu::zeroPage);
}

void Cpu::setMemoryBit0() {
    setMemoryBit(0);
}

void Cpu::decrementY() {
    if (TCU == 1) {
        if (!PHI2) {
            --Y;
            negativeZeroCheck(Y);
        }
        return;
    }
    fetch();
}

void Cpu::bitTestWithImediate() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            ++programCounter;
            return;
        }
        bitTest(dataBuffer);
    }
    fetch();
}

void Cpu::transferXtoA() {
    if (TCU == 1) {
        if (!PHI2) {
            A = X;
        }
        return;
    }
    fetch();
}

void Cpu::storeYAtAbsolute() {
    store(Y, &Cpu::absolute);
}

void Cpu::storeAAtAbsolute() {
    store(A, &Cpu::absolute);
}

void Cpu::storeXAtAbsolute() {
    store(X, &Cpu::absolute);
}

void Cpu::branchOnBitSet0() {
    branchOnBitSet(0);
}

void Cpu::branchIfCarryClear() {
    bool carry = prozessorStatus & 1;
    branchIf(!carry);
}

void Cpu::storeAAtZeroPageIndirectWithY() {
    store(A, &Cpu::zeroPageIndirectWithY);
}

void Cpu::storeAAtZeroPageIndirect() {
    store(A, &Cpu::zeroPageIndirect);
}

void Cpu::storeYAtZeroPageWithX() {
    store(Y, &Cpu::zeroPageWithX);
}

void Cpu::storeAAtZeroPageWithX() {
    store(A, &Cpu::zeroPageWithX);
}

void Cpu::storeXAtZeroPageWithY() {
    store(X, &Cpu::zeroPageWithX);
}

void Cpu::setMemoryBit1() {
    setMemoryBit(1);
}

void Cpu::transferYToA() {
    if (TCU == 1) {
        if (!PHI2) {
            A = Y;
            return;
        }
        return;
    }
    fetch();
}

void Cpu::storeAAtAbsoluteWithY() {
    store(A, &Cpu::absoluteWithY);
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

void Cpu::store0AtAbsolute() {
    store(0, &Cpu::absolute);
}

void Cpu::storeAAtAbsoluteWithX() {
    store(A, &Cpu::absoluteWithX);
}

void Cpu::store0AtAbsoluteWithX() {
    store(0, &Cpu::absoluteWithX);
}

void Cpu::branchOnBitSet1() {
    branchOnBitSet(1);
}

void Cpu::loadYImediate() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return;
        }
        Y = dataBuffer;
        negativeZeroCheck(X);
        ++programCounter;
        return;
    }
    fetch();
}

void Cpu::loadAZeroPageWithXIndirect() {
    load(A, &Cpu::zeroPageWithXIndirect);
}

void Cpu::loadXImidiate() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return;
        }
        X = dataBuffer;
        negativeZeroCheck(X);
        ++programCounter;
        return;
    }
    fetch();
}

void Cpu::loadYZeroPage() {
    load(Y, &Cpu::zeroPage);
}

void Cpu::loadAZeroPage() {
    load(A, &Cpu::zeroPage);
}

void Cpu::loadXZeroPage() {
    load(X, &Cpu::zeroPage);
}

void Cpu::setMemoryBit2() {
    setMemoryBit(2);
}

void Cpu::transferAToY() {
    if (TCU == 1) {
        if (!PHI2) {
            Y = A;
        }
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
        negativeZeroCheck(A);
        ++programCounter;
        return;
    }
    fetch();
}

void Cpu::transferAToX() {
    if (TCU == 1) {
        if (!PHI2) {
            X = A;
        }
        return;
    }
    fetch();
}

void Cpu::loadYAbsolute() {
    load(Y, &Cpu::absolute);
}

void Cpu::loadAAbsolute() {
    load(A, &Cpu::absolute);
}

void Cpu::loadXAbsolute() {
    load(X, &Cpu::absolute);
}

void Cpu::branchOnBitSet2() {
    branchOnBitSet(2);
}

void Cpu::branchIfCarrySet() {
    bool carry = prozessorStatus & 1;
    branchIf(carry);
}

void Cpu::loadAZeroPageIndirectWithY() {
    load(A, &Cpu::zeroPageIndirectWithY);
}

void Cpu::loadAZeroPageIndirect() {
    load(A, &Cpu::zeroPageIndirect);
}

void Cpu::loadYZeroPageWithX() {
    load(Y, &Cpu::zeroPageWithX);
}

void Cpu::loadAZeroPageWithX() {
    load(A, &Cpu::zeroPageWithX);
}

void Cpu::loadXZeroPageWithY() {
    load(X, &Cpu::zeroPageWithY);
}

void Cpu::setMemoryBit3() {
    setMemoryBit(3);
}

void Cpu::clearOverflow() {
    if (TCU == 1) {
        if (!PHI2) {
            prozessorStatus &= 0b1111'1110;
        }
        return;
    }
    fetch();
}

void Cpu::loadAAbsoluteWithY() {
    load(A, &Cpu::absoluteWithY);
}

void Cpu::transferStackpointerToX() {
    if (TCU == 1) {
        if (!PHI2) {
            X = stackPointer;
        }
        return;
    }
    fetch();
}

void Cpu::loadYAbsoluteWithX() {
    load(Y, &Cpu::absoluteWithX);
}

void Cpu::loadAAbsoluteWithX() {
    load(A, &Cpu::absoluteWithX);
}

void Cpu::loadXAbsoluteWithY() {
    load(X, &Cpu::absoluteWithY);
}

void Cpu::branchOnBitSet3() {
    branchOnBitSet(3);
}

void Cpu::compareYWithImediate() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            ++programCounter;
            return;
        }
        compare(Y, dataBuffer);
        return;
    }
    fetch();
}

void Cpu::compareAWithZeroPageWithXIndirect() {
    compare(A, &Cpu::zeroPageWithXIndirect);
}

void Cpu::compareYWithZeroPage() {
    compare(Y, &Cpu::zeroPage);
}

void Cpu::compareAWithZeroPage() {
    compare(A, &Cpu::zeroPage);
}

void Cpu::decrementZeroPage() {
    decrement(&Cpu::zeroPage);
}

void Cpu::setMemoryBit4() {
    setMemoryBit(4);
}

void Cpu::incrementY() {
    if (TCU == 1) {
        if (!PHI2) {
            ++Y;
            negativeZeroCheck(Y);
        }
        return;
    }
    fetch();
}

void Cpu::compareAWithImediate() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            ++programCounter;
            return;
        }
        compare(A, dataBuffer);
        return;
    }
    fetch();
}

void Cpu::decrementX() {
    if (TCU == 1) {
        if (!PHI2) {
            --X;
            negativeZeroCheck(X);
        }
        return;
    }
    fetch();
}

void Cpu::waitForInterupt() {
    throw std::runtime_error("interupt not implemented");
}

void Cpu::compareYWithAbsolute() {
    compare(Y, &Cpu::absolute);
}

void Cpu::compareAWithAbsolute() {
    compare(A, &Cpu::absolute);
}

void Cpu::decrementAbsolute() {
    decrement(&Cpu::absolute);
}

void Cpu::branchOnBitSet4() {
    branchOnBitSet(4);
}

void Cpu::branchNotEqual() {
    bool equal = prozessorStatus & 2; // zero flag
    branchIf(!equal);
}

void Cpu::compareAWithZeroPageIndirectWithY() {
    compare(A, &Cpu::zeroPageIndirectWithY);
}

void Cpu::compareAWithZeroPageIndirect() {
    compare(A, &Cpu::zeroPageIndirect);
}

void Cpu::compareAWithZeroPageWithX() {
    compare(A, &Cpu::zeroPageWithX);
}

void Cpu::decrementZeroPageWithX() {
    decrement(&Cpu::zeroPageWithX);
}

void Cpu::setMemoryBit5() {
    setMemoryBit(5);
}

void Cpu::clearDecimalMode() {
    if (TCU == 1) {
        if (!PHI2) {
            prozessorStatus &= 0b1111'0111;
        }
        return;
    }
    fetch();
}

void Cpu::compareAWithAbsoluteWithY() {
    compare(A, &Cpu::absoluteWithY);
}

void Cpu::pushX() {
    if (TCU == 1) {
        addressBuffer = 0x100 + stackPointer;
        return;
    }
    if (TCU == 2) {
        if (!PHI2) {
            push(X);
        }
        return;
    }
    fetch();
}

void Cpu::stopMode() {
    // does nothing stays this way until reset
}

void Cpu::compareAWithAbsoluteWithX() {
    compare(A, &Cpu::absoluteWithX);
}

void Cpu::decrementAbsoluteWithX() {
    decrement(&Cpu::absoluteWithX);
}

void Cpu::branchOnBitSet5() {
    branchOnBitSet(5);
}

void Cpu::compareXWithImediate() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            ++programCounter;
            return;
        }
        compare(X, dataBuffer);
        return;
    }
    fetch();
}

void Cpu::subtractWithZeroPageWithXIndirect() {
    subtractWithCarry(&Cpu::zeroPageWithXIndirect);
}

void Cpu::compareXWithZeroPage() {
    compare(X, &Cpu::zeroPage);
}

void Cpu::subtractWithZeroPage() {
    subtractWithCarry(&Cpu::zeroPage);
}

void Cpu::incrementZeroPage() {
    increment(&Cpu::zeroPage);
}

void Cpu::setMemoryBit6() {
    setMemoryBit(6);
}

void Cpu::incrementX() {
    if (TCU == 1) {
        if (!PHI2) {
            ++X;
            negativeZeroCheck(X);
        }
        return;
    }
    fetch();
}

void Cpu::subtractWithImediate() {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            ++programCounter;
            return;
        }
        subtractWithCarry(dataBuffer);
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

void Cpu::compareXWithAbsolute() {
    compare(X, &Cpu::absolute);
}

void Cpu::subtractWithAbsolute() {
    subtractWithCarry(&Cpu::absolute);
}

void Cpu::incrementAbsolute() {
    increment(&Cpu::absolute);
}

void Cpu::branchOnBitSet6() {
    branchOnBitSet(6);
}

void Cpu::branchIfEqual() {
    bool equal = prozessorStatus & 2; // zero flag
    branchIf(equal);
}

void Cpu::subtractWithZeroPageIndirectWithY() {
    subtractWithCarry(&Cpu::zeroPageIndirectWithY);
}

void Cpu::subtractWithZeroPageIndirect() {
    subtractWithCarry(&Cpu::zeroPageIndirect);
}

void Cpu::subtractWithZeroPageWithX() {
    subtractWithCarry(&Cpu::zeroPageWithX);
}

void Cpu::incrementZeroPageWithX() {
    increment(&Cpu::zeroPageWithX);
}

void Cpu::setMemoryBit7() {
    setMemoryBit(7);
}

void Cpu::setDecimalMode() {
    if (TCU == 1) {
        if (!PHI2) {
            prozessorStatus |= 0b0000'1000;
        }
        return;
    }
    fetch();
}

void Cpu::subtractWithAbsoluteWithY() {
    subtractWithCarry(&Cpu::absoluteWithY);
}

void Cpu::pullX() {
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
        X = dataBuffer;
        return;
    }
    fetch();
}

void Cpu::subtractWithAbsoluteWithX() {
    subtractWithCarry(&Cpu::absoluteWithX);
}

void Cpu::incrementAbsoluteWithX() {
    increment(&Cpu::absoluteWithX);
}

void Cpu::branchOnBitSet7() {
    branchOnBitSet(7);
}

Cpu::Cpu(unsigned char& _data, unsigned short& _address, bool& _VPB, bool& _RDY, const bool& _IRQB, bool& _MLB, const bool& _NMIB,
    bool& _SYNC, bool& _RWB, const bool& _BE, const bool& _SOB, const bool& _PHI2, bool& _PHI1O, bool& _PHI2O) :
    data{ _data }, address{ _address }, VPB{ _VPB }, RDY{ _RDY }, IRQB{ _IRQB }, MLB{ _MLB }, NMIB{ _NMIB },
    SYNC{ _SYNC }, RWB{ _RWB }, BE{ _BE }, SOB{ _SOB }, PHI2{ _PHI2 }, PHI1O{ _PHI1O }, PHI2O{ _PHI2O } {}

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
    if (!NMIB) {
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
        storeAAtAbsoluteWithY();
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
    prozessorStatus &= 0b1111'0111;
    prozessorStatus |= 0b0011'0100;
    resetTimer = 7;
    IR = 0x6C;
}
