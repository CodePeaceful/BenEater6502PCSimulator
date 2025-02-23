#include "MiniLCD.hpp"

#include <algorithm>
#include <bit>
#include <chrono>

void MiniLCD::cycleCommand() {
    if (RW) {
        if (eightBit) {
            data = addressCounter;
            return;
        }
        throw std::runtime_error("4 bit mode not implemented");
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
            return;
        case 2:
            cgRam = false;
            addressCounter = 0;
            curserCell = 0;
            curserLine1 = true;
            shiftPos = 0;
            return;
        case 3:
            cgRam = false;
            moveRight = data & 0b00000010;
            doShift = data & 0b00000001;
            return;
        case 4:
            cgRam = false;
            screenOn = data & 0b0000100;
            curserOn = data & 0b00000010;
            curserBlink = data & 0b00000001;
            return;
        case 5:
            cgRam = false;
            doShift = data & 0b0001000;
            moveRight = data & 0b00000100;
            return;
        case 6:
            eightBit = data & 0b00010000;
            if (!(data & 0b00001000)) {
                throw std::runtime_error("two lines only");
            }
            /*if (data & 0b00000100) {
                throw std::runtime_error("5by8 only");
            }*/
            return;
        case 7:
            addressCounter = data & 0b00111111;
            cgRam = true;
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

void MiniLCD::updateTexture() {
    int i = 0;
    for (volatile const auto& d : characterGeneratorRam) {
        std::uint8_t pixel[20] { 0 };
        if (d & 0b00010000) {
            pixel[3] = 255;
        }
        if (d & 0b00001000) {
            pixel[3 + 4] = 255;
        }
        if (d & 0b00000100) {
            pixel[3 + 8] = 255;
        }
        if (d & 0b00000010) {
            pixel[3 + 12] = 255;
        }
        if (d & 0b00000001) {
            pixel[3 + 16] = 255;
        }
        texture.update(pixel, sf::Vector2u(5, 1), sf::Vector2u(3, i));
        ++i;
    }
}

void MiniLCD::updateDisplay() {
    updateTexture();
    for (auto i = 0ull; i < topRow.size(); ++i) {
        unsigned char topInt = displayDataRam[(shiftPos + i) % 40];
        unsigned char topIntLitte = topInt & 0b00001111;
        unsigned char topIntBig = (topInt >> 4) & 0b00001111;
        unsigned char bottomInt = displayDataRam[(shiftPos + i) % 40 + 40];
        unsigned char bottomIntLitte = bottomInt & 0b00001111;
        unsigned char bottomIntBig = (bottomInt >> 4) & 0b00001111;
        sf::Vector2i topChar(topIntBig * 8, topIntLitte * 8);
        sf::Vector2i bottomChar(bottomIntBig * 8, bottomIntLitte * 8);
        topRow[i].setTextureRect(sf::IntRect(topChar, sf::Vector2i(8, 8)));
        bottomRow[i].setTextureRect(sf::IntRect(bottomChar, sf::Vector2i(8, 8)));
    }
    if (curserOn) {
        if (!curserBlink || std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 2 == 0) {
            curser.setFillColor(sf::Color(0, 0, 0, 255));
            float posx = background.getPosition().x + 6 + curserCell * 16;
            float posy = background.getPosition().y + 18 + (curserLine1 ? 0 : 20);
            sf::Vector2f posVec(posx, posy);
            curser.setPosition(posVec);
            return;
        }
    }
    curser.setFillColor(sf::Color(0, 0, 0, 0));
}

MiniLCD::MiniLCD(unsigned char& _data, const bool& _E, const bool& _RW, const bool& _RS) : data { _data }, E { _E }, RW { _RW }, RS { _RS } {
    texture = sf::Texture("resources/HD44780_5x8Symbols.png");
    background.setSize(sf::Vector2f(256, 42));
    background.setFillColor(sf::Color::Green);
    curser.setSize(sf::Vector2f(6, 2));
    for (auto i = 0ull; i < topRow.size(); ++i) {
        topRow[i].scale(sf::Vector2f(2, 2));
        topRow[i].setPosition(sf::Vector2f(16 * i, 2));
        bottomRow[i].scale(sf::Vector2f(2, 2));
        bottomRow[i].setPosition(sf::Vector2f(16 * i, 22));
    }
    displayDataRam.fill(0x20);
}

void MiniLCD::draw(sf::RenderTarget& window) {
    if (!screenOn) {
        return;
    }
    updateDisplay();
    window.draw(background);
    for (auto i = 0; i < 16; ++i) {
        window.draw(topRow[i + shiftPos]);
        window.draw(bottomRow[i + shiftPos]);
    }
    window.draw(curser);
}

void MiniLCD::move(sf::Vector2f vec) {
    background.move(vec);
    std::ranges::for_each(topRow, [&vec] (sf::Sprite& s) {s.move(vec); });
    std::ranges::for_each(bottomRow, [&vec] (sf::Sprite& s) {s.move(vec); });
    curser.move(vec);
}

void MiniLCD::cycle() {
    if (!E) {
        done = false;
        return;
    }

    if (done)
        return;

    done = true;

    if (RS)
        return cycleData();

    cycleCommand();
}
