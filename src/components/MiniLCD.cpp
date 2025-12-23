#include "MiniLCD.hpp"

#include <bit>

using namespace std::literals::chrono_literals;

namespace components
{
void MiniLCD::cycleCommand() {
    if (RW) {
        if (eightBit) {
            data = addressCounter;
            if (busy) {
                data |= 0x80;
            }
            return;
        }
        throw std::runtime_error("4 bit mode not implemented");
        return;
    }
    if (busy) {
        busyEndTime += 20us; // extend busy time exact value not known
        return;
    }
    if (eightBit) {
        switch (std::bit_width(data)) {
        case 1:
            cgRam = false;
            displayDataRam.fill(0x20);
            addressCounter = 0;
            curserCell = 0;
            curserLine1 = true;
            // not in data sheet
            busyEndTime = std::chrono::high_resolution_clock::now() + 1520us;
            return;
        case 2:
            cgRam = false;
            addressCounter = 0;
            curserCell = 0;
            curserLine1 = true;
            shiftPos = 0;
            busyEndTime = std::chrono::high_resolution_clock::now() + 1520us;
            return;
        case 3:
            cgRam = false;
            moveRight = data & 0b00000010;
            doShift = data & 0b00000001;
            busyEndTime = std::chrono::high_resolution_clock::now() + 37us;
            return;
        case 4:
            cgRam = false;
            screenOn = data & 0b0000100;
            curserOn = data & 0b00000010;
            curserBlink = data & 0b00000001;
            busyEndTime = std::chrono::high_resolution_clock::now() + 37us;
            return;
        case 5:
            cgRam = false;
            doShift = data & 0b0001000;
            moveRight = data & 0b00000100;
            busyEndTime = std::chrono::high_resolution_clock::now() + 37us;
            return;
        case 6:
            eightBit = data & 0b00010000;
            if (!(data & 0b00001000)) {
                throw std::runtime_error("two lines only");
            }
            /*if (data & 0b00000100) {
                throw std::runtime_error("5by8 only");
            }*/
            busyEndTime = std::chrono::high_resolution_clock::now() + 37us;
            return;
        case 7:
            addressCounter = data & 0b00111111;
            cgRam = true;
            busyEndTime = std::chrono::high_resolution_clock::now() + 37us;
            return;
        case 8:
            addressCounter = data & 0b01111111;
            if (addressCounter < 40) {
                curserLine1 = true;
                shiftPos = addressCounter;
            }
            else if (addressCounter >= 0x40 && addressCounter <= 0x67) {
                curserLine1 = false;
                shiftPos = addressCounter - 0x40;
            }
            cgRam = false;
            busyEndTime = std::chrono::high_resolution_clock::now() + 37us;
            return;
        default:
            return;
        }
    }
    throw std::runtime_error("4 bit mode not implemented");
}

void MiniLCD::cycleData() {
    if (!eightBit) {
        throw std::runtime_error("4 bit mode not implemented");
    }
    if (busy) {
        busyEndTime += 20us; // extend busy time exact value not known
        return;
    }
    busyEndTime = std::chrono::high_resolution_clock::now() + 37us;
    if (RW) {
        if (cgRam) {
            if (addressCounter < characterGeneratorRam.size()) {
                data = characterGeneratorRam[addressCounter];
            }
            else {
                data = 0;
            }
        }
        else {
            if (addressCounter < 40) {
                data = displayDataRam[addressCounter];
            }
            else if (addressCounter >= 0x40 && addressCounter < 0x68) {
                data = displayDataRam[addressCounter - 0x40 + 40];
            }
            else {
                data = 0;
            }
        }
    }
    else {
        if (cgRam) {
            if (addressCounter < characterGeneratorRam.size()) {
                characterGeneratorRam[addressCounter] = data;
            }
        }
        else {
            if (addressCounter < 40) {
                displayDataRam[addressCounter] = data;
            }
            else if (addressCounter >= 0x40 && addressCounter < 0x68) {
                displayDataRam[addressCounter - 0x40 + 40] = data;
            }
        }
    }
    if (moveRight) {
        if (doShift) {
            ++shiftPos;
            shiftPos %= 40;
        }
        else if (curserCell < 15) {
            ++curserCell;
        }
        ++addressCounter;
    }
    else {
        if (doShift) {
            if (shiftPos > 0) {
                --shiftPos;
            }
            else {
                shiftPos = 39;
            }
        }
        else if (curserCell > 0) {
            --curserCell;
        }
        --addressCounter;
    }
    if (curserLine1) {
        if (addressCounter == 255) {
            addressCounter = 39;
            return;
        }
        if (addressCounter == 40) {
            addressCounter = 0;
            return;
        }
    }
    else {
        if (addressCounter == 0x3f) {
            addressCounter = 0x67;
            return;
        }
        if (addressCounter == 0x68) {
            addressCounter = 0x40;
            return;
        }
    }
}

MiniLCD::MiniLCD(uint8_t& _data, const bool& _E, const bool& _RW, const bool& _RS) : data{_data}, E{_E}, RW{_RW}, RS{_RS} {
    displayDataRam.fill(0x20);
}

void MiniLCD::cycle() {
    if (!E) {
        done = false;
        return;
    }

    if (done)
        return;
#ifdef neverBusy
    busy = false;
#else
    busy = busyEndTime > std::chrono::high_resolution_clock::now();
#endif    
    done = true;

    if (RS)
        return cycleData();

    cycleCommand();
}
} // namespace components
