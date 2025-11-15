#pragma once

#include <array>
#include <cstdint>

//timing problems will be ignored
class Ram32k final
{
private:
    // pin references
    const uint16_t& addressPins; // 15ignored A14 - A0
    uint8_t& data; // D7 - D0
    const bool& OE; // output enable low
    const bool& WE; // write Select on low
    const bool& CS; // chip select on low

    // internals
    std::array<uint8_t, 0x8000> dataStore{0};

    // healper flags
    bool OEbefore{false};
    bool WEbefore{false};
    bool CSbefore{false};
    bool inOperation{false};
    uint16_t addressBefore{0};

public:
    Ram32k(const uint16_t& _addressPins, uint8_t& _data, const bool& _OE, const bool& _WE, const bool& _CS);
    void cycle();
};
