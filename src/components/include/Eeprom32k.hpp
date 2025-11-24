#pragma once
#include <array>
#include <cstdint>

class Eeprom32k final
{
private:
    // pin references
    const uint16_t& addressPins; // 15ignored  A14 - A0
    uint8_t& dataPins; // D7 - D0
    const bool& outputEnable; // active low
    const bool& chipSelect; // active low

    // internals
    std::array<uint8_t, 0x8000> data;

    // helpers
    uint16_t addressBefore{0};
    bool enabledBefore{false};
    bool OEBefore{false};
    bool isWriting{false};

public:
    Eeprom32k(const uint16_t& _addressPins, uint8_t& _dataPins, const bool& _outputEnable, const bool& _chipSelect);
    void cycle();
    void program(const std::array<uint8_t, 0x8000>& _data);
};