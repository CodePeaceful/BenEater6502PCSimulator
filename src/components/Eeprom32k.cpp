#include "Eeprom32k.hpp"

Eeprom32k::Eeprom32k(unsigned short& _addressAndEnable, unsigned char& _dataPins) :
    addressAndEnable { _addressAndEnable }, dataPins { _dataPins } {}

void Eeprom32k::cycle() {
    if (addressAndEnable < 0x8000) {
        dataPins = data[addressAndEnable];
    }
}

void Eeprom32k::program(std::array<unsigned char, 0x8000> _data) {
    data = _data;
}
