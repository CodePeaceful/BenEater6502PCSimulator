#include "Eeprom32k.hpp"

Eeprom32k::Eeprom32k(uint16_t& _addressAndEnable, uint8_t& _dataPins) :
    addressAndEnable{_addressAndEnable}, dataPins{_dataPins} { }

void Eeprom32k::cycle() const {
    if (addressAndEnable < 0x8000) {
        dataPins = data[addressAndEnable];
    }
}

void Eeprom32k::program(const std::array<uint8_t, 0x8000>& _data) {
    data = _data;
}
