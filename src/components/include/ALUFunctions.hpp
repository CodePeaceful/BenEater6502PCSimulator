#pragma once

#include <cstdint>

namespace ALUFunctions
{
inline void negativeZeroCheck(uint8_t value, uint8_t& processorStatus) noexcept {
    if (value == 0) {
        processorStatus |= 0x02; // Set zero flag
    }
    else {
        processorStatus &= ~0x02; // Clear zero flag
    }

    if (value & 0x80) {
        processorStatus |= 0x80; // Set negative flag
    }
    else {
        processorStatus &= ~0x80; // Clear negative flag
    }
}

inline uint8_t addWithCarry(uint8_t first, uint8_t second, uint8_t& processorStatus) noexcept {
    const uint16_t carry = (processorStatus & 0x01) ? 1 : 0;
    const uint16_t sum = static_cast<uint16_t>(first) + static_cast<uint16_t>(second) + carry;

    // Set or clear carry flag
    if (sum > 0xFF) {
        processorStatus |= 0x01; // Set carry flag
    }
    else {
        processorStatus &= ~0x01; // Clear carry flag
    }

    const uint8_t result = static_cast<uint8_t>(sum & 0xFF);

    // Set or clear overflow flag
    if (((first ^ result) & (second ^ result) & 0x80) != 0) {
        processorStatus |= 0x40; // Set overflow flag
    }
    else {
        processorStatus &= ~0x40; // Clear overflow flag
    }

    negativeZeroCheck(result, processorStatus);
    return result;
}

inline uint8_t subtractWithCarry(uint8_t first, uint8_t second, uint8_t& processorStatus) noexcept {
    const uint16_t carry = (processorStatus & 0x01) ? 0 : 1;
    const uint16_t diff = static_cast<uint16_t>(first) - static_cast<uint16_t>(second) - carry;

    // Set or clear carry flag
    if (diff <= 0xFF) {
        processorStatus |= 0x01; // Set carry flag
    }
    else {
        processorStatus &= ~0x01; // Clear carry flag
    }

    const uint8_t result = static_cast<uint8_t>(diff & 0xFF);

    // Set or clear overflow flag
    if (((first ^ second) & (first ^ result) & 0x80) != 0) {
        processorStatus |= 0x40; // Set overflow flag
    }
    else {
        processorStatus &= ~0x40; // Clear overflow flag
    }

    negativeZeroCheck(result, processorStatus);
    return result;
}

inline uint8_t andOperation(uint8_t first, uint8_t second, uint8_t& processorStatus) noexcept {
    const uint8_t result = first & second;
    negativeZeroCheck(result, processorStatus);
    return result;
}

inline uint8_t orOperation(uint8_t first, uint8_t second, uint8_t& processorStatus) noexcept {
    const uint8_t result = first | second;
    negativeZeroCheck(result, processorStatus);
    return result;
}

inline uint8_t xorOperation(uint8_t first, uint8_t second, uint8_t& processorStatus) noexcept {
    const uint8_t result = first ^ second;
    negativeZeroCheck(result, processorStatus);
    return result;
}


// TODO: Implement tests
inline void bitTest(uint8_t value, uint8_t& processorStatus, uint8_t testValue) noexcept {
    // Set zero flag
    if ((value & testValue) == 0) {
        processorStatus |= 0x02; // Set zero flag
    }
    else {
        processorStatus &= ~0x02; // Clear zero flag
    }

    // Set negative flag
    if (testValue & 0x80) {
        processorStatus |= 0x80; // Set negative flag
    }
    else {
        processorStatus &= ~0x80; // Clear negative flag
    }

    // Set overflow flag
    if (testValue & 0x40) {
        processorStatus |= 0x40; // Set overflow flag
    }
    else {
        processorStatus &= ~0x40; // Clear overflow flag
    }
}

inline void compare(uint8_t first, uint8_t second, uint8_t& processorStatus) noexcept {
    if (first >= second) {
        processorStatus |= 0x01; // Set carry flag
    }
    else {
        processorStatus &= ~0x01; // Clear carry flag
    }

    if (first == second) {
        processorStatus |= 0x02; // Set zero flag
    }
    else {
        processorStatus &= ~0x02; // Clear zero flag
    }

    if ((first & 0x80) != 0 && first != second) {
        processorStatus |= 0x80; // Set negative flag
    }
    else {
        processorStatus &= ~0x80; // Clear negative flag
    }
}

// TODO: Implement tests
inline uint8_t shiftLeft(uint8_t value, uint8_t& processorStatus) noexcept {
    if (value & 0x80) {
        processorStatus |= 0x01; // Set carry flag
    }
    else {
        processorStatus &= ~0x01; // Clear carry flag
    }
    value = value << 1;
    negativeZeroCheck(value, processorStatus);
    return value;
}

// TODO: Implement tests
inline uint8_t shiftRight(uint8_t value, uint8_t& processorStatus) noexcept {
    if (value & 0x01) {
        processorStatus |= 0x01; // Set carry flag
    }
    else {
        processorStatus &= ~0x01; // Clear carry flag
    }
    value = value >> 1;
    negativeZeroCheck(value, processorStatus);
    return value;
}

// TODO: Implement tests
inline uint8_t rotateLeft(uint8_t value, uint8_t& processorStatus) noexcept {
    const bool carryIn = (processorStatus & 0x01) != 0;
    const bool newCarry = (value & 0x80) != 0;

    value = (value << 1) | (carryIn ? 1 : 0);

    if (newCarry) {
        processorStatus |= 0x01; // Set carry flag
    }
    else {
        processorStatus &= ~0x01; // Clear carry flag
    }

    negativeZeroCheck(value, processorStatus);
    return value;
}

// TODO: Implement tests
inline uint8_t rotateRight(uint8_t value, uint8_t& processorStatus) noexcept {
    const bool carryIn = (processorStatus & 0x01) != 0;
    const bool newCarry = (value & 0x01) != 0;

    value = (value >> 1) | (carryIn ? 0x80 : 0);

    if (newCarry) {
        processorStatus |= 0x01; // Set carry flag
    }
    else {
        processorStatus &= ~0x01; // Clear carry flag
    }

    negativeZeroCheck(value, processorStatus);
    return value;
}

inline uint8_t resetBit(uint8_t value, uint8_t bitPosition) noexcept {
    value &= ~(1 << bitPosition);
    return value;
}

inline uint8_t setBit(uint8_t value, uint8_t bitPosition) noexcept {
    value |= (1 << bitPosition);
    return value;
}

inline uint8_t increment(uint8_t value, uint8_t& processorStatus) noexcept {
    ++value;
    negativeZeroCheck(value, processorStatus);
    return value;
}

inline uint8_t decrement(uint8_t value, uint8_t& processorStatus) noexcept {
    --value;
    negativeZeroCheck(value, processorStatus);
    return value;
}
} // namespace ALUFunctions