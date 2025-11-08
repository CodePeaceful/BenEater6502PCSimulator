#pragma once

#include <array>
#include <cstdint>

//timing problems will be ignored
class Ram32k final
{
private:
    // pin references
    const uint16_t& addressAndChipSelect; // chip Select on low A14 - A0
    uint8_t& data; // D7 - D0
    const bool& OE; //output enable low;
    const bool& WE; // write Select on low

    // internals
    std::array<uint8_t, 0x8000> dataStore{0};

public:
    Ram32k(const uint16_t& _addressAndChipSelect, uint8_t& _data, const bool& _OE, const bool& _WE);
    void cycle();
};
