#pragma once
#include <array>
#include <cstdint>

class Eeprom32k final
{
private:
    // pin references
    uint16_t& addressAndEnable; // low Enable pin + A14 - A0
    uint8_t& dataPins; // D7 - D0
    // ignoring output enable

// internals
    std::array<uint8_t, 0x8000> data;

public:
    Eeprom32k(uint16_t& _addressAndEnable, uint8_t& _dataPins);
    void cycle() const;
    void program(const std::array<uint8_t, 0x8000>& _data);
};