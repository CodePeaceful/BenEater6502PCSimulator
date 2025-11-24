#include "Eeprom32k.hpp"

Eeprom32k::Eeprom32k(const uint16_t& _addressPins, uint8_t& _dataPins, const bool& _outputEnable, const bool& _chipSelect) :
    addressPins{_addressPins}, dataPins{_dataPins}, outputEnable{_outputEnable}, chipSelect{_chipSelect} { }

void Eeprom32k::cycle() {
    uint16_t address = addressPins & 0x7FFF; // A14 - A0
    bool enabled = !chipSelect; // Enable is active low
    bool OE = !outputEnable; // Output Enable is active low
    if (enabledBefore && OEBefore && enabled && OE) {
        isWriting = true;
    }
    else if (!enabled || !OE) {
        // Finish write cycle
        isWriting = false;
    }
    if (isWriting) {
        dataPins = data[address];
    }
    addressBefore = address;
    enabledBefore = enabled;
    OEBefore = OE;
}

void Eeprom32k::program(const std::array<uint8_t, 0x8000>& _data) {
    data = _data;
}
