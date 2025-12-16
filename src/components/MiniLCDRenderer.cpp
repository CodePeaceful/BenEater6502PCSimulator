#include "MiniLCDRenderer.hpp"

#include <algorithm>

namespace components
{
void MiniLCDRenderer::updateTexture() {
    int i = 0;
    lcd.cgrMutex.lock();
    auto characterGeneratorRam = lcd.characterGeneratorRam;
    lcd.cgrMutex.unlock();
    for (const auto& d : characterGeneratorRam) {
        std::uint8_t pixel[20]{0};
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

void MiniLCDRenderer::updateDisplay() {
    updateTexture();
    lcd.ddrMutex.lock();
    auto displayDataRam = lcd.displayDataRam;
    lcd.ddrMutex.unlock();
    for (auto i = 0ull; i < topRow.size(); ++i) {
        unsigned char topInt = displayDataRam[(lcd.shiftPos + i) % 40];
        unsigned char topIntLittle = topInt & 0b00001111;
        unsigned char topIntBig = (topInt >> 4) & 0b00001111;
        unsigned char bottomInt = displayDataRam[(lcd.shiftPos + i) % 40 + 40];
        unsigned char bottomIntLittle = bottomInt & 0b00001111;
        unsigned char bottomIntBig = (bottomInt >> 4) & 0b00001111;
        sf::Vector2i topChar(topIntBig * 8, topIntLittle * 8);
        sf::Vector2i bottomChar(bottomIntBig * 8, bottomIntLittle * 8);
        topRow[i].setTextureRect(sf::IntRect(topChar, sf::Vector2i(8, 8)));
        bottomRow[i].setTextureRect(sf::IntRect(bottomChar, sf::Vector2i(8, 8)));
    }
    if (lcd.curserOn) {
        if (!lcd.curserBlink || std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 2 == 0) {
            curser.setFillColor(sf::Color(0, 0, 0, 255));
            float posX = background.getPosition().x + 6 + lcd.curserCell * 16;
            float posY = background.getPosition().y + 18 + (lcd.curserLine1 ? 0 : 20);
            sf::Vector2f posVec(posX, posY);
            curser.setPosition(posVec);
            return;
        }
    }
    curser.setFillColor(sf::Color(0, 0, 0, 0));
}

MiniLCDRenderer::MiniLCDRenderer(const MiniLCD& _lcd) : lcd{_lcd} {
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
}

void MiniLCDRenderer::draw(sf::RenderTarget& window) {
    if (!lcd.screenOn) {
        return;
    }
    updateDisplay();
    window.draw(background);
    for (auto i = 0; i < 16; ++i) {
        window.draw(topRow[i]);
        window.draw(bottomRow[i]);
    }
    window.draw(curser);
}

void MiniLCDRenderer::move(sf::Vector2f vec) {
    background.move(vec);
    std::ranges::for_each(topRow, [&vec](sf::Sprite& s) {s.move(vec); });
    std::ranges::for_each(bottomRow, [&vec](sf::Sprite& s) {s.move(vec); });
    curser.move(vec);
}
} // namespace components