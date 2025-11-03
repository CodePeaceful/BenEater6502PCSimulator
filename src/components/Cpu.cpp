#include "Cpu.hpp"

#include <stdexcept>
#include <functional>

#include <print>

#include "ALUFunctions.hpp"

void Cpu::fetch() noexcept {
    if (interruptDemanded) {
        TCU = 0;
        handleNonMaskableInterrupt();
        return;
    }
    if (interruptRequested) {
        TCU = 0;
        handleInterruptRequest();
        return;
    }
    if (!PHI2) {
        addressBuffer = programCounter;
        readWriteBuffer = true;
        return;
    }
    TCU = 0;
    writeBackCounter = 0;
    IR = dataBuffer;
    ++programCounter;
}

void Cpu::toBus() const noexcept {
    if (BE) {
        address = addressBuffer;
        RWB = readWriteBuffer;
        if (!readWriteBuffer) {
            data = dataBuffer;
        }
    }
}

void Cpu::handleInterruptRequest() noexcept {
    if (TCU == 0) {
        followingRequest = true;
        return;
    }
    if (TCU == 1) {
        return;
    }
    if (TCU == 2) {
        if (!PHI2) {
            const unsigned char pch = programCounter >> 8;
            push(pch);
        }
        return;
    }
    if (TCU == 3) {
        if (!PHI2) {
            const unsigned char pcl = programCounter % 256;
            push(pcl);
            return;
        }
        VPB = false;
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            push(processorStatus);
        }
        return;
    }
    if (TCU == 5) {
        if (!PHI2) {
            readWriteBuffer = true;
            addressBuffer = 0xfffe;
            return;
        }
        instructionBufferLow = dataBuffer;
        processorStatus |= 4;
        interruptRequested = false;
        return;
    }
    if (TCU == 6) {
        if (!PHI2) {
            addressBuffer = 0xffff;
            return;
        }
        instructionBufferHigh = dataBuffer;
        programCounter = 0x100 * instructionBufferHigh + instructionBufferLow;
        VPB = true;
        return;
    }
    fetch();
    if (PHI2) {
        followingRequest = false;
    }
}

void Cpu::handleNonMaskableInterrupt() noexcept {
    if (TCU == 0) {
        followingOrder = true;
        return;
    }
    if (TCU == 1) {
        interruptDemanded = false;
        return;
    }
    if (TCU == 2) {
        if (!PHI2) {
            unsigned char pch = programCounter >> 8;
            push(pch);
        }
        return;
    }
    if (TCU == 3) {
        if (!PHI2) {
            unsigned char pcl = programCounter % 256;
            push(pcl);
            return;
        }
        VPB = false;
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            push(processorStatus);
        }
        return;
    }
    if (TCU == 5) {
        if (!PHI2) {
            readWriteBuffer = true;
            addressBuffer = 0xfffa;
            return;
        }
        instructionBufferLow = dataBuffer;
        processorStatus |= 4;
        return;
    }
    if (TCU == 6) {
        if (!PHI2) {
            addressBuffer = 0xfffb;
            return;
        }
        instructionBufferHigh = dataBuffer;
        programCounter = 0x100 * instructionBufferHigh + instructionBufferLow;
        VPB = true;
        return;
    }
    fetch();
    if (PHI2) {
        followingOrder = false;
    }
}

void Cpu::negativeZeroCheck(unsigned char value) noexcept {
    if (value == 0) {
        processorStatus |= 0b0000'0010;
    }
    else {
        processorStatus &= 0b1111'1101;
    }
    if (value & 0x80) {
        processorStatus |= 0b1000'0000;
    }
    else {
        processorStatus &= 0b0111'1111;
    }
}

void Cpu::addWithCarry(bool (Cpu::* address)()) noexcept {
    if (!std::invoke(address, *this)) {
        return;
    }
    if (writeBackCounter == 0) {
        if (!PHI2) {
            return;
        }
        ++writeBackCounter;
        A = ALUFunctions::addWithCarry(A, dataBuffer, processorStatus);
        return;
    }
    fetch();
}

void Cpu::subtractWithCarry(bool (Cpu::* address)()) noexcept {
    if (!std::invoke(address, *this)) {
        return;
    }
    if (writeBackCounter == 0) {
        if (!PHI2) {
            return;
        }
        ++writeBackCounter;
        A = ALUFunctions::subtractWithCarry(A, dataBuffer, processorStatus);
        return;
    }
    fetch();
}

void Cpu::orA(bool (Cpu::* address)()) noexcept {
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

void Cpu::andA(bool (Cpu::* address)()) noexcept {
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

void Cpu::xorA(bool (Cpu::* address)()) noexcept {
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

void Cpu::load(unsigned char& cpuRegister, bool (Cpu::* address)()) noexcept {
    if (!std::invoke(address, *this)) {
        return;
    }
    if (writeBackCounter == 0) {
        if (!PHI2) {
            return;
        }
        ++writeBackCounter;
        cpuRegister = dataBuffer;
        negativeZeroCheck(cpuRegister);
        return;
    }
    fetch();
}

void Cpu::store(const unsigned char value, bool (Cpu::* address)()) noexcept {
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

void Cpu::bitTest(bool (Cpu::* address)()) noexcept {
    if (!std::invoke(address, *this)) {
        return;
    }
    if (writeBackCounter == 0) {
        if (!PHI2) {
            return;
        }
        ALUFunctions::bitTest(A, processorStatus, dataBuffer);
        ++writeBackCounter;
        return;
    }
    fetch();
}

void Cpu::compare(const unsigned char first, bool (Cpu::* address)()) noexcept {
    if (!std::invoke(address, *this)) {
        return;
    }
    if (writeBackCounter == 0) {
        if (!PHI2) {
            return;
        }
        ALUFunctions::compare(first, dataBuffer, processorStatus);
        ++writeBackCounter;
        return;
    }
    fetch();
}

void Cpu::testAndSetMemoryBit(bool (Cpu::* address)()) noexcept {
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
                processorStatus |= 2;
            }
            else {
                processorStatus &= 0b1111'1101;
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

void Cpu::testAndResetMemoryBit(bool (Cpu::* address)()) noexcept {
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
                processorStatus |= 2;
            }
            else {
                processorStatus &= 0b1111'1101;
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

void Cpu::rotateRight(bool (Cpu::* address)()) noexcept {
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
            const bool rememberCarry = processorStatus & 1;
            if (instructionBufferLow & 1) {
                processorStatus |= 1;
            }
            else {
                processorStatus &= 0b1111'1110;
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

void Cpu::rotateLeft(bool (Cpu::* address)()) noexcept {
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
            const bool rememberCarry = processorStatus & 1;
            if (instructionBufferLow & 0x80) {
                processorStatus |= 1;
            }
            else {
                processorStatus &= 0b1111'1110;
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

void Cpu::shiftRight(bool (Cpu::* address)()) noexcept {
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
            if ([[maybe_unused]] bool carry = instructionBufferLow & 1) {
                processorStatus |= 1;
            }
            else {
                processorStatus &= 0b1111'1110;
            }
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

void Cpu::shiftLeft(bool (Cpu::* address)()) noexcept {
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
            if ([[maybe_unused]] bool carry = instructionBufferLow & 0x80) {
                processorStatus |= 1;
            }
            else {
                processorStatus &= 0b1111'1110;
            }
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

void Cpu::increment(bool (Cpu::* address)()) noexcept {
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

void Cpu::decrement(bool (Cpu::* address)()) noexcept {
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

void Cpu::resetMemoryBit(const unsigned char bitId) noexcept {
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

void Cpu::setMemoryBit(const unsigned char bitId) noexcept {
    if (TCU >= 2) {
        zeroPage();
        if (TCU == 2 && PHI2) {
            instructionBufferLow = dataBuffer;
        }
        return;
    }
    if (TCU == 3) {
        const unsigned char thisBit = 1 << bitId;
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

void Cpu::branchOnBitReset(const unsigned char bitId) noexcept {
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

void Cpu::branchOnBitSet(const unsigned char bitId) noexcept {
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
        if (const unsigned char thisBit = 1 << bitId; thisBit & instructionBufferHigh) {
            if (!PHI2) {
                programCounter += instructionBufferLow;
            }
            return;
        }
    }
    fetch();
}

void Cpu::branchIf(const bool condition) noexcept {
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
                if (instructionBufferLow & 0xf0) {
                    programCounter -= 256;
                }
            }
            return;
        }
    }
    fetch();
}

void Cpu::push(const unsigned char data) noexcept {
    addressBuffer = 0x100 + stackPointer;
    readWriteBuffer = false;
    dataBuffer = data;
    --stackPointer;
}

void Cpu::pull() noexcept {
    ++stackPointer;
    addressBuffer = 0x100 + stackPointer;
}

bool Cpu::zeroPage() noexcept {
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

bool Cpu::zeroPageIndirect() noexcept {
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

bool Cpu::zeroPageWithXIndirect() noexcept {
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

bool Cpu::zeroPageIndirectWithY() noexcept {
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

bool Cpu::zeroPageWithX() noexcept {
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

bool Cpu::zeroPageWithY() noexcept {
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

bool Cpu::absoluteIndirect() noexcept {
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

bool Cpu::absolute() noexcept {
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

bool Cpu::absoluteWithX() noexcept {
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

bool Cpu::absoluteWithY() noexcept {
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

void Cpu::breakRequest() noexcept {
    throw std::runtime_error("no break");
}

void Cpu::orWithZeroPageWithXIndirect() noexcept {
    orA(&Cpu::zeroPageWithXIndirect);
}

void Cpu::testAndSetMemoryBitZeroPage() noexcept {
    testAndSetMemoryBit(&Cpu::zeroPage);
}

void Cpu::orWithZeroPage() noexcept {
    orA(&Cpu::zeroPage);
}


void Cpu::shiftLeftZeroPage() noexcept {
    shiftLeft(&Cpu::zeroPage);
}

void Cpu::resetMemoryBit0() noexcept {
    resetMemoryBit(0);
}

void Cpu::pushProcessorStatus() noexcept {
    if (TCU == 1) {
        addressBuffer = 0x100 + stackPointer;
        return;
    }
    if (TCU == 2) {
        if (!PHI2) {
            push(processorStatus);
        }
        return;
    }
    fetch();
}

void Cpu::orWithImmediate() noexcept {
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

void Cpu::shiftLeftA() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            if ([[maybe_unused]] bool carry = A & 0x80) {
                processorStatus |= 1;
            }
            else {
                processorStatus &= 0b1111'1110;
            }
            A = A << 1;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::testAndSetMemoryBitAbsolute() noexcept {
    testAndSetMemoryBit(&Cpu::absolute);
}

void Cpu::orWithAbsolute() noexcept {
    orA(&Cpu::absolute);
}

void Cpu::shiftLeftAbsolute() noexcept {
    shiftLeft(&Cpu::absolute);
}

void Cpu::branchOnBitReset0() noexcept {
    branchOnBitReset(0);
}

void Cpu::branchIfResultPlus() noexcept {
    bool negative = processorStatus & 0x80;
    branchIf(!negative);
}

void Cpu::orWithZeroPageIndirectWithY() noexcept {
    orA(&Cpu::zeroPageIndirectWithY);
}

void Cpu::orWithZeroPageIndirect() noexcept {
    orA(&Cpu::zeroPageIndirect);
}

void Cpu::testAndResetMemoryBitZeroPage() noexcept {
    testAndResetMemoryBit(&Cpu::zeroPage);
}

void Cpu::orWithZeroPageWithX() noexcept {
    orA(&Cpu::zeroPageWithX);
}

void Cpu::shiftLeftZeroPageWithX() noexcept {
    shiftLeft(&Cpu::zeroPageWithX);
}

void Cpu::resetMemoryBit1() noexcept {
    resetMemoryBit(1);
}

void Cpu::clearCarry() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            processorStatus &= 0b1111'1110;
        }
        return;
    }
    fetch();
}

void Cpu::orWithAbsoluteWithY() noexcept {
    orA(&Cpu::absoluteWithY);
}

void Cpu::incrementA() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            ++A;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::testAndResetMemoryBitAbsolute() noexcept {
    testAndResetMemoryBit(&Cpu::absolute);
}

void Cpu::orWithAbsoluteWithX() noexcept {
    orA(&Cpu::absoluteWithX);
}

void Cpu::shiftLeftAbsoluteWithX() noexcept {
    shiftLeft(&Cpu::absoluteWithX);
}

void Cpu::branchOnBitReset1() noexcept {
    branchOnBitReset(1);
}

void Cpu::jumpToSubroutine() noexcept {
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

void Cpu::andWithZeroPageWithXIndirect() noexcept {
    andA(&Cpu::zeroPageWithXIndirect);
}

void Cpu::bitTestWithZeroPage() noexcept {
    bitTest(&Cpu::zeroPage);
}

void Cpu::andWithZeroPage() noexcept {
    andA(&Cpu::zeroPage);
}

void Cpu::rotateLeftZeroPage() noexcept {
    rotateLeft(&Cpu::zeroPage);
}

void Cpu::resetMemoryBit2() noexcept {
    resetMemoryBit(2);
}

void Cpu::pullProcessorStatus() noexcept {
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
        processorStatus = dataBuffer;
        return;
    }
    fetch();
}

void Cpu::andWithImmediate() noexcept {
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

void Cpu::rotateLeftA() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            bool rememberCarry = processorStatus & 0x80;
            if (A & 1) {
                processorStatus |= 1;
            }
            else {
                processorStatus &= 0b1111'1110;
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

void Cpu::bitTestWithAbsolute() noexcept {
    bitTest(&Cpu::absolute);
}

void Cpu::andWithAbsolute() noexcept {
    andA(&Cpu::absolute);
}

void Cpu::rotateLeftAbsolute() noexcept {
    rotateLeft(&Cpu::absolute);
}

void Cpu::branchOnBitReset2() noexcept {
    branchOnBitReset(2);
}

void Cpu::branchIfResultMinus() noexcept {
    const bool negative = processorStatus & 0x80;
    branchIf(negative);
}

void Cpu::andWithZeroPageIndirectWithY() noexcept {
    andA(&Cpu::zeroPageIndirectWithY);
}

void Cpu::andWithZeroPageIndirect() noexcept {
    andA(&Cpu::zeroPageIndirect);
}

void Cpu::bitTestWithZeroPageWithX() noexcept {
    bitTest(&Cpu::zeroPageWithX);
}

void Cpu::andWithZeroPageWithX() noexcept {
    andA(&Cpu::zeroPageWithX);
}

void Cpu::rotateLeftZeroPageWithX() noexcept {
    rotateLeft(&Cpu::zeroPageWithX);
}

void Cpu::resetMemoryBit3() noexcept {
    resetMemoryBit(3);
}

void Cpu::setCarry() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            processorStatus |= 0x1;
        }
        return;
    }
    fetch();
}

void Cpu::andWithAbsoluteWithY() noexcept {
    andA(&Cpu::absoluteWithY);
}

void Cpu::decrementA() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            --A;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::bitTestWithAbsoluteWithX() noexcept {
    bitTest(&Cpu::absoluteWithX);
}

void Cpu::andWithAbsoluteWithX() noexcept {
    andA(&Cpu::absoluteWithX);
}

void Cpu::rotateLeftAbsoluteWithX() noexcept {
    rotateLeft(&Cpu::absoluteWithX);
}

void Cpu::branchOnBitReset3() noexcept {
    branchOnBitReset(3);
}

void Cpu::returnFromInterrupt() noexcept {
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
        processorStatus = dataBuffer;
        return;
    }
    if (TCU == 4) {
        if (!PHI2) {
            pull();
            return;
        }
        instructionBufferLow = dataBuffer;
        return;
    }
    if (TCU == 5) {
        if (!PHI2) {
            pull();
            return;
        }
        instructionBufferHigh = dataBuffer;
        programCounter = instructionBufferHigh * 0x100 + instructionBufferLow;
        return;
    }
    fetch();
}

void Cpu::xorWithZeroPageWithXIndirect() noexcept {
    xorA(&Cpu::zeroPageWithXIndirect);
}

void Cpu::xorZeroPage() noexcept {
    xorA(&Cpu::zeroPage);
}

void Cpu::shiftRightZeroPage() noexcept {
    shiftRight(&Cpu::zeroPage);
}

void Cpu::resetMemoryBit4() noexcept {
    resetMemoryBit(4);
}

void Cpu::pushA() noexcept {
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

void Cpu::xorImmediate() noexcept {
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

void Cpu::shiftRightA() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            if ([[maybe_unused]] bool carry = A & 1) {
                processorStatus |= 1;
            }
            else {
                processorStatus &= 0b1111'1110;
            }
            A = A >> 1;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::jumpAbsolute() noexcept {
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

void Cpu::xorWithAbsolute() noexcept {
    xorA(&Cpu::absolute);
}

void Cpu::shiftRightAbsolute() noexcept {
    shiftRight(&Cpu::absolute);
}

void Cpu::branchOnBitReset4() noexcept {
    branchOnBitReset(4);
}

void Cpu::branchIfOverflowClear() noexcept {
    const bool overflow = processorStatus & 0x40;
    branchIf(!overflow);
}

void Cpu::xorWithZeroPageIndirectWithY() noexcept {
    xorA(&Cpu::zeroPageIndirectWithY);
}

void Cpu::xorWithZeroPageIndirect() noexcept {
    xorA(&Cpu::zeroPageIndirect);
}

void Cpu::xorWithZeroPageWithX() noexcept {
    xorA(&Cpu::zeroPageWithX);
}

void Cpu::shiftRightZeroPageWithX() noexcept {
    shiftRight(&Cpu::zeroPageWithX);
}

void Cpu::resetMemoryBit5() noexcept {
    resetMemoryBit(5);
}

void Cpu::clearInterruptDisableFlag() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            processorStatus &= 0b1111'1011;
        }
        return;
    }
    fetch();
}

void Cpu::xorWithAbsoluteWithY() noexcept {
    xorA(&Cpu::absoluteWithY);
}

void Cpu::pushY() noexcept {
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

void Cpu::xorWithAbsoluteWithX() noexcept {
    xorA(&Cpu::absoluteWithX);
}

void Cpu::shiftRightAbsoluteWithX() noexcept {
    shiftRight(&Cpu::absoluteWithX);
}

void Cpu::branchOnBitReset5() noexcept {
    branchOnBitReset(5);
}

void Cpu::returnFromSubroutine() noexcept {
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

void Cpu::addWithZeroPageWithXIndirect() noexcept {
    addWithCarry(&Cpu::zeroPageWithXIndirect);
}

void Cpu::store0AtZeroPage() noexcept {
    store(0, &Cpu::zeroPage);
}

void Cpu::addWithZeroPage() noexcept {
    addWithCarry(&Cpu::zeroPage);
}

void Cpu::rotateRightZeroPage() noexcept {
    rotateRight(&Cpu::zeroPage);
}

void Cpu::resetMemoryBit6() noexcept {
    resetMemoryBit(6);
}

void Cpu::pullA() noexcept {
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
        negativeZeroCheck(A);
        return;
    }
    fetch();
}

void Cpu::addWithImmediate() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            ++programCounter;
            return;
        }
        A = ALUFunctions::addWithCarry(A, dataBuffer, processorStatus);
        return;
    }
    fetch();
}

void Cpu::rotateRightA() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            const bool rememberCarry = processorStatus & 1;
            if (A & 1) {
                processorStatus |= 1;
            }
            else {
                processorStatus &= 0b1111'1110;
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

void Cpu::jumpAbsoluteIndirect() noexcept {
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

void Cpu::addWithAbsolute() noexcept {
    addWithCarry(&Cpu::absolute);
}

void Cpu::rotateRightAbsolute() noexcept {
    rotateRight(&Cpu::absolute);
}

void Cpu::branchOnBitReset6() noexcept {
    branchOnBitReset(6);
}

void Cpu::branchIfOverflowSet() noexcept {
    bool overflow = processorStatus & 0x40;
    branchIf(overflow);
}

void Cpu::addWithZeroPageIndirectWithY() noexcept {
    addWithCarry(&Cpu::zeroPageIndirectWithY);
}

void Cpu::addWithZeroPageIndirect() noexcept {
    addWithCarry(&Cpu::zeroPageIndirect);
}

void Cpu::store0AtZeroPageWithX() noexcept {
    store(0, &Cpu::zeroPageWithX);
}

void Cpu::addWithZeroPageWithX() noexcept {
    addWithCarry(&Cpu::zeroPageWithX);
}

void Cpu::rotateRightZeroPageWithX() noexcept {
    rotateRight(&Cpu::zeroPageWithX);
}

void Cpu::resetMemoryBit7() noexcept {
    resetMemoryBit(7);
}

void Cpu::setInterruptDisable() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            processorStatus |= 0b0000'0100;
        }
    }
    fetch();
}

void Cpu::addWithAbsoluteWithY() noexcept {
    addWithCarry(&Cpu::absoluteWithY);
}

void Cpu::pullY() noexcept {
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
        negativeZeroCheck(Y);
        return;
    }
    fetch();
}

void Cpu::jumpAbsoluteWithXIndirect() noexcept {
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

void Cpu::addWithAbsoluteWithX() noexcept {
    addWithCarry(&Cpu::absoluteWithX);
}

void Cpu::rotateRightAbsoluteWithX() noexcept {
    rotateRight(&Cpu::absoluteWithX);
}

void Cpu::branchOnBitReset7() noexcept {
    branchOnBitReset(7);
}

void Cpu::branchAlways() noexcept {
    branchIf(true);
}

void Cpu::storeAAtZeroPageWithXIndirect() noexcept {
    store(A, &Cpu::zeroPageWithXIndirect);
}

void Cpu::storeYAtZeroPage() noexcept {
    store(Y, &Cpu::zeroPage);
}

void Cpu::storeAAtZeroPage() noexcept {
    store(A, &Cpu::zeroPage);
}

void Cpu::storeXAtZeroPage() noexcept {
    store(X, &Cpu::zeroPage);
}

void Cpu::setMemoryBit0() noexcept {
    setMemoryBit(0);
}

void Cpu::decrementY() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            --Y;
            negativeZeroCheck(Y);
        }
        return;
    }
    fetch();
}

void Cpu::bitTestWithImmediate() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            ++programCounter;
            return;
        }
        ALUFunctions::bitTest(A, dataBuffer, processorStatus);
    }
    fetch();
}

void Cpu::transferXtoA() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            A = X;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::storeYAtAbsolute() noexcept {
    store(Y, &Cpu::absolute);
}

void Cpu::storeAAtAbsolute() noexcept {
    store(A, &Cpu::absolute);
}

void Cpu::storeXAtAbsolute() noexcept {
    store(X, &Cpu::absolute);
}

void Cpu::branchOnBitSet0() noexcept {
    branchOnBitSet(0);
}

void Cpu::branchIfCarryClear() noexcept {
    bool carry = processorStatus & 1;
    branchIf(!carry);
}

void Cpu::storeAAtZeroPageIndirectWithY() noexcept {
    store(A, &Cpu::zeroPageIndirectWithY);
}

void Cpu::storeAAtZeroPageIndirect() noexcept {
    store(A, &Cpu::zeroPageIndirect);
}

void Cpu::storeYAtZeroPageWithX() noexcept {
    store(Y, &Cpu::zeroPageWithX);
}

void Cpu::storeAAtZeroPageWithX() noexcept {
    store(A, &Cpu::zeroPageWithX);
}

void Cpu::storeXAtZeroPageWithY() noexcept {
    store(X, &Cpu::zeroPageWithX);
}

void Cpu::setMemoryBit1() noexcept {
    setMemoryBit(1);
}

void Cpu::transferYToA() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            A = Y;
            negativeZeroCheck(A);
        }
        return;
    }
    fetch();
}

void Cpu::storeAAtAbsoluteWithY() noexcept {
    store(A, &Cpu::absoluteWithY);
}

void Cpu::transferXToStackpointer() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            stackPointer = X;
            return;
        }
        return;
    }
    fetch();
}

void Cpu::store0AtAbsolute() noexcept {
    store(0, &Cpu::absolute);
}

void Cpu::storeAAtAbsoluteWithX() noexcept {
    store(A, &Cpu::absoluteWithX);
}

void Cpu::store0AtAbsoluteWithX() noexcept {
    store(0, &Cpu::absoluteWithX);
}

void Cpu::branchOnBitSet1() noexcept {
    branchOnBitSet(1);
}

void Cpu::loadYImmediate() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            return;
        }
        Y = dataBuffer;
        negativeZeroCheck(Y);
        ++programCounter;
        return;
    }
    fetch();
}

void Cpu::loadAZeroPageWithXIndirect() noexcept {
    load(A, &Cpu::zeroPageWithXIndirect);
}

void Cpu::loadXImmediate() noexcept {
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

void Cpu::loadYZeroPage() noexcept {
    load(Y, &Cpu::zeroPage);
}

void Cpu::loadAZeroPage() noexcept {
    load(A, &Cpu::zeroPage);
}

void Cpu::loadXZeroPage() noexcept {
    load(X, &Cpu::zeroPage);
}

void Cpu::setMemoryBit2() noexcept {
    setMemoryBit(2);
}

void Cpu::transferAToY() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            Y = A;
            negativeZeroCheck(Y);
        }
        return;
    }
    fetch();
}

void Cpu::loadAImmediate() noexcept {
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

void Cpu::transferAToX() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            X = A;
            negativeZeroCheck(X);
        }
        return;
    }
    fetch();
}

void Cpu::loadYAbsolute() noexcept {
    load(Y, &Cpu::absolute);
}

void Cpu::loadAAbsolute() noexcept {
    load(A, &Cpu::absolute);
}

void Cpu::loadXAbsolute() noexcept {
    load(X, &Cpu::absolute);
}

void Cpu::branchOnBitSet2() noexcept {
    branchOnBitSet(2);
}

void Cpu::branchIfCarrySet() noexcept {
    bool carry = processorStatus & 1;
    branchIf(carry);
}

void Cpu::loadAZeroPageIndirectWithY() noexcept {
    load(A, &Cpu::zeroPageIndirectWithY);
}

void Cpu::loadAZeroPageIndirect() noexcept {
    load(A, &Cpu::zeroPageIndirect);
}

void Cpu::loadYZeroPageWithX() noexcept {
    load(Y, &Cpu::zeroPageWithX);
}

void Cpu::loadAZeroPageWithX() noexcept {
    load(A, &Cpu::zeroPageWithX);
}

void Cpu::loadXZeroPageWithY() noexcept {
    load(X, &Cpu::zeroPageWithY);
}

void Cpu::setMemoryBit3() noexcept {
    setMemoryBit(3);
}

void Cpu::clearOverflow() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            processorStatus &= 0b1111'1110;
        }
        return;
    }
    fetch();
}

void Cpu::loadAAbsoluteWithY() noexcept {
    load(A, &Cpu::absoluteWithY);
}

void Cpu::transferStackpointerToX() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            X = stackPointer;
            negativeZeroCheck(X);
        }
        return;
    }
    fetch();
}

void Cpu::loadYAbsoluteWithX() noexcept {
    load(Y, &Cpu::absoluteWithX);
}

void Cpu::loadAAbsoluteWithX() noexcept {
    load(A, &Cpu::absoluteWithX);
}

void Cpu::loadXAbsoluteWithY() noexcept {
    load(X, &Cpu::absoluteWithY);
}

void Cpu::branchOnBitSet3() noexcept {
    branchOnBitSet(3);
}

void Cpu::compareYWithImmediate() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            ++programCounter;
            return;
        }
        ALUFunctions::compare(Y, dataBuffer, processorStatus);
        return;
    }
    fetch();
}

void Cpu::compareAWithZeroPageWithXIndirect() noexcept {
    compare(A, &Cpu::zeroPageWithXIndirect);
}

void Cpu::compareYWithZeroPage() noexcept {
    compare(Y, &Cpu::zeroPage);
}

void Cpu::compareAWithZeroPage() noexcept {
    compare(A, &Cpu::zeroPage);
}

void Cpu::decrementZeroPage() noexcept {
    decrement(&Cpu::zeroPage);
}

void Cpu::setMemoryBit4() noexcept {
    setMemoryBit(4);
}

void Cpu::incrementY() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            ++Y;
            negativeZeroCheck(Y);
        }
        return;
    }
    fetch();
}

void Cpu::compareAWithImmediate() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            ++programCounter;
            return;
        }
        ALUFunctions::compare(A, dataBuffer, processorStatus);
        return;
    }
    fetch();
}

void Cpu::decrementX() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            --X;
            negativeZeroCheck(X);
        }
        return;
    }
    fetch();
}

void Cpu::waitForInterrupt() noexcept {
    // Documentation confusing
    if (!PHI2) {
        if (interruptDemanded) {
            TCU = 0;
            handleNonMaskableInterrupt();
            return;
        }
        if (interruptRequested) {
            TCU = 0;
            handleInterruptRequest();
            return;
        }
    }
    TCU = 1;
}

void Cpu::compareYWithAbsolute() noexcept {
    compare(Y, &Cpu::absolute);
}

void Cpu::compareAWithAbsolute() noexcept {
    compare(A, &Cpu::absolute);
}

void Cpu::decrementAbsolute() noexcept {
    decrement(&Cpu::absolute);
}

void Cpu::branchOnBitSet4() noexcept {
    branchOnBitSet(4);
}

void Cpu::branchNotEqual() noexcept {
    const bool equal = processorStatus & 2; // zero flag
    branchIf(!equal);
}

void Cpu::compareAWithZeroPageIndirectWithY() noexcept {
    compare(A, &Cpu::zeroPageIndirectWithY);
}

void Cpu::compareAWithZeroPageIndirect() noexcept {
    compare(A, &Cpu::zeroPageIndirect);
}

void Cpu::compareAWithZeroPageWithX() noexcept {
    compare(A, &Cpu::zeroPageWithX);
}

void Cpu::decrementZeroPageWithX() noexcept {
    decrement(&Cpu::zeroPageWithX);
}

void Cpu::setMemoryBit5() noexcept {
    setMemoryBit(5);
}

void Cpu::clearDecimalMode() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            processorStatus &= 0b1111'0111;
        }
        return;
    }
    fetch();
}

void Cpu::compareAWithAbsoluteWithY() noexcept {
    compare(A, &Cpu::absoluteWithY);
}

void Cpu::pushX() noexcept {
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

void Cpu::stopMode() noexcept {
    // does nothing, stays this way until reset
}

void Cpu::compareAWithAbsoluteWithX() noexcept {
    compare(A, &Cpu::absoluteWithX);
}

void Cpu::decrementAbsoluteWithX() noexcept {
    decrement(&Cpu::absoluteWithX);
}

void Cpu::branchOnBitSet5() noexcept {
    branchOnBitSet(5);
}

void Cpu::compareXWithImmediate() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            ++programCounter;
            return;
        }
        ALUFunctions::compare(X, dataBuffer, processorStatus);
        return;
    }
    fetch();
}

void Cpu::subtractWithZeroPageWithXIndirect() noexcept {
    subtractWithCarry(&Cpu::zeroPageWithXIndirect);
}

void Cpu::compareXWithZeroPage() noexcept {
    compare(X, &Cpu::zeroPage);
}

void Cpu::subtractWithZeroPage() noexcept {
    subtractWithCarry(&Cpu::zeroPage);
}

void Cpu::incrementZeroPage() noexcept {
    increment(&Cpu::zeroPage);
}

void Cpu::setMemoryBit6() noexcept {
    setMemoryBit(6);
}

void Cpu::incrementX() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            ++X;
            negativeZeroCheck(X);
        }
        return;
    }
    fetch();
}

void Cpu::subtractWithImmediate() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            addressBuffer = programCounter;
            ++programCounter;
            return;
        }
        A = ALUFunctions::subtractWithCarry(A, dataBuffer, processorStatus);
        return;
    }
    fetch();
}

void Cpu::noop() noexcept {
    if (TCU == 1) {
        return;
    }
    fetch();
}

void Cpu::compareXWithAbsolute() noexcept {
    compare(X, &Cpu::absolute);
}

void Cpu::subtractWithAbsolute() noexcept {
    subtractWithCarry(&Cpu::absolute);
}

void Cpu::incrementAbsolute() noexcept {
    increment(&Cpu::absolute);
}

void Cpu::branchOnBitSet6() noexcept {
    branchOnBitSet(6);
}

void Cpu::branchIfEqual() noexcept {
    bool equal = processorStatus & 2; // zero flag
    branchIf(equal);
}

void Cpu::subtractWithZeroPageIndirectWithY() noexcept {
    subtractWithCarry(&Cpu::zeroPageIndirectWithY);
}

void Cpu::subtractWithZeroPageIndirect() noexcept {
    subtractWithCarry(&Cpu::zeroPageIndirect);
}

void Cpu::subtractWithZeroPageWithX() noexcept {
    subtractWithCarry(&Cpu::zeroPageWithX);
}

void Cpu::incrementZeroPageWithX() noexcept {
    increment(&Cpu::zeroPageWithX);
}

void Cpu::setMemoryBit7() noexcept {
    setMemoryBit(7);
}

void Cpu::setDecimalMode() noexcept {
    if (TCU == 1) {
        if (!PHI2) {
            processorStatus |= 0b0000'1000;
        }
        return;
    }
    fetch();
}

void Cpu::subtractWithAbsoluteWithY() noexcept {
    subtractWithCarry(&Cpu::absoluteWithY);
}

void Cpu::pullX() noexcept {
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
        negativeZeroCheck(X);
        return;
    }
    fetch();
}

void Cpu::subtractWithAbsoluteWithX() noexcept {
    subtractWithCarry(&Cpu::absoluteWithX);
}

void Cpu::incrementAbsoluteWithX() noexcept {
    increment(&Cpu::absoluteWithX);
}

void Cpu::branchOnBitSet7() noexcept {
    branchOnBitSet(7);
}

Cpu::Cpu(unsigned char& _data, unsigned short& _address, bool& _VPB, bool& _RDY, const bool& _IRQB, bool& _MLB,
    const bool& _NMIB,
    bool& _SYNC, bool& _RWB, const bool& _BE, const bool& _SOB, const bool& _PHI2, bool& _PHI1O, bool& _PHI2O,
    const bool& _RESB) noexcept : data{_data}, address{_address}, VPB{_VPB}, RDY{_RDY}, IRQB{_IRQB}, MLB{_MLB},
    NMIB{_NMIB},
    SYNC{_SYNC}, RWB{_RWB}, BE{_BE}, SOB{_SOB}, PHI2{_PHI2}, PHI1O{_PHI1O},
    PHI2O{_PHI2O}, RESB{_RESB} { }

void Cpu::cycle() noexcept {
    if (RESB) {
        lastRESB = true;
    }
    if (BE && readWriteBuffer) {
        dataBuffer = data;
    }
    if (!PHI2 && !RDY) {
        toBus();
        return;
    }
    PHI1O = !PHI2;
    PHI2O = PHI2;
    if (!(IRQB || (processorStatus & 0x4))) {
        interruptRequested = true;
    }
    if (!NMIB && NMIB_Before) {
        interruptDemanded = true;
    }
    NMIB_Before = NMIB;
    if (lastClockState == PHI2) {
        toBus();
        return;
    }
    if (!PHI2 && !RESB) {
        if (!lastRESB) {
            reset();
            return;
        }
        lastRESB = false;
    }
    lastClockState = PHI2;
    if (resetTimer) {
        interruptRequested = false;
        interruptDemanded = false;
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
    if (followingRequest) {
        handleInterruptRequest();
        toBus();
        return;
    }
    if (followingOrder) {
        handleNonMaskableInterrupt();
        toBus();
        return;
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
        pushProcessorStatus();
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
        andWithImmediate();
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
        returnFromInterrupt();
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
        xorImmediate();
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
        clearInterruptDisableFlag();
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
        addWithImmediate();
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
        setInterruptDisable();
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
        bitTestWithImmediate();
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
        loadYImmediate();
        break;
    case 0xa1:
        loadAZeroPageWithXIndirect();
        break;
    case 0xa2:
        loadXImmediate();
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
        loadAImmediate();
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
        compareYWithImmediate();
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
        compareAWithImmediate();
        break;
    case 0xca:
        decrementX();
        break;
    case 0xcb:
        waitForInterrupt();
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
        compareXWithImmediate();
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
        subtractWithImmediate();
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
        //TODO: add all the noop times
        noop();
        break;
    }
    toBus();
}

void Cpu::reset() noexcept {
    followingRequest = false;
    followingOrder = false;
    processorStatus &= 0b1111'0111;
    processorStatus |= 0b0011'0100;
    resetTimer = 7;
    IR = 0x6C;
}
