#pragma once

#include <vector>
#include <ranges>

#include <SFML/Graphics.hpp>

/// busy expects the lcd to be updated twice per cycle of 1MHz
/// four bit mode or any non screen equivalent mode throws exception
class MiniLCD final
{
private:
    // Pin References
    unsigned char& data;
    const bool& E;
    const bool& RW; // Read high / Write Low
    const bool& RS; // Register Select

    // Visuals
    sf::RectangleShape background;
    std::vector<sf::Sprite> topRow{std::views::repeat(sf::Sprite(texture)) | std::views::take(16) | std::ranges::to<std::vector<sf::Sprite>>()};
    std::vector<sf::Sprite> bottomRow{std::views::repeat(sf::Sprite(texture)) | std::views::take(16) | std::ranges::to<std::vector<sf::Sprite>>()};
    sf::RectangleShape curser;


    // Internals
    unsigned char addressCounter{0};
    std::array<unsigned char, 80> displayDataRam;
    std::array<volatile unsigned char, 96> characterGeneratorRam;

    // helpers
    sf::Texture texture;
    unsigned char shiftPos{0};
    unsigned char curserCell;
    unsigned char fourBitBuffer;
    bool curserLine1{true};
    bool curserOn;
    bool curserBlink;
    bool doShift;
    bool moveRight;
    bool eightBit{true};
    bool screenOn;
    bool cgRam;
    bool done;

    unsigned short busyCounter{0};

    void cycleCommand();
    void cycleData();

    // Magic 
        // to be run is architectural thread
    void updateTexture();
    void updateDisplay();

public:
    MiniLCD(unsigned char& data, const bool& E, const bool& RW, const bool& RS);
    void draw(sf::RenderTarget& window);
    void move(sf::Vector2f vec);
    void cycle();
};
