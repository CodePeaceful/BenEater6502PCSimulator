#include "Ram32k.hpp"

Ram32k::Ram32k(const uint16_t& _addressAndChipSelect, uint8_t& _data, const bool& _OE, const bool& _WE, const bool& _CS) :
    addressPins{_addressAndChipSelect}, data{_data}, OE{_OE}, WE{_WE}, CS{_CS} { }

void Ram32k::cycle() {
    uint16_t address = addressPins & 0x7FFF; // mask A14 - A0
    if (!CS) {
        if (!CSbefore && !WE && !WEbefore) {
            // Write operation
            dataStore[addressBefore] = data;
            inOperation = true;
        }
        else if ((!OE || inOperation) && !OEbefore && (WE || inOperation) && WEbefore) {
            // Read operation
            data = dataStore[address];
            if (!OE && WE) {
                inOperation = true;
            }
        }
        else {
            inOperation = false;
        }
    }
    else {
        inOperation = false;
    }
    addressBefore = address;
    OEbefore = OE;
    WEbefore = WE;
    CSbefore = CS;
}
