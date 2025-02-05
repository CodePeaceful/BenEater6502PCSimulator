#include "Ram32k.hpp"

Ram32k::Ram32k(const unsigned short& _addressAndChipSelect, unsigned char& _data, const bool& _OE, const bool& _WE) :
    addressAndChipSelect { _addressAndChipSelect }, data { _data }, OE { _OE }, WE { _WE } {}

void Ram32k::cycle() {
    if (addressAndChipSelect & 0x8000) {
        return;
    }
    if (!WE) {
        dataStore[addressAndChipSelect] = data;
    }
    if (WE && !OE) {
        data = dataStore[addressAndChipSelect];
    }
}
