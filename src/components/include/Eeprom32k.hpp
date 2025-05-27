#pragma once
#include <array>

class Eeprom32k final
{
private:
    // pin references
    unsigned short& addressAndEnable; // low Enable pin + A14 - A0
    unsigned char& dataPins; // D7 - D0
    // ignoring output enable

// internals
    std::array<unsigned char, 0x8000> data;

public:
    Eeprom32k(unsigned short& _addressAndEnable, unsigned char& _dataPins);
    void cycle() const;
    void program(const std::array<unsigned char, 0x8000> &_data);
};