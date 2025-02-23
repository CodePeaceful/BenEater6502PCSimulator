#pragma once

#include <array>

//timing problems will be ignored
class Ram32k
{
private:
// pin references
    const unsigned short& addressAndChipSelect; // chip Select on low A14 - A0
    unsigned char& data; // D7 - D0
    const bool& OE; //output enable low;
    const bool& WE; // write Select on low

// internals
    std::array<unsigned char, 0x8000> dataStore { 0 };

public:
    Ram32k(const unsigned short& _addressAndChipSelect, unsigned char& _data, const bool& _OE, const bool& _WE);
    void cycle();
};
